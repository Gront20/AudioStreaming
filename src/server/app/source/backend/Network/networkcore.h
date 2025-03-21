#ifndef NETWORKCORE_H
#define NETWORKCORE_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QDebug>
#include <QThread>
#include <QVariant>
#include <QVector>
#include <QMutex>
#include <opus.h>
#include "defines.h"

class NetworkCore : public QObject {
    Q_OBJECT

public:
    explicit NetworkCore(QObject* parent = nullptr);
    ~NetworkCore();

    void setDestination(const QHostAddress& address, quint16 port);
    void closeConnection();
    bool initOpus(int sampleRate = 48000, int channels = 2, int bitrate = 64000);
    void setPacketSize(int size);

public slots:

    void startStreaming();
    void stopStreaming();
    // void restartStreaming();
    void sendNextRtpPacket(const QVector<float> &samples);

signals:

    void sendSocketStatus(const QVariant data);

private:
    QUdpSocket          *m_udpSocket;
    QHostAddress        m_clientAddress;
    quint16             m_clientPort{0};
    quint16             m_packetSize{200};
    std::vector<float>  m_samplesData;

    QVector<float>      m_audioBuffer;
    int                 m_bufferIndex{0};
    bool                m_isStreamingFlag{false};
    bool                m_isMulticast{false};
    QMutex              m_mutex;
    QVector<float>      m_samplesBuffer;


    OpusEncoder*        m_opusEncoder{nullptr};
    int                 m_opusFrameSize{960};  // 20 мс для 48 кГц
    quint16             m_sequenceNumber{0};
    quint32             m_timestamp{0};
    quint32             m_ssrc{123456};  // Уникальный идентификатор источника
    quint16             m_frameSize{960}; // размер фрейма
    quint8              m_numChannels{2}; // СТерео пока что
};

#endif // NETWORKCORE_H
