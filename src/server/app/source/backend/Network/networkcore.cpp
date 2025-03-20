#include "NetworkCore.h"
#include <QTimer>
#include <QDebug>

NetworkCore::NetworkCore(QObject* parent)
    : QObject(parent), m_opusEncoder(nullptr), m_isMulticast(false), m_packetSize(4000) {}

NetworkCore::~NetworkCore() {
    if (m_opusEncoder) {
        opus_encoder_destroy(m_opusEncoder);
    }
}

void NetworkCore::setSamplesData(const std::vector<float>& samples)
{
    this->m_samplesData = samples;
}

bool NetworkCore::initOpus(int sampleRate, int channels, int bitrate) {
    int error;
    m_opusEncoder = opus_encoder_create(sampleRate, channels, OPUS_APPLICATION_AUDIO, &error);
    if (error != OPUS_OK) {
        qWarning() << "Opus encoder init failed:" << opus_strerror(error);
        return false;
    }

    opus_encoder_ctl(m_opusEncoder, OPUS_SET_BITRATE(bitrate));
    opus_encoder_ctl(m_opusEncoder, OPUS_SET_COMPLEXITY(10));
    return true;
}

void NetworkCore::setDestination(const QHostAddress& address, quint16 port, bool isMulticast) {
    QMutexLocker locker(&m_mutex);
    m_clientAddress = address;
    m_clientPort = port;
    m_isMulticast = isMulticast;
}

void NetworkCore::setPacketSize(int size) {
    QMutexLocker locker(&m_mutex);
    m_packetSize = qBound(200, size, 4000);
}

void NetworkCore::startStreaming() {
    QMutexLocker locker(&m_mutex);
    m_isStreamingFlag = true;
}

void NetworkCore::stopStreaming() {
    QMutexLocker locker(&m_mutex);
    m_isStreamingFlag = false;
}

void NetworkCore::sendNextRtpPacket(std::vector<float> samples) {
    // QMutexLocker locker(&m_mutex);
    if (!m_isStreamingFlag || samples.empty()) return;

    QByteArray opusData(m_packetSize, 0);
    int encodedBytes = opus_encode_float(m_opusEncoder, samples.data(), 480,
                                         reinterpret_cast<unsigned char*>(opusData.data()), opusData.size());
    if (encodedBytes <= 0) {
        qWarning() << "Opus encoding error!";
        return;
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
    m_udpSocket.writeDatagram(rtpPacket, m_clientAddress, m_clientPort);

    qDebug() << "Sent RTP packet: size" << encodedBytes + 12;

    m_sequenceNumber++;
    m_timestamp += samples.size() / 2;
}
