#include "defines.h"

QString AUDIO::CORE::errorCodeToString(const AUDIO::CORE::ERROR_HANDLER &errorCode)
{
    QString message{};
    switch (errorCode) {
    case AUDIO::CORE::ERROR_HANDLER::STREAM_INFO:
        message = QString("Error occurred while getting stream info!");
        break;
    case AUDIO::CORE::ERROR_HANDLER::STREAM_ERROR:
        message = QString("Stream not found!");
        break;
    case AUDIO::CORE::ERROR_HANDLER::PA_INIT:
        message = QString("PortAudio init failed!");
        break;
    case AUDIO::CORE::ERROR_HANDLER::PA_ERROR:
        message = QString("PortAudio error!");
        break;
    case AUDIO::CORE::ERROR_HANDLER::DEVICE_ERROR:
        message = QString("No default audio output device!");
        break;
    case AUDIO::CORE::ERROR_HANDLER::SWRESAMPLE_ERROR:
        message = QString("Swresample not initialized!");
        break;
    case AUDIO::CORE::ERROR_HANDLER::BUFFER_EMPTY:
        message = QString("Buffer is empty!");
        break;
    default:
        message = QString("Unknown error occurred.");
        break;
    }
    return message;
}

QString AUDIO::CORE::audioStatusToString(const AUDIO::CORE::STATUS &status)
{
    switch (status) {
    case AUDIO::CORE::STATUS::READY:
        return QString("Audio ready to play!");
    case AUDIO::CORE::STATUS::START:
        return QString("Audio started!");
    case AUDIO::CORE::STATUS::STOP:
        return QString("Audio stopped!");
    case AUDIO::CORE::STATUS::UNDEFINED:
    default:
        return QString("Audio has unknown status");
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
    case NETWORK::CORE::STATUS::FAILED_READ:
        message = QString("Failed to read data!");
        break;
    case NETWORK::CORE::STATUS::ERROR_OPUS_INIT:
        message = QString("Opus error initializaion!");
        break;
    case NETWORK::CORE::STATUS::ERROR_OPUS_DECODE:
        message = QString("Opus decoding error!");
        break;
    case NETWORK::CORE::STATUS::ERROR_SOCKET:
        message = QString("Socket occurs error.");
        break;
    case NETWORK::CORE::STATUS::STATE_CHANGED:
        message = QString("Socket state changed.");
        break;
    case NETWORK::CORE::STATUS::RECEIVE_PACKET:
        message = QString("Packet recieved.");
        break;
    case NETWORK::CORE::STATUS::BOUNDED:
        message = QString("UDP socket bounded");
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
