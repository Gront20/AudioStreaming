#ifndef NETWORKHANDLER_H
#define NETWORKHANDLER_H

#include <QObject>
#include "networkcore.h"
#include "defines.h"

class NetworkHandler : public QObject
{
    Q_OBJECT
public:
    explicit NetworkHandler(QObject *parent = nullptr);
    ~NetworkHandler();

private slots:

    void handleNetworkCoreStatusData(const QVariant data);

signals:

    void sendMessageToAppLogger(const QString message);
    void sendNetworkDataSended(const quint32 data);

public slots:

    void sendAudioSamples(const QVector<float> &samples);
    void handleAudioStatusUpdate(const AUDIO::CORE::STATUS &status);
    void handleNetworkConnectionOpen(const QHostAddress &ip, const quint16 port);
    void handleNetworkConnectionClose();

    void setPacketSize(const quint16 packetSize);

private:

    NetworkCore m_networkCoreObject;

};

#endif // NETWORKHANDLER_H
