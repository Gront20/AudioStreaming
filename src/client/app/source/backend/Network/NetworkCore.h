#ifndef NETWORKCORE_H
#define NETWORKCORE_H

#include <QUdpSocket>
#include <QDebug>
#include <QThread>
#include <QVariant>
#include <QMutex>
#include <opus.h>
#include "AudioCore.h"
#include "defines.h"

class NetworkCore : public QObject {
    Q_OBJECT
public:
    explicit NetworkCore(QObject* parent = nullptr, quint32 sampleRate = DEFAULT_SAMPLERATE,
                         quint8 channels = DEFAULT_CHANNELS);
    ~NetworkCore();

    void setDestination(const QHostAddress& address, const quint16 &port);
    bool initOpus(int sampleRate, int channels, int bitrate);
    void startListening();
    void closeConnection();

private slots:
    void processPendingDatagrams();

signals:

    void sendAudioData(const QVector<float> &decodedSamples, const int &frameSize);
    void sendSocketStatus(const QVariant data);

private:
    QUdpSocket      *m_udpSocket;
    QThread         m_networkThread;
    OpusDecoder     *m_opusDecoder;
    QByteArray      m_receivedBuffer;
    QByteArray      m_partialFrameBuffer;
    QMutex          m_mutex;
    QHostAddress    m_clientAddress;
    quint16         m_clientPort;
    quint32         m_sampleRate;
    quint8          m_numChannels{2};
    quint16         m_frameSize{960};
    uint32_t        m_ssrc{123456};
};


#endif // NETWORKCORE_H
