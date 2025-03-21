#include "baseAppCore.h"

#include <QApplication>

BaseAppCore::BaseAppCore(QObject *parent) : QObject(parent)
{
    objectsInit();
}


void BaseAppCore::objectsInit()
{
    initializeMainWindow();
    initializeAudioHandler();
    initializeNetworkHandler();
    //    initializeLogger();
}

void BaseAppCore::start()
{
    m_mainWindowObject->show();
}

void BaseAppCore::initializeMainWindow()
{
    m_mainWindowObject = new ServerWindow(nullptr);
    m_mainWindowObject->setObjectName("MainWindow");

    connect(this, &BaseAppCore::sendMessageInitialization, m_mainWindowObject, &ServerWindow::recieveMessage);

    handleInitializationProccess("Initialization successful!", m_mainWindowObject->objectName());
}

void BaseAppCore::initializeNetworkHandler()
{
    try {
        m_networkHandlerObject = new NetworkHandler();
        m_networkHandlerObject->setObjectName("NetworkHandler");

        if (!m_networkHandlerObject)
            qCritical() << "Not initialized!";

        m_networkHandlerObject->moveToThread(&m_appServerNetworkHandlerThread);

        connect(m_audioHandlerObject, &AudioHandler::handleAudioStatusUpdate, m_networkHandlerObject, &NetworkHandler::handleAudioStatusUpdate);

        connect(&m_appServerNetworkHandlerThread, &QThread::finished, m_networkHandlerObject, &QObject::deleteLater, Qt::QueuedConnection);

        connect(m_audioHandlerObject, &AudioHandler::sendAudioSamplesFromCore, m_networkHandlerObject, &NetworkHandler::sendAudioSamples);
        connect(m_mainWindowObject, &ServerWindow::openConnectionNetwork, m_networkHandlerObject, &NetworkHandler::handleNetworkConnectionOpen, Qt::QueuedConnection);
        connect(m_mainWindowObject, &ServerWindow::closeConnectionNetwork, m_networkHandlerObject, &NetworkHandler::handleNetworkConnectionClose, Qt::QueuedConnection);
        connect(m_audioHandlerObject, &AudioHandler::handleAudioStatusUpdate, m_networkHandlerObject, &NetworkHandler::handleAudioStatusUpdate, Qt::QueuedConnection);
        connect(m_networkHandlerObject, &NetworkHandler::sendMessageToAppLogger, m_mainWindowObject, &ServerWindow::recieveMessage);
        connect(m_networkHandlerObject, &NetworkHandler::sendNetworkDataSended, m_mainWindowObject, &ServerWindow::addPacket);

        m_appServerNetworkHandlerThread.start();

        if (!m_appServerNetworkHandlerThread.isRunning()) {
            qCritical() << "Thread not running";
        }

        handleInitializationProccess("Initialization successful!", m_networkHandlerObject->objectName());

    } catch (const std::exception &e) {
        handleInitializationProccess(QString(e.what()), m_networkHandlerObject->objectName());
        m_appServerNetworkHandlerThread.quit();
        m_appServerNetworkHandlerThread.wait();
    }

}

void BaseAppCore::initializeAudioHandler()
{
    try {
        m_audioHandlerObject = new AudioHandler();
        m_audioHandlerObject->setObjectName("AudioHandler");

        if (!m_audioHandlerObject) {
            qCritical() << "Not initialized!";
            return;
        }

        connect(&m_appServerAudioHandlerThread, &QThread::finished, m_audioHandlerObject, &QObject::deleteLater, Qt::QueuedConnection);
        m_audioHandlerObject->moveToThread(&m_appServerAudioHandlerThread);

        connect(m_mainWindowObject, &ServerWindow::audioFileStartProcessing,
                m_audioHandlerObject, &AudioHandler::receiveAudioFilePath/*, Qt::DirectConnection*/);
        connect(m_mainWindowObject, &ServerWindow::audioPlayerChangeState,
                m_audioHandlerObject, &AudioHandler::audioPlayerChangeState);
        connect(m_audioHandlerObject, &AudioHandler::sendErrorMessage,
                m_mainWindowObject, &ServerWindow::recieveMessage);
        connect(m_audioHandlerObject, &AudioHandler::sendAudioStatusMessage,
                m_mainWindowObject, &ServerWindow::recieveMessage);
        connect(m_audioHandlerObject, &AudioHandler::handleAudioStatusUpdate,
                m_mainWindowObject, &ServerWindow::handleAudioStatusUpdate);
        connect(m_audioHandlerObject, &AudioHandler::sendAudioSamplesFromCore,
                m_mainWindowObject, &ServerWindow::recieveAudioSamples);


        m_appServerAudioHandlerThread.start();

        if (!m_appServerAudioHandlerThread.isRunning()) {
            qCritical() << "Thread not running";
            return;
        }
        handleInitializationProccess("Initialization successful!", m_audioHandlerObject->objectName());
    } catch (const std::exception &e) {
        handleInitializationProccess(QString(e.what()), m_audioHandlerObject->objectName());
        m_appServerAudioHandlerThread.requestInterruption();
        m_appServerAudioHandlerThread.quit();
        m_appServerAudioHandlerThread.wait();
    }
}

void BaseAppCore::handleInitializationProccess(const QString &message, const QString &senderName)
{
    emit sendMessageInitialization(senderName + ": " + message);
}

BaseAppCore::~BaseAppCore()
{
    // Network
    m_appServerNetworkHandlerThread.quit();
    m_appServerNetworkHandlerThread.wait();

    // Audio

    m_appServerAudioHandlerThread.quit();
    m_appServerAudioHandlerThread.wait();

    // Logger
    m_appServerLoggerThread.quit();
    m_appServerLoggerThread.wait();
}
