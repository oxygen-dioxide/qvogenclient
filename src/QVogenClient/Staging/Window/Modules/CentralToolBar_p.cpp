#include "CentralToolBar_p.h"

CentralToolBarPrivate::CentralToolBarPrivate() {
}

CentralToolBarPrivate::~CentralToolBarPrivate() {
}

void CentralToolBarPrivate::init() {
    Q_Q(CentralToolBar);

    q->setAttribute(Qt::WA_StyledBackground);

    m_cursorButton = new CToolButton();
    m_cursorButton->setProperty("type", "cursor");
    m_cursorButton->setObjectName("cursor-button");
    m_cursorButton->setCheckable(true);

    m_noteButton = new CToolButton();
    m_noteButton->setProperty("type", "cursor");
    m_noteButton->setObjectName("note-button");
    m_noteButton->setCheckable(true);

    m_freehandButton = new CToolButton();
    m_freehandButton->setProperty("type", "cursor");
    m_freehandButton->setObjectName("freehand-button");
    m_freehandButton->setCheckable(true);

    m_playButton = new CToolButton();
    m_playButton->setProperty("type", "play");
    m_playButton->setObjectName("play-button");
    m_playButton->setCheckable(true);

    m_stopButton = new CToolButton();
    m_stopButton->setProperty("type", "play");
    m_stopButton->setObjectName("stop-button");

    m_replayButton = new CToolButton();
    m_replayButton->setProperty("type", "play");
    m_replayButton->setObjectName("replay-button");

    m_cursorGroup = new QButtonGroup(q);
    m_cursorGroup->setExclusive(true);
    m_cursorGroup->addButton(m_cursorButton, CentralToolBar::Select);
    m_cursorGroup->addButton(m_noteButton, CentralToolBar::Sketch);
    m_cursorGroup->addButton(m_freehandButton, CentralToolBar::Free);

    // Layout
    layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addWidget(m_cursorButton);
    layout->addWidget(m_noteButton);
    layout->addWidget(m_freehandButton);

    layout->addWidget(m_playButton);
    layout->addWidget(m_stopButton);
    layout->addWidget(m_replayButton);

    layout->addStretch();

    q->setLayout(layout);

    q->connect(m_cursorButton, &QAbstractButton::clicked, q, &CentralToolBar::_q_buttonClicked);
    q->connect(m_noteButton, &QAbstractButton::clicked, q, &CentralToolBar::_q_buttonClicked);
    q->connect(m_freehandButton, &QAbstractButton::clicked, q, &CentralToolBar::_q_buttonClicked);
    q->connect(m_playButton, &QAbstractButton::clicked, q, &CentralToolBar::_q_buttonClicked);
    q->connect(m_stopButton, &QAbstractButton::clicked, q, &CentralToolBar::_q_buttonClicked);
    q->connect(m_replayButton, &QAbstractButton::clicked, q, &CentralToolBar::_q_buttonClicked);
}
