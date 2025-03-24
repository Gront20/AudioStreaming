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
    bool initOpus(int sampleRate = DEFAULT_SAMPLERATE, int channels = DEFAULT_CHANNELS, int bitrate = DEFAULT_BITRATE);
    void setPacketSize(const int size);

public slots:

    void startStreaming();
    void stopStreaming();
    // void restartStreaming();
    void sendNextRtpPacket(const QVector<float> &samples);
    void processPendingDatagrams();
    void setMode(const NETWORK::CORE::MODE mode);

signals:

    void sendSocketStatus(const QVariant data);
    void sendAudioData(QVector<float> data);

private:

    NETWORK::CORE::MODE m_currentMode{NETWORK::CORE::MODE::UNDEFINED};

private:
    QUdpSocket          *m_udpSocket;
    QHostAddress        m_clientAddress{DEFAULT_IP};
    quint16             m_clientPort{DEFAULT_PORT};
    quint16             m_packetSize{DEFAULT_PACKETSIZE};
    std::vector<float>  m_samplesData;

    QVector<float>      m_audioBuffer;
    int                 m_bufferIndex{0};
    bool                m_isStreamingFlag{false};
    bool                m_isPlayingFlag{false};
    QVector<float>      m_samplesBuffer;
    QByteArray          m_receivedBuffer; ///< Буффер получаемых данных
    QByteArray          m_partialFrameBuffer; ///< Буффер данных до метки "незавершенный буффер"

    QMutex              m_mtx;

    OpusEncoder*        m_opusEncoder{nullptr};
    OpusDecoder*        m_opusDecoder{nullptr};
    quint16             m_sequenceNumber{0};
    QByteArray          m_encodedBuffer; ///< Закодированный буффер
    quint32             m_timestamp{0};
    quint32             m_ssrc{DEFAULT_ID};  // Уникальный идентификатор источника
    quint16             m_frameSize{DEFAULT_FRAMES_PER_BUFFER}; // размер фрейма
    quint32             m_sampleRate{DEFAULT_SAMPLERATE}; // частота семплирования
    quint8              m_numChannels{DEFAULT_CHANNELS}; // СТерео пока что
};

#endif // NETWORKCORE_H
