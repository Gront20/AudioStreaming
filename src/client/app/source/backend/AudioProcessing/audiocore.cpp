#include "AudioCore.h"
#include <QDebug>

AudioCore::AudioCore(QObject* parent)
    : QObject(parent), m_formatContext(nullptr), m_codecContext(nullptr), m_swrContext(nullptr), m_isPlaying(false) {

    PaError err = Pa_Initialize();
    if (err != paNoError) {
        qWarning() << "PortAudio init failed:" << Pa_GetErrorText(err);
        return;
    }

    PaStreamParameters outputParams;
    outputParams.device = Pa_GetDefaultOutputDevice();
    if (outputParams.device == paNoDevice) {
        qWarning() << "No default audio output device!";
        return;
    }

    outputParams.channelCount = m_channels;
    outputParams.sampleFormat = paFloat32;
    outputParams.suggestedLatency = Pa_GetDeviceInfo(outputParams.device)->defaultLowOutputLatency;
    outputParams.hostApiSpecificStreamInfo = nullptr;

    err = Pa_OpenStream(&stream, nullptr, &outputParams, m_sampleRate, paFramesPerBufferUnspecified, paClipOff, nullptr, nullptr);
    if (err != paNoError) {
        qWarning() << "Failed to open PortAudio stream:" << Pa_GetErrorText(err);
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
        qWarning() << "Failed to initialize resampler";
        return false;
    }
    return true;
}

void AudioCore::playAudio(const QVector<float>& samples, int frameSize) {
    if (samples.isEmpty()) {
        qWarning() << "Received empty audio data!";
        return;
    }

    if (!stream) {
        qWarning() << "PortAudio stream is not initialized!";
        return;
    }

    int numFrames = samples.size() / m_channels;

    PaError err = Pa_WriteStream(stream, samples.data(), numFrames);
    if (err != paNoError) {
        qWarning() << "PortAudio error:" << Pa_GetErrorText(err);
    }
}


void AudioCore::start() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        qWarning() << "PortAudio init failed:" << Pa_GetErrorText(err);
        return;
    }

    PaStreamParameters outputParams;
    outputParams.device = Pa_GetDefaultOutputDevice();
    if (outputParams.device == paNoDevice) {
        qWarning() << "No default audio output device!";
        Pa_Terminate();
        return;
    }

    outputParams.channelCount = m_channels;
    outputParams.sampleFormat = paFloat32;
    outputParams.suggestedLatency = Pa_GetDeviceInfo(outputParams.device)->defaultLowOutputLatency;
    outputParams.hostApiSpecificStreamInfo = nullptr;

    err = Pa_OpenStream(&stream, nullptr, &outputParams, m_sampleRate, paFramesPerBufferUnspecified, paClipOff, nullptr, nullptr);
    if (err != paNoError) {
        qWarning() << "Failed to open PortAudio stream:" << Pa_GetErrorText(err);
        Pa_Terminate();
        return;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        qWarning() << "Failed to start PortAudio stream:" << Pa_GetErrorText(err);
        Pa_CloseStream(stream);
        Pa_Terminate();
        stream = nullptr;
        return;
    }

    m_isPlaying = true;
    qDebug() << "Audio playback started";
}

void AudioCore::stop() {
    if (stream) {
        PaError err = Pa_StopStream(stream);
        if (err != paNoError) {
            qWarning() << "Failed to stop PortAudio stream:" << Pa_GetErrorText(err);
        }

        err = Pa_CloseStream(stream);
        if (err != paNoError) {
            qWarning() << "Failed to close PortAudio stream:" << Pa_GetErrorText(err);
        }

        stream = nullptr;
    }

    Pa_Terminate();

    if (m_swrContext) {
        swr_free(&m_swrContext);
        m_swrContext = nullptr;
    }

    m_isPlaying = false;
    qDebug() << "Audio playback stopped";
}
