#pragma once

#include <qglobal.h>

#ifdef Q_MIDI_BUILD_STATIC
    #define Q_MIDI_API
#else
    #ifdef _WIN32
        #ifdef Q_MIDI_LIBRARY
            #define Q_MIDI_API __declspec(dllexport)
        #else
            #define Q_MIDI_API __declspec(dllimport)
        #endif
    #else
        #define Q_MIDI_API
    #endif
#endif
