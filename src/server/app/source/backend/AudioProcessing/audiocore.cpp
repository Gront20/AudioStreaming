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
    bool wasPlaying = isPlaying;
    m_fileName = QFileInfo(QString::fromUtf8(newFilePath)).fileName();
    stop();
    cleanup();

    if (!loadFile(newFilePath)) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::FILE_READ);
        emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::UNDEFINED);
        return;
    }

    emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::READY);

    isFinished = false;

    if (wasPlaying) {
        play();
    }
}

bool AudioCore::loadFile(const std::string &filename)
{

    cleanup();
    fileChanged = true;

    if (avformat_open_input(&fmt_ctx, filename.c_str(), nullptr, nullptr) < 0) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::FILE_READ);
        emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::UNDEFINED);
        return false;
    }

    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::STREAM_INFO);
        emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::UNDEFINED);
        return false;
    }

    int stream_index = -1;
    for (unsigned int i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            stream_index = i;
            break;
        }
    }

    if (stream_index == -1) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::STREAM_ERROR);
                emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::UNDEFINED);
        return false;
    }

    auto codec = avcodec_find_decoder(fmt_ctx->streams[stream_index]->codecpar->codec_id);
    if (!codec) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::CODEC_ERROR);
                emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::UNDEFINED);
        return false;
    }

    codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_ctx, fmt_ctx->streams[stream_index]->codecpar);
    avcodec_open2(codec_ctx, codec, nullptr);

    swr_ctx = swr_alloc();

    AVChannelLayout in_ch_layout, out_ch_layout;
    av_channel_layout_default(&in_ch_layout, codec_ctx->ch_layout.nb_channels);
    av_channel_layout_default(&out_ch_layout, DEFAULT_CHANNELS);

    swr_alloc_set_opts2(
        &swr_ctx,
        &out_ch_layout, AV_SAMPLE_FMT_FLT, DEFAULT_SAMPLERATE,
        &in_ch_layout, codec_ctx->sample_fmt, codec_ctx->sample_rate,
        0, nullptr
    );

    if (swr_init(swr_ctx) < 0) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::SWRESAMPLE_ERROR);
                emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::UNDEFINED);
        return false;
    }

    packet = av_packet_alloc();
    frame = av_frame_alloc();

    audioBuffer.clear();
    bufferIndex = 0;

    while (av_read_frame(fmt_ctx, packet) >= 0) {
        if (packet->stream_index == stream_index) {
            if (avcodec_send_packet(codec_ctx, packet) == 0) {
                while (avcodec_receive_frame(codec_ctx, frame) == 0) {
                    uint8_t *out_buf[2] = { nullptr };
                    int out_linesize;
                    int out_samples = av_rescale_rnd(
                                          swr_get_delay(swr_ctx, codec_ctx->sample_rate) + frame->nb_samples,
                                          DEFAULT_SAMPLERATE, codec_ctx->sample_rate, AV_ROUND_UP
                                      );

                    av_samples_alloc(out_buf, &out_linesize, DEFAULT_CHANNELS, out_samples, AV_SAMPLE_FMT_FLT, 0);

                    int converted = swr_convert(swr_ctx, out_buf, out_samples,
                                                (const uint8_t **)frame->data, frame->nb_samples);
                    if (converted > 0) {
                        size_t sampleCount = converted * DEFAULT_CHANNELS;
                        std::vector<float> tempBuffer(sampleCount);
                        memcpy(tempBuffer.data(), out_buf[0], sampleCount * sizeof(float));
                        audioBuffer.insert(audioBuffer.end(), tempBuffer.begin(), tempBuffer.end());
                    }

                    av_freep(&out_buf[0]);
                }
            }
        }
        av_packet_unref(packet);
    }

    return !audioBuffer.empty();
}

void AudioCore::play()
{
    if (audioBuffer.empty()) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::BUFFER_EMPTY);
                emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::UNDEFINED);
        return;
    }

    if (fileChanged || isFinished) {
        bufferIndex = 0;
        fileChanged = false;
        isFinished = false;
    }

    if (bufferIndex != 0)
        emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::RESUME);
    else
        emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::START);

    QMutexLocker locker(&mtx);

    if (isPlaying && !isPaused) return;

    isPlaying = true;
    isPaused = false;

    if (!stream) {
        Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, DEFAULT_SAMPLERATE, DEFAULT_FRAMES_PER_BUFFER, paCallback, this);
    }

    if (Pa_IsStreamStopped(stream) == 1) {
        Pa_StartStream(stream);
    }

    cv.notify_all();
}

void AudioCore::stop()
{

    if (!isPlaying) return;
    QMutexLocker locker(&mtx);


    emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::STOP);

    isPlaying = false;
    isPaused = true;

    if (stream && Pa_IsStreamActive(stream) == 1) {
        Pa_StopStream(stream);
    }

    cv.notify_all();
}

void AudioCore::restart()
{

    QMutexLocker locker(&mtx);

    emit sendAudioStatus(m_fileName, AUDIO::CORE::STATUS::RESTART);

    bufferIndex = 0;
    isPaused = false;
    isPlaying = true;

    if (stream) {
        Pa_CloseStream(stream);
        stream = nullptr;
    }

    Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, DEFAULT_SAMPLERATE, DEFAULT_FRAMES_PER_BUFFER, paCallback, this);
    Pa_StartStream(stream);

    cv.notify_all();
}

int AudioCore::paCallback(const void *inputBuffer, void *outputBuffer,
                          unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo,
                          PaStreamCallbackFlags statusFlags, void *userData)
{
    AudioCore *player = static_cast<AudioCore *>(userData);
    float *out = (float *)outputBuffer;

    QMutexLocker locker(&player->mtx);

    if (player->isPaused || player->isFinished) {
        memset(out, 0, framesPerBuffer * DEFAULT_CHANNELS * sizeof(float));
        return paContinue;
    }

    if (!player->isPlaying || player->bufferIndex >= player->audioBuffer.size()) {
        player->sendAudioStatus(player->m_fileName, AUDIO::CORE::STATUS::END);
        memset(out, 0, framesPerBuffer * DEFAULT_CHANNELS * sizeof(float));
        player->isPlaying = false;
        player->isFinished = true;
        Pa_StopStream(player->stream);
        return paComplete;
    }

    size_t samplesToCopy = framesPerBuffer * DEFAULT_CHANNELS;
    size_t remainingSamples = player->audioBuffer.size() - player->bufferIndex;

    QVector<float> audioChunk(samplesToCopy);
    memcpy(audioChunk.data(), &player->audioBuffer[player->bufferIndex], samplesToCopy * sizeof(float));

    emit player->sendAudioSamples(audioChunk);

    for (unsigned i = 0; i < samplesToCopy; ++i) {
        out[i] = audioChunk[i] * player->m_volumeValue;
    }
    if (remainingSamples < samplesToCopy) {
        player->sendAudioStatus(player->m_fileName, AUDIO::CORE::STATUS::END);
        memset(out, 0, samplesToCopy * sizeof(float));
        player->isPlaying = false;
        player->isFinished = true;
        Pa_StopStream(player->stream);
        return paComplete;
    }

    // memcpy(out, &player->audioBuffer[player->bufferIndex], samplesToCopy * sizeof(float));
    player->bufferIndex += samplesToCopy;

    return paContinue;
}

void AudioCore::cleanup()
{
    if (swr_ctx) {
        swr_free(&swr_ctx);
    }
    if (codec_ctx) {
        avcodec_free_context(&codec_ctx);
    }
    if (fmt_ctx) {
        avformat_close_input(&fmt_ctx);
    }
    if (packet) {
        av_packet_free(&packet);
    }
    if (frame) {
        av_frame_free(&frame);
    }

    audioBuffer.clear();
}


//AudioCore::AudioCore(QObject *parent) : QObject(parent)
//{
//    m_AudioFormat.wFormatTag = WAVE_FORMAT_PCM;
//    m_AudioFormat.nChannels = 2;
//    m_AudioFormat.nSamplesPerSec = 44100;
//    m_AudioFormat.nAvgBytesPerSec = 176400;
//    m_AudioFormat.nBlockAlign = 4;
//    m_AudioFormat.wBitsPerSample = 16;
//    m_AudioFormat.cbSize = 0;
//}

//void AudioCore::initialize()
//{
//    CoInitialize(NULL);

//    m_hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&m_Enumerator);
//    if (FAILED(m_hr)) {
//        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::DEVICE_ENUMERATOR, m_hr);
//        return;
//    }

//    m_hr = m_Enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m_DefaultDevice);
//    if (FAILED(m_hr)) {
//        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::ENDPOINT, m_hr);
//        m_Enumerator->Release();
//        return;
//    }

//    m_hr = m_DefaultDevice->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&m_AudioClient);
//    if (FAILED(m_hr)) {
//        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::AUDIOCLIENT, m_hr);
//        m_DefaultDevice->Release();
//        m_Enumerator->Release();
//        return;
//    }

//    // format settings
//    m_cbWfx = sizeof(WAVEFORMATEX);
//    m_hr = m_AudioClient->GetMixFormat(&m_pwfx);
//    if (FAILED(m_hr)) {
//        // emit signal error
//        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::MIX_FORMAT, m_hr);
//        m_AudioClient->Release();
//        m_DefaultDevice->Release();
//        m_Enumerator->Release();
//        return;
//    }

//    m_AudioFormat.nChannels = m_pwfx->nChannels;
//    m_AudioFormat.nSamplesPerSec = m_pwfx->nSamplesPerSec;
//    m_AudioFormat.wBitsPerSample = m_pwfx->wBitsPerSample;
//    m_AudioFormat.nBlockAlign = m_pwfx->nBlockAlign;
//    m_AudioFormat.nAvgBytesPerSec = m_pwfx->nAvgBytesPerSec;

//    CoTaskMemFree(m_pwfx);

//    m_hr = m_AudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
//                                     0, 0, m_pwfx, NULL);

//    if (FAILED(m_hr)) {
//        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::AUDIOCLIENT, m_hr);
//        m_AudioClient->Release();
//        m_DefaultDevice->Release();
//        m_Enumerator->Release();
//        return;
//    }

//    m_hr = m_AudioClient->GetService(__uuidof(IAudioRenderClient), (LPVOID *)&m_RenderClient);
//    if (FAILED(m_hr)) {
//        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::RENDERCLIENT, m_hr);
//        m_AudioClient->Release();
//        m_DefaultDevice->Release();
//        m_Enumerator->Release();
//        return;
//    }

//    m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
//    m_hr = m_AudioClient->SetEventHandle(m_hEvent);
//    if (FAILED(m_hr)) {
//        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::HEVENT, m_hr);
//        return;
//    }
//}

//bool AudioCore::setFilePath(const char *filepath)
//{
//    m_file = fopen(filepath, "rb");
//    if (m_file) {
//        fseek(m_file, 44, SEEK_SET); // Переместиться к началу аудиоданных
//        m_hr = m_AudioClient->GetBufferSize(&m_bufferFrameCount);
//        if (FAILED(m_hr)) {
//            emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::BUFFER_SIZE, m_hr);
//            fclose(m_file);
//            return false;
//        }
//    }

//    IAudioRenderClient *renderClient;
//    m_hr = m_AudioClient->GetService(__uuidof(IAudioRenderClient), (void **)&renderClient);
//    if (FAILED(m_hr)) {
//        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::RENDERCLIENT, m_hr);
//        m_AudioClient->Stop();
//        CloseHandle(m_hEvent);
//        fclose(m_file);
//        return false;
//    }

//    return true;
//}

//void AudioCore::start()
//{
//    if (!m_file) {
//        qDebug() << "Audio file not set!";
//        return;
//    }

//    emit sendAudioStatus(AUDIO::CORE::STATUS::START);

//    if (!m_AudioClient || !m_RenderClient) {
//        qDebug() << "AudioClient or RenderClient is not initialized!";
//        return;
//    }

//    if (m_hr == S_OK) {
//        m_AudioClient->Stop();
//    }

//    // Возвращаемся к месту, где остановились
//    fseek(m_file, m_lastReadPosition, SEEK_SET);

//    m_hr = m_AudioClient->Start();
//    if (FAILED(m_hr)) {
//        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::AUDIOCLIENT, m_hr);
//        return;
//    }

//    BYTE *buffer = nullptr;
//    DWORD bytesToRead = m_bufferFrameCount * m_AudioFormat.nBlockAlign;
//    DWORD bytesRead = 0;
//    DWORD framesWritten = 0;

//    while (true) {
//        WaitForSingleObject(m_hEvent, INFINITE); // Ждем готовность буфера

//        UINT32 padding = 0;
//        m_hr = m_AudioClient->GetCurrentPadding(&padding);
//        if (FAILED(m_hr)) {
//            emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::GET_PADDING, m_hr);
//            return;
//        }

//        UINT32 availableFrames = m_bufferFrameCount - padding;
//        if (availableFrames == 0) {
//            continue;
//        }

//        BYTE *buffer = nullptr;
//        m_hr = m_RenderClient->GetBuffer(availableFrames, &buffer);
//        if (FAILED(m_hr)) {
//            emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::GET_BUFFER, m_hr);
//            break;
//        }

//        DWORD bytesToRead = availableFrames * m_pwfx->nBlockAlign;
//        DWORD bytesRead = fread(buffer, 1, bytesToRead, m_file);

//        if (bytesRead < bytesToRead) {
//            memset(buffer + bytesRead, 0, bytesToRead - bytesRead); // Заполняем тишиной
//        }

//        UINT32 framesWritten = bytesRead / m_pwfx->nBlockAlign;
//        m_hr = m_RenderClient->ReleaseBuffer(framesWritten, 0);

//        if (FAILED(m_hr)) {
//            emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::RELEASE_BUFFER, m_hr);
//            break;
//        }

//        if (bytesRead == 0) {
//            m_lastReadPosition = ftell(m_file);
//            m_AudioClient->Stop();
//            emit sendAudioStatus(AUDIO::CORE::STATUS::END);
//            break;
//        }
//    }

//}

//void AudioCore::stop()
//{
//    if (!m_AudioClient) {
//        qDebug() << "AudioClient is not initialized!";
//        return;
//    }

//    m_hr = m_AudioClient->Stop();
//    if (FAILED(m_hr)) {
//        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::STOP_CLIENT, m_hr);
//        return;
//    }

//    if (m_file) {
//        m_lastReadPosition = ftell(m_file);
//        qDebug() << "Paused at position:" << m_lastReadPosition;
//    }
//}

//void AudioCore::restart()
//{
//    if (!m_AudioClient || !m_file) {
//        qDebug() << "AudioClient or file is not initialized!";
//        return;
//    }

//    m_hr = m_AudioClient->Stop();
//    if (FAILED(m_hr)) {
//        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::STOP_CLIENT, m_hr);
//        return;
//    }

//    fseek(m_file, 44, SEEK_SET);
//    m_lastReadPosition = 44;

//    start();
//}

//AudioCore::~AudioCore()
//{
//    // m_RenderClient->Release();
//    // m_AudioClient->Release();
//    // m_DefaultDevice->Release();
//    // m_Enumerator->Release();

//    CoUninitialize();
//}
