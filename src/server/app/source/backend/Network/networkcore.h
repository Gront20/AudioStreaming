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

/**
 * @brief The NetworkCore class
 *
 * Сетевой модуль для передачи и приёма аудиоданных через UDP.
 *
 * @details
 * - Поддерживает два режима работы: TRANSMIT (передача) и RECIEVE (приём)
 * - Использует кодек OPUS для сжатия аудио
 * - Реализует протокол RTP для передачи пакетов
 *
 * @see NETWORK::CORE::MODE для возможных режимов работы
 * @see initOpus() для настройки параметров кодека
 */

class NetworkCore : public QObject {
    Q_OBJECT

public:
    explicit NetworkCore(QObject* parent = nullptr);
    ~NetworkCore();

    void setDestination(const QHostAddress& address, quint16 port); ///< Установка соединения по адресу и порту
    void closeConnection(); ///< Закрытие соединения
    bool initOpus(int sampleRate = DEFAULT_SAMPLERATE, int channels = DEFAULT_CHANNELS, int bitrate = DEFAULT_BITRATE); ///< инициализация декодера и кодера OPUS
    void setPacketSize(const int size); ///< Установка размера отправляемых пакетов в режиме TRANSMIT

public slots:

    void startStreaming(); ///< Начало стриминга аудио
    void stopStreaming(); ///< Остановка стриминга аудио
    // void restartStreaming();
    void sendNextRtpPacket(const QVector<float> &samples); ///< Отправка rtp-пакета в режиме TRANSMIT
    void processPendingDatagrams(); ///< Обработка входящих датаграмм в режиме RECIEVE
    void setMode(const NETWORK::CORE::MODE mode); ///< Изменение режима TRANSMIT или RECIEVE

signals:

    void sendSocketStatus(const QVariant data); ///< Отправка текущего состояния соединения
    void sendAudioData(QVector<float> data); ///< Отправка декодированных аудиосемплов в режиме RECIEVE

private:

    NETWORK::CORE::MODE m_currentMode{NETWORK::CORE::MODE::UNDEFINED}; ///< Хранение текущего режима TRANSMIT или RECIEVE

private:

    QUdpSocket          *m_udpSocket; ///< Объект UDP-сокета
    QHostAddress        m_clientAddress{DEFAULT_IP}; ///< IP-адрес
    quint16             m_clientPort{DEFAULT_PORT}; ///< Порт
    quint16             m_packetSize{DEFAULT_PACKETSIZE}; ///< Размер пакетов

    int                 m_bufferIndex{0}; ///< индекс буффера
    bool                m_isStreamingFlag{false}; ///< вкл/выкл сокета
    bool                m_isPlayingFlag{false}; ///< вкл/выкл проигрывание
    QByteArray          m_receivedBuffer; ///< Буффер получаемых данных
    QByteArray          m_partialFrameBuffer; ///< Буффер данных до метки "незавершенный буффер"

    QMutex              m_mtx;

    OpusEncoder*        m_opusEncoder{nullptr}; ///< Кодер ОПУС-а
    OpusDecoder*        m_opusDecoder{nullptr}; ///< Декодер ОПУС-а
    quint16             m_sequenceNumber{0}; ///< Номер последовательности (для RTP пакета)
    QByteArray          m_encodedBuffer; ///< Закодированный буффер
    quint32             m_timestamp{0}; ///< Временные метки (для RTP пакета)
    quint32             m_ssrc{DEFAULT_ID};  ///< Уникальный идентификатор источника (для RTP пакета)

    quint16             m_frameSize{DEFAULT_FRAMES_PER_BUFFER}; ///< Размер фрейма
    quint32             m_sampleRate{DEFAULT_SAMPLERATE}; ///< Частота семплирования
    quint8              m_numChannels{DEFAULT_CHANNELS}; ///< СТерео - 2, моно - 1

};

#endif // NETWORKCORE_H
