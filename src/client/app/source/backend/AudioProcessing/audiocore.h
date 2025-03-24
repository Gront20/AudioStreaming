#ifndef AUDIOCORE_H
#define AUDIOCORE_H

#include <QObject>
#include <QVector>
#include "defines.h"

extern "C" {
#include <libswresample/swresample.h>
#include <portaudio.h>
}

class AudioCore : public QObject
{
    Q_OBJECT
public:
    explicit AudioCore(QObject *parent = nullptr);
    ~AudioCore();

    bool init(int sampleRate, int channels);
    void playAudio(QVector<float> &samples);
    void start();
    void stop();
    void setVolumeValue(const float& value);

private:

    void cleanup();

signals:

    void sendCurrentStateError(const AUDIO::CORE::ERROR_HANDLER &errorCode);
    void sendAudioStatus(const AUDIO::CORE::STATUS &statusCode);
    void sendAudioSamples(QVector<float> &samples);

private:

    SwrContext      *m_swr_ctx{nullptr};
    int             m_sampleRate;
    int             m_numChannels;
    bool            m_isPlaying;
    float           m_volumeValue{DEFAULT_VOLUME};

    PaStream        *stream{nullptr};
};

#endif // AUDIOCORE_H
