#include "wave/header_list.h"

namespace QWave {

    HeaderList::Iterator::Iterator(QIODevice *stream, qint64 position)
        : stream_(stream), position_(position) {
    }

    HeaderList::Iterator HeaderList::Iterator::operator++() {
        Header h;
        h.Init(stream_, position_);
        position_ += h.chunk_size();
        return *this;
    }

    HeaderList::Iterator HeaderList::Iterator::operator++(int) {
        // increment operator is called with an integer argument 0
        // see http://en.cppreference.com/w/cpp/language/operators
        Iterator next = *this;
        operator++();
        return next;
    }

    Header HeaderList::Iterator::operator*() {
        Header h;
        h.Init(stream_, position_);
        return h;
    }

    bool HeaderList::Iterator::operator==(const Iterator &rhs) {
        return rhs.stream_ == stream_ && rhs.position_ == position_;
    }
    bool HeaderList::Iterator::operator!=(const Iterator &rhs) {
        return !operator==(rhs);
    }

    Error HeaderList::Init(const QString &path) {
        stream_.setFileName(path);
        if (!stream_.open(QIODevice::ReadOnly)) {
            return Error::kFailedToOpen;
        }
        return Error::kNoError;
    }

    HeaderList::Iterator HeaderList::begin() {
        return HeaderList::Iterator(&stream_, 0);
    }

    HeaderList::Iterator HeaderList::end() {
        return HeaderList::Iterator(&stream_, stream_.size());
    }

    Header HeaderList::header(const QByteArray &header_id) {
        for (auto iterator = begin(); iterator != end(); iterator++) {
            auto header = *iterator;
            if (header.chunk_id() == header_id) {
                return header;
            }
        }
        return *begin();
    }

    Header HeaderList::riff() {
        return header("RIFF");
    }
    Header HeaderList::fmt() {
        return header("fmt ");
    }
    Header HeaderList::data() {
        return header("data");
    }

} // namespace wave
