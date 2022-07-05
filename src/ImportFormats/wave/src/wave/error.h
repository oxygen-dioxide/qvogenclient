#ifndef WAVE_WAVE_ERROR_H_
#define WAVE_WAVE_ERROR_H_

#include <QtGlobal>

#ifndef WAVE_API

#ifdef WAVE_BUILD_STATIC
    #define WAVE_API
#else
    #ifdef _WIN32
        #ifdef WAVE_LIBRARY
            #define WAVE_API Q_DECL_EXPORT
        #else
            #define WAVE_API Q_DECL_IMPORT
        #endif
    #else
        #define WAVE_API
    #endif
#endif

#endif

namespace QWave {

    enum Error {
        kNoError = 0,
        kFailedToOpen,
        kNotOpen,
        kInvalidFormat,
        kWriteError,
        kReadError,
        kInvalidSeek
    };

} // namespace wave

#endif // WAVE_WAVE_ERROR_H_
