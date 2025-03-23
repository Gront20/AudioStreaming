#include "networkhandler.h"

NetworkHandler::NetworkHandler(QObject *parent) : QObject(parent)
{
    connect(&m_networkCoreObject, &NetworkCore::sendSocketStatus, this, &NetworkHandler::handleNetworkCoreStatusData);
    m_networkCoreObject.initOpus(DEFAULT_SAMPLERATE, DEFAULT_CHANNELS, DEFAULT_BITRATE);
}

void NetworkHandler::handleNetworkConnectionOpen(const QHostAddress &ip, const quint16 port)
{
    m_networkCoreObject.setDestination(ip, port, true);
}

void NetworkHandler::handleNetworkConnectionClose()
{
    m_networkCoreObject.closeConnection();
}

void NetworkHandler::sendAudioSamples(const QVector<float> &samples)
{
    m_networkCoreObject.sendNextRtpPacket(samples);
}

void NetworkHandler::setPacketSize(const quint16 packetSize)
{
    m_networkCoreObject.setPacketSize(packetSize);
}

void NetworkHandler::handleNetworkCoreStatusData(const QVariant data)
{
    QString message{""};
    if (data.canConvert<QVariantMap>()) {
        QVariantMap dataMap = data.toMap();

        int status = dataMap["status"].toInt();
        quint32 packetSize = dataMap["packetSize"].toInt();
        emit sendNetworkDataSended(packetSize);
    }
    if (data.canConvert<int>()){
        int statusCode = data.toInt();
        message = NETWORK::CORE::networkCoreStatusToString(static_cast<NETWORK::CORE::STATUS>(statusCode));
        emit sendMessageToAppLogger(message);
    }
}

void NetworkHandler::handleAudioStatusUpdate(const AUDIO::CORE::STATUS &status)
{
    if (status == AUDIO::CORE::STATUS::START || status == AUDIO::CORE::STATUS::RESUME || status == AUDIO::CORE::STATUS::RESTART) m_networkCoreObject.startStreaming();
    if (status == AUDIO::CORE::STATUS::STOP) m_networkCoreObject.stopStreaming();
}

// void NetworkHandler::

NetworkHandler::~NetworkHandler()
{

}
