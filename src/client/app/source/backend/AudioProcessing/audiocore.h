#ifndef AUDIOCORE_H
#define AUDIOCORE_H

#include <QObject>
#include <QVector>
#include "defines.h"

extern "C"{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswresample/swresample.h>
    #include <portaudio.h>
}

class AudioCore : public QObject {
    Q_OBJECT
public:
    explicit AudioCore(QObject* parent = nullptr);
    ~AudioCore();

    bool init(int sampleRate, int channels);
    void playAudio(const QVector<float>& samples, int frameSize);
    void start();
    void stop();

signals:

    void sendCurrentStateError(const AUDIO::CORE::ERROR_HANDLER &errorCode);
    void sendAudioStatus(const AUDIO::CORE::STATUS &statusCode);
    void sendAudioSamples(const QVector<float> &samples);

private:
    AVFormatContext* m_formatContext;
    AVCodecContext* m_codecContext;
    SwrContext* m_swrContext;
    int m_sampleRate;
    int m_channels;
    bool m_isPlaying;

    PaStream *stream = nullptr;
};

#endif // AUDIOCORE_H
