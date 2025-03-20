#ifndef NETWORKCORE_H
#define NETWORKCORE_H

#include <QUdpSocket>
#include <QDebug>
#include <QThread>
#include <QMutex>
#include <opus.h>
#include "AudioCore.h"

class NetworkCore : public QObject {
    Q_OBJECT
public:
    explicit NetworkCore(QObject* parent = nullptr);
    ~NetworkCore();

    void setDestination(const QHostAddress& address, const quint16 &port);
    bool initOpus(int sampleRate, int channels);
    void startListening();

private slots:
    void processPendingDatagrams();
    void handleSocketError(QAbstractSocket::SocketError error);

signals:

    void sendAudioData(const QVector<float> &decodedSamples, const int &frameSize);

private:
    QUdpSocket      *m_udpSocket;
    QThread         m_networkThread;
    OpusDecoder     *m_opusDecoder;
    QMutex          m_mutex;
    QHostAddress    m_listenAddress;
    quint16         m_listenPort;
    int             m_sampleRate;
    int             m_channels;
};


#endif // NETWORKCORE_H
