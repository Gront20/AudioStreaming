#ifndef AUDIOCORE_H
#define AUDIOCORE_H

#include <QObject>
#include <QVector>

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
