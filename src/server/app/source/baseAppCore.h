#ifndef BASEAPPCORE_H
#define BASEAPPCORE_H

#include <QObject>
#include <QThread>
#include "defines.h"

#include "serverwindow.h"
#include "audiohandler.h"
#include "networkhandler.h"

class BaseAppCore : public QObject
{
    Q_OBJECT
public:
    // Constructor
    explicit BaseAppCore(QObject *parent = nullptr);
    // Destructor
    ~BaseAppCore();

    void start();

private:

    void objectsInit();
    void initializeMainWindow();
    void initializeAudioHandler();
    void initializeNetworkHandler();
    void handleInitializationProccess(const QString &message, const QString &senderName);

// Main objects
private:
    ServerWindow    *m_mainWindowObject{nullptr}; ///< Main window object
//    std::unique_ptr<ServerLogger> m_serverLoggerObject; ///< Logger object
    AudioHandler    *m_audioHandlerObject{nullptr}; ///< Audio Handler object
    NetworkHandler  *m_networkHandlerObject{nullptr}; ///< Network Handler object

// Threads
private:

    QThread m_appServerLoggerThread; ///< Thread for logger logic
    QThread m_appServerAudioHandlerThread; ///< Thread for audio handler logic
    QThread m_appServerNetworkHandlerThread; ///< Thread for network handler logic

signals:

    void sendMessageInitialization(const QString &message);

public slots:

};

#endif // BASEAPPCORE_H
