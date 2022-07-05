#include "wave/header/riff_header.h"

#include <QString>

namespace QWave {

    RIFFHeader MakeRIFFHeader() {
        RIFFHeader header;
        memcpy(header.chunk_id, "RIFF", 4);
        memcpy(header.format, "WAVE", 4);
        return header;
    }

} // namespace QWave
