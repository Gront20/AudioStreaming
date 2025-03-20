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
    void recieveAudioSamples(const std::vector<float> &samples);

// slots for GUI

private slots:

    void selectAudioFile();
    void startAudio();
    void restartAudio();
    void stopAudio();

// Signals

signals:

    void fileSelected();
    void connectNetwork(QString ip, QString port);

    void audioFileStartProcessing(const QString& filePath);

    void audioPlayerChangeState(AUDIO::HANDLER::MODE mode);

private:
    QChart *m_chart{nullptr};
    QChartView *chartView{nullptr};
    QLineSeries *series{nullptr};

private:
    Ui::ServerWindow *ui;
};
#endif // CLIENTWINDOW_H
