#include "NetworkCore.h"

NetworkCore::NetworkCore(QObject *parent)
    : QObject(parent), m_opusEncoder(nullptr), m_isMulticast(false), m_packetSize(4000)
{

    m_udpSocket = new QUdpSocket(this);
    m_udpSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

    connect(m_udpSocket, &QAbstractSocket::stateChanged, [this](QAbstractSocket::SocketState state) {
        emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::STATE_CHANGED)));
    });
    connect(m_udpSocket, &QAbstractSocket::errorOccurred, [this](QAbstractSocket::SocketError error) {
        emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::ERROR_SOCKET)));;
    });
}

NetworkCore::~NetworkCore()
{
    if (m_opusEncoder) {
        opus_encoder_destroy(m_opusEncoder);
    }
}

bool NetworkCore::initOpus(int sampleRate, int channels, int bitrate)
{
    int error;
    m_opusEncoder = opus_encoder_create(sampleRate, channels, OPUS_APPLICATION_AUDIO, &error);
    if (error != OPUS_OK) {
        emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::ERROR_OPUS_INIT)));
        return false;
    }

    opus_encoder_ctl(m_opusEncoder, OPUS_SET_BITRATE(bitrate));
    opus_encoder_ctl(m_opusEncoder, OPUS_SET_COMPLEXITY(10));
    return true;
}

void NetworkCore::closeConnection()
{
    if (QThread::currentThread() != m_udpSocket->thread()) {
        QMetaObject::invokeMethod(this, &NetworkCore::closeConnection, Qt::QueuedConnection);
        return;
    }

    QMutexLocker locker(&m_mutex);
    m_udpSocket->close();
    m_udpSocket->abort();

    m_clientAddress = QHostAddress::Null;
    m_clientPort = 0;
    m_isMulticast = false;

    emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::CLOSED)));
}

void NetworkCore::setDestination(const QHostAddress &address, quint16 port)
{
    // if (QThread::currentThread() != m_udpSocket->thread()) {
    //     QMetaObject::invokeMethod(m_udpSocket, [this, address, port]() {
    //         setDestination(address, port);
    //     }, Qt::QueuedConnection);
    //     return;
    // }

    QMutexLocker locker(&m_mutex);
    m_clientAddress = address;
    m_clientPort = port;
    m_isMulticast = true;

    if (m_udpSocket->localAddress() == address && m_udpSocket->localPort() == port) {
        emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::ALREADY_BOUNDED)));
        return;
    }

    m_udpSocket->abort();

    if (!m_udpSocket->bind(m_clientAddress, m_clientPort)) {
        emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::FAILED_BIND)));
        return;
    } else {
        emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::BOUNDED)));
    }
}

void NetworkCore::setPacketSize(int size)
{
    m_packetSize = qBound(200, size, 4000);

    if (m_packetSize < 4000) {
        if (m_packetSize >= 120 * m_numChannels * sizeof(float)) m_frameSize = 120; // 2.5 мс
        if (m_packetSize >= 240 * m_numChannels * sizeof(float)) m_frameSize = 240; // 5 мс
        if (m_packetSize >= 480 * m_numChannels * sizeof(float)) m_frameSize = 480; // 10 мс
        if (m_packetSize >= 960 * m_numChannels * sizeof(float)) m_frameSize = 960; // 20 мс
        if (m_packetSize >= 1920 * m_numChannels * sizeof(float)) m_frameSize = 1920; // 40 мс
        if (m_packetSize >= 2880 * m_numChannels * sizeof(float)) m_frameSize = 2880; // 60 мс
    }
}

void NetworkCore::startStreaming()
{
    // QMutexLocker locker(&m_mutex);
    m_isStreamingFlag = true;
}

void NetworkCore::stopStreaming()
{
    // QMutexLocker locker(&m_mutex);
    m_isStreamingFlag = false;
}

void NetworkCore::sendNextRtpPacket(const QVector<float>& samples) {
    if (!m_isStreamingFlag || !m_isMulticast) return;

    m_samplesBuffer.append(samples);

    while (m_samplesBuffer.size() >= m_frameSize * m_numChannels) {
        QVector<float> packetSamples = m_samplesBuffer.mid(0, m_frameSize * m_numChannels);
        m_samplesBuffer.remove(0, m_frameSize * m_numChannels);

        QByteArray opusData(m_packetSize - 12, 0);
        int encodedBytes = opus_encode_float(m_opusEncoder, packetSamples.data(), m_frameSize,
                                             reinterpret_cast<unsigned char*>(opusData.data()), opusData.size());

        if (encodedBytes <= 0) {
            emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::ERROR_OPUS_ENCODE)));
            continue;
        }

        QByteArray rtpPacket(12 + encodedBytes, 0);
        uint8_t* rtpHeader = reinterpret_cast<uint8_t*>(rtpPacket.data());

        rtpHeader[0] = 0x80;
        rtpHeader[1] = 0xE0;
        rtpHeader[2] = (m_sequenceNumber >> 8) & 0xFF;
        rtpHeader[3] = m_sequenceNumber & 0xFF;
        rtpHeader[4] = (m_timestamp >> 24) & 0xFF;
        rtpHeader[5] = (m_timestamp >> 16) & 0xFF;
        rtpHeader[6] = (m_timestamp >> 8) & 0xFF;
        rtpHeader[7] = m_timestamp & 0xFF;
        rtpHeader[8] = (m_ssrc >> 24) & 0xFF;
        rtpHeader[9] = (m_ssrc >> 16) & 0xFF;
        rtpHeader[10] = (m_ssrc >> 8) & 0xFF;
        rtpHeader[11] = m_ssrc & 0xFF;

        memcpy(rtpPacket.data() + 12, opusData.data(), encodedBytes);

        m_udpSocket->writeDatagram(rtpPacket, m_clientAddress, m_clientPort);

        QVariantMap data;
        data["status"] = static_cast<int>(NETWORK::CORE::STATUS::SEND_PACKET);
        data["packetSize"] = rtpPacket.size();
        emit sendSocketStatus(data);

        m_sequenceNumber++;
        m_timestamp += m_frameSize;
    }
}
