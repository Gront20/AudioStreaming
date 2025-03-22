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

    void sendAudioSamples(const QVector<float> &samples);
    void recieveAudioData(const QVector<float> &decodedSamples, const int &frameSize);
    void audioPlayerChangeState(AUDIO::HANDLER::MODE mode);

    void startAudio();
    void stopAudio();

private slots:

    void errorCatched(const AUDIO::CORE::ERROR_HANDLER &errorCode);
    void audioStatusCatched(const AUDIO::CORE::STATUS &status);

signals:

    void sendAudioSamplesFromCore(const QVector<float> &samples, int frameSize);
    void sendErrorMessage(const QString &message);
    void sendAudioStatusMessage(const QString &message);
    void handleAudioStatusUpdate(const AUDIO::CORE::STATUS &status);

private:

    AudioCore                   m_audioCoreObject;

};

#endif // AUDIOHANDLER_H
