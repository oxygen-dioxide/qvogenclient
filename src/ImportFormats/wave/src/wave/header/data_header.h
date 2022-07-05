#ifndef WAVE_HEADER_DATA_HEADER_H_
#define WAVE_HEADER_DATA_HEADER_H_

#include "../error.h"

namespace QWave {

    struct DataHeader {
        char sub_chunk_2_id[4];
        quint32 sub_chunk_2_size;
    };

    DataHeader WAVE_API MakeDataHeader();

} // namespace QWave

#endif // WAVE_HEADER_DATA_HEADER_H_
