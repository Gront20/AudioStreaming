#ifndef NETWORKCORE_H
#define NETWORKCORE_H

#include <QObject>
#include <QUdpSocket>
#include <QVector>
#include <QMutex>
#include <opus.h>

class NetworkCore : public QObject {
    Q_OBJECT

public:
    explicit NetworkCore(QObject* parent = nullptr);
    ~NetworkCore();

    void setDestination(const QHostAddress& address, quint16 port, bool isMulticast);
    bool initOpus(int sampleRate = 48000, int channels = 2, int bitrate = 64000);
    void setPacketSize(int size);
    void setSamplesData(const std::vector<float>& samples);

public slots:

    void startStreaming();
    void stopStreaming();
    // void restartStreaming();
    void sendNextRtpPacket(std::vector<float> samples);

private:
    QUdpSocket          m_udpSocket;
    QHostAddress        m_clientAddress;
    quint16             m_clientPort{0};
    quint16             m_packetSize{200};
    std::vector<float>  m_samplesData;

    QVector<float>  m_audioBuffer;
    int             m_bufferIndex{0};
    bool            m_isStreamingFlag{false};
    bool            m_isMulticast{false};
    QMutex          m_mutex;

    OpusEncoder*    m_opusEncoder{nullptr};
    int             m_opusFrameSize{960};  // 20 мс для 48 кГц
    quint16         m_sequenceNumber{0};
    quint32         m_timestamp{0};
    quint32         m_ssrc{123456};  // Уникальный идентификатор источника
};

#endif // NETWORKCORE_H
