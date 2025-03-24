#ifndef NETWORKHANDLER_H
#define NETWORKHANDLER_H

#include <QObject>
#include "networkcore.h"
#include "defines.h"

class NetworkHandler : public QObject
{
    Q_OBJECT
public:
    explicit NetworkHandler(QObject *parent);
    ~NetworkHandler();

public slots:

    void handleNetworkConnectionOpen(const QHostAddress &ip, const quint16 &port);
    void handleNetworkConnectionClose();

signals:

    void sendAudioDataToAudio(QVector<float> &decodedSamples);
    void sendMessageToAppLogger(const QString message);
    void sendNetworkDataSended(const quint32 data);

private slots:

    void sendAudioData(QVector<float> &decodedSamples);
    void handleNetworkCoreStatusData(const QVariant data);


private:

    NetworkCore *m_networkCoreObject;

};

#endif // NETWORKHANDLER_H
