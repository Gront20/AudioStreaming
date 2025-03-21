#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

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
#include <QtCharts/QChart>
#include <QtCharts/QSplineSeries>
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

private:

    void componentsConnections();
    void componentsInitStates();
    HANDLERTYPE getHandlerType(QObject *sender);

public slots:

    void recieveMessage(const QString &message);
    void handleAudioStatusUpdate(const AUDIO::CORE::STATUS &status);
    void recieveAudioSamples(const QVector<float> &samples);
    void addPacket(const quint16 packetSize);

// slots for GUI

private slots:

    void selectAudioFile();
    void startAudio();
    void restartAudio();
    void stopAudio();
    void clearLog();
    void handleConnectionInputs();
    void updateNetworkGraph();

// Signals

signals:

    void fileSelected();

    void audioFileStartProcessing(const QString& filePath);
    void audioPlayerChangeState(AUDIO::HANDLER::MODE mode);
    void openConnectionNetwork(const QHostAddress &ip, const quint16 port);
    void closeConnectionNetwork();

private:
    // logger lines
    QStringList         logLines;

    QChart              *m_chartAudioSamples{nullptr};
    QChartView          *m_chartAudioSamplesView{nullptr};
    QLineSeries         *m_seriesAudioSamples{nullptr};

    QChart              *m_chartNetworkData{nullptr};
    QSplineSeries       *m_seriesNetworkData{nullptr};
    QValueAxis          *m_axisXNetworkData{nullptr};
    QValueAxis          *m_axisYNetworkData{nullptr};
    QElapsedTimer       m_updateElapsedTimerNetworkData;
    QTimer              m_updateTimerNetworkData;
    qint64              m_startTime{10};

    QVector<QPointF>    m_dataNetworkData;
    qreal               m_timeNetworkData{0};
    quint16             m_lastPacketSize{0};
    bool                m_packetReceived{false};


private:
    Ui::ServerWindow *ui;
};
#endif // CLIENTWINDOW_H
