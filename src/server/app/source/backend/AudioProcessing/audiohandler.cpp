#include "audiohandler.h"

AudioHandler::AudioHandler(QObject *parent) : QObject(parent)
{
   connect(&m_audioCoreObject, &AudioCore::sendAudioSamples, this, &AudioHandler::sendAudioSamples);
   connect(&m_audioCoreObject, &AudioCore::sendCurrentStateError, this, &AudioHandler::errorCatched);
   connect(&m_audioCoreObject, &AudioCore::sendAudioStatus, this, &AudioHandler::audioStatusCatched);
//    m_audioCoreObject.initialize();
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
    checkFileTimerConnection();
    m_audioCoreObject.changeFile(path.toUtf8().constData());
//    if (result)
    changeAudioStatus(AUDIO::HANDLER::STATUS::ISREADY);
}

void AudioHandler::sendAudioSamples(const std::vector<float> &samples)
{
    emit sendAudioSamplesFromCore(samples);
}

void AudioHandler::changeAudioStatus(AUDIO::HANDLER::STATUS status)
{
    this->m_status = status;
}

void AudioHandler::startAudio()
{
    if(m_status == AUDIO::HANDLER::STATUS::ISREADY){
        m_audioCoreObject.play();
    }
}

void AudioHandler::stopAudio()
{
    if(m_status == AUDIO::HANDLER::STATUS::ISREADY){
        m_audioCoreObject.stop();
    }
}

void AudioHandler::restartAudio()
{
    if(m_status == AUDIO::HANDLER::STATUS::ISREADY){
        m_audioCoreObject.restart();
    }
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
