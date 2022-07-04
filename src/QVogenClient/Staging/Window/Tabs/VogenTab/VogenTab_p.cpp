#include "VogenTab_p.h"

#include "DataManager.h"

#include "Utils/Events/PianoRoll/TSetPlayheadEvent.h"
#include "Utils/Events/TDigitTimeSigEvent.h"
#include "Utils/Events/TOperateEvent.h"

#include "SystemHelper.h"

#include "MainWindow.h"
#include "Modules/QCommandPalette.h"

#include "ExtensionManager.h"

#include "RHProtocol.h"

#include <QApplication>
#include <QMessageBox>

#include "wave/file.h"

#include <cstdio>

using namespace QEventImpl;

static const char FLAG_EXPORT_RECENT[] = "%EXPORT_RECENT%";

int VogenTabPrivate::s_untitledIndex = 0;

static QPair<QString, QString> splitLyricRom(const QString &rom) {
    QString newLrc, newRom;

    if (rom != "-") {
        for (const QChar &ch : qAsConst(rom)) {
            if (ch < 128) {
                // Latin1
                newRom.append(ch);
            } else {
                // Unicode
                newLrc.append(ch);
            }
        }
    } else {
        newLrc = "-";
        newRom = "-";
    }

    return qMakePair(newLrc, newRom);
}

VogenTabPrivate::VogenTabPrivate() {
}

VogenTabPrivate::~VogenTabPrivate() {
    clearHistory();

    // Remove Temp Dir
    Sys::rmDir(tempDir);
}

void VogenTabPrivate::init() {
    Q_Q(VogenTab);

    untitledIndex = -1;

    historyIndex = 0;
    savedHistoryIndex = 0;

    // Init UI
    piano = new TPianoPanel(q);
    piano->setObjectName("piano-panel");

    layout = new QGridLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addWidget(piano, 0, 0);

    q->setLayout(layout);

    // Init Temp Dir
    tempDir = qData->allocProjectTempDirName();
    Sys::mkDir(tempDir);
}

bool VogenTabPrivate::saveFile(const QString &filename) {
    Q_Q(VogenTab);

    QVogenFile vog(filename);
    vog.accomPath = this->vog.accomPath;

    TWProject pd = piano->notesArea()->projectData();
    vog.projectName = pd.projectName;
    vog.tempo = pd.tempo;
    vog.beat = pd.beat;
    vog.accomOffset = pd.accomOffset;

    QList<QVogenFile::Utterance> utterances;
    for (const auto &utter : qAsConst(pd.utterances)) {
        QVogenFile::Utterance u;

        u.name = utter.name;
        u.singer = utter.singer;
        u.romScheme = utter.romScheme;

        QList<QVogenFile::Note> notes;
        for (const auto &note : utter.notes) {
            auto pair = splitLyricRom(note.rom);
            QVogenFile::Note p{note.noteNum, pair.first, pair.second, note.start, note.length, {}};
            notes.append(p);
        }
        u.notes = std::move(notes);
        utterances.append(u);
    }
    vog.utterances = std::move(utterances);

    bool res = vog.save();
    if (!res) {
        Q_ERROR(q, qData->errorTitle(), VogenTab::tr("Failed to save file!"));
        return false;
    }

    this->vog = std::move(vog);

    return true;
}

bool VogenTabPrivate::earliest() const {
    return historyIndex == 0;
}

bool VogenTabPrivate::latest() const {
    return historyIndex == historyList.size();
}

void VogenTabPrivate::addHistory(TBaseOperation *op) {
    Q_Q(VogenTab);

    // Saved status doesn't exist
    if (savedHistoryIndex > historyIndex) {
        savedHistoryIndex = -1;
    }
    // Remove all operations after
    while (historyList.size() > historyIndex) {
        delete historyList.back();
        historyList.pop_back();
    }
    historyList.append(op);
    historyIndex++;

    q->setEdited(savedHistoryIndex != historyIndex);
}

void VogenTabPrivate::clearHistory() {
    for (auto op : qAsConst(historyList)) {
        delete op;
    }
    historyList.clear();
    historyIndex = 0;
    savedHistoryIndex = 0;
}

void VogenTabPrivate::dispatchEvent(TPianoRollEvent *event) {
    Q_Q(VogenTab);

    switch (event->pType()) {
    case TPianoRollEvent::Operate: {
        auto e = static_cast<TOperateEvent *>(event);
        addHistory(e->takeData());
        break;
    }
    case TPianoRollEvent::Select: {
        selectionFlags = piano->notesArea()->hasSelection();

        QEventImpl::MenuUpdateRequestEvent e(ActionImpl::SelectState);
        qApp->sendEvent(q->window(), &e);
        break;
    }
    case TPianoRollEvent::ChangeTimeSig: {
        inputBeat();
        break;
    }
    case TPianoRollEvent::ChangeTempo: {
        inputTempo();
        break;
    }
    case TPianoRollEvent::ChangeVoice: {
        auto e = static_cast<TChangeVoiceEvent *>(event);
        changeVoice(e);
        break;
    }
    case TPianoRollEvent::PlayState: {
        auto a = piano->notesArea();

        playFlags = 0;
        playFlags |= a->isPlaying() ? ActionImpl::StopFlag : ActionImpl::PlayFlag;
        playFlags |= a->hasCache(a->currentGroupId()) ? ActionImpl::NoFlag : ActionImpl::RenderFlag;

        QEventImpl::MenuUpdateRequestEvent e(ActionImpl::PlayState);
        qApp->sendEvent(q->window(), &e);
        break;
    }
    case TPianoRollEvent::SetPlayhead: {
        auto e = static_cast<TSetPlayheadEvent *>(event);
        auto a = piano->notesArea();
        if (!a->isPlaying()) {
            int tick = a->convertPositionToValue(QPointF(e->x, 0)).first;
            a->setCurrentTick(tick);
        }
        break;
    }
    default:
        break;
    }
}

void VogenTabPrivate::inputLyrics() {
    Q_Q(VogenTab);

    piano->notesArea()->stop();

    // Call editor to start accepting stdin
    StdinRequestEvent e1(StdinRequestEvent::Lyrics, StdinRequestEvent::InputStart);
    qApp->sendEvent(piano->notesArea(), &e1);

    if (!e1.isAccepted()) {
        return;
    }

    // Temporary Struct
    struct Hint : public QCommandPalette::Hint {
        TNotesArea *notesArea;
        Hint(TNotesArea *a, const QString &text, const QString &placeholder, bool hold = false)
            : QCommandPalette::Hint(text, placeholder, hold), notesArea(a){};
        void preview(const QString &text) override {
            // Call editor to update stdin
            StdinRequestEvent e2(StdinRequestEvent::Lyrics, StdinRequestEvent::InputUpdate);
            e2.text = text;
            qApp->sendEvent(notesArea, &e2);
        }
    };

    auto w = qobject_cast<MainWindow *>(q->window());
    Hint hint(piano->notesArea(), e1.text,
              VogenTab::tr("Enter the lyrics, separated by white space"), true);
    int res = w->showLineEdit(&hint);

    // Call editor to finish stdin
    StdinRequestEvent e3(StdinRequestEvent::Lyrics, (res == 0) ? StdinRequestEvent::InputCommit
                                                               : StdinRequestEvent::InputAbort);
    qApp->sendEvent(piano->notesArea(), &e3);
}

void VogenTabPrivate::inputBeat() {
    Q_Q(VogenTab);

    piano->notesArea()->stop();

    auto timeSig = piano->notesArea()->timeSig();
    const char fmt[] = "%d/%d";

    auto w = qobject_cast<MainWindow *>(q->window());
    QCommandPalette::Hint hint(QString::asprintf(fmt, timeSig.first, timeSig.second),
                               VogenTab::tr("Enter the new time signature, e.g. 3/4, 4/4"), false);
    int res = w->showLineEdit(&hint);

    if (res == 0) {
        const QString &str = hint.text;
        QRegExp reg("^[1-9][0-9]*/[1-9][0-9]*$");
        if (reg.exactMatch(str)) {
            int a, b;
            int n = ::sscanf(str.toUtf8().data(), fmt, &a, &b);
            if (n == 2) {
                QList<int> allowedDenominators{1, 2, 4, 8, 16, 32, 64, 128};
                if ((a >= 1 && a <= 256) && allowedDenominators.contains(b)) {
                    TDigitTimeSigEvent e;
                    e.at = a;
                    e.b = b;
                    qApp->sendEvent(piano->notesArea(), &e);
                }
            }
        }
    }
}

void VogenTabPrivate::inputTempo() {
    Q_Q(VogenTab);

    piano->notesArea()->stop();

    auto w = qobject_cast<MainWindow *>(q->window());
    QCommandPalette::Hint hint(QString::number(piano->notesArea()->tempo()),
                               VogenTab::tr("Enter the new tempo (10 ~ 512)"), false);
    int res = w->showLineEdit(&hint);

    if (res == 0) {
        bool ok;
        double val = hint.text.toDouble(&ok);
        if (ok) {
            // Check range
            if (val >= 10 && val <= 512) {
                TDigitalEvent e(TDigitalEvent::Tempo);
                e.digitF = val;
                qApp->sendEvent(piano->notesArea(), &e);
            }
        }
    }
}

void VogenTabPrivate::inputTranspose() {
    Q_Q(VogenTab);

    piano->notesArea()->stop();

    auto w = qobject_cast<MainWindow *>(q->window());
    QCommandPalette::Hint hint(QString(), VogenTab::tr("Enter the transpose offset (0 ~ 84)"),
                               false);
    int res = w->showLineEdit(&hint);

    if (res == 0) {
        bool ok;
        int val = hint.text.toInt(&ok);
        if (ok) {
            transpose(val);
        }
    }
}

void VogenTabPrivate::transpose(int val) {
    piano->notesArea()->stop();

    TDigitalEvent e(TDigitalEvent::Transpose);
    e.digit = val;
    qApp->sendEvent(piano->notesArea(), &e);
}

void VogenTabPrivate::changeVoice(TChangeVoiceEvent *event) {
    Q_Q(VogenTab);

    piano->notesArea()->stop();

    auto w = qobject_cast<MainWindow *>(q->window());

    if (event->vType() == TChangeVoiceEvent::Singer) {
        const auto &voiceList = qTheme->voiceList();
        QStringList names;
        int index = -1;
        for (int i = 0; i < voiceList.size(); ++i) {
            const auto &voice = voiceList.at(i);
            if (voice.id == event->singerId) {
                index = i;
            }
            names.append(QString("%1 (%2)").arg(voice.name, voice.id));
        }

        int res = w->showList(names, index, VogenTab::tr("Select Voice Library"));
        if (res >= 0) {
            event->singerId = voiceList.at(res).id;
        }
    } else {
        QStringList ids{"man", "yue", "yue-wz"};

        int index = -1;
        for (int i = 0; i < ids.size(); ++i) {
            const auto &id = ids.at(i);
            if (id == event->rom) {
                index = i;
            }
        }

        QStringList names{
            VogenTab::tr("Mandarin (man)"),
            VogenTab::tr("Cantonese (yue)"),
            VogenTab::tr("Cantonese-Wuzhou (yue-wz)"),
        };

        int res = w->showList(names, index, VogenTab::tr("Select Dictionary"));
        if (res >= 0) {
            event->rom = ids.at(res);
        }
    }
}

void VogenTabPrivate::violentRender() {
    Q_Q(VogenTab);

    auto a = piano->notesArea();
    a->stop();

    // Render Current Group
    quint64 gid = a->currentGroupId();
    auto utter = a->validUtterance(gid);

    if (utter.notes.isEmpty()) {
        return;
    }

    if (a->groupDuration(gid) > 30000) {
        auto btn = QMessageBox::question(
            q, qData->mainTitle(),
            VogenTab::tr("The current note sequence is too long, are you sure to render?"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if (btn != QMessageBox::Yes) {
            return;
        }
    }

    QDialog msgbox(q);
    msgbox.setObjectName("block-message-box");
    {
        auto label = new QLabel(VogenTab::tr("Synthesizing, please wait..."));
        label->setAlignment(Qt::AlignCenter);

        auto layout = new QHBoxLayout();
        layout->setMargin(0);
        layout->setSpacing(0);
        layout->addWidget(label);
        msgbox.setLayout(layout);
        msgbox.resize(300, 100);

        msgbox.setWindowTitle(qData->mainTitle());
        msgbox.setWindowModality(Qt::ApplicationModal);
        msgbox.setWindowFlags((msgbox.windowFlags() | Qt::WindowMinimizeButtonHint) &
                              ~Qt::WindowCloseButtonHint);
        msgbox.show();
    }

    bool res = violentRender_helper(gid, utter);

    msgbox.close();

    if (!res) {
        QMessageBox::warning(q, qData->mainTitle(), VogenTab::tr("Synthesizing failed."));
    }
}

void VogenTabPrivate::violentRenderAll() {
    Q_Q(VogenTab);

    auto a = piano->notesArea();
    a->stop();

    auto gids = a->groupIdList();

    QDialog msgbox(q);
    msgbox.setObjectName("block-message-box");

    auto label = new QLabel();
    label->setAlignment(Qt::AlignCenter);
    {
        auto layout = new QHBoxLayout();
        layout->setMargin(0);
        layout->setSpacing(0);
        layout->addWidget(label);
        msgbox.setLayout(layout);
        msgbox.resize(300, 100);

        msgbox.setWindowTitle(qData->mainTitle());
        msgbox.setWindowModality(Qt::ApplicationModal);
        msgbox.setWindowFlags((msgbox.windowFlags() | Qt::WindowMinimizeButtonHint) &
                              ~Qt::WindowCloseButtonHint);
        msgbox.show();
    }

    bool success = true;
    int groups = gids.size();
    for (int i = 0; i < groups; ++i) {
        // Display
        label->setText(VogenTab::tr("Synthesizing %1/%2, please wait...")
                           .arg(QString::number(i + 1), QString::number(groups)));

        quint64 gid = gids.at(i);
        if (a->hasCache(gid)) {
            continue;
        }

        if (a->groupDuration(gid) > 30000) {
            auto btn = QMessageBox::question(
                q, qData->mainTitle(),
                VogenTab::tr("The current note sequence is too long, are you sure to render?"),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            if (btn != QMessageBox::Yes) {
                break;
            }
        }

        auto utter = a->validUtterance(gid);
        if (utter.notes.isEmpty()) {
            continue;
        }

        bool res = violentRender_helper(gid, utter);
        if (!res) {
            success = false;
        }
    }

    msgbox.close();

    if (!success) {
        QMessageBox::warning(q, qData->mainTitle(),
                             VogenTab::tr("Synthesizing completed, but failure occured."));
    }
}

void VogenTabPrivate::violentExportAudio() {
    Q_Q(VogenTab);

    auto a = piano->notesArea();
    a->stop();

    auto tracks = a->audioData();
    if (tracks.isEmpty()) {
        QMessageBox::warning(q, qData->mainTitle(), VogenTab::tr("No audio generated recently."));
        return;
    }

    QString waveName;
    if (filename.isEmpty()) {
        waveName = "temp.wav";
    } else {
        QFileInfo info(filename);
        waveName = info.absolutePath() + "/" + info.baseName() + ".wav";
    }

    QString path = qData->saveFile(VogenTab::tr("Export Recent Audio"), waveName,
                                   qData->getFileFilter(DataManager::ExportRecent),
                                   FLAG_EXPORT_RECENT, q->window());
    if (path.isEmpty()) {
        return;
    }

#define SAMPLE_RATE 44100

    QList<int> indexes;

    // Init Index List
    int startIndex = 0;
    for (int i = 0; i < tracks.size(); ++i) {
        const auto &track = tracks.at(i);
        int index = double(track.first) / 1000 * SAMPLE_RATE;
        indexes.append(index);
        startIndex = qMin(startIndex, index);
    }
    for (int i = 0; i < tracks.size(); ++i) {
        indexes[i] -= startIndex;
    }

    QList<qint32> mergedSamples;
    for (int i = 0; i < indexes.size(); ++i) {
        const auto &track = tracks.at(i);
        const auto &samples = track.second->channel1();
        int index = indexes.at(i);

        // Complement
        while (mergedSamples.size() < index + samples.size()) {
            mergedSamples.append(0);
        }

        // Merge
        for (int i = 0; i < samples.size(); ++i) {
            mergedSamples[index + i] += samples.at(i);
        }
    }

    // Export wave file
    wave::File write_file;

#ifdef Q_OS_WINDOWS
    wave::Error err = write_file.Open(path.toStdWString(), wave::kOut);
#else
    wave::Error err = write_file.Open(path.toStdString(), wave::kOut);
#endif
    if (err) {
        QMessageBox::critical(q, qData->errorTitle(), VogenTab::tr("Failed to create file."));
        return;
    }

    write_file.set_sample_rate(SAMPLE_RATE);
    write_file.set_bits_per_sample(16);
    write_file.set_channel_number(1);

    std::vector<float> arr;
    for (auto sample : qAsConst(mergedSamples)) {
        arr.push_back(static_cast<float>(sample) / std::numeric_limits<qint16>::max());
    }

    err = write_file.Write(arr);
    if (err) {
        QMessageBox::critical(q, qData->errorTitle(), VogenTab::tr("Failed to write wave file."));
        return;
    }
}

bool VogenTabPrivate::violentRender_helper(quint64 gid, const TWProject::Utterance &utter) {
    auto a = piano->notesArea();
    auto bpm0 = a->tempo();

    // Wrap
    RH::FUtt u;
    u.name = utter.name;
    u.singerId = utter.singer;
    u.romScheme = utter.romScheme;

    QList<RH::FNote> notes;
    for (const auto &note : qAsConst(utter.notes)) {
        auto pair = splitLyricRom(note.rom);
        RH::FNote p{note.noteNum, pair.first, pair.second, note.start, note.length};
        notes.append(p);
    }
    u.notes = std::move(notes);

    QString path = tempDir + "/temp_" + QString::number(gid) + ".wav";

    // Render
    RH::SynthArgs args{bpm0, u, path};
    int code;
    QList<double> pitches;

    bool res = qTheme->server()->synthAll(args, &pitches, &code);

    if (!res || code == RH::SYNTH_FAILED) {
        return false;
    }
    a->setGroupCache(gid, pitches, path);

    return true;
}

QString VogenTabPrivate::setTabNameProxy(const QString &tabName) {
    QString newName;
    if (untitled && untitledIndex >= 0) {
        newName = QString(qData->untitledFileName() + " " + QString::number(untitledIndex));
    } else {
        newName = tabName;
    }
    newName = DocumentTabPrivate::setTabNameProxy(newName);
    return newName;
}
