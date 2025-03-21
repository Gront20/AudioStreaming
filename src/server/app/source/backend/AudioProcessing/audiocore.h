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
    #include <libavutil/opt.h>
    #include <portaudio.h>
}

#define SAMPLE_RATE 48000
#define CHANNELS 2
#define FRAMES_PER_BUFFER 960

class AudioCore : public QObject {

    Q_OBJECT

public:
    explicit AudioCore(QObject *parent = nullptr);
    ~AudioCore();

    bool loadFile(const std::string& filename);
    void changeFile(const std::string& newFilePath);\

    void play();
    void stop();
    void restart();

signals:

    void sendAudioSamples(const QVector<float> &samples);
    void sendCurrentStateError(const AUDIO::CORE::ERROR_HANDLER &errorCode);
    void sendAudioStatus(const QString &fileName, const AUDIO::CORE::STATUS &status);

private:

    static int paCallback(const void *inputBuffer, void *outputBuffer,
                          unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo,
                          PaStreamCallbackFlags statusFlags, void *userData);

    void cleanup();

    std::vector<float> audioBuffer;
    size_t bufferIndex = 0;

    QString m_fileName{};

    std::atomic<bool> isPlaying{false};
    std::atomic<bool> isPaused{false};
    std::atomic<bool> fileChanged{false};
    bool isFinished = false;

    QMutex mtx;
    std::condition_variable cv;

    PaStream *stream = nullptr;

    AVFormatContext *fmt_ctx = nullptr;
    AVCodecContext *codec_ctx = nullptr;
    AVPacket *packet = nullptr;
    AVFrame *frame = nullptr;
    SwrContext *swr_ctx = nullptr;

};


// #include <Windows.h>
// #include <Audioclient.h>
// #include <Audiopolicy.h>
// #include <mmdeviceapi.h>

//class AudioCore : public QObject
//{
//    Q_OBJECT
//public:
//    explicit AudioCore(QObject *parent = nullptr);
//    ~AudioCore();

//public:

//    void initialize();

//    // main mediaplayer functions
//    void start();
//    void stop();
//    void restart();

//    bool setFilePath(const char *filepath);

//signals:

//    void sendCurrentStateError(AUDIO::CORE::ERROR_HANDLER error, HRESULT hr);
//    void sendAudioStatus(AUDIO::CORE::STATUS status);

//private:

//    const char      *m_filepath{};
//    FILE            *m_file{nullptr};
//    BYTE            m_audioData[1024];
//    size_t          m_bytesRead;
//    UINT32          m_bufferFrameCount{50};
//    WAVEFORMATEX    *m_pwfx;
//    UINT32          m_cbWfx{sizeof(WAVEFORMATEX)};
//    HRESULT         m_hr;
//    HANDLE          m_hEvent;
//    UINT32          m_lastReadPosition{0};

//    QThread m_audioThread;

//private:

//    IMMDeviceEnumerator    *m_Enumerator{nullptr};
//    IMMDevice              *m_DefaultDevice{nullptr};
//    IAudioClient           *m_AudioClient{nullptr};
//    IAudioRenderClient     *m_RenderClient{nullptr};
//    WAVEFORMATEX            m_AudioFormat;

//};

#endif // AUDIOCORE_H
