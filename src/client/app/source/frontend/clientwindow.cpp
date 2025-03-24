#include "clientwindow.h"
#include "./ui_clientwindow.h"

ClientWindow::ClientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientWindow)
{
    ui->setupUi(this);

    QApplication::setStyle(QStyleFactory::create("Fusion"));
    // connectWidgetComponents
    // connectThreadsWithLogicAndOthers
    // connectOthers
    // drawAllElements
    // loadAllStyles
    // loadSettings
    componentsInitStates();
    componentsConnections();
    componentsSetStyles();
    setupAudioChart();
    setupNetworkChart();
}

void ClientWindow::componentsSetStyles()
{
    QFile widgetStyleFile(":/GUI/styles/widget.qss");
    QFile labelStyleFile(":/GUI/styles/label.qss");
    QFile lineEditStyleFile(":/GUI/styles/lineEdit.qss");
    QFile textBrowserStyleFile(":/GUI/styles/textBrowser.qss");

    widgetStyleFile.open(QFile::ReadOnly);
    labelStyleFile.open(QFile::ReadOnly);
    lineEditStyleFile.open(QFile::ReadOnly);
    textBrowserStyleFile.open(QFile::ReadOnly);

    m_labelStyle = QLatin1String(labelStyleFile.readAll());
    m_widgetStyle = QLatin1String(widgetStyleFile.readAll());
    m_lineEditStyle = QLatin1String(lineEditStyleFile.readAll());
    m_textBrowserStyle = QLatin1String(textBrowserStyleFile.readAll());


    ui->centralwidget->setObjectName("mainWidget");
    ui->widgetAppLogger->setObjectName("carcasWidget");
    ui->widgetMediaPlayer->setObjectName("carcasWidget");
    ui->widgetMediaPlayerControls->setObjectName("carcasWidget");
    ui->widgetConnector->setObjectName("carcasWidget");
    ui->widgetBroadcast->setObjectName("carcasWidget");
    ui->widgetPlayer->setObjectName("carcasWidget");
    ui->widgetAppLoggerSettings->setObjectName("carcasWidget");
    ui->widgetVolume->setObjectName("carcasWidget");

    ui->centralwidget->setStyleSheet(m_widgetStyle);
    ui->widgetAppLogger->setStyleSheet(m_widgetStyle);
    ui->widgetMediaPlayer->setStyleSheet(m_widgetStyle);
    ui->widgetMediaPlayerControls->setStyleSheet(m_widgetStyle);
    ui->widgetConnector->setStyleSheet(m_widgetStyle);
    ui->widgetBroadcast->setStyleSheet(m_widgetStyle);
    ui->widgetPlayer->setStyleSheet(m_widgetStyle);
    ui->widgetAppLoggerSettings->setStyleSheet(m_widgetStyle);
    ui->widgetVolume->setStyleSheet(m_widgetStyle);

    ui->labelAppLogger->setObjectName("mainTitleLabel");
    ui->labelVolumeMainInfo->setObjectName("mainTitleLabel");
    ui->labelMediaPlayerControls->setObjectName("mainTitleLabel");
    ui->labelNetworkSettings->setObjectName("mainTitleLabel");
    ui->labelAppLogger->setFont(QFont("Arial", 10, QFont::Bold));
    ui->labelVolumeMainInfo->setFont(QFont("Arial", 10, QFont::Bold));
    ui->labelMediaPlayerControls->setFont(QFont("Arial", 10, QFont::Bold));
    ui->labelNetworkSettings->setFont(QFont("Arial", 10, QFont::Bold));

    ui->labelIP->setObjectName("titleLabel");
    ui->labelPort->setObjectName("titleLabel");
    ui->labelAppLoggerCapacity->setObjectName("titleLabel");
    ui->labelVolumeValue->setObjectName("titleLabel");

    ui->labelAppLogger->setStyleSheet(m_labelStyle);
    ui->labelIP->setStyleSheet(m_labelStyle);
    ui->labelPort->setStyleSheet(m_labelStyle);
    ui->labelAppLoggerCapacity->setStyleSheet(m_labelStyle);
    ui->labelVolumeValue->setStyleSheet(m_labelStyle);
    ui->labelVolumeMainInfo->setStyleSheet(m_labelStyle);
    ui->labelMediaPlayerControls->setStyleSheet(m_labelStyle);
    ui->labelNetworkSettings->setStyleSheet(m_labelStyle);

    ui->lineEditIP->setObjectName("lineEdit");
    ui->lineEditPort->setObjectName("lineEdit");

    ui->lineEditIP->setStyleSheet(m_lineEditStyle);
    ui->lineEditPort->setStyleSheet(m_lineEditStyle);

    ui->textBrowserAppLogger->setObjectName("textBrowser");

    ui->textBrowserAppLogger->setStyleSheet(m_textBrowserStyle);
}

void ClientWindow::setupAudioChart()
{
    m_chartAudioSamples = new QChart();
    m_seriesAudioSamples = new QLineSeries();

    m_chartAudioSamples->createDefaultAxes();
    m_chartAudioSamples->addSeries(m_seriesAudioSamples);
    m_chartAudioSamples->legend()->hide();
    m_chartAudioSamples->setBackgroundBrush(QBrush(QColor(50, 80, 50)));
    m_chartAudioSamples->setBackgroundRoundness(0);

    m_chartViewAudioSamples = new QChartView(m_chartAudioSamples);
    m_chartViewAudioSamples->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout *layout = new QVBoxLayout(ui->widgetMediaPlayer);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(0);

    QLabel *titleLabel = new QLabel("Audio Samples");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFont(QFont("Arial", 10, QFont::Bold));
    titleLabel->setObjectName("mainTitleLabel");
    titleLabel->setStyleSheet(m_labelStyle);
    layout->addWidget(titleLabel);

    ui->widgetMediaPlayer->setLayout(layout);
    m_chartViewAudioSamples->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(m_chartViewAudioSamples, 100);
    m_chartViewAudioSamples->setBackgroundBrush(QBrush(QColor(50, 50, 50)));

    ui->widgetMediaPlayer->setLayout(layout);
}

void ClientWindow::setupNetworkChart()
{

    ui->widgetBroadcast->setContentsMargins(0, 0, 0, 0);

    m_chartNetworkData = new QChart();
    m_seriesNetworkData = new QBarSeries();

    m_chartNetworkData->legend()->hide();
    m_chartNetworkData->addSeries(m_seriesNetworkData);
    m_chartNetworkData->setBackgroundBrush(QBrush(QColor(50, 50, 50)));
    m_chartNetworkData->setBackgroundRoundness(0);

    QStringList categories;
    categories << "10" << "9"<< "8"<< "7"<< "6"<< "5" << "4" << "3" << "2" << "1" << "0";

    m_axisXNetworkData = new QBarCategoryAxis();
    m_axisXNetworkData->append(categories);
    m_axisXNetworkData->setTitleText("time");
    m_axisXNetworkData->setLabelsFont(QFont("Arial", 8));
    m_axisXNetworkData->setGridLineColor(Qt::white);
    m_axisXNetworkData->setLinePen(QPen(Qt::white));
    m_axisXNetworkData->setLabelsColor(Qt::white);
    m_axisXNetworkData->setTitleBrush(Qt::yellow);

    m_axisYNetworkData = new QValueAxis();
    m_axisYNetworkData->setRange(0, 600);
    m_axisYNetworkData->setTickInterval(100);
    m_axisYNetworkData->setTitleText("bytes");
    m_axisYNetworkData->setLabelsFont(QFont("Arial", 8));
    m_axisYNetworkData->setGridLineColor(Qt::white);
    m_axisYNetworkData->setLinePen(QPen(Qt::white));
    m_axisYNetworkData->setLabelsColor(Qt::white);
    m_axisYNetworkData->setTitleBrush(Qt::yellow);

    m_axisXNetworkData->setLabelsFont(QFont("Arial", 6));
    m_axisYNetworkData->setLabelsFont(QFont("Arial", 6));

    m_chartNetworkData->addAxis(m_axisXNetworkData, Qt::AlignBottom);
    m_chartNetworkData->addAxis(m_axisYNetworkData, Qt::AlignLeft);

    QBarSet *barSet = new QBarSet("networkChunk");
    barSet->setColor(Qt::yellow);
    barSet->setBorderColor(Qt::black);
    m_seriesNetworkData->append(barSet);
    m_seriesNetworkData->setBarWidth(1.0);

    QVBoxLayout *layout = new QVBoxLayout(ui->widgetBroadcast);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(0);

    QLabel *titleLabel = new QLabel("Network Upload Diagramm");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFont(QFont("Arial", 10, QFont::Bold));
    titleLabel->setObjectName("mainTitleLabel");
    titleLabel->setStyleSheet(m_labelStyle);
    layout->addWidget(titleLabel);

    QChartView *chartView = new QChartView(m_chartNetworkData);
    chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(chartView, 100);
    chartView->setBackgroundBrush(QBrush(QColor(50, 50, 50)));
    ui->widgetBroadcast->setLayout(layout);

    connect(&m_updateTimerNetworkData, &QTimer::timeout, this, &ClientWindow::updateNetworkGraph);
    m_updateTimerNetworkData.start(200);
}


void ClientWindow::componentsInitStates()
{
    ui->textBrowserAppLogger->setReadOnly(true);
    ui->textBrowserAppLogger->clear();

    QIcon iconPlay = QApplication::style()->standardIcon(QStyle::SP_MediaPlay);
    QIcon iconStop = QApplication::style()->standardIcon(QStyle::SP_MediaStop);

    ui->pushButtonStartPlayer->setIconSize(QSize(16, 16));
    ui->pushButtonStartPlayer->setIcon(QIcon(iconPlay));

    ui->pushButtonStopPlayer->setIconSize(QSize(16, 16));
    ui->pushButtonStopPlayer->setIcon(QIcon(iconStop));

    ui->pushButtonAppLoggerClear->setIconSize(QSize(16, 16));
    ui->pushButtonAppLoggerClear->setIcon(QIcon(":/GUI/images/bucket.png"));

    ui->pushButtonStartPlayer->setEnabled(true);
    ui->pushButtonStopPlayer->setEnabled(false);

    ui->lineEditIP->setText("192.168.100.101");
    ui->lineEditPort->setText(QString("%1").arg(DEFAULT_PORT));

    this->setFixedSize(752, 615);

    ui->spinBoxAppLoggerCapacity->setMinimum(10);
    ui->spinBoxAppLoggerCapacity->setMaximum(1000);
    ui->spinBoxAppLoggerCapacity->setSingleStep(10);
    ui->spinBoxAppLoggerCapacity->setValue(100);

    ui->sliderVolume->setValue(DEFAULT_VOLUME);
    ui->labelVolumeValue->setText(QString("%1").arg(DEFAULT_VOLUME));
    setVolumeValue(DEFAULT_VOLUME);

    setupLineEdits();

}

void ClientWindow::addPacket(const quint16 packetSize)
{
    m_packetDataBuffer.append(packetSize);
}

void ClientWindow::updateNetworkGraph()
{
    if (m_packetDataBuffer.isEmpty()) {
        m_packetDataBuffer.append(0);
    }

    int sum = std::accumulate(m_packetDataBuffer.begin(), m_packetDataBuffer.end(), 0);
    int averageSize = sum / m_packetDataBuffer.size();
    m_packetDataBuffer.clear();

    if (m_seriesNetworkData->barSets().isEmpty()) {
        QBarSet *barSet = new QBarSet("networkChunk");
        barSet->setColor(Qt::black);
        m_seriesNetworkData->append(barSet);
    }

    QBarSet *barSet = m_seriesNetworkData->barSets().first();
    *barSet << averageSize;

    if (barSet->count() > 50) {
        barSet->remove(0);
    }

    int maxValue = 0;
    for (int i = 0; i < barSet->count(); ++i) {
        maxValue = std::max(maxValue, static_cast<int>(barSet->at(i)));
    }
    m_axisYNetworkData->setRange(0, maxValue);
    barSet->setBorderColor(Qt::black);

    m_seriesNetworkData->setBarWidth(1.0);
    m_chartNetworkData->removeSeries(m_seriesNetworkData);
    m_chartNetworkData->addSeries(m_seriesNetworkData);

    m_chartNetworkData->update();
}


void ClientWindow::setupLineEdits()
{
    QIntValidator* validatorPort = new QIntValidator(1, 65535, this);
    ui->lineEditPort->setValidator(validatorPort);

    QString ipRange = "(?:25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9][0-9]?|0)";
    QRegularExpression ipRegex("^" + ipRange + "\\." + ipRange + "\\." + ipRange + "\\." + ipRange + "$");
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(ipRegex, this);
    ui->lineEditIP->setValidator(validator);
}


void ClientWindow::componentsConnections()
{
    connect(ui->pushButtonStartPlayer, &QPushButton::clicked, this, &ClientWindow::startAudio, Qt::DirectConnection);
    connect(ui->pushButtonStopPlayer, &QPushButton::clicked, this, &ClientWindow::stopAudio, Qt::DirectConnection);
    connect(ui->pushButtonOpenConnection, &QPushButton::clicked, this, &ClientWindow::handleConnectionInputs, Qt::DirectConnection);
    connect(ui->pushButtonCloseConnection, &QPushButton::clicked, this, &ClientWindow::handleConnectionInputs, Qt::DirectConnection);
    connect(ui->pushButtonAppLoggerClear, &QPushButton::clicked, this, &ClientWindow::clearLog, Qt::DirectConnection);
    connect(ui->sliderVolume, &QSlider::valueChanged, this, &ClientWindow::setVolumeValue);
    connect(ui->pushButtonVolume, &QPushButton::clicked, this, [this](){
        if (ui->sliderVolume->value() != 0){
            ui->sliderVolume->setValue(0);
        }
        else
            ui->sliderVolume->setValue(100);
    });
}

void ClientWindow::handleConnectionInputs()
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

void ClientWindow::startAudio()
{
    emit audioPlayerChangeState(AUDIO::HANDLER::MODE::START);
}

void ClientWindow::stopAudio()
{
    emit audioPlayerChangeState(AUDIO::HANDLER::MODE::STOP);
}

void ClientWindow::setVolumeValue(int value)
{
    QIcon iconVolumeOn = QApplication::style()->standardIcon(QStyle::SP_MediaVolume);
    QIcon iconVolumeOff = QApplication::style()->standardIcon(QStyle::SP_MediaVolumeMuted);

    if (value == 0)
        ui->pushButtonVolume->setIcon(iconVolumeOff);
    else
        ui->pushButtonVolume->setIcon(iconVolumeOn);

    ui->labelVolumeValue->setText(QString("%1").arg(value));
    emit setVolumeValueToAudio(static_cast<float>(value) / 100.);
}


void ClientWindow::recieveAudioSamples(QVector<float> &samples)
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
        m_seriesAudioSamples->setColor(Qt::yellow);
        m_chartAudioSamples->addSeries(m_seriesAudioSamples);
    }
}

void ClientWindow::clearLog()
{
    m_logLines.clear();
    ui->textBrowserAppLogger->clear();
}


HANDLERTYPE ClientWindow::getHandlerType(QObject *sender)
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
            tag = "<font color='#0000FF'>[NETWORK]</font> ";
            break;
        case HANDLERTYPE::CORE:
            tag = "<font color='#FF0000'>[CORE]</font> ";
            break;
        default:
            tag = "<font color='#FF0000'>[UNKNOWN]</font> ";
            break;
        }

        QString newLine = timeString + tag + message;
        m_logLines.append(newLine);

        int maxLines = ui->spinBoxAppLoggerCapacity->value();
        if (m_logLines.size() > maxLines) {
            m_logLines.removeFirst();
        }

        ui->textBrowserAppLogger->clear();
        foreach (const QString &line, m_logLines) {
            ui->textBrowserAppLogger->append(line);
        }
    }
}

void ClientWindow::handleAudioStatusUpdate(const AUDIO::CORE::STATUS &status)
{
    switch (status) {
    case AUDIO::CORE::STATUS::RESUME:
    case AUDIO::CORE::STATUS::START:
        ui->pushButtonStartPlayer->setEnabled(false);
        ui->pushButtonStopPlayer->setEnabled(true);
        break;
    case AUDIO::CORE::STATUS::STOP:
        ui->pushButtonStartPlayer->setEnabled(true);
        ui->pushButtonStopPlayer->setEnabled(false);
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
