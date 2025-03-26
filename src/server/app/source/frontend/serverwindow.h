#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QApplication>
#include <QFileDialog>
#include <QUrl>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QThread>
#include <QDateTime>
#include <QTimer>
#include <QWidget>
#include <QStyle>
#include <QRegularExpression>

#include <QtCharts/QChart>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>


#include "defines.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class ServerWindow;
}
QT_END_NAMESPACE

using namespace APPCORE;

class ServerWindow : public QMainWindow
{
    Q_OBJECT

public:
    ServerWindow(QWidget *parent = nullptr);
    ~ServerWindow();

    void sendHello();

// ==================================================================================================

private:

    /**
 * @brief Устанавливает связи между компонентами интерфейса.
 *
 * @details
 * - Связывает сигналы и слоты компонентов
 * - Настройка взаимодействия виджетов
 */
    void componentsConnections();

    /**
 * @brief Инициализирует начальные состояния компонентов.
 *
 * @details
 * - Устанавливает дефолтные значения полей ввода
 * - Сбрасывает состояния виджетов
 */
    void componentsInitStates();

    /**
 * @brief Применяет стили к компонентам интерфейса.
 */
    void componentsSetStyles();

    /**
 * @brief Настривает график для отображения аудиоданных (семплов).
 * @see setupNetworkChart() для сетевого графика
 */
    void setupAudioChart();

    /**
 * @brief Настривает график для отображения сетевых данных.
 * @see setupAudioChart() для аудиографа
 */
    void setupNetworkChart();

    /**
 * @brief Настривает поля ввода (QLineEdit).
 * @details
 * - Устанавливает валидаторы ввода
 */
    void setupLineEdits();

    /**
 * @brief Определяет тип обработчика для объекта.
 *
 * @param [in] sender - Объект, вызвавший событие
 *
 * @return Тип обработчика (HANDLERTYPE)
 *
 * @details
 * - Анализирует тип объекта
 * - Возвращает соответствующий обработчик
 *
 * @note Используется для динамической обработки событий
 */
    HANDLERTYPE getHandlerType(QObject *sender);

// ==================================================================================================

public slots:

    /**
 * @brief Обрабатывает полученное сообщение и выводит в AppLogger.
 * @param [in] message - Текст сообщения
 */
    void recieveMessage(const QString &message);

    /**
 * @brief Обрабатывает обновление статуса процессинга аудиофайла.
 * @param [in] status - Текущий статус (AUDIO::CORE::STATUS)
 * @see AUDIO::CORE::STATUS для возможных статусов
 */
    void handleAudioStatusUpdate(const AUDIO::CORE::STATUS &status);

    /**
 * @brief Принимает аудио-сэмплы для визуализирования в audioChart.
 *
 * @param [in] samples - Вектор сэмплов (тип float)
 *
 * @see setupAudioChart() для настройки графика
 */
    void recieveAudioSamples(const QVector<float> &samples);

    /**
 * @brief Добавляет пакет в сетевой буфер.
 *
 * @param [in] packetSize - Размер пакета (quint16)
 * @see setupNetworkChart() для визуализации
 */
    void addPacket(const quint16 packetSize);

    /**
 * @brief Обрабатывает изменение позиции воспроизведения.
 * @param [in] pos - Текущая позиция (в секундах)
 */
    void playbackPositionChanged(float pos);

    void changeSocketConnectionStatus(bool isEnabled);


// ==================================================================================================
// slots for GUI

private slots:

    /**
 * @brief Выбирает аудиофайл для воспроизведения.
 * Открывает диалоговое окно для выбора аудиофайла.
 *
 * @details
 * - Поддерживаемые форматы: WAV, MP3, и тд.
 */
    void selectAudioFile();

    /**
 * @brief Запускает воспроизведение выбранного аудиофайла.
 * @pre Файл должен быть выбран через selectAudioFile()
 */
    void startAudio();

    /**
 * @brief Перезапускает воспроизведение с текущей позиции.
 * @pre Воспроизведение должно быть активным
 */
    void restartAudio();

    /**
 * @brief Останавливает воспроизведение аудио.
 */
    void stopAudio();

    /**
 * @brief Очищает appLogger
 *
 * @details
 * - Удаляет все записи из лог-файла
 */
    void clearLog();

    /**
 * @brief Обрабатывает вводимые пользователем port и ip
 */
    void handleConnectionInputs();

    /**
 * @brief Обновляет текущий график принятых и отправленных пакетов
 * @see setPacketSize() для настройки размера пакетов
 */
    void updateNetworkGraph();

    /**
 * @brief Устанавливает уровень громкости.
 *
 * @param [in] value - Значение громкости (0-100%)
 *
 * @details
 * - Применяет изменения мгновенно
 * - Сохраняет значение в конфигурации
 *
 * @pre value должно быть в диапазоне [0,100]
 */
    void setVolumeValue(int value);

    /**
 * @brief Устанавливает размер пакета для сетевого взаимодействия.
 */
    void setPacketSize();


// ==================================================================================================
// Signals

signals:

    void audioFileStartProcessing(const QString& filePath); ///< Сигнал для старта обработки выбранного пути файла
    void audioPlayerChangeState(AUDIO::HANDLER::MODE mode); ///< Сигнал для изменения состояния плеера
    void openConnectionNetwork(const QHostAddress &ip, const quint16 port); ///< Подключение сокета по указанному адресу и порту
    void closeConnectionNetwork(); ///< Закрытие соединения
    void setVolumeValueToAudio(const float &value); ///< Изменение громкости

    void setNetworkMode(const NETWORK::CORE::MODE &mode); ///< Установка режима RECIVE или TRANSMIT
    void setPlaybackPosition(float pos); ///< Ползунок проигрывания

    void sendPacketSize(const quint16 packetSize); ///< Регулировка размера отправляемых пакетов


// ==================================================================================================

private:

    Ui::ServerWindow *ui;
    // Стили виджетов
    QString             m_widgetStyle{""};
    QString             m_labelStyle{""};
    QString             m_lineEditStyle{""};
    QString             m_textBrowserStyle{""};
    // Флаг, что слайдер аудио держит пользователь
    bool                m_sliderPressed{false};

    // logger lines
    QStringList         m_logLines;

    NETWORK::CORE::MODE m_mode{NETWORK::CORE::MODE::SEND};


// ==================================================================================================
// Audio chart
private:

    QChart              *m_chartAudioSamples{nullptr};
    QChartView          *m_chartViewAudioSamples{nullptr};
    QLineSeries         *m_seriesAudioSamples{nullptr};

// ==================================================================================================
// Network chart
private:

    QChart              *m_chartNetworkData{nullptr};
    QBarSeries          *m_seriesNetworkData{nullptr};
    QBarCategoryAxis    *m_axisXNetworkData{nullptr};
    QValueAxis          *m_axisYNetworkData{nullptr};
    QTimer              m_updateTimerNetworkData;

    QVector<QPointF>    m_dataNetworkData;
    QVector<int>        m_packetDataBuffer;

// ==================================================================================================

};
#endif // SERVERWINDOW_H
