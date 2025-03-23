#include "NetworkCore.h"

NetworkCore::NetworkCore(QObject *parent)
    : QObject(parent), m_opusEncoder(nullptr)
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
    m_opusDecoder = opus_decoder_create(sampleRate, channels, &error);
    if (error != OPUS_OK) {
        emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::ERROR_OPUS_INIT)));
        return false;
    }

    opus_encoder_ctl(m_opusEncoder, OPUS_SET_BITRATE(bitrate));
    opus_encoder_ctl(m_opusEncoder, OPUS_SET_COMPLEXITY(10));
    opus_encoder_ctl(m_opusEncoder, OPUS_SET_VBR(1)); // можео 0 поставить если нужен CBR опционально в общем
    return true;
}

void NetworkCore::closeConnection()
{
    if (QThread::currentThread() != m_udpSocket->thread()) {
        QMetaObject::invokeMethod(this, &NetworkCore::closeConnection, Qt::QueuedConnection);
        return;
    }

    m_udpSocket->close();
    m_udpSocket->abort();

    m_clientAddress = QHostAddress::Null;
    m_clientPort = 0;
    m_isMulticast = false;

    emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::CLOSED)));
}

void NetworkCore::setDestination(const QHostAddress &address, quint16 port, bool multicast)
{
    if (QThread::currentThread() != m_udpSocket->thread()) {
        QMetaObject::invokeMethod(m_udpSocket, [this, address, port, multicast]() {
            setDestination(address, port, multicast);
        }, Qt::QueuedConnection);
        return;
    }

    m_clientAddress = address;
    m_clientPort = port;
    m_isMulticast = true;

    if (m_udpSocket->localAddress() == address && m_udpSocket->localPort() == port) {
        emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::ALREADY_BOUNDED)));
        return;
    }

    m_udpSocket->abort();

    m_isMulticast = multicast;

    if (!m_udpSocket->bind(m_clientAddress, m_clientPort, multicast ? QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint
                             : QUdpSocket::DefaultForPlatform)) {
        emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::FAILED_BIND)));
        return;
    } else {
        emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::BOUNDED)));
    }
}

void NetworkCore::setPacketSize(const int size)
{
    m_packetSize = qBound(200, size, 1500);
}

void NetworkCore::startStreaming()
{
    m_isStreamingFlag = true;
}

void NetworkCore::stopStreaming()
{
    m_isStreamingFlag = false;
}

void NetworkCore::sendNextRtpPacket(const QVector<float>& samples) {

    if (!m_isStreamingFlag || !m_isMulticast) return;

    QByteArray opusData(m_packetSize - 12, 0);
    int encodedBytes = opus_encode_float(m_opusEncoder, samples.data(), m_frameSize,
                                         reinterpret_cast<unsigned char*>(opusData.data()), opusData.size());

    if (encodedBytes <= 0) {
        emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::ERROR_OPUS_ENCODE)));
        return;
    }

    m_encodedBuffer.append(opusData.mid(0, encodedBytes));
    m_encodedBuffer.append(QByteArray("\xDE\xAD\xBE\xEF", 4));

    int payloadSize = m_packetSize - 12 - 4;

    if(m_encodedBuffer.size() >= payloadSize){

        QByteArray rtpPacket(m_packetSize, 0);
        uint8_t* rtpHeader = reinterpret_cast<uint8_t*>(rtpPacket.data());

		// можно конечно добавить там в хидере rtp-пакета метки о неполном фрейме, но я решил вручную это сделать
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

        while (m_encodedBuffer.size() > 0) {

            int chunkSize = qMin(payloadSize, m_encodedBuffer.size());

            memcpy(rtpPacket.data() + 12, m_encodedBuffer.data(), chunkSize);

            if (m_encodedBuffer.size() <= payloadSize) {
                // memcpy(rtpPacket.data() + 12 + chunkSize, "\xDE\xAD\xBE\xEF", 4);
            } else {
                memcpy(rtpPacket.data() + 12 + chunkSize, "\xCA\xFE\xBA\xBE", 4);
            }

            m_encodedBuffer.remove(0, chunkSize);

            if (m_encodedBuffer.size() > 0) {
                break;
            }
        }

        m_udpSocket->writeDatagram(rtpPacket, m_clientAddress, m_clientPort);

        QVariantMap data;
        data["status"] = static_cast<int>(NETWORK::CORE::STATUS::SEND_PACKET);
        data["packetSize"] = rtpPacket.size();
        emit sendSocketStatus(data);

        m_sequenceNumber++;
        m_timestamp += m_frameSize;

    }

}
