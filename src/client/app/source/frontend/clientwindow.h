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
#include <QStyle>
#include "qstylefactory.h"
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
    void componentsSetStyles();
    void setupAudioChart();
    void setupNetworkChart();
    void setupLineEdits();

    HANDLERTYPE getHandlerType(QObject *sender);

public slots:

    void recieveMessage(const QString &message);
    void handleAudioStatusUpdate(const AUDIO::CORE::STATUS &status);
    void recieveAudioSamples(QVector<float> &samples);
    void addPacket(const quint16 packetSize);

// slots for GUI

private slots:

    void startAudio();
    void stopAudio();
    void handleConnectionInputs();
    void clearLog();
    void updateNetworkGraph();
    void setVolumeValue(int value);

// Signals

signals:

    void openConnectionNetwork(const QHostAddress &ip, const quint16 port);
    void closeConnectionNetwork();
    void audioPlayerChangeState(AUDIO::HANDLER::MODE mode);
    void setVolumeValueToAudio(const float &value);

private:

    Ui::ClientWindow *ui;

    QString             m_widgetStyle{""};
    QString             m_labelStyle{""};
    QString             m_lineEditStyle{""};
    QString             m_textBrowserStyle{""};

    // logger lines
    QStringList         m_logLines;

// Audio chart
private:

    QChart              *m_chartAudioSamples{nullptr};
    QChartView          *m_chartViewAudioSamples{nullptr};
    QLineSeries         *m_seriesAudioSamples{nullptr};

// Network chart
private:

    QChart              *m_chartNetworkData{nullptr};
    QBarSeries          *m_seriesNetworkData{nullptr};
    QBarCategoryAxis    *m_axisXNetworkData{nullptr};
    QValueAxis          *m_axisYNetworkData{nullptr};
    QTimer              m_updateTimerNetworkData;

    QVector<QPointF>    m_dataNetworkData;
    QVector<int>        m_packetDataBuffer;

};
#endif // CLIENTWINDOW_H
