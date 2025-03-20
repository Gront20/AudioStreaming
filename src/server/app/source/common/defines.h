#pragma once

#include <QHostAddress>
#include <QString>

#define IP_DEFAULT QHostAddress::AnyIPv4
#define PORT_DEFAULT 8080

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
        LOGGER,
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

    // enum class CONNECTION : int {
    //     UNDEFINED,
    //     ERROR
    // };
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
