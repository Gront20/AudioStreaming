#include "serverwindow.h"

#include "./ui_serverwindow.h"
#include "qstylefactory.h"

ServerWindow::ServerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerWindow)
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

void ServerWindow::componentsSetStyles()
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
    ui->widgetAudioFileSelector->setObjectName("carcasWidget");
    ui->widgetMediaPlayer->setObjectName("carcasWidget");
    ui->widgetFileInfo->setObjectName("carcasWidget");
    ui->widgetMediaPlayerControls->setObjectName("carcasWidget");
    ui->widgetConnector->setObjectName("carcasWidget");
    ui->widgetBroadcast->setObjectName("carcasWidget");
    ui->widgetPacketSize->setObjectName("carcasWidget");
    ui->widgetPlayer->setObjectName("carcasWidget");
    ui->widgetLoggerSettings->setObjectName("carcasWidget");
    ui->widgetVolume->setObjectName("carcasWidget");

    ui->centralwidget->setStyleSheet(m_widgetStyle);
    ui->widgetAppLogger->setStyleSheet(m_widgetStyle);
    ui->widgetAudioFileSelector->setStyleSheet(m_widgetStyle);
    ui->widgetMediaPlayer->setStyleSheet(m_widgetStyle);
    ui->widgetFileInfo->setStyleSheet(m_widgetStyle);
    ui->widgetMediaPlayerControls->setStyleSheet(m_widgetStyle);
    ui->widgetConnector->setStyleSheet(m_widgetStyle);
    ui->widgetBroadcast->setStyleSheet(m_widgetStyle);
    ui->widgetPacketSize->setStyleSheet(m_widgetStyle);
    ui->widgetPlayer->setStyleSheet(m_widgetStyle);
    ui->widgetLoggerSettings->setStyleSheet(m_widgetStyle);
    ui->widgetVolume->setStyleSheet(m_widgetStyle);

    ui->labelAppLogger->setObjectName("mainTitleLabel");
    ui->labelFileInfo->setObjectName("mainTitleLabel");
    ui->labelAudioFileSelectioInfo->setObjectName("mainTitleLabel");
    ui->labelVolumeMainInfo->setObjectName("mainTitleLabel");
    ui->labelMediaPlayerControls->setObjectName("mainTitleLabel");
    ui->labelPacketSettings->setObjectName("mainTitleLabel");
    ui->labelNetworkSettings->setObjectName("mainTitleLabel");
    ui->labelNetworkMode->setObjectName("mainTitleLabel");
    ui->labelAudioSlider->setObjectName("mainTitleLabel");
    ui->labelAppLogger->setFont(QFont("Arial", 10, QFont::Bold));
    ui->labelFileInfo->setFont(QFont("Arial", 10, QFont::Bold));
    ui->labelAudioFileSelectioInfo->setFont(QFont("Arial", 10, QFont::Bold));
    ui->labelVolumeMainInfo->setFont(QFont("Arial", 10, QFont::Bold));
    ui->labelMediaPlayerControls->setFont(QFont("Arial", 10, QFont::Bold));
    ui->labelPacketSettings->setFont(QFont("Arial", 10, QFont::Bold));
    ui->labelNetworkSettings->setFont(QFont("Arial", 10, QFont::Bold));
    ui->labelNetworkMode->setFont(QFont("Arial", 10, QFont::Bold));
    ui->labelAudioSlider->setFont(QFont("Arial", 10, QFont::Bold));

    ui->labelIP->setObjectName("titleLabel");
    ui->labelPort->setObjectName("titleLabel");
    ui->labelPath->setObjectName("titleLabel");
    ui->labelSelectAudioFileInfo->setObjectName("titleLabel");
    ui->labelCapacityLogger->setObjectName("titleLabel");
    ui->labelPacketSizeInfo->setObjectName("titleLabel");
    ui->labelCurrentPacketSizeInfo->setObjectName("titleLabel");
    ui->labelCurrentPacketSize->setObjectName("titleLabel");
    ui->labelPacketSizeUnitInfo->setObjectName("titleLabel");
    ui->labelVolumeValue->setObjectName("titleLabel");

    ui->labelAppLogger->setStyleSheet(m_labelStyle);
    ui->labelAudioFileSelectioInfo->setStyleSheet(m_labelStyle);
    ui->labelFileInfo->setStyleSheet(m_labelStyle);
    ui->labelIP->setStyleSheet(m_labelStyle);
    ui->labelPort->setStyleSheet(m_labelStyle);
    ui->labelPath->setStyleSheet(m_labelStyle);
    ui->labelSelectAudioFileInfo->setStyleSheet(m_labelStyle);
    ui->labelCapacityLogger->setStyleSheet(m_labelStyle);
    ui->labelPacketSizeInfo->setStyleSheet(m_labelStyle);
    ui->labelCurrentPacketSizeInfo->setStyleSheet(m_labelStyle);
    ui->labelCurrentPacketSize->setStyleSheet(m_labelStyle);
    ui->labelPacketSizeUnitInfo->setStyleSheet(m_labelStyle);
    ui->labelVolumeValue->setStyleSheet(m_labelStyle);
    ui->labelVolumeMainInfo->setStyleSheet(m_labelStyle);
    ui->labelMediaPlayerControls->setStyleSheet(m_labelStyle);
    ui->labelPacketSettings->setStyleSheet(m_labelStyle);
    ui->labelNetworkSettings->setStyleSheet(m_labelStyle);
    ui->labelNetworkMode->setStyleSheet(m_labelStyle);
    ui->labelAudioSlider->setStyleSheet(m_labelStyle);

    ui->lineEditIP->setObjectName("lineEdit");
    ui->lineEditPort->setObjectName("lineEdit");
    ui->lineEditPacketSize->setObjectName("lineEdit");
    ui->lineEditPathSelectedInfo->setObjectName("lineEdit");

    ui->lineEditIP->setStyleSheet(m_lineEditStyle);
    ui->lineEditPort->setStyleSheet(m_lineEditStyle);
    ui->lineEditPacketSize->setStyleSheet(m_lineEditStyle);
    ui->lineEditPathSelectedInfo->setStyleSheet(m_lineEditStyle);

    ui->textBrowserAppLogger->setObjectName("textBrowser");
    ui->textBrowserFileInfo->setObjectName("textBrowser");

    ui->textBrowserAppLogger->setStyleSheet(m_textBrowserStyle);
    ui->textBrowserFileInfo->setStyleSheet(m_textBrowserStyle);
}

void ServerWindow::setupAudioChart()
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

void ServerWindow::setupNetworkChart()
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

    QLabel *titleLabel = new QLabel("Network Diagramm");
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

    connect(&m_updateTimerNetworkData, &QTimer::timeout, this, &ServerWindow::updateNetworkGraph);
    m_updateTimerNetworkData.start(200);

}

void ServerWindow::componentsInitStates()
{
    ui->textBrowserFileInfo->setReadOnly(true);
    ui->textBrowserFileInfo->clear();

    ui->textBrowserAppLogger->setReadOnly(true);
    ui->textBrowserAppLogger->clear();

    ui->lineEditPathSelectedInfo->setReadOnly(true);
    ui->lineEditPathSelectedInfo->clear();

    QIcon iconPlay = QApplication::style()->standardIcon(QStyle::SP_MediaPlay);
    QIcon iconStop = QApplication::style()->standardIcon(QStyle::SP_MediaStop);
    QIcon iconRestart = QApplication::style()->standardIcon(QStyle::SP_BrowserReload);

    ui->pushButtonStartPlayer->setIconSize(QSize(16, 16));
    ui->pushButtonStartPlayer->setIcon(QIcon(iconPlay));

    ui->pushButtonRestartPlayer->setIconSize(QSize(16, 16));
    ui->pushButtonRestartPlayer->setIcon(QIcon(iconRestart));

    ui->pushButtonStopPlayer->setIconSize(QSize(16, 16));
    ui->pushButtonStopPlayer->setIcon(QIcon(iconStop));

    ui->pushButtonLoggerClear->setIconSize(QSize(16, 16));
    ui->pushButtonLoggerClear->setIcon(QIcon(":/GUI/images/bucket.png"));

    ui->pushButtonStartPlayer->setEnabled(false);
    ui->pushButtonStopPlayer->setEnabled(false);
    ui->pushButtonRestartPlayer->setEnabled(false);

    ui->lineEditIP->setText("192.168.100.101");
    ui->lineEditPort->setText(QString("%1").arg(DEFAULT_PORT));

    ui->lineEditPacketSize->setText(QString("%1").arg(DEFAULT_PACKETSIZE));
    setPacketSize();

    this->setFixedSize(765, 680);

    ui->spinBoxLoggerCapacity->setMinimum(10);
    ui->spinBoxLoggerCapacity->setMaximum(1000);
    ui->spinBoxLoggerCapacity->setSingleStep(10);
    ui->spinBoxLoggerCapacity->setValue(100);

    ui->sliderVolume->setValue(DEFAULT_VOLUME);
    ui->labelVolumeValue->setText(QString("%1").arg(DEFAULT_VOLUME));
    setVolumeValue(DEFAULT_VOLUME);

    ui->radioButtonSendPackets->setChecked(true);
    ui->pushButtonChangeNetworkMode->setText("Transmit");
    ui->labelNetworkMode->setText("Mode: Transmit");

    ui->pushButtonOpenConnection->setEnabled(false);
    ui->pushButtonCloseConnection->setEnabled(false);

    ui->sliderAudioSlider->setRange(0, 100);
    ui->sliderAudioSlider->setSingleStep(0);
    ui->sliderAudioSlider->setPageStep(0);
    ui->sliderAudioSlider->setEnabled(false);

    setupLineEdits();
}

void ServerWindow::setupLineEdits()
{
    QIntValidator* validatorPort = new QIntValidator(1, 65535, this);
    ui->lineEditPort->setValidator(validatorPort);

    connect(ui->lineEditPacketSize, &QLineEdit::textChanged, [this]() {
        QString text = ui->lineEditPacketSize->text();
        if (text.startsWith('0') && text.length() > 1) {
            ui->lineEditPacketSize->setText(text.remove(QRegularExpression("^0+")));
        }
    });

    QIntValidator* validatorPacketSize = new QIntValidator(50, 1500, this);
    ui->lineEditPacketSize->setValidator(validatorPacketSize);

    QString ipRange = "(?:25[0-5]|2[0-4][0-9]|1[0-9]{2}|[0-9][0-9]?|0)";
    QRegularExpression ipRegex("^" + ipRange + "\\." + ipRange + "\\." + ipRange + "\\." + ipRange + "$");
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(ipRegex, this);
    ui->lineEditIP->setValidator(validator);


}

void ServerWindow::addPacket(const quint16 packetSize)
{
    m_packetDataBuffer.append(packetSize);
}

void ServerWindow::updateNetworkGraph()
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

    if (m_mode == NETWORK::CORE::MODE::SEND){
        barSet->setColor(Qt::yellow);
        barSet->setBorderColor(Qt::black);
    }
    else if(m_mode == NETWORK::CORE::MODE::RECIEVE) {
        barSet->setColor(Qt::blue);
        barSet->setBorderColor(Qt::black);
    }

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

void ServerWindow::componentsConnections()
{
    connect(ui->pushButtonSelectAudioFile, &QPushButton::clicked, this, &ServerWindow::selectAudioFile, Qt::DirectConnection);
    connect(ui->pushButtonStartPlayer, &QPushButton::clicked, this, &ServerWindow::startAudio, Qt::DirectConnection);
    connect(ui->pushButtonStopPlayer, &QPushButton::clicked, this, &ServerWindow::stopAudio, Qt::DirectConnection);
    connect(ui->pushButtonRestartPlayer, &QPushButton::clicked, this, &ServerWindow::restartAudio, Qt::DirectConnection);
    connect(ui->pushButtonOpenConnection, &QPushButton::clicked, this, &ServerWindow::handleConnectionInputs, Qt::DirectConnection);
    connect(ui->pushButtonCloseConnection, &QPushButton::clicked, this, &ServerWindow::handleConnectionInputs, Qt::DirectConnection);
    connect(ui->pushButtonLoggerClear, &QPushButton::clicked, this, &ServerWindow::clearLog, Qt::DirectConnection);
    connect(ui->radioButtonSendPackets, &QRadioButton::clicked, this, [this](){
        ui->pushButtonChangeNetworkMode->setText("Transmit");
    });
    connect(ui->radioButtonRecievePackets, &QRadioButton::clicked, this, [this](){
        ui->pushButtonChangeNetworkMode->setText("Recieve");
    });
    connect(ui->pushButtonChangeNetworkMode, &QPushButton::clicked, this, [this](){
        ui->pushButtonOpenConnection->setEnabled(true);
        ui->pushButtonCloseConnection->setEnabled(false);

        if (ui->radioButtonRecievePackets->isChecked()){
            stopAudio();
            ui->pushButtonStartPlayer->setEnabled(false);
            ui->pushButtonRestartPlayer->setEnabled(false);
            ui->pushButtonStopPlayer->setEnabled(false);
            ui->pushButtonSelectAudioFile->setEnabled(false);
            ui->pushButtonSetPacketSize->setEnabled(false);
            ui->sliderAudioSlider->setEnabled(false);

            ui->labelNetworkMode->setText("Mode: Recieve");
            m_mode = NETWORK::CORE::MODE::RECIEVE;
            setNetworkMode(m_mode);
        }
        else if(ui->radioButtonSendPackets->isChecked()) {
            if (ui->lineEditPathSelectedInfo->text() != ""){
                ui->sliderAudioSlider->setEnabled(true);
                ui->pushButtonStartPlayer->setEnabled(true);
            }
            ui->pushButtonSetPacketSize->setEnabled(true);
            ui->pushButtonSelectAudioFile->setEnabled(true);

            ui->labelNetworkMode->setText("Mode: Transmit");
            m_mode = NETWORK::CORE::MODE::SEND;
            setNetworkMode(m_mode);
        }
    });
    connect(ui->sliderVolume, &QSlider::valueChanged, this, &ServerWindow::setVolumeValue);
    connect(ui->pushButtonVolume, &QPushButton::clicked, this, [this](){
        if (ui->sliderVolume->value() != 0){
            ui->sliderVolume->setValue(0);
        }
        else
            ui->sliderVolume->setValue(100);
    });

    connect(ui->sliderAudioSlider, &QSlider::sliderPressed, [this]() {
        m_sliderPressed = true;
    });

    connect(ui->sliderAudioSlider, &QSlider::sliderReleased, [this]() {
        m_sliderPressed = false;
        float value = ui->sliderAudioSlider->value();
        emit setPlaybackPosition(value);
    });

    connect(ui->sliderAudioSlider, &QSlider::valueChanged, [this](int value) {
        if (m_sliderPressed) {
            float val_float = value;
            emit setPlaybackPosition(val_float);
        }
    });

    connect(ui->pushButtonSetPacketSize, &QPushButton::clicked, this, &ServerWindow::setPacketSize);
}

void ServerWindow::playbackPositionChanged(float pos)
{
    ui->sliderAudioSlider->blockSignals(true);
    if (!m_sliderPressed) {
        float val = pos * 100.;
        ui->sliderAudioSlider->setValue(val);
    }
    ui->sliderAudioSlider->blockSignals(false);
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

        ui->pushButtonOpenConnection->setEnabled(false);
        ui->pushButtonCloseConnection->setEnabled(true);
        ui->pushButtonChangeNetworkMode->setEnabled(false);

        emit openConnectionNetwork(address, port16);
    } else if (senderButton == ui->pushButtonCloseConnection) {
        ui->pushButtonOpenConnection->setEnabled(true);
        ui->pushButtonCloseConnection->setEnabled(false);
        ui->pushButtonChangeNetworkMode->setEnabled(true);
        emit closeConnectionNetwork();
    }
}

void ServerWindow::startAudio()
{
    if (ui->radioButtonSendPackets->isChecked())
        emit audioPlayerChangeState(AUDIO::HANDLER::MODE::START);
    else if(ui->radioButtonRecievePackets->isChecked()){
            // emit audioPlayerChangeState(AUDIO::HANDLER::MODE::START);
    }
}

void ServerWindow::restartAudio()
{
    emit audioPlayerChangeState(AUDIO::HANDLER::MODE::RESTART);
}

void ServerWindow::stopAudio()
{
    emit audioPlayerChangeState(AUDIO::HANDLER::MODE::STOP);
}

void ServerWindow::setVolumeValue(int value)
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
        m_seriesAudioSamples->setColor(Qt::yellow);
        m_chartAudioSamples->addSeries(m_seriesAudioSamples);
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
            tag = "<font color='#19b2e6'>[NETWORK]</font> ";
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
        m_logLines.append(newLine);

        int maxLines = ui->spinBoxLoggerCapacity->value();
        if (m_logLines.size() > maxLines) {
            m_logLines.removeFirst();
        }

        ui->textBrowserAppLogger->clear();
        foreach (const QString &line, m_logLines) {
            ui->textBrowserAppLogger->append(line);
        }
    }
}

void ServerWindow::setPacketSize()
{
    int packetSize = qBound(100, ui->lineEditPacketSize->text().toInt(), 1500);
    ui->labelCurrentPacketSize->setText(QString::number(packetSize));
    emit sendPacketSize(static_cast<quint16>(packetSize));
}

void ServerWindow::clearLog()
{
    m_logLines.clear();
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
        if (ui->radioButtonRecievePackets->isChecked()){
            ui->pushButtonStartPlayer->setEnabled(false);
            ui->pushButtonRestartPlayer->setEnabled(false);
            ui->pushButtonStopPlayer->setEnabled(false);
            break;
        }
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
        ui->sliderAudioSlider->setEnabled(true);
        emit audioFileStartProcessing(filePath);

        QFileInfo fileInfo(filePath);
        QString fileInfoText = "<font color='yellow'>File Information:</font><br><br>";
        fileInfoText += "<font color='lightblue'>Full Path:</font> " + fileInfo.absoluteFilePath() + "<br>";
        fileInfoText += "<font color='lightblue'>File Name:</font> " + fileInfo.fileName() + "<br>";
        fileInfoText += "<font color='lightblue'>File Extension:</font> " + fileInfo.suffix() + "<br>";
        fileInfoText += "<font color='lightblue'>File Size:</font> " + QString::number(fileInfo.size()) + " bytes<br>";
        fileInfoText += "<font color='lightblue'>Creation Date:</font> " + fileInfo.birthTime().toString() + "<br>";
        fileInfoText += "<font color='lightblue'>Last Modified Date:</font> " + fileInfo.lastModified().toString() + "<br>";
        fileInfoText += "<font color='lightblue'>Last Accessed Date:</font> " + fileInfo.lastRead().toString() + "<br>";

        ui->textBrowserFileInfo->setHtml(fileInfoText);

    }
}

ServerWindow::~ServerWindow()
{
    delete ui;
}
