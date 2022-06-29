#include "VogenTab_p.h"

#include "DataManager.h"

#include "Utils/Events/TDigitTimeSigEvent.h"
#include "Utils/Events/TOperateEvent.h"

#include "MainWindow.h"
#include "Modules/QCommandPalette.h"

#include <QApplication>
#include <QMessageBox>

#include <cstdio>

using namespace QEventImpl;

int VogenTabPrivate::s_untitledIndex = 0;

VogenTabPrivate::VogenTabPrivate() {
}

VogenTabPrivate::~VogenTabPrivate() {
    clearHistory();
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
            QVogenFile::Note p{note.noteNum, note.lyric, note.rom, note.start, note.length, {}};
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
    default:
        break;
    }
}

void VogenTabPrivate::inputLyrics() {
    Q_Q(VogenTab);

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
    TDigitalEvent e(TDigitalEvent::Transpose);
    e.digit = val;
    qApp->sendEvent(piano->notesArea(), &e);
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
