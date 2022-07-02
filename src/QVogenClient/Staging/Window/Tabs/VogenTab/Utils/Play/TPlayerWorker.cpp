#include "TPlayerWorker.h"

#include "TMultiPlayer_p.h"

#include <QThread>

#define SAMPLE_RATE 44100

TPlayerWorker::TPlayerWorker(QObject *parent) : QObject(parent) {
    sig = 0;
    time = 0;

    format.setSampleRate(44100);
    format.setSampleSize(16);
    format.setChannelCount(1);
    format.setCodec("audio/pcm");
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
}

TPlayerWorker::~TPlayerWorker() {
}

bool TPlayerWorker::test() {
    // auto infos = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    // for (const QAudioDeviceInfo &info : infos) {
    //     qDebug() << info.deviceName() << info.isFormatSupported(format);
    // }
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        qDebug() << "Raw audio format not supported by backend, cannot play audio.";
        return false;
    }
    return true;
}

void TPlayerWorker::start() {
    QAudioOutput out(format);
    auto dev = out.start();

    QList<int> indexes;
    quint64 startTime = time;
    int aheadSamples = 5 * SAMPLE_RATE;

    // Init Index List
    for (int i = 0; i < ref->tracks.size(); ++i) {
        int index = -1;

        const auto &track = ref->tracks.at(i);
        const auto &samples = track.wave->channel1();

        if (time >= track.time + track.wave->duration() * 1000) {
            index = samples.size();
        } else if (time > track.time) {
            index = double(time - track.time) / 1000 * SAMPLE_RATE;
        }

        indexes.append(index);
    }

    Q_ASSERT(dev);

    QByteArray pcm; // Buffer

    while (true) {
        // Update Time
        qint64 time = startTime + qMax<qint64>(0, out.processedUSecs() / 1000 - 150);
        this->time = time;

        int samples_needed = qMax(0, aheadSamples - pcm.size() / 2);
        qint64 time_ahead = double(samples_needed) / SAMPLE_RATE * 1000;

        QVector<qint32> mergedSamples;
        mergedSamples.reserve(samples_needed);
        for (int i = 0; i < samples_needed; ++i) {
            mergedSamples.append(0);
        }

        // Update Buffer
        for (int i = 0; i < indexes.size(); ++i) {
            const auto &track = ref->tracks.at(i);
            const auto &samples = track.wave->channel1();

            int &index = indexes[i];
            if (index >= samples.size()) {
                continue;
            } else if (index < 0) {
                qint64 delta_time = track.time - time;
                if (delta_time >= time_ahead) {
                    // One More Wait
                    continue;
                } else {
                    // Cut First
                    int cut_samples = double(time_ahead - delta_time) / 1000 * SAMPLE_RATE;
                    cut_samples = qMin(samples.size(), cut_samples);

                    for (int i = 0; i < cut_samples; ++i) {
                        mergedSamples[mergedSamples.size() - cut_samples + i] += samples[i];
                    }
                    index = cut_samples;
                }
            } else {
                // Cut
                int cut_samples = double(time_ahead) / 1000 * SAMPLE_RATE;
                cut_samples = qMin(samples.size() - index, cut_samples);

                for (int i = 0; i < cut_samples; ++i) {
                    mergedSamples[i] += samples[index + i];
                }
                index += cut_samples;
            }
        }
        for (auto sample : qAsConst(mergedSamples)) {
            short num = sample;
            pcm.append(QByteArray((char *) &num, 2));
        }

        // Update Output
        int read_size = out.periodSize();
        int bytes_free = out.bytesFree();

        int chunks = bytes_free / read_size;
        while (chunks) {
            QByteArray samples = pcm.mid(0, read_size);
            int len = samples.size();
            if (len != read_size) {
                break;
            }
            pcm.remove(0, len);

            if (len > 0) {
                dev->write(samples);
            }
            chunks--;
        }

        // Check if finished
        if (sig > 0) {
            break;
        }
    }

    emit finished();
}
