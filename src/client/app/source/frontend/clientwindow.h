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
#include <QTime>
#include <QtCharts>
#include <QWidget>

#include "defines.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class ClientWindow;
}
QT_END_NAMESPACE

using namespace APPCORE;

class ClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow();

private:

    void componentsConnections();
    void componentsInitStates();
    HANDLERTYPE getHandlerType(QObject *sender);

public slots:

    void recieveMessage(const QString &message);
    void handleAudioStatusUpdate(const AUDIO::CORE::STATUS &status);
    void recieveAudioSamples(const QVector<float> &samples, const int &frameSize);
    void addPacket(const quint16 packetSize);

// slots for GUI

private slots:

    void startAudio();
    void stopAudio();
    void handleConnectionInputs();
    void clearLog();
    void updateNetworkGraph();

// Signals

signals:

    void openConnectionNetwork(const QHostAddress &ip, const quint16 port);
    void closeConnectionNetwork();
    void audioPlayerChangeState(AUDIO::HANDLER::MODE mode);

private:
    QStringList         m_logLines;

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
    Ui::ClientWindow *ui;
};
#endif // CLIENTWINDOW_H
