#include "wave/header/data_header.h"

#include <QString>

namespace QWave {

    DataHeader MakeDataHeader() {
        DataHeader header;
        memcpy(header.sub_chunk_2_id, "data", 4);
        return header;
    }

} // namespace QWave
