#include "networkhandler.h"

NetworkHandler::NetworkHandler(QObject *parent)
    : QObject(parent) {

    m_networkCoreObject = new NetworkCore(nullptr, DEFAULT_SAMPLERATE, DEFAULT_CHANNELS);

    connect(m_networkCoreObject, &NetworkCore::sendAudioData, this, &NetworkHandler::sendAudioData);
    connect(m_networkCoreObject, &NetworkCore::sendSocketStatus, this, &NetworkHandler::handleNetworkCoreStatusData);

    m_networkCoreObject->initOpus(DEFAULT_SAMPLERATE, DEFAULT_CHANNELS, DEFAULT_BITRATE);
}

void NetworkHandler::sendAudioData(QVector<float> &decodedSamples)
{
    emit sendAudioDataToAudio(decodedSamples);
}

void NetworkHandler::handleNetworkCoreStatusData(const QVariant data)
{
    QString message{""};
    if (data.canConvert<QVariantMap>()) {
        QVariantMap dataMap = data.toMap();

        int status = dataMap["status"].toInt();
        quint32 packetSize = dataMap["packetSize"].toInt();

        emit sendNetworkDataSended(packetSize);
        // emit sendMessageToAppLogger(NETWORK::CORE::networkCoreStatusToString(static_cast<NETWORK::CORE::STATUS>(message))); // for debug
    }
    if (data.canConvert<int>()){
        int statusCode = data.toInt();
        message = NETWORK::CORE::networkCoreStatusToString(static_cast<NETWORK::CORE::STATUS>(statusCode));
        emit sendMessageToAppLogger(message);
    }
}


void NetworkHandler::handleNetworkConnectionOpen(const QHostAddress &ip, const quint16 &port)
{
    m_networkCoreObject->setDestination(ip, port);
}

void NetworkHandler::handleNetworkConnectionClose()
{
    m_networkCoreObject->closeConnection();
}

NetworkHandler::~NetworkHandler()
{
    delete m_networkCoreObject;
    m_networkCoreObject = nullptr;
}
