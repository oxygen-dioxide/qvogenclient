#include "VogenTab_p.h"

#include "DataManager.h"

#include "Utils/Events/TDigitTimeSigEvent.h"
#include "Utils/Events/TOperateEvent.h"

#include "MainWindow.h"

#include <QApplication>
#include <QMessageBox>

#include <cstdio>

using namespace QEventImpl;

int VogenTabPrivate::s_untitledIndex = 0;

static const auto placeholder_pipe = [](const QString &text) { Q_UNUSED(text); };

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

    auto w = qobject_cast<MainWindow *>(q->window());
    auto pipe = [=](const QString &text) -> void {
        // Call editor to update stdin
        StdinRequestEvent e2(StdinRequestEvent::Lyrics, StdinRequestEvent::InputUpdate);
        e2.text = text;
        qApp->sendEvent(piano->notesArea(), &e2);
    };

    int res = w->showLineEdit(e1.text, pipe, //
                              QObject::tr("Enter the lyrics, separated by white space"));

    // Call editor to finish stdin
    StdinRequestEvent e3(StdinRequestEvent::Lyrics, (res == 0) ? StdinRequestEvent::InputCommit
                                                               : StdinRequestEvent::InputAbort);
    qApp->sendEvent(piano->notesArea(), &e3);
}

void VogenTabPrivate::inputBeat() {
    Q_Q(VogenTab);
    auto w = qobject_cast<MainWindow *>(q->window());

    const char fmt[] = "%d/%d";

    auto timeSig = piano->notesArea()->timeSig();

    QString str;
    int res =
        w->showLineEdit(QString::asprintf(fmt, timeSig.first, timeSig.second), placeholder_pipe,
                        QObject::tr("Enter the new time signature, e.g. 3/4, 4/4"), &str);
    if (res == 0) {
        int a, b;
        int n = ::sscanf(str.toUtf8().data(), fmt, &a, &b);
        if (n == 2) {
            if (a > 0 && b > 0 && !(b & (b - 1))) {
                TDigitTimeSigEvent e;
                e.a = a;
                e.b = b;
                qApp->sendEvent(piano->notesArea(), &e);
            }
        }
    }
}

void VogenTabPrivate::inputTempo() {
    Q_Q(VogenTab);

    auto w = qobject_cast<MainWindow *>(q->window());

    QString str;
    int res = w->showLineEdit(QString::number(piano->notesArea()->tempo()), placeholder_pipe,
                              QObject::tr("Enter the new tempo (10 ~ 512)"), &str);
    if (res == 0) {
        bool ok;
        double val = str.toDouble(&ok);
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

    QString str;
    int res = w->showLineEdit(QString(), placeholder_pipe,
                              QObject::tr("Enter the transpose offset (0 ~ 84)"), &str);
    if (res == 0) {
        bool ok;
        int val = str.toInt(&ok);
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
