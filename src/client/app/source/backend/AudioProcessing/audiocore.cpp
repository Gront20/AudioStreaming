#include "AudioCore.h"
#include <QDebug>

AudioCore::AudioCore(QObject* parent)
    : QObject(parent), m_formatContext(nullptr), m_codecContext(nullptr), m_swrContext(nullptr), m_isPlaying(false) {

    PaError err = Pa_Initialize();
    if (err != paNoError) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::PA_INIT);
        return;
    }

    PaStreamParameters outputParams;
    outputParams.device = Pa_GetDefaultOutputDevice();
    if (outputParams.device == paNoDevice) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::DEVICE_ERROR);
        return;
    }

    outputParams.channelCount = m_channels;
    outputParams.sampleFormat = paFloat32;
    outputParams.suggestedLatency = Pa_GetDeviceInfo(outputParams.device)->defaultLowOutputLatency;
    outputParams.hostApiSpecificStreamInfo = nullptr;

    err = Pa_OpenStream(&stream, nullptr, &outputParams, m_sampleRate, paFramesPerBufferUnspecified, paClipOff, nullptr, nullptr);
    if (err != paNoError) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::STREAM_ERROR);
        return;
    }
}

AudioCore::~AudioCore() {
    stop();
}

bool AudioCore::init(int sampleRate, int channels) {
    m_sampleRate = sampleRate;
    m_channels = channels;

    if (m_swrContext) {
        swr_free(&m_swrContext);
    }

    m_swrContext = swr_alloc_set_opts(nullptr,
                                      av_get_default_channel_layout(channels), AV_SAMPLE_FMT_FLT, sampleRate,
                                      av_get_default_channel_layout(channels), AV_SAMPLE_FMT_S16, sampleRate,
                                      0, nullptr);
    if (!m_swrContext || swr_init(m_swrContext) < 0) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::SWRESAMPLE_ERROR);
        return false;
    }
    return true;
}

void AudioCore::playAudio(const QVector<float>& samples, int frameSize) {

    if (!m_isPlaying) return;

    if (samples.isEmpty()) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::BUFFER_EMPTY);
        return;
    }

    if (!stream) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::STREAM_ERROR);
        return;
    }

    int numFrames = samples.size() / m_channels;


    PaError err = Pa_WriteStream(stream, samples.data(), numFrames);
    if (err != paNoError) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::PA_ERROR);
    }

    emit sendAudioSamples(samples);
}


void AudioCore::start() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::PA_INIT);
        return;
    }

    PaStreamParameters outputParams;
    outputParams.device = Pa_GetDefaultOutputDevice();
    if (outputParams.device == paNoDevice) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::DEVICE_ERROR);
        Pa_Terminate();
        return;
    }

    outputParams.channelCount = m_channels;
    outputParams.sampleFormat = paFloat32;
    outputParams.suggestedLatency = Pa_GetDeviceInfo(outputParams.device)->defaultLowOutputLatency;
    outputParams.hostApiSpecificStreamInfo = nullptr;

    err = Pa_OpenStream(&stream, nullptr, &outputParams, m_sampleRate, paFramesPerBufferUnspecified, paClipOff, nullptr, nullptr);
    if (err != paNoError) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::STREAM_ERROR);
        Pa_Terminate();
        return;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::STREAM_ERROR);
        Pa_CloseStream(stream);
        Pa_Terminate();
        stream = nullptr;
        return;
    }

    m_isPlaying = true;
    emit sendAudioStatus(AUDIO::CORE::STATUS::START);
}

void AudioCore::stop() {

    emit sendAudioStatus(AUDIO::CORE::STATUS::STOP);

    if (stream) {
        PaError err = Pa_StopStream(stream);
        if (err != paNoError) {
            emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::STREAM_ERROR);
        }

        err = Pa_CloseStream(stream);
        if (err != paNoError) {
            emit sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER::STREAM_ERROR);
        }

        stream = nullptr;
    }

    Pa_Terminate();

    if (m_swrContext) {
        swr_free(&m_swrContext);
        m_swrContext = nullptr;
    }

    m_isPlaying = false;
}
