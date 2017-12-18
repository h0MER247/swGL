#include <ctime>
#include <stdarg.h>
#include "Log.h"

namespace SWGL {

    Log::Log() {

        fopen_s(&m_file, LOG_FILENAME, "a");
    }

    Log::~Log() {

        if (m_file != nullptr) {

            fclose(m_file);
        }
    }



    Log &Log::getInstance() {

        static Log instance;
        return instance;
    }

    void Log::printf(const char *fmt, ...) {

        if (m_file != nullptr) {

            std::lock_guard<std::mutex> cs(m_mutex);

            tm t;
            time_t tt = time(NULL);
            localtime_s(&t, &tt);

            char timeBuf[256];
            strftime(timeBuf, sizeof(timeBuf) - 1, "[%c] ", &t);
            fputs(timeBuf, m_file);

            char logBuf[LOG_BUFFERSIZE];
            va_list argptr;
            va_start(argptr, fmt);
            _vsnprintf_s(logBuf, LOG_BUFFERSIZE, LOG_BUFFERSIZE - 1, fmt, argptr);
            va_end(argptr);

            fputs(logBuf, m_file);
            #if SWGL_ENABLE_LOG_FLUSH
            fflush(m_file);
            #endif
        }
    }
}
