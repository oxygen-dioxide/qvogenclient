#ifndef WAVE_HEADER_RIFF_HEADER_H_
#define WAVE_HEADER_RIFF_HEADER_H_

#include <cstdint>

#include "../error.h"

namespace wave {

    struct RIFFHeader {
        char chunk_id[4];
        uint32_t chunk_size;
        char format[4];
    };
    RIFFHeader WAVE_API MakeRIFFHeader();

} // namespace wave

#endif // WAVE_HEADER_RIFF_HEADER_H_
