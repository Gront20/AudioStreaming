#include "serverwindow.h"
#include "./ui_serverwindow.h"

ServerWindow::ServerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerWindow)
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

void ServerWindow::componentsInitStates()
{
    ui->textEditFileInfo->setReadOnly(true);
    ui->textEditFileInfo->clear();

    ui->textBrowserAppLogger->setReadOnly(true);
    ui->textBrowserAppLogger->clear();

    ui->lineEditPathSelectedInfo->setReadOnly(true);
    ui->lineEditPathSelectedInfo->clear();

    ui->pushButtonStartPlayer->setIconSize(QSize(16, 16));
    ui->pushButtonStartPlayer->setIcon(QIcon(":/GUI/images/playButton.png"));

    ui->pushButtonRestartPlayer->setIconSize(QSize(16, 16));
    ui->pushButtonRestartPlayer->setIcon(QIcon(":/GUI/images/restartButton.png"));

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
    ui->widgetAudioFileSelector->setObjectName("carcasWidget");
    ui->widgetMediaPlayer->setObjectName("carcasWidget");
    ui->widgetFileInfo->setObjectName("carcasWidget");
    ui->widgetMediaPlayerControls->setObjectName("carcasWidget");
    ui->widgetConnector->setObjectName("carcasWidget");
    ui->widgetBroadcast->setObjectName("carcasWidget");

    ui->widgetAppLogger->setStyleSheet(style);
    ui->widgetAudioFileSelector->setStyleSheet(style);
    ui->widgetMediaPlayer->setStyleSheet(style);
    ui->widgetFileInfo->setStyleSheet(style);
    ui->widgetMediaPlayerControls->setStyleSheet(style);
    ui->widgetConnector->setStyleSheet(style);
    ui->widgetBroadcast->setStyleSheet(style);

    ui->pushButtonStartPlayer->setEnabled(false);
    ui->pushButtonStopPlayer->setEnabled(false);
    ui->pushButtonRestartPlayer->setEnabled(false);

    ui->lineEditIP->setText("192.168.100.101");
    ui->lineEditPort->setText("8080");

    this->setFixedSize(765, 700);

    ui->spinBoxLoggerCapacity->setMinimum(10);
    ui->spinBoxLoggerCapacity->setMaximum(1000);
    ui->spinBoxLoggerCapacity->setSingleStep(10);
    ui->spinBoxLoggerCapacity->setValue(100);

    //-------------------------------------------------------------------

    ui->widgetBroadcast->setContentsMargins(0, 0, 0, 0);

    m_dataNetworkData = QVector<QPointF>(50);

    for (int i = 0; i < 50; ++i) {
        m_dataNetworkData[i] = QPointF(i * 0.2, 0);
    };

    m_chartNetworkData = new QChart();
    m_seriesNetworkData = new QSplineSeries();

    m_chartNetworkData->legend()->hide();
    m_chartNetworkData->addSeries(m_seriesNetworkData);

    m_axisXNetworkData = new QValueAxis();
    m_axisXNetworkData->setRange(0, 10);
    m_axisXNetworkData->setLabelFormat("%.1f");

    m_axisYNetworkData = new QValueAxis();
    m_axisYNetworkData->setRange(0, 1000);
    m_axisYNetworkData->setTickInterval(100);

    QFont smallFont = font();
    smallFont.setPointSize(8);
    m_axisXNetworkData->setLabelsFont(smallFont);
    m_axisYNetworkData->setLabelsFont(smallFont);

    // m_chartNetworkData->addAxis(m_axisXNetworkData, Qt::AlignBottom);
    // m_chartNetworkData->addAxis(m_axisYNetworkData, Qt::AlignLeft);

    QChartView *chartView = new QChartView(m_chartNetworkData);
    chartView->setContentsMargins(-10, -10, -10, -10);
    chartView->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(chartView);

    if (ui->widgetBroadcast->layout()) {
        delete ui->widgetBroadcast->layout();
    }
    ui->widgetBroadcast->setLayout(layout);

    m_updateElapsedTimerNetworkData.start();
    m_startTime = m_updateElapsedTimerNetworkData.elapsed();

    connect(&m_updateTimerNetworkData, &QTimer::timeout, this, &ServerWindow::updateNetworkGraph);
    m_updateTimerNetworkData.start(200);
}

double previousElapsedTime = 0;

void ServerWindow::addPacket(const quint16 packetSize)
{
    m_lastPacketSize = packetSize;
    m_packetReceived = true;
}

void ServerWindow::updateNetworkGraph()
{
    const int MAX_POINTS = 50;

    double elapsedTime = m_updateElapsedTimerNetworkData.elapsed() / 1000.0;

    if (elapsedTime != previousElapsedTime) {
        previousElapsedTime = elapsedTime;

        if (!m_packetReceived) {
            m_lastPacketSize = 0;
        }

        for (int i = MAX_POINTS - 1; i > 0; --i) {
            m_dataNetworkData[i] = m_dataNetworkData[i - 1];
        }

        m_dataNetworkData[0] = QPointF(0, static_cast<qreal>(m_lastPacketSize));

        for (int i = 1; i < MAX_POINTS; ++i) {
            m_dataNetworkData[i].setX(i * 0.02);
        }

        m_axisXNetworkData->setRange(0, MAX_POINTS * 0.02);

        m_seriesNetworkData->replace(m_dataNetworkData);
        m_chartNetworkData->update();

        m_packetReceived = false;
    }
}

void ServerWindow::componentsConnections()
{
    connect(ui->pushButtonSelectAudioFile, &QPushButton::clicked, this, &ServerWindow::selectAudioFile, Qt::DirectConnection);
    connect(ui->pushButtonStartPlayer, &QPushButton::clicked, this, &ServerWindow::startAudio, Qt::DirectConnection);
    connect(ui->pushButtonStopPlayer, &QPushButton::clicked, this, &ServerWindow::stopAudio, Qt::DirectConnection);
    connect(ui->pushButtonRestartPlayer, &QPushButton::clicked, this, &ServerWindow::restartAudio, Qt::DirectConnection);
    connect(ui->pushButtonOpenConnection, &QPushButton::clicked, this, &ServerWindow::handleConnectionInputs, Qt::DirectConnection);
    connect(ui->pushButtonCloseConnection, &QPushButton::clicked, this, &ServerWindow::handleConnectionInputs, Qt::DirectConnection);
    connect(ui->pushButtonLoggerClear, &QPushButton::clicked, this, &ServerWindow::clearLog, Qt::DirectConnection);
}

void ServerWindow::handleConnectionInputs()
{
    QObject *senderObject = sender();
    QPushButton *senderButton = qobject_cast<QPushButton *>(senderObject);
    if (senderButton == ui->pushButtonOpenConnection) {
        QString ipAddress = ui->lineEditIP->text();
        QString portString = ui->lineEditPort->text();

        if (ipAddress.isEmpty() || portString.isEmpty()) {
            recieveMessage("Error! Field (IP or Port) is empty!");
            return;
        }

        bool ok;
        quint16 port = portString.toUShort(&ok);
        if (!ok) {
            recieveMessage("Error! Invalid port format.");
            return;
        }

        if (port < 1 || port > 65535) {
            recieveMessage("Error! Port must be in the range from 1 to 65535.");
            return;
        }

        QHostAddress address(ipAddress);
        if (address.isNull()) {
            recieveMessage("Error! Invalid IP address format.");
            return;
        }
        quint16 port16 = static_cast<quint16>(port);

        emit openConnectionNetwork(address, port16);
    } else if (senderButton == ui->pushButtonCloseConnection) {
        emit closeConnectionNetwork();
    }
}

void ServerWindow::startAudio()
{
    emit audioPlayerChangeState(AUDIO::HANDLER::MODE::START);
}

void ServerWindow::restartAudio()
{
    emit audioPlayerChangeState(AUDIO::HANDLER::MODE::RESTART);
}

void ServerWindow::stopAudio()
{
    emit audioPlayerChangeState(AUDIO::HANDLER::MODE::STOP);
}

void ServerWindow::recieveAudioSamples(const QVector<float> &samples)
{
    m_seriesAudioSamples = new QLineSeries();
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

            m_seriesAudioSamples->append(i, avg);
        }
    } else {
        for (size_t i = 0; i < samples.size(); ++i) {
            m_seriesAudioSamples->append(i, samples[i]);
        }
    }
    if (m_chartAudioSamples != nullptr) {
        while (m_chartAudioSamples->series().size() > 0) {
            m_chartAudioSamples->removeSeries(m_chartAudioSamples->series().first());
        }
        m_chartAudioSamples->addSeries(m_seriesAudioSamples);
    } else {
        m_chartAudioSamples = new QChart();

        m_chartAudioSamples->createDefaultAxes();
        m_chartAudioSamples->addSeries(m_seriesAudioSamples);
        m_chartAudioSamples->legend()->hide();

        m_chartAudioSamplesView = new QChartView(m_chartAudioSamples);
        m_chartAudioSamplesView->setRenderHint(QPainter::Antialiasing);

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(m_chartAudioSamplesView);
        ui->widgetMediaPlayer->setLayout(layout);
    }
}

HANDLERTYPE ServerWindow::getHandlerType(QObject *sender)
{
    const QString className = sender->metaObject()->className();
    if (className == "BaseAppCore") return HANDLERTYPE::CORE;
    if (className == "AudioHandler") return HANDLERTYPE::AUDIO;
    if (className == "NetworkHandler") return HANDLERTYPE::NETWORK;
    if (className.contains("QPushButton")) {
        return HANDLERTYPE::GUI;
    }

    return HANDLERTYPE::UNDEFINDED;
}

void ServerWindow::recieveMessage(const QString &message)
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
            tag = "<font color='#0000FF'>[NETWORK]</font> ";
            break;
        case HANDLERTYPE::CORE:
            tag = "<font color='#FF0000'>[CORE]</font> ";
            break;
        case HANDLERTYPE::GUI:
            tag = "<font color='#00FF00'>[GUI]</font> ";
            break;
        default:
            tag = "<font color='red'>[UNKNOWN]</font> ";
            break;
        }

        QString newLine = timeString + tag + message;
        logLines.append(newLine);

        int maxLines = ui->spinBoxLoggerCapacity->value();
        if (logLines.size() > maxLines) {
            logLines.removeFirst();
        }

        ui->textBrowserAppLogger->clear();
        foreach (const QString &line, logLines) {
            ui->textBrowserAppLogger->append(line);
        }
    }
}

void ServerWindow::clearLog()
{
    logLines.clear();
    ui->textBrowserAppLogger->clear();
}

void ServerWindow::handleAudioStatusUpdate(const AUDIO::CORE::STATUS &status)
{
    switch (status) {
    case AUDIO::CORE::STATUS::READY:
        ui->pushButtonStartPlayer->setEnabled(true);
        ui->pushButtonStopPlayer->setEnabled(false);
        ui->pushButtonRestartPlayer->setEnabled(false);
        break;
    case AUDIO::CORE::STATUS::RESUME:
    case AUDIO::CORE::STATUS::START:
        ui->pushButtonStartPlayer->setEnabled(false);
        ui->pushButtonStopPlayer->setEnabled(true);
        ui->pushButtonRestartPlayer->setEnabled(true);
        break;
    case AUDIO::CORE::STATUS::END:
        ui->pushButtonStartPlayer->setEnabled(true);
        ui->pushButtonStopPlayer->setEnabled(false);
        ui->pushButtonRestartPlayer->setEnabled(false);
        break;
    case AUDIO::CORE::STATUS::STOP:
        ui->pushButtonStartPlayer->setEnabled(true);
        ui->pushButtonStopPlayer->setEnabled(false);
        ui->pushButtonRestartPlayer->setEnabled(true);
        break;
    case AUDIO::CORE::STATUS::RESTART:
        ui->pushButtonStartPlayer->setEnabled(false);
        ui->pushButtonStopPlayer->setEnabled(true);
        ui->pushButtonRestartPlayer->setEnabled(true);
        break;
    case AUDIO::CORE::STATUS::UNDEFINED:
    default:
        ui->pushButtonStartPlayer->setEnabled(false);
        ui->pushButtonStopPlayer->setEnabled(false);
        ui->pushButtonRestartPlayer->setEnabled(false);
        break;
    }
}

void ServerWindow::selectAudioFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select audio file", "", "Audio file (*.mp3 *.wav *.ogg)");
    if (!filePath.isEmpty()) {
        ui->lineEditPathSelectedInfo->setText(filePath);

        emit audioFileStartProcessing(filePath);

        QFileInfo fileInfo(filePath);
        QString fileInfoText = "Информация о файле:\n\n";
        fileInfoText += "Полный путь: " + fileInfo.absoluteFilePath() + "\n";
        fileInfoText += "Имя файла: " + fileInfo.fileName() + "\n";
        fileInfoText += "Расширение файла: " + fileInfo.suffix() + "\n";
        fileInfoText += "Размер файла: " + QString::number(fileInfo.size()) + " байт\n";
        fileInfoText += "Дата создания: " + fileInfo.birthTime().toString() + "\n";
        fileInfoText += "Дата последнего изменения: " + fileInfo.lastModified().toString() + "\n";
        fileInfoText += "Дата последнего чтения: " + fileInfo.lastRead().toString() + "\n";

        ui->textEditFileInfo->setPlainText(fileInfoText);
    }
}


ServerWindow::~ServerWindow()
{
    delete ui;
}
