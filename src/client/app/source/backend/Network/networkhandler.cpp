#include "networkhandler.h"

NetworkHandler::NetworkHandler(QObject *parent)
    : QObject(parent) {

    m_networkCoreObject = new NetworkCore();

    connect(m_networkCoreObject, &NetworkCore::sendAudioData, this, &NetworkHandler::sendAudioData);

    m_networkCoreObject->initOpus(48000, 2);
}

void NetworkHandler::sendAudioData(const QVector<float> &decodedSamples, const int &frameSize)
{
    emit sendAudioDataToAudio(decodedSamples, frameSize);
}

void NetworkHandler::handleAudioStatusUpdate(const AUDIO::CORE::STATUS &status)
{
    // if (status == AUDIO::CORE::STATUS::START) m_networkCoreObject.startStreaming();
    // if (status == AUDIO::CORE::STATUS::STOP) m_networkCoreObject.stopStreaming();
    // if (status == AUDIO::CORE::STATUS::RESTART) m_networkCoreObject.restartStreaming();
}

void NetworkHandler::handleNetworkConnection(const QHostAddress &ip, const quint16 &port)
{
    m_networkCoreObject->setDestination(ip, port);
    m_networkCoreObject->startListening();
}

NetworkHandler::~NetworkHandler()
{
    delete m_networkCoreObject;
    m_networkCoreObject = nullptr;
}
