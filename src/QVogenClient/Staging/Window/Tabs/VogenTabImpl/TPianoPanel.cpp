#include "TPianoPanel.h"

#include <QApplication>
#include <QEvent>
#include <QMetaType>
#include <QScrollBar>
#include <QWheelEvent>

TPianoPanel::TPianoPanel(VogenTab *tab, QWidget *parent) : TVogenPanel(tab, parent) {
    init();
}

TPianoPanel::~TPianoPanel() {
}

void TPianoPanel::init() {
    m_layout = new QGridLayout();
    m_layout->setMargin(0);
    m_layout->setSpacing(0);
    setLayout(m_layout);

    m_pianoScroll = new TPianoScroll();
    m_pianoArea = new TPianoArea(m_pianoScroll);
    m_pianoScroll->setArea(m_pianoArea);

    m_notesScroll = new TNotesScroll();
    m_notesArea = new TNotesArea(m_notesScroll);
    m_notesScroll->setScene(m_notesArea);

    m_sectionsArea = new TSectionsArea();

    m_layout->addWidget(new QWidget(), 0, 0);
    m_layout->addWidget(m_sectionsArea, 0, 1);
    m_layout->addWidget(m_pianoScroll, 1, 0);
    m_layout->addWidget(m_notesScroll, 1, 1);

    // Signals and events
    m_pianoScroll->installEventFilter(this);
    connect(m_notesScroll, &TNotesScroll::viewMoved, this, &TPianoPanel::handleNotesAreaMoved);
    connect(m_notesScroll, &TNotesScroll::zoomRequested, this,
            &TPianoPanel::handleNotesAreaZoomRequested);

    // Init sizes
    setCurrentSizes(60, 30, 4);
    setBlankSections(1);
}

int TPianoPanel::sectionCount() const {
    return m_notesArea->sectionCount();
}

void TPianoPanel::setSectionCount(int sectionCount) {
    m_notesArea->setSectionCount(sectionCount);
}

int TPianoPanel::currentWidth() const {
    return m_notesArea->currentWidth();
}

void TPianoPanel::setCurrentWidth(int currentWidth) {
    m_sectionsArea->setCurrentWidth(currentWidth);
    m_notesArea->setCurrentWidth(currentWidth);
}

int TPianoPanel::currentHeight() const {
    return m_notesArea->currentHeight();
}

void TPianoPanel::setCurrentHeight(int currentHeight) {
    m_pianoArea->setCurrentHeight(currentHeight);
    m_notesArea->setCurrentHeight(currentHeight);
}

int TPianoPanel::currentQuantize() const {
    return m_notesArea->currentQuantize();
}

void TPianoPanel::setCurrentQuantize(int currentQuantize) {
    m_notesArea->setCurrentQuantize(currentQuantize);
}

void TPianoPanel::setCurrentSizes(int w, int h, int q) {
    m_pianoArea->setCurrentHeight(h);
    m_sectionsArea->setCurrentWidth(w);
    m_notesArea->setCurrentSizes(w, h, q);
}

int TPianoPanel::blankSections() const {
    return m_notesArea->blankSections();
}

void TPianoPanel::setBlankSections(int blankSections) {
    m_sectionsArea->setBlankSections(blankSections);
    m_notesArea->setBlankSections(blankSections);
}

TNotesArea *TPianoPanel::notesArea() const {
    return m_notesArea;
}

TPianoArea *TPianoPanel::pianoArea() const {
    return m_pianoArea;
}

TSectionsArea *TPianoPanel::sectionsArea() const {
    return m_sectionsArea;
}

void TPianoPanel::handleNotesAreaMoved(int dx, int dy) {
    Q_UNUSED(dx)
    Q_UNUSED(dy)

    m_pianoScroll->setValueY(m_notesScroll->valueY());
    m_sectionsArea->setHorizontalValue(m_notesScroll->valueX());
}

void TPianoPanel::handleNotesAreaZoomRequested(Qt::Orientation orient, int offset,
                                               const QPoint &anchor) {
    if (orient == Qt::Horizontal) {
        QPoint relativePos = m_notesScroll->mapFromGlobal(anchor);
        QPointF absPos = m_notesScroll->mapToScene(relativePos);

        int fromX = relativePos.x();

        if (fromX < 0) {
            fromX = 0;
        } else if (fromX > m_notesScroll->width()) {
            fromX = m_notesScroll->width();
        }

        if (qAbs(offset) > 12) {
            offset = double(qAbs(offset)) / offset * 12;
        }
        int fromW = currentWidth();
        int toW = fromW + offset;

        if (toW < 12) {
            toW = 12;
        }
        if (toW > 480) {
            toW = 480;
        }

        int fromAbs = absPos.x() - (relativePos.x() - fromX);
        int toX = fromX - fromAbs * double(toW) / fromW;
        if (toX > 0) {
            toX = 0;
        }

        setCurrentWidth(toW);

        QScrollBar *bar = m_notesScroll->horizontalScrollBar();
        bar->setValue(-toX);
    } else {
        QPoint relativePos = m_notesScroll->mapFromGlobal(anchor);
        QPointF absPos = m_notesScroll->mapToScene(relativePos);

        int fromY = relativePos.y();

        if (fromY < 0) {
            fromY = 0;
        } else if (fromY > m_notesScroll->height()) {
            fromY = m_notesScroll->height();
        }

        if (qAbs(offset) > 4) {
            offset = double(qAbs(offset)) / offset * 4;
        }
        int fromH = currentHeight();
        int toH = fromH + offset;

        if (toH < 12) {
            toH = 12;
        }
        if (toH > 60) {
            toH = 60;
        }

        int fromAbs = absPos.y() - (relativePos.y() - fromY);
        int toY = fromY - fromAbs * double(toH) / fromH;

        if (toY > 0) {
            toY = 0;
        }

        setCurrentHeight(toH);

        QScrollBar *bar = m_notesScroll->verticalScrollBar();
        bar->setValue(-toY);
    }
}
