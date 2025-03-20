#include "networkhandler.h"

NetworkHandler::NetworkHandler(QObject *parent) : QObject(parent)
{
    m_networkCoreObject.initOpus(48000, 2, 128000);
    // m_networkCoreObject.setDestination(IP_DEFAULT, PORT_DEFAULT, false);
}

void NetworkHandler::handleNetworkConnection(const QHostAddress &ip, const quint16 port)
{
    m_networkCoreObject.setDestination(ip, port, true);
}

void NetworkHandler::sendAudioSamples(const std::vector<float> &samples)
{
    m_networkCoreObject.sendNextRtpPacket(samples);
}

void NetworkHandler::handleAudioStatusUpdate(const AUDIO::CORE::STATUS &status)
{
    if (status == AUDIO::CORE::STATUS::START) m_networkCoreObject.startStreaming();
    if (status == AUDIO::CORE::STATUS::STOP) m_networkCoreObject.stopStreaming();
    // if (status == AUDIO::CORE::STATUS::RESTART) m_networkCoreObject.restartStreaming();
}

// void NetworkHandler::

NetworkHandler::~NetworkHandler()
{

}
