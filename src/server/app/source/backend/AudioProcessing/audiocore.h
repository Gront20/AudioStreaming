#ifndef AUDIOCORE_H
#define AUDIOCORE_H

#include <QThread>
#include <QObject>
#include <QMutex>
#include <QDebug>
#include <QFileInfo>
#include <QString>
#include "defines.h"

extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswresample/swresample.h>
    #include <portaudio.h>
}

/**
 * @brief The AudioCore class
 *
 * Ядро аудиопроцессинга для воспроизведения и управления аудиофайлами.
 *
 * @details
 * - Поддерживает форматы: WAV, MP3 и тд
 * - Использует PortAudio для низкоуровневого доступа к звуковой карте
 * - Реализует потоковое воспроизведение через FFmpeg
 *
 * @see AUDIO::CORE::ERROR_HANDLER для кодов ошибок
 * @see AUDIO::CORE::STATUS для статусов воспроизведения
 */

class AudioCore : public QObject {

    Q_OBJECT

public:
    explicit AudioCore(QObject *parent = nullptr);
    ~AudioCore();

    /**
     * @brief Загружает аудиофайл для воспроизведения.
     * @param [in] filename - Путь к файлу
     * @return true при успешной загрузке
     * @see changeFile() для смены файла во время воспроизведения
     */
    bool loadFile(const std::string& filename);

    /**
     * @brief Меняет текущий аудиофайл.
     * @param [in] newFilePath - Новый путь к файлу
     * @pre Файл должен быть загружен через loadFile()
     */
    void changeFile(const std::string& newFilePath);

    /**
     * @brief Устанавливает громкость воспроизведения.
     * @param [in] value - Значение громкости (0.0 - 1.0)
     * @pre value должно быть в диапазоне [0.0, 1.0]
     */
    void setVolumeValue(const float &value);

    /**
     * @brief Воспроизводит аудиосэмплы. Работает в режиме RECIEVE
     * @param [in] samples - Вектор аудиосэмплов
     */
    void playAudio(QVector<float>& samples);

    /**
     * @brief Устанавливает позицию воспроизведения.
     * @param [in] position - Позиция в секундах
     * @see playbackPositionChanged() для отслеживания позиции
     */
    void setPlaybackPosition(float position);

    /**
     * @brief Запускает воспроизведение.
     */
    void play();

    /**
     * @brief Останавливает воспроизведение.
     */
    void stop();

    /**
     * @brief Перезапускает воспроизведение.
     */
    void restart();

signals:

    /**
     * @brief Отправляет аудиосэмплы для обработки.
     * @param [in] samples - Вектор аудиосэмплов
     */
    void sendAudioSamples(const QVector<float> &samples);

    /**
     * @brief Отправляет код ошибки.
     * @param [in] errorCode - Код ошибки (AUDIO::CORE::ERROR_HANDLER)
     * @see AUDIO::CORE::ERROR_HANDLER для возможных кодов
     */
    void sendCurrentStateError(const AUDIO::CORE::ERROR_HANDLER &errorCode);

    /**
     * @brief Отправляет статус воспроизведения текущего файла.
     * @param [in] fileName - Имя текущего файла
     * @param [in] status - Статус (AUDIO::CORE::STATUS)
     * @see AUDIO::CORE::STATUS для возможных статусов
     */
    void sendAudioStatus(const QString &fileName, const AUDIO::CORE::STATUS &status);

    /**
     * @brief Уведомляет о изменении позиции воспроизведения.
     * @param [in] position - Текущая позиция в секундах
     * @see setPlaybackPosition() для ручного управления
     */
    void playbackPositionChanged(float position);

private:

    /**
     * @brief Callback-функция для PortAudio.
     *
     * @param [in] inputBuffer - Входной буфер (не используется)
     * @param [out] outputBuffer - Выходной буфер
     * @param [in] framesPerBuffer - Количество фреймов
     * @param [in] timeInfo - Временные метки
     * @param [in] statusFlags - Флаги состояния
     * @param [in] userData - Указатель на объект AudioCore
     *
     * @return 0 при успешной обработке
     *
     * @details
     * - Обрабатывает данные из аудиобуфера
     * - Применяет громкость и позиционирование
     *
     * @see m_audioBuffer для источника данных
     */
    static int paCallback(const void *inputBuffer, void *outputBuffer,
                          unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo,
                          PaStreamCallbackFlags statusFlags, void *userData);

    /**
     * @brief Очищает ресурсы.
     */
    void cleanup();

    /**
     * @brief Очищает аудиопоток.
     */
    void cleanupStream();

private:

    std::vector<float>      m_audioBuffer; ///< Основной буфер аудиоданных
    size_t                  m_bufferIndex{0}; ///< Индекс текущего фрейма
    quint8                  m_numChannels{DEFAULT_CHANNELS}; ///< Количество каналов (выбираю 2)

    QString                 m_fileName{}; ///< Имя текущего файла

    float                   m_volumeValue{DEFAULT_VOLUME / 100.}; ///< Громкость

    QMutex                  m_mtx; ///< Мьютекс для синхронизации (может и не нужен)
    std::condition_variable m_cv; ///< Тоже для синхронизации

    std::atomic<bool>       m_isPlaying{false}; ///< Флаг активности воспроизведения
    std::atomic<bool>       m_isPaused{false}; ///< Флаг паузы
    std::atomic<bool>       m_fileChanged{false}; ///< Флаг смены файла
    bool                    m_isFinished = false; ///< Флаг завершения воспроизведения

    PaStream                *m_stream = nullptr; ///< Поток PortAudio
    AVFormatContext         *m_fmt_ctx = nullptr; ///< Контекст FFmpeg
    AVCodecContext          *m_codec_ctx = nullptr; ///< Контекст кодека
    AVPacket                *m_packet = nullptr; ///< Пакет FFmpeg
    AVFrame                 *m_frame = nullptr; ///< Кадр FFmpeg
    SwrContext              *m_swr_ctx = nullptr; ///< Контекст ресэмплирования

};

#endif // AUDIOCORE_H
