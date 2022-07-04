#include "TPlayerWorker.h"

#include "TMultiPlayer_p.h"

#include <QDateTime>
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

    qint64 startTime = time;
    qint64 procTime = startTime; // Buffer End Time

    double aheadTime = 0.08;
    int aheadSamples = aheadTime * SAMPLE_RATE;

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

        qint64 time_needed = double(samples_needed) / SAMPLE_RATE * 1000;
        qint64 time_ahead = procTime + time_needed;
        procTime = time_ahead;

        if (samples_needed > 0) {
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
                    qint64 delta = time_ahead - track.time;
                    if (delta < 0) {
                        // One More Wait
                        continue;
                    } else {
                        // Cut First
                        int cut_samples = double(delta) / 1000 * SAMPLE_RATE;
                        int real_cut_samples = qMin(samples.size(), cut_samples);
                        int offset = mergedSamples.size() - cut_samples;
                        for (int i = 0; i < real_cut_samples; ++i) {
                            mergedSamples[offset + i] += samples[i];
                        }
                        index = real_cut_samples;
                    }
                } else {
                    // Cut
                    int cut_samples = double(time_needed) / 1000 * SAMPLE_RATE;
                    int real_cut_samples = qMin(samples.size() - index, cut_samples);
                    for (int i = 0; i < real_cut_samples; ++i) {
                        mergedSamples[i] += samples[index + i];
                    }
                    index += real_cut_samples;
                }
            }
            for (auto sample : qAsConst(mergedSamples)) {
                short num = sample;
                pcm.append(QByteArray((char *) &num, 2));
            }
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
