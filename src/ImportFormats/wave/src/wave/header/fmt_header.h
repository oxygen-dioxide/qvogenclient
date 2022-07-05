#ifndef WAVE_HEADER_FMT_HEADER_H_
#define WAVE_HEADER_FMT_HEADER_H_

#include "../error.h"

namespace QWave {

    struct FMTHeader {
        char sub_chunk_1_id[4];
        quint32 sub_chunk_1_size;
        quint16 audio_format;
        quint16 num_channel;
        quint32 sample_rate;
        quint32 byte_rate;
        quint16 byte_per_block;
        quint16 bits_per_sample;
    };

    FMTHeader WAVE_API MakeFMTHeader();

} // namespace wave

#endif // WAVE_HEADER_FMT_HEADER_H_
