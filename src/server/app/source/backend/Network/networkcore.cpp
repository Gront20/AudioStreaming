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
    opus_decoder_ctl(m_opusDecoder, OPUS_SET_BITRATE(bitrate));

    m_sampleRate = sampleRate;
    m_numChannels = channels;

    return true;
}

void NetworkCore::setMode(NETWORK::CORE::MODE mode) {
    if (QThread::currentThread() != m_udpSocket->thread()) {
        QMetaObject::invokeMethod(this, [this, mode]() { setMode(mode); }, Qt::QueuedConnection);
        return;
    }

    m_currentMode = mode;

    if (mode == NETWORK::CORE::MODE::RECIEVE) {
        m_udpSocket->open(QIODevice::ReadOnly);

        connect(m_udpSocket, &QUdpSocket::readyRead, this, &NetworkCore::processPendingDatagrams, Qt::DirectConnection);
        emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::MODE_RECIEVE_SWITCHED)));
    } else {
        disconnect(m_udpSocket, &QUdpSocket::readyRead, this, &NetworkCore::processPendingDatagrams);
        m_udpSocket->open(QIODevice::WriteOnly);
        emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::MODE_SEND_SWITCHED)));
    }
}


void NetworkCore::closeConnection() {
    QMetaObject::invokeMethod(this, [this]() {
            if (m_udpSocket) {
                m_udpSocket->disconnect();

                if (m_udpSocket->state() != QAbstractSocket::UnconnectedState) {
                    m_udpSocket->close();
                }

                m_clientAddress = QHostAddress::Null;
                m_clientPort = 0;
                m_isStreamingFlag = false;

                emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::CLOSED)));
            }
        }, Qt::BlockingQueuedConnection);  // Важно использовать BlockingQueued
}

void NetworkCore::setDestination(const QHostAddress &address, quint16 port) {
    if (QThread::currentThread() != m_udpSocket->thread()) {
        QMetaObject::invokeMethod(this, [this, address, port]() {
                setDestination(address, port);
            }, Qt::QueuedConnection);
        return;
    }

    m_clientAddress = address;
    m_clientPort = port;

    if (m_udpSocket->state() != QAbstractSocket::UnconnectedState) {
        m_udpSocket->abort();
    }

    if (m_currentMode == NETWORK::CORE::MODE::RECIEVE) {
        if (!m_udpSocket->bind(address, port)) {
            emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::FAILED_BIND)));
            return;
        }
        connect(m_udpSocket, &QUdpSocket::readyRead,
                this, &NetworkCore::processPendingDatagrams);
        m_isStreamingFlag = true;
        emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::BOUNDED)));
    }
    else {
        m_isStreamingFlag = true;
        emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::BOUNDED)));
    }
}

void NetworkCore::setPacketSize(const int size)
{
    m_packetSize = qBound(200, size, 1500);
}

void NetworkCore::startStreaming()
{
    m_isPlayingFlag = true;
}

void NetworkCore::stopStreaming()
{
    m_isPlayingFlag = false;
}

void NetworkCore::sendNextRtpPacket(const QVector<float>& samples) {

    if (m_currentMode != NETWORK::CORE::MODE::SEND || !m_isStreamingFlag || !m_isPlayingFlag) {
        return;
    }

    QMutexLocker locker(&m_mtx);

    QMetaObject::invokeMethod(this, [this, samples]() {
            QMutexLocker locker(&m_mtx);

            QByteArray opusData(m_packetSize - 12, 0);
            int encodedBytes = opus_encode_float(m_opusEncoder, samples.data(), m_frameSize,
                                                 reinterpret_cast<unsigned char*>(opusData.data()),
                                                 opusData.size());

            if (encodedBytes <= 0) {
                emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::ERROR_OPUS_ENCODE)));
                return;
            }

            m_encodedBuffer.append(opusData.mid(0, encodedBytes));
            m_encodedBuffer.append(QByteArray("\xDE\xAD\xBE\xEF", 4));

            int payloadSize = m_packetSize - 12 - 4;

            if (m_encodedBuffer.size() >= payloadSize) {
                QByteArray rtpPacket(m_packetSize, 0);
                uint8_t* rtpHeader = reinterpret_cast<uint8_t*>(rtpPacket.data());

                // можно конечно добавить там в хидере rtp-пакета метки о неполном фрейме, но я решил вручную это сделать
                rtpHeader[0] = 0x80;  // Version + flags
                rtpHeader[1] = 0xE0;  // Payload type
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
        }, Qt::QueuedConnection);
}


void NetworkCore::processPendingDatagrams() {
    if (m_currentMode != NETWORK::CORE::MODE::RECIEVE || !m_isStreamingFlag) return;

    QMutexLocker locker(&m_mtx);

    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(datagram.data(), datagram.size());

        QByteArray payload = datagram.mid(12);

        m_receivedBuffer.append(payload);

        int pos = 0;
        while (true) {
            int markerPos = m_receivedBuffer.indexOf(MARKER_FRAME_ENDED, pos);
            if (markerPos != -1) {
                QByteArray frame = m_receivedBuffer.mid(pos, markerPos - pos);
                pos = markerPos + 4; ///< маркер 4 байта занимает

                QVector<float> pcmData(m_frameSize * m_numChannels);
                int decodedSamples = opus_decode_float(
                    m_opusDecoder,
                    reinterpret_cast<const unsigned char*>(frame.constData()),
                    frame.size(),
                    pcmData.data(),
                    m_frameSize,
                    0
                    );

                if (decodedSamples > 0) {
                    emit sendAudioData(pcmData);
                }
            } else {
                markerPos = m_receivedBuffer.indexOf(MARKER_FRAME_NOT_ENDED, pos);
                if (markerPos != -1) {
                    QByteArray partialFrame = m_receivedBuffer.mid(pos, markerPos - pos);
                    pos = markerPos + 4; ///< маркер 4 байта занимает
                    m_partialFrameBuffer.append(partialFrame);
                } else {
                    break;
                }
            }
        }

        m_receivedBuffer.remove(0, pos);
        m_receivedBuffer.append(m_partialFrameBuffer);
        m_partialFrameBuffer.clear();

        QVariantMap data;
        data["status"] = static_cast<int>(NETWORK::CORE::STATUS::RECEIVE_PACKET);
        data["packetSize"] = datagram.size();
        emit sendSocketStatus(data);
    }
}
