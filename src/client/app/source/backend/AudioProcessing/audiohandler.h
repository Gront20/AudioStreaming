#ifndef AUDIOHANDLER_H
#define AUDIOHANDLER_H

#include <QObject>
#include <QTimer>
#include <QFile>
#include <QDebug>

#include "defines.h"
#include "audiocore.h"

class AudioHandler : public QObject
{
    Q_OBJECT
public:
    explicit AudioHandler(QObject *parent = nullptr);
    ~AudioHandler();

public slots:

    void recieveAudioData(const QVector<float> &decodedSamples, const int &frameSize);
    void audioPlayerChangeState(AUDIO::HANDLER::MODE mode);
    void sendAudioSamples(const std::vector<float> &samples);

    void startAudio();
    void stopAudio();
    void restartAudio();

private slots:

    void errorCatched(const AUDIO::CORE::ERROR_HANDLER &errorCode);
    void audioStatusCatched(const QString &fileName, const AUDIO::CORE::STATUS &status);

signals:

    // void sendAudioStatus();
    void sendErrorMessage(const QString &message);
    void sendAudioStatusMessage(const QString &message);
    void handleAudioStatusUpdate(const AUDIO::CORE::STATUS &status);
    void sendAudioSamplesFromCore(const std::vector<float> &samples);

private:

    void checkFilePresence();
    void checkFileTimerConnection();
    void changeAudioStatus(AUDIO::HANDLER::STATUS status);

private:

    QString                     m_filePath{};
    AudioCore                   m_audioCoreObject;
    AUDIO::HANDLER::STATUS      m_status;

};

#endif // AUDIOHANDLER_H
