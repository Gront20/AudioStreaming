#ifndef NETWORKCORE_H
#define NETWORKCORE_H

#include <QObject>
#include <QUdpSocket>
#include <QVector>
#include <QMutex>

class NetworkCore : public QObject
{
    Q_OBJECT

public:
    explicit NetworkCore(QObject *parent = nullptr);
    ~NetworkCore() = default;
    void setDestination(const QHostAddress &address, quint16 port);

    void setAudioBuffer(const std::vector<float> &samples);

public slots:
    void startStreaming();
    void stopStreaming();
    void restartStreaming();
    void sendNextChunk();

private:
    QUdpSocket              m_udpSocket;
    QHostAddress            m_clientAddress;
    quint16                 m_clientPort{0};
    QMutex                  m_mutex;

    std::vector<float>      m_audioBuffer;
    size_t                  m_bufferIndex{0};
    bool                    m_isStreamingFlag{false};
};

#endif // NETWORKCORE_H
