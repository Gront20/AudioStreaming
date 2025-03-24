#ifndef AUDIOCORE_H
#define AUDIOCORE_H

#include <QThread>
#include <QObject>
#include <QMutex>
#include <QDebug>
#include <QFileInfo>
#include <QString>
#include "defines.h"

extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswresample/swresample.h>
    #include <portaudio.h>
}

class AudioCore : public QObject {

    Q_OBJECT

public:
    explicit AudioCore(QObject *parent = nullptr);
    ~AudioCore();

    bool loadFile(const std::string& filename);
    void changeFile(const std::string& newFilePath);
    void setVolumeValue(const float &value);
    void playAudio(QVector<float>& samples);
    void setPlaybackPosition(float position);

    void play();
    void stop();
    void restart();

signals:

    void sendAudioSamples(const QVector<float> &samples);
    void sendCurrentStateError(const AUDIO::CORE::ERROR_HANDLER &errorCode);
    void sendAudioStatus(const QString &fileName, const AUDIO::CORE::STATUS &status);
    void playbackPositionChanged(float position);

private:

    static int paCallback(const void *inputBuffer, void *outputBuffer,
                          unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo,
                          PaStreamCallbackFlags statusFlags, void *userData);

    void cleanup();
    void cleanupStream();

private:

    std::vector<float>      m_audioBuffer;
    size_t                  m_bufferIndex{0};
    quint8                  m_numChannels{DEFAULT_CHANNELS};

    QString                 m_fileName{};

    float                   m_volumeValue{DEFAULT_VOLUME / 100.};

    QMutex                  m_mtx;
    std::condition_variable m_cv;

    std::atomic<bool>       m_isPlaying{false};
    std::atomic<bool>       m_isPaused{false};
    std::atomic<bool>       m_fileChanged{false};
    bool                    m_isFinished = false;

    PaStream                *m_stream = nullptr;
    AVFormatContext         *m_fmt_ctx = nullptr;
    AVCodecContext          *m_codec_ctx = nullptr;
    AVPacket                *m_packet = nullptr;
    AVFrame                 *m_frame = nullptr;
    SwrContext              *m_swr_ctx = nullptr;

};

#endif // AUDIOCORE_H
