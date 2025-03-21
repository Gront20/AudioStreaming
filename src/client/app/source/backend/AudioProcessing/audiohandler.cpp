#include "audiohandler.h"

AudioHandler::AudioHandler(QObject *parent) : QObject(parent)
{
   // connect(&m_audioCoreObject, &AudioCore::sendAudioSamples, this, &AudioHandler::sendAudioSamples);
   // connect(&m_audioCoreObject, &AudioCore::sendCurrentStateError, this, &AudioHandler::errorCatched);
   // connect(&m_audioCoreObject, &AudioCore::sendAudioStatus, this, &AudioHandler::audioStatusCatched);
   m_audioCoreObject.init(48000, 2);
}

AudioHandler::~AudioHandler()
{

}

void AudioHandler::recieveAudioData(const QVector<float> &decodedSamples, const int &frameSize)
{
    m_audioCoreObject.playAudio(decodedSamples, frameSize);
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

    // to GUI logic
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

void AudioHandler::sendAudioSamples(const QVector<float> &samples)
{
    emit sendAudioSamplesFromCore(samples, 0);
}

void AudioHandler::changeAudioStatus(AUDIO::HANDLER::STATUS status)
{
    this->m_status = status;
}

void AudioHandler::startAudio()
{
    m_audioCoreObject.start();
}

void AudioHandler::stopAudio()
{
    m_audioCoreObject.stop();
}

void AudioHandler::restartAudio()
{
    // if(m_status == AUDIO::HANDLER::STATUS::ISREADY){
    //     m_audioCoreObject.restart();
    // }
}

void AudioHandler::checkFileTimerConnection()
{
    // QTimer *timer = new QTimer(this);
    // connect(timer, &QTimer::timeout, this, &AudioHandler::checkFilePresence);
    // timer->start(200);
}

void AudioHandler::checkFilePresence()
{
    if (!QFile::exists(m_filePath)) {
//        m_audioCoreObject.stop();
        // emit sendAudioStatus();
    }
}
