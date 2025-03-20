#include "networkhandler.h"

NetworkHandler::NetworkHandler(QObject *parent) : QObject(parent)
{
    m_networkCoreObject.setDestination(IP_DEFAULT, PORT_DEFAULT);
}


void NetworkHandler::sendAudioSamples(const std::vector<float> &samples)
{
    m_networkCoreObject.stopStreaming();
    m_networkCoreObject.setAudioBuffer(samples);
    m_networkCoreObject.restartStreaming();
}

void NetworkHandler::handleAudioStatusUpdate(const AUDIO::CORE::STATUS &status)
{
    if (status == AUDIO::CORE::STATUS::START) m_networkCoreObject.startStreaming();
    if (status == AUDIO::CORE::STATUS::STOP) m_networkCoreObject.stopStreaming();
    if (status == AUDIO::CORE::STATUS::RESTART) m_networkCoreObject.restartStreaming();
}

// void NetworkHandler::

NetworkHandler::~NetworkHandler()
{

}
