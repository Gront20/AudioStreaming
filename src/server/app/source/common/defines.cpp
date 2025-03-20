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
