#include "audiohandler.h"

AudioHandler::AudioHandler(QObject *parent) : QObject(parent)
{
   connect(&m_audioCoreObject, &AudioCore::sendAudioSamples, this, &AudioHandler::sendAudioSamples);
   connect(&m_audioCoreObject, &AudioCore::sendCurrentStateError, this, &AudioHandler::errorCatched);
   connect(&m_audioCoreObject, &AudioCore::sendAudioStatus, this, &AudioHandler::audioStatusCatched);
   connect(&m_audioCoreObject, &AudioCore::playbackPositionChanged, this, &AudioHandler::playbackPositionChangedSLOT);
}

AudioHandler::~AudioHandler()
{

}

void AudioHandler::errorCatched(const AUDIO::CORE::ERROR_HANDLER &errorCode)
{
    QString message = QString("%1").arg(AUDIO::CORE::errorCodeToString(errorCode));
    emit sendErrorMessage(message);
}

void AudioHandler::audioStatusCatched(const QString &fileName, const AUDIO::CORE::STATUS &status)
{
    // to logger logic
    QString message = audioStatusToString(status, fileName);
    emit sendAudioStatusMessage(message);

    // to GUI and Network logic
    emit handleAudioStatusUpdate(status);
}

void AudioHandler::audioPlayerChangeState(AUDIO::HANDLER::MODE mode)
{
    switch (mode) {
    case AUDIO::HANDLER::MODE::START:
        startAudio();
        break;
    case AUDIO::HANDLER::MODE::STOP:
        stopAudio();
        break;
    case AUDIO::HANDLER::MODE::RESTART:
        restartAudio();
        break;
    case AUDIO::HANDLER::MODE::UNDEFINED:
    default:
        break;
    }
}

void AudioHandler::receiveAudioFilePath(const QString& path)
{
    m_filePath = path;
    m_audioCoreObject.changeFile(path.toUtf8().constData());
}

void AudioHandler::setVolumeValue(const float &value)
{
    m_audioCoreObject.setVolumeValue(value);
}

void AudioHandler::playbackPositionChangedSLOT(float value)
{
    emit playbackPositionChanged(value);
}

void AudioHandler::sendAudioSamples(const QVector<float> &samples)
{
    emit sendAudioSamplesFromCore(samples);
}

void AudioHandler::recieveAudioData(QVector<float> decodedSamples)
{
    m_audioCoreObject.playAudio(decodedSamples);
}

void AudioHandler::setPlaybackPosition(float pos)
{
    m_audioCoreObject.setPlaybackPosition(pos);
}

void AudioHandler::startAudio()
{
    m_audioCoreObject.play();
}

void AudioHandler::stopAudio()
{
    m_audioCoreObject.stop();
}

void AudioHandler::restartAudio()
{
    m_audioCoreObject.restart();
}
