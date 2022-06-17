#include "VogenTab_p.h"

#include "DataManager.h"

int VogenTabPrivate::s_untitledIndex = 0;

VogenTabPrivate::VogenTabPrivate() {
}

VogenTabPrivate::~VogenTabPrivate() {
}

void VogenTabPrivate::init() {
    Q_Q(VogenTab);

    untitledIndex = -1;

    // Init UI
    piano = new TPianoPanel(q);
    piano->setObjectName("piano-panel");

    layout = new QGridLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addWidget(piano, 0, 0);

    q->setLayout(layout);
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
