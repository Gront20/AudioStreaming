#include "NetworkCore.h"
#include <QTimer>
#include <QDebug>

NetworkCore::NetworkCore(QObject* parent, quint32 sampleRate, quint8 channels)
    : QObject(parent), m_opusDecoder(nullptr), m_sampleRate(sampleRate), m_numChannels(channels) {
    m_udpSocket = new QUdpSocket(this);
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &NetworkCore::processPendingDatagrams);

    connect(m_udpSocket, &QAbstractSocket::stateChanged, [this](QAbstractSocket::SocketState state) {
        emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::STATE_CHANGED)));
    });
    connect(m_udpSocket, &QAbstractSocket::errorOccurred, [this](QAbstractSocket::SocketError error) {
        emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::ERROR_SOCKET)));;
    });
}

NetworkCore::~NetworkCore() {
    if (m_opusDecoder) {
        opus_decoder_destroy(m_opusDecoder);
    }
}

void NetworkCore::setDestination(const QHostAddress& address, const quint16 &port) {
    m_clientAddress = address;
    m_clientPort = port;

    if (m_udpSocket->localAddress() == address && m_udpSocket->localPort() == port) {
        emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::ALREADY_BOUNDED)));
        return;
    }

    startListening();
}

bool NetworkCore::initOpus(int sampleRate, int channels, int bitrate) {
    int error;
    m_opusDecoder = opus_decoder_create(sampleRate, channels, &error);
    if (error != OPUS_OK) {
        emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::ERROR_OPUS_INIT)));
        return false;
    }

    opus_decoder_ctl(m_opusDecoder, OPUS_SET_BITRATE(bitrate));

    m_sampleRate = sampleRate;
    m_numChannels = channels;
    return true;
}

void NetworkCore::startListening() {

    if (QThread::currentThread() != m_udpSocket->thread()) {
        QMetaObject::invokeMethod(m_udpSocket, [this]() {
                startListening();
            }, Qt::QueuedConnection);
        return;
    }

    m_udpSocket->abort();

    if (m_udpSocket->state() != QAbstractSocket::UnconnectedState) {
        emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::REBINDING)));
        m_udpSocket->close();
    }

    if (!m_udpSocket->bind(m_clientAddress, m_clientPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::FAILED_BIND)));
    } else {
        emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::BOUNDED)));
    }
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

    emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::CLOSED)));
}

void NetworkCore::processPendingDatagrams() {
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(datagram.data(), datagram.size());

        QByteArray payload = datagram.mid(12);

        m_receivedBuffer.append(payload);

        int pos = 0;
        while (true) {
            int markerPos = m_receivedBuffer.indexOf("\xDE\xAD\xBE\xEF", pos);
            if (markerPos != -1) {
                QByteArray frame = m_receivedBuffer.mid(pos, markerPos - pos);
                pos = markerPos + 4;

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
                    emit sendAudioData(pcmData, decodedSamples);
                }
            } else {
                markerPos = m_receivedBuffer.indexOf("\xCA\xFE\xBA\xBE", pos);
                if (markerPos != -1) {
                    QByteArray partialFrame = m_receivedBuffer.mid(pos, markerPos - pos);
                    pos = markerPos + 4;
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
        // data["status"] = static_cast<int>(NETWORK::CORE::STATUS::RECEIVE_PACKET);
        data["packetSize"] = datagram.size();
        emit sendSocketStatus(data);
    }
}
