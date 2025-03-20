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
    void recieveAudioSamples(const std::vector<float> &samples);

// slots for GUI

private slots:

    void startAudio();
    void stopAudio();
    void handleConnectionInputs();

// Signals

signals:

    void fileSelected();
    void connectNetwork(const QHostAddress &ip, const quint16 port);
    void audioPlayerChangeState(AUDIO::HANDLER::MODE mode);

private:
    QChart *m_chart{nullptr};
    QChartView *chartView{nullptr};
    QLineSeries *series{nullptr};

private:
    Ui::ClientWindow *ui;
};
#endif // CLIENTWINDOW_H
