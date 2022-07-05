#ifndef WAVE_WAVE_HEADER_LIST_H_
#define WAVE_WAVE_HEADER_LIST_H_

#include <QFile>

#include "wave/error.h"
#include "wave/header.h"

namespace QWave {

    class WAVE_API HeaderList {
    public:
        class Iterator {
        public:
            Iterator(QIODevice *stream, qint64 position);
            Iterator operator++();
            Iterator operator++(int);
            Header operator*();
            bool operator==(const Iterator &rhs);
            bool operator!=(const Iterator &rhs);

        private:
            QIODevice *stream_;
            qint64 position_;
        };

        Error Init(const QString &path);

        Iterator begin();
        Iterator end();

        Header riff();
        Header fmt();
        Header data();

    private:
        Header header(const QByteArray &header_id);

        QFile stream_;
    };

} // namespace wave

#endif // WAVE_WAVE_HEADER_LIST_H_
