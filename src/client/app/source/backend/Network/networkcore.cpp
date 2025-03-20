#include "NetworkCore.h"
#include <QTimer>
#include <QDebug>

NetworkCore::NetworkCore(QObject* parent)
    : QObject(parent), m_opusDecoder(nullptr), m_sampleRate(48000), m_channels(2) {
    m_udpSocket = new QUdpSocket(this);
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &NetworkCore::processPendingDatagrams);
    connect(m_udpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &NetworkCore::handleSocketError);
}

NetworkCore::~NetworkCore() {
    if (m_opusDecoder) {
        opus_decoder_destroy(m_opusDecoder);
    }
}

void NetworkCore::setDestination(const QHostAddress& address, const quint16 &port) {
    m_listenAddress = address;
    m_listenPort = port;
    startListening();
}

bool NetworkCore::initOpus(int sampleRate, int channels) {
    int error;
    m_opusDecoder = opus_decoder_create(sampleRate, channels, &error);
    if (error != OPUS_OK) {
        qWarning() << "Opus decoder init failed:" << opus_strerror(error);
        return false;
    }
    m_sampleRate = sampleRate;
    m_channels = channels;
    return true;
}

void NetworkCore::startListening() {
    if (!m_listenPort) {
        qWarning() << "Listening port not set!";
        return;
    }

    if (m_udpSocket->state() != QAbstractSocket::UnconnectedState) {
        qDebug() << "Rebinding UDP socket...";
        m_udpSocket->close();
    }

    if (!m_udpSocket->bind(m_listenAddress, m_listenPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        qWarning() << "Failed to bind UDP socket on" << m_listenAddress.toString() << "port" << m_listenPort;
    } else {
        qDebug() << "Listening on" << m_listenAddress.toString() << "port" << m_listenPort;
    }
}

void NetworkCore::processPendingDatagrams() {
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        qint64 bytesRead = m_udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        if (bytesRead == -1) {
            qWarning() << "Failed to read datagram from" << sender.toString() << "port" << senderPort;
            continue;
        }

        if (datagram.size() > 12) {
            QByteArray opusData = datagram.mid(12);
            QVector<float> decodedSamples(960 * m_channels);
            int frameSize = opus_decode_float(m_opusDecoder,
                                              reinterpret_cast<const unsigned char*>(opusData.constData()),
                                              opusData.size(), decodedSamples.data(),
                                              decodedSamples.size() / m_channels, 0);
            if (frameSize > 0) {
                qDebug() << "Received and decoded" << frameSize << "samples";
                emit sendAudioData(decodedSamples, frameSize);
            } else {
                qWarning() << "Opus decoding error";
            }
        }
    }
}

void NetworkCore::handleSocketError(QAbstractSocket::SocketError error) {
    qWarning() << "Socket error:" << m_udpSocket->errorString();
}
