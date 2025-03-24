#include "audiocore.h"

AudioCore::AudioCore(QObject *parent) : QObject(parent)
{
    Pa_Initialize();
    av_log_set_level(AV_LOG_ERROR);
}

AudioCore::~AudioCore()
{
    stop();
    cleanup();
    Pa_Terminate();
}

void AudioCore::setVolumeValue(const float& value)
{
    this->m_volumeValue = qBound(0.0f, value, 1.0f);
}

void AudioCore::changeFile(const std::string &newFilePath)
{
    bool wasPlaying = m_isPlaying;
    m_fileName = QFileInfo(QString::fromUtf8(newFilePath)).fileName();
    stop();
    cleanup();

    if (!loadFile(newFilePath)) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::FILE_READ);
        emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::UNDEFINED);
        return;
    }

    emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::READY);

    m_isFinished = false;

    if (wasPlaying) {
        play();
    }
}

bool AudioCore::loadFile(const std::string &filename)
{

    cleanup();
    m_fileChanged = true;

    if (avformat_open_input(&m_fmt_ctx, filename.c_str(), nullptr, nullptr) < 0) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::FILE_READ);
        emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::UNDEFINED);
        return false;
    }

    if (avformat_find_stream_info(m_fmt_ctx, nullptr) < 0) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::STREAM_INFO);
        emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::UNDEFINED);
        return false;
    }

    int stream_index = -1;
    for (unsigned int i = 0; i < m_fmt_ctx->nb_streams; i++) {
        if (m_fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            stream_index = i;
            break;
        }
    }

    if (stream_index == -1) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::STREAM_ERROR);
                emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::UNDEFINED);
        return false;
    }

    auto codec = avcodec_find_decoder(m_fmt_ctx->streams[stream_index]->codecpar->codec_id);
    if (!codec) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::CODEC_ERROR);
                emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::UNDEFINED);
        return false;
    }

    m_codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(m_codec_ctx, m_fmt_ctx->streams[stream_index]->codecpar);
    avcodec_open2(m_codec_ctx, codec, nullptr);

    m_swr_ctx = swr_alloc();

    AVChannelLayout in_ch_layout, out_ch_layout;
    av_channel_layout_default(&in_ch_layout, m_codec_ctx->ch_layout.nb_channels);
    av_channel_layout_default(&out_ch_layout, DEFAULT_CHANNELS);

    swr_alloc_set_opts2(
        &m_swr_ctx,
        &out_ch_layout, AV_SAMPLE_FMT_FLT, DEFAULT_SAMPLERATE,
        &in_ch_layout, m_codec_ctx->sample_fmt, m_codec_ctx->sample_rate,
        0, nullptr
    );

    if (swr_init(m_swr_ctx) < 0) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::SWRESAMPLE_ERROR);
                emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::UNDEFINED);
        return false;
    }

    m_packet = av_packet_alloc();
    m_frame = av_frame_alloc();

    m_audioBuffer.clear();
    m_bufferIndex = 0;

    while (av_read_frame(m_fmt_ctx, m_packet) >= 0) {
        if (m_packet->stream_index == stream_index) {
            if (avcodec_send_packet(m_codec_ctx, m_packet) == 0) {
                while (avcodec_receive_frame(m_codec_ctx, m_frame) == 0) {
                    uint8_t *out_buf[2] = { nullptr };
                    int out_linesize;
                    int out_samples = av_rescale_rnd(
                                          swr_get_delay(m_swr_ctx, m_codec_ctx->sample_rate) + m_frame->nb_samples,
                                          DEFAULT_SAMPLERATE, m_codec_ctx->sample_rate, AV_ROUND_UP
                                      );

                    av_samples_alloc(out_buf, &out_linesize, DEFAULT_CHANNELS, out_samples, AV_SAMPLE_FMT_FLT, 0);

                    int converted = swr_convert(m_swr_ctx, out_buf, out_samples,
                                                (const uint8_t **)m_frame->data, m_frame->nb_samples);
                    if (converted > 0) {
                        size_t sampleCount = converted * DEFAULT_CHANNELS;
                        std::vector<float> tempBuffer(sampleCount);
                        memcpy(tempBuffer.data(), out_buf[0], sampleCount * sizeof(float));
                        m_audioBuffer.insert(m_audioBuffer.end(), tempBuffer.begin(), tempBuffer.end());
                    }

                    av_freep(&out_buf[0]);
                }
            }
        }
        av_packet_unref(m_packet);
    }

    return !m_audioBuffer.empty();
}

void AudioCore::play()
{
    if (m_audioBuffer.empty()) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::BUFFER_EMPTY);
                emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::UNDEFINED);
        return;
    }

    if (m_fileChanged || m_isFinished) {
        m_bufferIndex = 0;
        m_fileChanged = false;
        m_isFinished = false;
    }

    if (m_bufferIndex != 0)
        emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::RESUME);
    else
        emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::START);

    if (m_isPlaying && !m_isPaused) return;

    m_isPlaying = true;
    m_isPaused = false;

    if (!m_stream) {
        Pa_OpenDefaultStream(&m_stream, 0, 2, paFloat32, DEFAULT_SAMPLERATE, DEFAULT_FRAMES_PER_BUFFER, paCallback, this);
    }

    if (Pa_IsStreamStopped(m_stream) == 1) {
        Pa_StartStream(m_stream);
    }
}

void AudioCore::stop()
{

    if (!m_isPlaying) return;


    emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::STOP);

    m_isPlaying = false;
    m_isPlaying = true;

    if (m_stream && Pa_IsStreamActive(m_stream) == 1) {
        Pa_StopStream(m_stream);
    }
}

void AudioCore::restart()
{

    emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::RESTART);

    m_bufferIndex = 0;
    m_isPaused = false;
    m_isPlaying = true;

    if (m_stream) {
        Pa_CloseStream(m_stream);
        m_stream = nullptr;
    }

    Pa_OpenDefaultStream(&m_stream, 0, 2, paFloat32, DEFAULT_SAMPLERATE, DEFAULT_FRAMES_PER_BUFFER, paCallback, this);
    Pa_StartStream(m_stream);

}

int AudioCore::paCallback(const void *inputBuffer, void *outputBuffer,
                          unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo,
                          PaStreamCallbackFlags statusFlags, void *userData)
{
    AudioCore *player = static_cast<AudioCore *>(userData);
    float *out = (float *)outputBuffer;


    if (player->m_isPaused || player->m_isFinished) {
        memset(out, 0, framesPerBuffer * DEFAULT_CHANNELS * sizeof(float));
        return paContinue;
    }

    if (!player->m_isPlaying || player->m_bufferIndex >= player->m_audioBuffer.size()) {
        player->sendAudioStatus(player->m_fileName, AUDIO::CORE::STATUS::END);
        memset(out, 0, framesPerBuffer * DEFAULT_CHANNELS * sizeof(float));
        player->m_isPlaying = false;
        player->m_isFinished = true;
        Pa_StopStream(player->m_stream);
        return paComplete;
    }

    size_t samplesToCopy = framesPerBuffer * DEFAULT_CHANNELS;
    size_t remainingSamples = player->m_audioBuffer.size() - player->m_bufferIndex;

    QVector<float> audioChunk(samplesToCopy);
    memcpy(audioChunk.data(), &player->m_audioBuffer[player->m_bufferIndex], samplesToCopy * sizeof(float));

    emit player->sendAudioSamples(audioChunk);

    for (unsigned i = 0; i < samplesToCopy; ++i) {
        out[i] = audioChunk[i] * player->m_volumeValue;
    }
    if (remainingSamples < samplesToCopy) {
        player->sendAudioStatus(player->m_fileName, AUDIO::CORE::STATUS::END);
        memset(out, 0, samplesToCopy * sizeof(float));
        player->m_isPlaying = false;
        player->m_isFinished = true;
        Pa_StopStream(player->m_stream);
        return paComplete;
    }

    // memcpy(out, &player->audioBuffer[player->bufferIndex], samplesToCopy * sizeof(float));
    player->m_bufferIndex += samplesToCopy;

    return paContinue;
}

void AudioCore::cleanup()
{
    if (m_swr_ctx) {
        swr_free(&m_swr_ctx);
    }
    if (m_codec_ctx) {
        avcodec_free_context(&m_codec_ctx);
    }
    if (m_fmt_ctx) {
        avformat_close_input(&m_fmt_ctx);
    }
    if (m_packet) {
        av_packet_free(&m_packet);
    }
    if (m_frame) {
        av_frame_free(&m_frame);
    }

    m_audioBuffer.clear();
}
