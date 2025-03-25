#ifndef AUDIOHANDLER_H
#define AUDIOHANDLER_H

#include <QObject>
#include <QTimer>
#include <QFile>
#include <QDebug>

#include "defines.h"
#include "audiocore.h"

/**
 * @brief The AudioHandler class
    ОБРАБОТЧИК внешних связей с аудио модулем
 */

class AudioHandler : public QObject
{
    Q_OBJECT
public:
    explicit AudioHandler(QObject *parent = nullptr);
    ~AudioHandler();

public slots:

    void receiveAudioFilePath(const QString &path);
    void audioPlayerChangeState(AUDIO::HANDLER::MODE mode);
    void setVolumeValue(const float &value);
    void recieveAudioData(QVector<float> decodedSamples);
    void setPlaybackPosition(float pos);

    void startAudio();
    void stopAudio();
    void restartAudio();

private slots:

    void sendAudioSamples(const QVector<float> &samples);
    void errorCatched(const AUDIO::CORE::ERROR_HANDLER &errorCode);
    void audioStatusCatched(const QString &fileName, const AUDIO::CORE::STATUS &status);
    void playbackPositionChangedSLOT(float pos);

signals:

    void sendErrorMessage(const QString &message);
    void sendAudioStatusMessage(const QString &message);
    void handleAudioStatusUpdate(const AUDIO::CORE::STATUS &status);
    void sendAudioSamplesFromCore(const QVector<float> &samples);
    void playbackPositionChanged(float pos);

private:

    AudioCore m_audioCoreObject;

};

#endif // AUDIOHANDLER_H
