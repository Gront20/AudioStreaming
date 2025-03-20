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

    void handleAudioStatusUpdate(const AUDIO::CORE::STATUS &status);
    void handleNetworkConnection(const QHostAddress &ip, const quint16 &port);

signals:

    void sendAudioDataToAudio(const QVector<float> &decodedSamples, const int &frameSize);

private slots:

    void sendAudioData(const QVector<float> &decodedSamples, const int &frameSize);

private:

    NetworkCore *m_networkCoreObject;

};

#endif // NETWORKHANDLER_H
