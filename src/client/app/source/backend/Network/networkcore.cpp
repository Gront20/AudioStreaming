#include "NetworkCore.h"
#include <QTimer>
#include <QDebug>

NetworkCore::NetworkCore(QObject* parent, quint32 sampleRate, quint8 channels)
    : QObject(parent), m_opusDecoder(nullptr), m_sampleRate(sampleRate), m_channels(channels) {
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

bool NetworkCore::initOpus(int sampleRate, int channels) {
    int error;
    m_opusDecoder = opus_decoder_create(sampleRate, channels, &error);
    if (error != OPUS_OK) {
        emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::ERROR_OPUS_INIT)));
        return false;
    }
    m_sampleRate = sampleRate;
    m_channels = channels;
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
        QHostAddress sender;
        quint16 senderPort;
        qint64 bytesRead = m_udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        if (bytesRead == -1) {
            emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::FAILED_READ)));
            continue;
        }

        if (datagram.size() > 12) {

            quint8* data = reinterpret_cast<quint8*>(datagram.data());

            quint8 version = (data[0] >> 6) & 0x3; // Версия RTP
            quint8 padding = (data[0] >> 5) & 0x1; // Флаг заполнения
            quint8 extension = (data[0] >> 4) & 0x1; // Флаг расширения
            quint8 csrcCount = data[0] & 0xF; // Количество CSRC
            quint8 marker = (data[1] >> 7) & 0x1; // Бит маркера
            quint8 payloadType = data[1] & 0x7F; // Тип полезной нагрузки
            quint16 sequenceNumber = (data[2] << 8) | data[3]; // Порядковый номер
            quint32 timestamp = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7]; // Временная метка
            quint32 ssrc = (data[8] << 24) | (data[9] << 16) | (data[10] << 8) | data[11]; // SSRC

            QByteArray opusData = datagram.mid(12);
            QVector<float> decodedSamples(960 * m_channels);
            int frameSize = opus_decode_float(m_opusDecoder,
                                              reinterpret_cast<const unsigned char*>(opusData.constData()),
                                              opusData.size(), decodedSamples.data(),
                                              decodedSamples.size() / m_channels, 0);

            QVariantMap qdata;
            qdata["status"] = static_cast<int>(NETWORK::CORE::STATUS::SEND_PACKET);
            qdata["packetSize"] = datagram.size();
            emit sendSocketStatus(qdata);

            if (frameSize > 0) {
                emit sendAudioData(decodedSamples, frameSize);
            } else {
                emit sendSocketStatus(QVariant(static_cast<int>(NETWORK::CORE::STATUS::ERROR_OPUS_DECODE)));
            }
        }
    }
}
