#include "clientwindow.h"
#include "./ui_clientwindow.h"

ClientWindow::ClientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientWindow)
{
    ui->setupUi(this);

    // connectWidgetComponents
    // connectThreadsWithLogicAndOthers
    // connectOthers
    // drawAllElements
    // loadAllStyles
    // loadSettings
    componentsInitStates();
    componentsConnections();
}

void ClientWindow::componentsInitStates()
{
    ui->textBrowserAppLogger->setReadOnly(true);
    ui->textBrowserAppLogger->clear();

    ui->pushButtonStartPlayer->setIconSize(QSize(16, 16));
    ui->pushButtonStartPlayer->setIcon(QIcon(":/GUI/images/playButton.png"));

    ui->pushButtonStopPlayer->setIconSize(QSize(16, 16));
    ui->pushButtonStopPlayer->setIcon(QIcon(":/GUI/images/stopButton.png"));

    ui->pushButtonLoggerDownload->setIconSize(QSize(16, 16));
    ui->pushButtonLoggerDownload->setIcon(QIcon(":/GUI/images/download.png"));

    ui->pushButtonLoggerClear->setIconSize(QSize(16, 16));
    ui->pushButtonLoggerClear->setIcon(QIcon(":/GUI/images/bucket.png"));

    QFile styleFile(":/GUI/styles/widget.qss");
    styleFile.open(QFile::ReadOnly);
    QString style = QLatin1String(styleFile.readAll());

    ui->widgetAppLogger->setObjectName("carcasWidget");
    ui->widgetMediaPlayer->setObjectName("carcasWidget");
    ui->widgetMediaPlayerControls->setObjectName("carcasWidget");
    ui->widgetConnector->setObjectName("carcasWidget");

    ui->widgetAppLogger->setStyleSheet(style);
    ui->widgetMediaPlayer->setStyleSheet(style);
    ui->widgetMediaPlayerControls->setStyleSheet(style);
    ui->widgetConnector->setStyleSheet(style);

    ui->pushButtonStartPlayer->setEnabled(true);
    ui->pushButtonStopPlayer->setEnabled(false);

    ui->lineEditIP->setText("127.0.0.1");
    ui->lineEditPort->setText("8080");
}

void ClientWindow::componentsConnections()
{
    connect(ui->pushButtonStartPlayer, &QPushButton::clicked, this, &ClientWindow::startAudio, Qt::DirectConnection);
    connect(ui->pushButtonStopPlayer, &QPushButton::clicked, this, &ClientWindow::stopAudio, Qt::DirectConnection);
    connect(ui->pushButtonConnection, &QPushButton::clicked, this, &ClientWindow::handleConnectionInputs, Qt::DirectConnection);
}

void ClientWindow::handleConnectionInputs()
{
    QString ipAddress = ui->lineEditIP->text();
    QString portString = ui->lineEditPort->text();

    if (ipAddress.isEmpty() || portString.isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Пожалуйста, заполните все поля.");
        return;
    }

    bool ok;
    quint16 port = portString.toUShort(&ok);
    if (!ok) {
        QMessageBox::critical(this, "Ошибка", "Неправильный формат порта.");
        return;
    }

    if (port < 1 || port > 65535) {
        QMessageBox::critical(this, "Ошибка", "Порт должен быть в диапазоне от 1 до 65535.");
        return;
    }

    QHostAddress address(ipAddress);
    if (address.isNull()) {
        QMessageBox::critical(this, "Ошибка", "Неправильный формат IP-адреса.");
        return;
    }
    quint16 port16 = static_cast<quint16>(port);

    emit connectNetwork(address, port16);
}

void ClientWindow::startAudio()
{
    emit audioPlayerChangeState(AUDIO::HANDLER::MODE::START);
}

void ClientWindow::stopAudio()
{
    emit audioPlayerChangeState(AUDIO::HANDLER::MODE::STOP);
}

void ClientWindow::recieveAudioSamples(const std::vector<float> &samples)
{
    series = new QLineSeries();
    const int maxPoints = 1000;

    if (samples.size() > maxPoints) {
        int step = samples.size() / maxPoints;
        for (int i = 0; i < maxPoints; ++i) {
            int start = i * step;
            int end = (i + 1) * step;
            if (end > samples.size()) end = samples.size();

            float sum = 0.0f;
            for (int j = start; j < end; ++j) {
                sum += samples[j];
            }
            float avg = sum / (end - start);

            series->append(i, avg);
        }
    } else {
        for (size_t i = 0; i < samples.size(); ++i) {
            series->append(i, samples[i]);
        }
    }
    if (m_chart != nullptr) {
        while (m_chart->series().size() > 0) {
            m_chart->removeSeries(m_chart->series().first());
        }
        m_chart->addSeries(series);
    } else {
        m_chart = new QChart();

        m_chart->createDefaultAxes();
        m_chart->addSeries(series);
        m_chart->legend()->hide();

        chartView = new QChartView(m_chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(chartView);
        ui->widgetMediaPlayer->setLayout(layout);
    }
}

HANDLERTYPE ClientWindow::getHandlerType(QObject *sender)
{
    const QString className = sender->metaObject()->className();
    if (className == "BaseAppCore") return HANDLERTYPE::CORE;
    if (className == "AudioHandler") return HANDLERTYPE::AUDIO;
    if (className == "NetworkHandler") return HANDLERTYPE::NETWORK;
    return HANDLERTYPE::UNDEFINDED;
}

void ClientWindow::recieveMessage(const QString &message)
{
    QDateTime currentTime = QDateTime::currentDateTime();
    QString timeString{"[" + currentTime.toString("hh:mm:ss") + "] "}, tag{};
    QObject *sender = QObject::sender();
    if (sender) {
        HANDLERTYPE type = getHandlerType(sender);
        switch (type) {
        case HANDLERTYPE::AUDIO:
            tag = "<font color='#00FF00'>[PLAYER]</font> ";
            break;
        case HANDLERTYPE::NETWORK:
            tag = "<font color='#FFFF00'>[NETWORK]</font> ";
            break;
        case HANDLERTYPE::CORE:
            tag = "<font color='#FF0000'>[CORE]</font> ";
            break;
        default:
            tag = "<font color='red'>[UNKNOWN]</font> ";
            break;
        }
        ui->textBrowserAppLogger->append(timeString + tag + message);
    }
}

void ClientWindow::handleAudioStatusUpdate(const AUDIO::CORE::STATUS &status)
{
    switch (status) {
    case AUDIO::CORE::STATUS::READY:
        ui->pushButtonStartPlayer->setEnabled(true);
        ui->pushButtonStopPlayer->setEnabled(false);
        break;
    case AUDIO::CORE::STATUS::RESUME:
    case AUDIO::CORE::STATUS::START:
        ui->pushButtonStartPlayer->setEnabled(false);
        ui->pushButtonStopPlayer->setEnabled(true);
        break;
    case AUDIO::CORE::STATUS::END:
        ui->pushButtonStartPlayer->setEnabled(true);
        ui->pushButtonStopPlayer->setEnabled(false);
        break;
    case AUDIO::CORE::STATUS::STOP:
        ui->pushButtonStartPlayer->setEnabled(true);
        ui->pushButtonStopPlayer->setEnabled(false);
        break;
    case AUDIO::CORE::STATUS::RESTART:
        ui->pushButtonStartPlayer->setEnabled(false);
        ui->pushButtonStopPlayer->setEnabled(true);
        break;
    case AUDIO::CORE::STATUS::UNDEFINED:
    default:
        ui->pushButtonStartPlayer->setEnabled(false);
        ui->pushButtonStopPlayer->setEnabled(false);
        break;
    }
}

ClientWindow::~ClientWindow()
{
    delete ui;
}
