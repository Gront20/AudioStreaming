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

    ui->widgetAppLogger->setStyleSheet(style);
    ui->widgetAudioFileSelector->setStyleSheet(style);
    ui->widgetMediaPlayer->setStyleSheet(style);
    ui->widgetFileInfo->setStyleSheet(style);
    ui->widgetMediaPlayerControls->setStyleSheet(style);
    ui->widgetConnector->setStyleSheet(style);

    ui->pushButtonStartPlayer->setEnabled(false);
    ui->pushButtonStopPlayer->setEnabled(false);
    ui->pushButtonRestartPlayer->setEnabled(false);

}

void ServerWindow::componentsConnections()
{
    connect(ui->pushButtonSelectAudioFile, &QPushButton::clicked, this, &ServerWindow::selectAudioFile, Qt::DirectConnection);
    connect(ui->pushButtonStartPlayer, &QPushButton::clicked, this, &ServerWindow::startAudio, Qt::DirectConnection);
    connect(ui->pushButtonStopPlayer, &QPushButton::clicked, this, &ServerWindow::stopAudio, Qt::DirectConnection);
    connect(ui->pushButtonRestartPlayer, &QPushButton::clicked, this, &ServerWindow::restartAudio, Qt::DirectConnection);
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

void ServerWindow::recieveAudioSamples(const std::vector<float> &samples)
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

HANDLERTYPE ServerWindow::getHandlerType(QObject *sender)
{
    const QString className = sender->metaObject()->className();
    if (className == "BaseAppCore") return HANDLERTYPE::CORE;
    if (className == "AudioHandler") return HANDLERTYPE::AUDIO;
    if (className == "NetworkHandler") return HANDLERTYPE::NETWORK;
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
