#ifndef WAVE_WAVE_HEADER_LIST_H_
#define WAVE_WAVE_HEADER_LIST_H_

#include <fstream>

#include "wave/error.h"
#include "wave/header.h"

namespace wave {

    class WAVE_API HeaderList {
    public:
        class Iterator {
        public:
            Iterator(std::ifstream *stream, uint64_t position);
            Iterator operator++();
            Iterator operator++(int);
            Header operator*();
            bool operator==(const Iterator &rhs);
            bool operator!=(const Iterator &rhs);

        private:
            std::ifstream *stream_;
            uint64_t position_;
        };

        Error Init(const std::wstring &path);
        Iterator begin();
        Iterator end();

        Header riff();
        Header fmt();
        Header data();

    private:
        Header header(const std::string &header_id);
        std::ifstream stream_;
    };
} // namespace wave

#endif // WAVE_WAVE_HEADER_LIST_H_
