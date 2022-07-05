#include "wave/header.h"

#include "wave/header/riff_header.h"

namespace QWave {
    Error Header::Init(QIODevice *stream, qint64 position) {
        position_ = position;
        if (!stream->isOpen()) {
            return Error::kNotOpen;
        }

        // read chunk ID
        const auto chunk_id_size = 4;
        stream->seek(position_);
        char result[chunk_id_size];
        stream->read(result, chunk_id_size * sizeof(char));
        id_ = QByteArray(result, chunk_id_size);

        // and size
        stream->read(reinterpret_cast<char *>(&size_), sizeof(quint32));
        size_ += chunk_id_size * sizeof(char) + sizeof(quint32);

        return Error::kNoError;
    }

    QByteArray Header::chunk_id() const {
        return id_;
    }

    quint32 Header::chunk_size() const {
        if (chunk_id() == "RIFF") {
            return sizeof(QWave::RIFFHeader);
        }
        return size_;
    }

    qint64 Header::position() const {
        return position_;
    }
}; // namespace wave
