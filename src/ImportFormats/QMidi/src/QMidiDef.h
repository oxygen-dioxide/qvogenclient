#pragma once

#include <qglobal.h>

#ifdef Q_MIDI_BUILD_STATIC
#define Q_MIDI_API
#else
#ifdef Q_OS_WINDOWS
#define Q_MIDI_API __declspec(dllexport)
#else
#define Q_MIDI_API
#endif
