#include "wave/file.h"

#include <QDebug>
#include <QFile>

#include "wave/header/data_header.h"
#include "wave/header/fmt_header.h"
#include "wave/header/riff_header.h"
#include "wave/header/wave_header.h"
#include "wave/header_list.h"

#define INT24_MAX 8388607

namespace QWave {

    namespace internal {
        void NoEncrypt(char *data, size_t size) {
            Q_UNUSED(data);
            Q_UNUSED(size);
        }
        void NoDecrypt(char *data, size_t size) {
            Q_UNUSED(data);
            Q_UNUSED(size);
        }
    } // namespace internal

    enum Format {
        WAVE_FORMAT_PCM = 0x0001,
        WAVE_FORMAT_IEEE_FLOAT = 0x0003,
        WAVE_FORMAT_ALAW = 0x0006,
        WAVE_FORMAT_MULAW = 0x0007,
        WAVE_FORMAT_EXTENSIBLE = 0xFFFE
    };

    class File::Impl {
    public:
        Error WriteHeader(qint64 data_size) {
            if (!ostream.isOpen()) {
                return kNotOpen;
            }

            auto original_position = ostream.pos();
            // Position to beginning of file
            ostream.seek(0);

            // make header
            auto bits_per_sample = header.fmt.bits_per_sample;
            auto bytes_per_sample = bits_per_sample / 8;
            auto channel_number = header.fmt.num_channel;
            auto sample_rate = header.fmt.sample_rate;

            header.riff.chunk_size = sizeof(WAVEHeader) + (data_size * (bits_per_sample / 8)) - 8;
            // fmt header
            header.fmt.byte_per_block = bytes_per_sample * channel_number;
            header.fmt.byte_rate = sample_rate * header.fmt.byte_per_block;
            // data header
            header.data.sub_chunk_2_size = data_size * bytes_per_sample;

            qDebug() << QByteArray((char *) (&header), sizeof(header)).toHex();

            auto cnt = ostream.write(reinterpret_cast<char *>(&header), sizeof(WAVEHeader));
            if (cnt != sizeof(WAVEHeader)) {
                return kWriteError;
            }

            // reposition to old position if was > to current position
            if (ostream.pos() < original_position) {
                ostream.seek(original_position);
            }

            // the offset of data will be right after the headers
            data_offset_ = sizeof(WAVEHeader);
            return kNoError;
        }

        template <typename T>
        void ReadHeader(Header generic_header, T *output) {
            istream.seek(generic_header.position());
            istream.read(reinterpret_cast<char *>(output), sizeof(T));
        }

        Error ReadHeader(HeaderList *headers) {
            if (!istream.isOpen()) {
                return kNotOpen;
            }
            auto file_size = istream.size();
            // If not enough data
            if (file_size < (int) sizeof(WAVEHeader)) {
                return kInvalidFormat;
            }
            istream.seek(0);

            // read headers
            ReadHeader(headers->riff(), &header.riff);
            ReadHeader(headers->fmt(), &header.fmt);
            ReadHeader(headers->data(), &header.data);
            // data offset is right after data header's ID and size
            auto data_header = headers->data();
            data_offset_ = data_header.position() + sizeof(data_header.chunk_size()) +
                           (data_header.chunk_id().size() * sizeof(char));

            // check headers ids (make sure they are set)
            if (QByteArray(header.riff.chunk_id, 4) != "RIFF") {
                return kInvalidFormat;
            }
            if (QByteArray(header.riff.format, 4) != "WAVE") {
                return kInvalidFormat;
            }
            if (QByteArray(header.fmt.sub_chunk_1_id, 4) != "fmt ") {
                return kInvalidFormat;
            }
            if (QByteArray(header.data.sub_chunk_2_id, 4) != "data") {
                return kInvalidFormat;
            }

            // we only support 8 / 16 / 32  bit per sample
            auto bps = header.fmt.bits_per_sample;
            if (bps != 8 && bps != 16 && bps != 32) {
                return kInvalidFormat;
            }

            // And only support uncompressed PCM format
            if (header.fmt.audio_format != Format::WAVE_FORMAT_PCM) {
                return kInvalidFormat;
            }

            return kNoError;
        }

        qint64 current_sample_index() {
            auto bits_per_sample = header.fmt.bits_per_sample;
            auto bytes_per_sample = bits_per_sample / 8;
            qint64 data_index = 0;
            if (ostream.isOpen()) {
                data_index = static_cast<qint64>(ostream.pos()) - data_offset_;
            } else if (istream.isOpen()) {
                data_index = static_cast<qint64>(istream.pos()) - data_offset_;
            } else {
                return 0;
            }
            return data_index / bytes_per_sample;
        }

        void set_current_sample_index(qint64 sample_idx) {
            auto bits_per_sample = header.fmt.bits_per_sample;
            auto bytes_per_sample = bits_per_sample / 8;

            std::streampos stream_index = data_offset_ + (sample_idx * bytes_per_sample);
            if (ostream.isOpen()) {
                ostream.seek(stream_index);
            } else if (istream.isOpen()) {
                istream.seek(stream_index);
            }
        }

        qint64 sample_number() {
            auto bits_per_sample = header.fmt.bits_per_sample;
            auto bytes_per_sample = bits_per_sample / 8;

            auto total_data_size = header.data.sub_chunk_2_size;
            return total_data_size / bytes_per_sample;
        }

        QFile istream;
        QFile ostream;

        WAVEHeader header;
        qint64 data_offset_;
    };

    File::File()
        : impl_(new Impl()) {
        impl_->header = MakeWAVEHeader(); // Use Constructor
    }
    File::~File() {
        if (impl_ != nullptr && impl_->istream.isOpen()) {
            impl_->ostream.flush();
        }
    }

    Error File::Open(const QString &path, OpenMode mode) {
        // Write
        if (mode == OpenMode::kOut) {
            impl_->ostream.setFileName(path);
            if (!impl_->ostream.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
                return Error::kFailedToOpen;
            }
            return impl_->WriteHeader(0);
        }

        // Read
        impl_->istream.setFileName(path);
        if (!impl_->istream.open(QIODevice::ReadOnly)) {
            return Error::kFailedToOpen;
        }

        HeaderList headers;
        auto error = headers.Init(path);
        if (error != kNoError) {
            return error;
        }

        return impl_->ReadHeader(&headers);
    }

    quint16 File::channel_number() const {
        return impl_->header.fmt.num_channel;
    }

    void File::set_channel_number(quint16 channel_number) {
        impl_->header.fmt.num_channel = channel_number;
    }

    quint32 File::sample_rate() const {
        return impl_->header.fmt.sample_rate;
    }
    void File::set_sample_rate(quint32 sample_rate) {
        impl_->header.fmt.sample_rate = sample_rate;
    }

    quint16 File::bits_per_sample() const {
        return impl_->header.fmt.bits_per_sample;
    }
    void File::set_bits_per_sample(quint16 bits_per_sample) {
        impl_->header.fmt.bits_per_sample = bits_per_sample;
    }

    qint64 File::frame_number() const {
        return impl_->sample_number() / channel_number();
    }

    Error File::Read(QVector<float> *output) {
        return Read(internal::NoDecrypt, output);
    }

    Error File::Read(void (*decrypt)(char *, size_t), QVector<float> *output) {
        return Read(frame_number(), decrypt, output);
    }

    Error File::Read(qint64 frame_number, QVector<float> *output) {
        return Read(frame_number, internal::NoDecrypt, output);
    }

    Error File::Read(qint64 frame_number, void (*decrypt)(char *, size_t), QVector<float> *output) {
        if (!impl_->istream.isOpen()) {
            return kNotOpen;
        }
        auto requested_samples = frame_number * channel_number();

        // check if we have enough data available
        if (impl_->sample_number() < requested_samples + impl_->current_sample_index()) {
            return kInvalidFormat;
        }
        // resize output to desired size
        output->resize(requested_samples);

        // read every sample one after another
        for (int sample_idx = 0; sample_idx < output->size(); sample_idx++) {
            if (impl_->header.fmt.bits_per_sample == 8) {
                // 8bits case
                qint8 value;
                impl_->istream.read(reinterpret_cast<char *>(&value), sizeof(value));
                decrypt(reinterpret_cast<char *>(&value), sizeof(value) / sizeof(char));
                (*output)[sample_idx] =
                    static_cast<float>(value) / std::numeric_limits<qint8>::max();
            } else if (impl_->header.fmt.bits_per_sample == 16) {
                // 16 bits
                qint16 value;
                impl_->istream.read(reinterpret_cast<char *>(&value), sizeof(value));
                decrypt(reinterpret_cast<char *>(&value), sizeof(value) / sizeof(char));
                (*output)[sample_idx] =
                    static_cast<float>(value) / std::numeric_limits<qint16>::max();
            } else if (impl_->header.fmt.bits_per_sample == 24) {
                // 24bits int doesn't exist in c++. We create a 3 * 8bits struct to
                // simulate
                unsigned char value[3];
                impl_->istream.read(reinterpret_cast<char *>(&value), sizeof(value));
                decrypt(reinterpret_cast<char *>(&value), sizeof(value) / sizeof(char));
                int integer_value;
                // check if value is negative
                if (value[2] & 0x80) {
                    integer_value =
                        (0xff << 24) | (value[2] << 16) | (value[1] << 8) | (value[0] << 0);
                } else {
                    integer_value = (value[2] << 16) | (value[1] << 8) | (value[0] << 0);
                }
                (*output)[sample_idx] = static_cast<float>(integer_value) / INT24_MAX;
            } else if (impl_->header.fmt.bits_per_sample == 32) {
                // 32bits
                qint32 value;
                impl_->istream.read(reinterpret_cast<char *>(&value), sizeof(value));
                decrypt(reinterpret_cast<char *>(&value), sizeof(value) / sizeof(char));
                (*output)[sample_idx] =
                    static_cast<float>(value) / (float) std::numeric_limits<qint32>::max();
            } else {
                return kInvalidFormat;
            }
        }
        return kNoError;
    }

    Error File::Write(const QVector<float> &data, bool clip) {
        return Write(data, internal::NoEncrypt, clip);
    }

    Error File::Write(const QVector<float> &data, void (*encrypt)(char *data, size_t size),
                      bool clip) {
        if (!impl_->ostream.isOpen()) {
            return kNotOpen;
        }

        auto current_data_size = impl_->current_sample_index();
        auto bits_per_sample = impl_->header.fmt.bits_per_sample;

        // write each samples
        for (auto sample : data) {
            // hard-clip if asked
            if (clip) {
                if (sample > 1.f) {
                    sample = 1.f;
                } else if (sample < -1.f) {
                    sample = -1.f;
                }
            }
            if (bits_per_sample == 8) {
                // 8bits case
                qint8 value = static_cast<qint8>(sample * std::numeric_limits<qint8>::max());
                encrypt(reinterpret_cast<char *>(&value), sizeof(value) / sizeof(char));
                impl_->ostream.write(reinterpret_cast<char *>(&value), sizeof(value));
            } else if (bits_per_sample == 16) {
                // 16 bits
                qint16 value = static_cast<qint16>(sample * std::numeric_limits<qint16>::max());
                encrypt(reinterpret_cast<char *>(&value), sizeof(value) / sizeof(char));
                impl_->ostream.write(reinterpret_cast<char *>(&value), sizeof(value));
            } else if (bits_per_sample == 24) {
                // 24bits int doesn't exist in c++. We create a 3 * 8bits struct to
                // simulate
                int v = sample * INT24_MAX;
                qint8 value[3];
                value[0] = reinterpret_cast<char *>(&v)[0];
                value[1] = reinterpret_cast<char *>(&v)[1];
                value[2] = reinterpret_cast<char *>(&v)[2];

                encrypt(reinterpret_cast<char *>(&value), sizeof(value) / sizeof(char));
                impl_->ostream.write(reinterpret_cast<char *>(&value), sizeof(value));
            } else if (bits_per_sample == 32) {
                // 32bits
                qint32 value =
                    static_cast<qint32>(sample * (float) std::numeric_limits<qint32>::max());
                encrypt(reinterpret_cast<char *>(&value), sizeof(value) / sizeof(char));
                impl_->ostream.write(reinterpret_cast<char *>(&value), sizeof(value));
            } else {
                return kInvalidFormat;
            }
        }

        // update header to show the right data size
        impl_->WriteHeader(current_data_size + data.size());

        return kNoError;
    }

    Error File::Seek(qint64 frame_index) {
        if (!impl_->ostream.isOpen() && !impl_->istream.isOpen()) {
            return kNotOpen;
        }
        if (frame_index > frame_number()) {
            return kInvalidSeek;
        }

        impl_->set_current_sample_index(frame_index * channel_number());
        return kNoError;
    }

    qint64 File::Tell() const {
        if (!impl_->ostream.isOpen() && !impl_->istream.isOpen()) {
            return 0;
        }

        auto sample_position = impl_->current_sample_index();
        return sample_position / channel_number();
    }


#if __cplusplus >= 201103L
    File::File(File &&other)
        : impl_(nullptr) {
        impl_.reset(other.impl_.take());
    }

    File &File::operator=(File &&other) {
        impl_.reset(other.impl_.take());
        return *this;
    }
#endif // __cplusplus > 201103L

#if __cplusplus > 199711L

    std::error_code make_error_code(Error err) {
        switch (err) {
            case kFailedToOpen:
                return std::make_error_code(std::errc::io_error);
            case kNotOpen:
                return std::make_error_code(std::errc::operation_not_permitted);
            case kInvalidFormat:
                return std::make_error_code(std::errc::executable_format_error);
            case kWriteError:
                return std::make_error_code(std::errc::io_error);
            case kReadError:
                return std::make_error_code(std::errc::io_error);
            default:
                return std::error_code();
        }
    }

    QVector<float> File::Read(std::error_code &err) {
        QVector<float> output;
        auto wave_error = Read(&output);
        err = make_error_code(wave_error);
        return output;
    }

    QVector<float> File::Read(qint64 frame_number, std::error_code &err) {
        QVector<float> output;
        auto wave_error = Read(frame_number, &output);
        err = make_error_code(wave_error);
        return output;
    }

    void File::Write(const QVector<float> &data, std::error_code &err, bool clip) {
        auto wave_error = Write(data, clip);
        err = make_error_code(wave_error);
    }

    void File::Open(const QString &path, OpenMode mode, std::error_code &err) {
        auto wave_error = Open(path, mode);
        err = make_error_code(wave_error);
    }

#endif // __cplusplus > 199711L

} // namespace QWave
