#pragma once

#include <QHostAddress>
#include <QString>

#define IP_DEFAULT QHostAddress::LocalHost
#define PORT_DEFAULT 8080

// for codec default parameters
#define DEFAULT_SAMPLERATE 48000
#define DEFAULT_CHANNELS 2

namespace APPCORE{

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
    REBINDING,
    FAILED_READ,
    ERROR_SOCKET,
    ERROR_OPUS_INIT,
    ERROR_OPUS_DECODE,
    STATE_CHANGED,
    SEND_PACKET,
    CLOSED
};

QString networkCoreStatusToString(const STATUS code);
}
}

namespace AUDIO {

    namespace HANDLER {

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
            STREAM_INFO,
            STREAM_ERROR,
            PA_INIT,
            PA_ERROR,
            DEVICE_ERROR,
            CODEC_ERROR,
            SWRESAMPLE_ERROR,
            BUFFER_EMPTY
        };

        QString errorCodeToString(const ERROR_HANDLER &errorCode);
        QString audioStatusToString(const STATUS &status);
    }
}
