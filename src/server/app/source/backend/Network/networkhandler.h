#ifndef NETWORKHANDLER_H
#define NETWORKHANDLER_H

#include <QObject>
#include "networkcore.h"
#include "defines.h"

class NetworkHandler : public QObject
{
    Q_OBJECT
public:
    explicit NetworkHandler(QObject *parent = nullptr);
    ~NetworkHandler();

public slots:

    void sendAudioSamples(const std::vector<float> &samples);
    void handleAudioStatusUpdate(const AUDIO::CORE::STATUS &status);

private:

    NetworkCore m_networkCoreObject;

};

#endif // NETWORKHANDLER_H
