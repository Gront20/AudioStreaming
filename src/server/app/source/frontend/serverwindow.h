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
    void recieveAudioSamples(const QVector<float> &samples);
    void addPacket(const quint16 packetSize);
    void playbackPositionChanged(float pos);

// slots for GUI

private slots:

    void selectAudioFile();
    void startAudio();
    void restartAudio();
    void stopAudio();
    void clearLog();
    void handleConnectionInputs();
    void updateNetworkGraph();
    void setVolumeValue(int value);
    void setPacketSize();

// Signals

signals:

    void fileSelected();

    void audioFileStartProcessing(const QString& filePath);
    void audioPlayerChangeState(AUDIO::HANDLER::MODE mode);
    void openConnectionNetwork(const QHostAddress &ip, const quint16 port);
    void closeConnectionNetwork();
    void setVolumeValueToAudio(const float &value);

    void setNetworkMode(const NETWORK::CORE::MODE &mode);
    void setPlaybackPosition(float pos);
    void setPauseForSeek(bool pause);

    void sendPacketSize(const quint16 packetSize);

private:
    Ui::ServerWindow *ui;

    QString             m_widgetStyle{""};
    QString             m_labelStyle{""};
    QString             m_lineEditStyle{""};
    QString             m_textBrowserStyle{""};

    bool                m_sliderPressed{false};

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
#endif // SERVERWINDOW_H
