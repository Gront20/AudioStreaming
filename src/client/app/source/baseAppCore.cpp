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
    m_mainWindowObject = new ClientWindow(nullptr);
    m_mainWindowObject->setObjectName("MainWindow");

    connect(this, &BaseAppCore::sendMessageInitialization, m_mainWindowObject, &ClientWindow::recieveMessage);

    handleInitializationProccess("Initialization successful!", m_mainWindowObject->objectName());
}

void BaseAppCore::initializeNetworkHandler()
{
    try {
        m_networkHandlerObject = new NetworkHandler(nullptr);
        m_networkHandlerObject->setObjectName("NetworkHandler");

        if (!m_networkHandlerObject)
            qCritical() << "Not initialized!";

        m_networkHandlerObject->moveToThread(&m_appClientNetworkHandlerThread);
        connect(&m_appClientNetworkHandlerThread, &QThread::finished, m_networkHandlerObject, &QObject::deleteLater, Qt::QueuedConnection);

        connect(m_networkHandlerObject, &NetworkHandler::sendAudioDataToAudio, m_audioHandlerObject, &AudioHandler::recieveAudioData, Qt::QueuedConnection);
        connect(m_networkHandlerObject, &NetworkHandler::sendAudioDataToAudio, m_mainWindowObject, &ClientWindow::recieveAudioSamples, Qt::QueuedConnection);
        connect(m_mainWindowObject, &ClientWindow::connectNetwork, m_networkHandlerObject, &NetworkHandler::handleNetworkConnection);

        m_appClientNetworkHandlerThread.start();

        if (!m_appClientNetworkHandlerThread.isRunning()) {
            qCritical() << "Thread not running";
        }

        handleInitializationProccess("Initialization successful!", m_networkHandlerObject->objectName());

    } catch (const std::exception &e) {
        handleInitializationProccess(QString(e.what()), m_networkHandlerObject->objectName());
        m_appClientNetworkHandlerThread.quit();
        m_appClientNetworkHandlerThread.wait();
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

        connect(&m_appClientAudioHandlerThread, &QThread::finished, m_audioHandlerObject, &QObject::deleteLater, Qt::QueuedConnection);
        m_audioHandlerObject->moveToThread(&m_appClientAudioHandlerThread);
;
        connect(m_mainWindowObject, &ClientWindow::audioPlayerChangeState,
                m_audioHandlerObject, &AudioHandler::audioPlayerChangeState);
        connect(m_audioHandlerObject, &AudioHandler::sendErrorMessage,
                m_mainWindowObject, &ClientWindow::recieveMessage);
        connect(m_audioHandlerObject, &AudioHandler::sendAudioStatusMessage,
                m_mainWindowObject, &ClientWindow::recieveMessage);
        connect(m_audioHandlerObject, &AudioHandler::handleAudioStatusUpdate,
                m_mainWindowObject, &ClientWindow::handleAudioStatusUpdate);
        connect(m_audioHandlerObject, &AudioHandler::sendAudioSamplesFromCore,
                m_mainWindowObject, &ClientWindow::recieveAudioSamples);


        m_appClientAudioHandlerThread.start();

        if (!m_appClientAudioHandlerThread.isRunning()) {
            qCritical() << "Thread not running";
            return;
        }
        handleInitializationProccess("Initialization successful!", m_audioHandlerObject->objectName());
    } catch (const std::exception &e) {
        handleInitializationProccess(QString(e.what()), m_audioHandlerObject->objectName());
        m_appClientAudioHandlerThread.requestInterruption();
        m_appClientAudioHandlerThread.quit();
        m_appClientAudioHandlerThread.wait();
    }
}

void BaseAppCore::handleInitializationProccess(const QString &message, const QString &senderName)
{
    emit sendMessageInitialization(senderName + ": " + message);
}

BaseAppCore::~BaseAppCore()
{
    // Network
    m_appClientNetworkHandlerThread.quit();
    m_appClientNetworkHandlerThread.wait();

    // Audio

    m_appClientAudioHandlerThread.quit();
    m_appClientAudioHandlerThread.wait();

    // Logger
    m_appClientLoggerThread.quit();
    m_appClientLoggerThread.wait();
}
