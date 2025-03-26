#include "networkhandler.h"

NetworkHandler::NetworkHandler(QObject *parent) : QObject(parent)
{
    connect(&m_networkCoreObject, &NetworkCore::sendSocketStatus, this, &NetworkHandler::handleNetworkCoreStatusData);
    connect(&m_networkCoreObject, &NetworkCore::sendAudioData, this, &NetworkHandler::sendAudioData);
    m_networkCoreObject.initOpus(DEFAULT_SAMPLERATE, DEFAULT_CHANNELS, DEFAULT_BITRATE);
}

void NetworkHandler::handleNetworkConnectionOpen(const QHostAddress &ip, const quint16 port)
{
    m_networkCoreObject.setDestination(ip, port);
}

void NetworkHandler::setNetworkMode(const NETWORK::CORE::MODE mode)
{
    m_networkCoreObject.setMode(mode);
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

        NETWORK::CORE::MODE mode = static_cast<NETWORK::CORE::MODE>(dataMap["mode"].toInt());
        quint32 packetSize = dataMap["packetSize"].toInt();
        emit sendNetworkDataSended(packetSize);
    }
    if (data.canConvert<int>()){
        NETWORK::CORE::STATUS statusCode = static_cast<NETWORK::CORE::STATUS>(data.toInt());
        if (statusCode == NETWORK::CORE::STATUS::FAILED_BIND || statusCode == NETWORK::CORE::STATUS::CLOSED){
            emit sendSocketStatus(false);
        }
        else if(statusCode == NETWORK::CORE::STATUS::BOUNDED){
            emit sendSocketStatus(true);
        }

        message = NETWORK::CORE::networkCoreStatusToString(statusCode);
        emit sendMessageToAppLogger(message);
    }
}

void NetworkHandler::sendAudioData(QVector<float> decodedSamples)
{
    emit sendAudioDataToAudio(decodedSamples);
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
