#include "VogenTab_p.h"

#include "DataManager.h"

#include "Utils/Events/TOperateEvent.h"

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
    switch (event->cType()) {
    case QEventImpl::PianoRollChangeEvent::Operate: {
        auto e = static_cast<TOperateEvent *>(event);
        addHistory(e->takeData());
        break;
    }
    default:
        break;
    }
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
