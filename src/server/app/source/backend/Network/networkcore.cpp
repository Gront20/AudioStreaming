#include "networkcore.h"
#include <QDebug>
#include <QTimer>

NetworkCore::NetworkCore(QObject *parent)
    : QObject(parent) {}

void NetworkCore::setDestination(const QHostAddress &address, quint16 port)
{
    m_clientAddress = address;
    m_clientPort = port;
}

void NetworkCore::setAudioBuffer(const std::vector<float> &samples)
{
    this->m_audioBuffer = samples;
}

void NetworkCore::startStreaming()
{
    QMutexLocker locker(&m_mutex);
    if (m_isStreamingFlag || m_clientPort == 0) return;
    m_isStreamingFlag = true;
    sendNextChunk();
}

void NetworkCore::stopStreaming()
{
    QMutexLocker locker(&m_mutex);
    m_isStreamingFlag = false;
}

void NetworkCore::restartStreaming()
{
    QMutexLocker locker(&m_mutex);
    m_bufferIndex = 0;
    m_isStreamingFlag = true;
    sendNextChunk();
}

void NetworkCore::sendNextChunk()
{
    QMutexLocker locker(&m_mutex);
    if (!m_isStreamingFlag || m_bufferIndex >= m_audioBuffer.size()) return;

    size_t chunkSize = 1024;
    int samplesToSend = std::min(chunkSize, m_audioBuffer.size() - m_bufferIndex);

    QByteArray data(reinterpret_cast<const char *>(&m_audioBuffer[m_bufferIndex]), samplesToSend * sizeof(float));
    m_udpSocket.writeDatagram(data, m_clientAddress, m_clientPort);
    m_bufferIndex += samplesToSend;

    if (m_bufferIndex < m_audioBuffer.size()) {
        QTimer::singleShot(20, this, &NetworkCore::sendNextChunk);
    } else {
        stopStreaming();
    }
}
