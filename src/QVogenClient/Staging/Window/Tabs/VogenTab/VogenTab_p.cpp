#include "VogenTab_p.h"

#include "DataManager.h"

#include "Utils/Events/TDigitalEvent.h"
#include "Utils/Events/TOperateEvent.h"

#include "MainWindow.h"

#include <QApplication>

using namespace QEventImpl;

int VogenTabPrivate::s_untitledIndex = 0;

VogenTabPrivate::VogenTabPrivate() {
}

VogenTabPrivate::~VogenTabPrivate() {
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

void VogenTabPrivate::dispatchEvent(QEventImpl::PianoRollChangeEvent *event) {
    Q_Q(VogenTab);

    switch (event->cType()) {
    case QEventImpl::PianoRollChangeEvent::Operate: {
        auto e = static_cast<TOperateEvent *>(event);
        addHistory(e->takeData());
        break;
    }
    case QEventImpl::PianoRollChangeEvent::Select: {
        selectionFlags = piano->notesArea()->hasSelection();
        QEventImpl::MenuUpdateRequestEvent e(ActionImpl::SelectState);
        qApp->sendEvent(q->window(), &e);
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

void VogenTabPrivate::inputTranspose() {
    Q_Q(VogenTab);

    auto w = qobject_cast<MainWindow *>(q->window());

    auto pipe = [](const QString &text) { Q_UNUSED(text); };

    QString str;
    int res =
        w->showLineEdit(QString(), pipe, QObject::tr("Enter the transpose offset (0 ~ 84)"), &str);
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
    if (untitled) {
        if (untitledIndex >= 0) {
            newName = QString(qData->untitledFileName() + " " + QString::number(untitledIndex));
        } else {
            newName = tabName;
        }
    } else {
        newName = DocumentTabPrivate::setTabNameProxy(tabName);
    }
    return newName;
}
