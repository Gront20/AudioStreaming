#include "defines.h"

QString AUDIO::CORE::errorCodeToString(const AUDIO::CORE::ERROR_HANDLER &errorCode)
{
    QString message{};
    switch (errorCode) {
    case AUDIO::CORE::ERROR_HANDLER::FILE_READ:
        message = QString("Error occurred while reading file.");
        break;
    case AUDIO::CORE::ERROR_HANDLER::STREAM_INFO:
        message = QString("Error occurred while getting stream info.");
        break;
    case AUDIO::CORE::ERROR_HANDLER::STREAM_ERROR:
        message = QString("Stream not found.");
        break;
    case AUDIO::CORE::ERROR_HANDLER::SWRESAMPLE_ERROR:
        message = QString("Swresample not initialized.");
        break;
    case AUDIO::CORE::ERROR_HANDLER::BUFFER_EMPTY:
        message = QString("Buffer is empty.");
        break;
    case AUDIO::CORE::ERROR_HANDLER::PA_ERROR:
        message = QString("Stream error.");
        break;
    default:
        message = QString("Unknown error occurred.");
        break;
    }
    return message;
}

QString AUDIO::CORE::audioStatusToString(const AUDIO::CORE::STATUS &status, const QString &fileName)
{
    switch (status) {
    case AUDIO::CORE::STATUS::READY:
        return QString("Audio file \"%1\" ready to play!").arg(fileName);
    case AUDIO::CORE::STATUS::START:
        return QString("Audio file \"%1\" started!").arg(fileName);
    case AUDIO::CORE::STATUS::RESUME:
        return QString("Audio file \"%1\" resumed!").arg(fileName);
    case AUDIO::CORE::STATUS::END:
        return QString("Audio file \"%1\" ended!").arg(fileName);
    case AUDIO::CORE::STATUS::RESTART:
        return QString("Audio file \"%1\" restarted!").arg(fileName);
    case AUDIO::CORE::STATUS::STOP:
        return QString("Audio file \"%1\" stopped!").arg(fileName);
    case AUDIO::CORE::STATUS::UNDEFINED:
    default:
        return QString("Audio file \"%1\" has unknown status").arg(fileName);
    }
}


QString NETWORK::CORE::networkCoreStatusToString(const NETWORK::CORE::STATUS code)
{
    QString message{};
    switch (code) {
    case NETWORK::CORE::STATUS::ALREADY_BOUNDED:
        message = QString("UDP socket is already bound to the same address and port.");
        break;
    case NETWORK::CORE::STATUS::FAILED_BIND:
        message = QString("Failed to bind UDP socket!");
        break;
    case NETWORK::CORE::STATUS::ERROR_OPUS_INIT:
        message = QString("Opus error initializaion!");
        break;
    case NETWORK::CORE::STATUS::ERROR_OPUS_ENCODE:
        message = QString("Opus encoding error!");
        break;
    case NETWORK::CORE::STATUS::ERROR_SOCKET:
        message = QString("Socket occurs error.");
        break;
    case NETWORK::CORE::STATUS::STATE_CHANGED:
        message = QString("Socket state changed.");
        break;
    case NETWORK::CORE::STATUS::SEND_PACKET:
        message = QString("Packet sended.");
        break;
    case NETWORK::CORE::STATUS::RECEIVE_PACKET:
        message = QString("Packet recieved.");
        break;
    case NETWORK::CORE::STATUS::MODE_RECIEVE_SWITCHED:
        message = QString("Network mode switched to RECIEVE.");
        break;
    case NETWORK::CORE::STATUS::MODE_SEND_SWITCHED:
        message = QString("Network mode switched to TRANSMIT");
        break;
    case NETWORK::CORE::STATUS::BOUNDED:
        message = QString("UDP socket ready");
        break;
    case NETWORK::CORE::STATUS::CLOSED:
        message = QString("UDP socket closed connection!");
        break;
    default:
        message = QString("Unknown error occurred.");
        break;
    }
    return message;
}

