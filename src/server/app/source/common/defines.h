#pragma once

#include <QHostAddress>
#include <QString>
#include <QMetaType>


#define DEFAULT_IP QHostAddress::LocalHost
#define DEFAULT_PORT 8080

#define DEFAULT_CHANNELS 2
#define DEFAULT_SAMPLERATE 48000
#define DEFAULT_BITRATE 128000
#define DEFAULT_FRAMES_PER_BUFFER 960
#define DEFAULT_ID 123456
#define DEFAULT_PACKETSIZE 200

#define DEFAULT_VOLUME 50

namespace APPCORE {

enum class INITILIZE : int {
    SUCCESS,
    ERROR_INIT,
    UNDEFINED
};

enum class HANDLERTYPE {
    CORE,
    AUDIO,
    NETWORK,
    GUI,
    UNDEFINDED
};

}

namespace NETWORK {

enum class STATUS : int {
    DISCONNECTED = 0,
    CONNECTING,
    CONNECTED,
    CONNECTION_LOST,
    UNDEFINDED
};

namespace CORE {
enum class STATUS : int {
    ALREADY_BOUNDED = 1,
    FAILED_BIND,
    BOUNDED,
    ERROR_SOCKET,
    ERROR_OPUS_INIT,
    ERROR_OPUS_ENCODE,
    STATE_CHANGED,
    SEND_PACKET,
    CLOSED
};

QString networkCoreStatusToString(const STATUS code);

}
};

namespace AUDIO {

namespace HANDLER {

enum class STATUS : int {
    ERROR_AUDIO = -1, ///< file is ready or not
    PROCESSING,
    ISREADY,
    UNDEFINDED
};

enum class MODE : int {
    START, ///< media player current mode
    STOP,
    RESTART,
    UNDEFINED
};

}

namespace CORE {

enum class STATUS : int {
    READY,
    START,
    RESUME,
    END,
    RESTART,
    STOP,
    UNDEFINED
};

enum class ERROR_HANDLER : int {
    FILE_READ,
    STREAM_INFO,
    STREAM_ERROR,
    CODEC_ERROR,
    SWRESAMPLE_ERROR,
    BUFFER_EMPTY
};

QString errorCodeToString(const ERROR_HANDLER &errorCode);
QString audioStatusToString(const STATUS &status, const QString &fileName);
}
}
