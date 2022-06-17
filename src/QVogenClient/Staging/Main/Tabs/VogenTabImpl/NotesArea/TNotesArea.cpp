#include "TNotesArea.h"
#include "TNotesScroll.h"

#include <QPainter>

TNotesArea::StyleData::StyleData() {
    lineWidth = 1;

    backDark = QColor(0xF6F6F6);
    backLight = QColor(0xFFFFFF);

    levelLine = QColor(0xDDDDDD);
    pitchLine = QColor(0xEAEAEA);

    sectionLine = QColor(0xD8D8D8);
    quarterLine = QColor(0xE4E4E4);
    timeLine = QColor(0xECECEC);
}

TNotesArea::TNotesArea(TNotesScroll *parent) : CGraphicsScene(parent), m_view(parent) {
    init();
}

TNotesArea::~TNotesArea() {
}

TNotesScroll *TNotesArea::view() const {
    return m_view;
}

QPointF TNotesArea::mousePosition() const {
    return m_view->mapToScene(m_view->mapFromGlobal(QCursor::pos()));
}

void TNotesArea::init() {
    m_sectionCount = 40;

    m_currentWidth = 60;
    m_currentHeight = 30;
    m_currentQuantize = 4;

    m_blankSections = 1;

    m_drawMode = PlainSelect;
    m_pointMode = SingleClick;

    initSelectElements();
    initSpriteElements();
    initScreenElements();
    initPlayElements();

    connect(this, &QGraphicsScene::sceneRectChanged, this, &TNotesArea::handleSceneRectChanged);

    adjustSize();
}

TNotesArea::StyleData TNotesArea::styleData() const {
    return m_styleData;
}

void TNotesArea::setStyleData(const StyleData &data) {
    m_styleData = data;
    adjustBackground();
}

int TNotesArea::sectionCount() const {
    return m_sectionCount;
}

void TNotesArea::setSectionCount(int sectionCount) {
    m_sectionCount = sectionCount;
    adjustSize();
}

int TNotesArea::currentWidth() const {
    return m_currentWidth;
}

void TNotesArea::setCurrentWidth(int currentWidth) {
    m_currentWidth = currentWidth;
    adjustSize();
}

int TNotesArea::currentHeight() const {
    return m_currentHeight;
}

void TNotesArea::setCurrentHeight(int currentHeight) {
    m_currentHeight = currentHeight;
    adjustSize();
}

int TNotesArea::currentQuantize() const {
    return m_currentQuantize;
}

void TNotesArea::setCurrentQuantize(int currentQuantize) {
    m_currentQuantize = currentQuantize;
    adjustSize();
}

int TNotesArea::blankSections() const {
    return m_blankSections;
}

void TNotesArea::setBlankSections(int blankSections) {
    m_blankSections = blankSections;
    adjustSize();
}

double TNotesArea::zeroLine() const {
    return m_currentWidth * 4 * m_blankSections;
}

void TNotesArea::setCurrentSizes(int w, int h, int q) {
    m_currentWidth = w;
    m_currentHeight = h;
    m_currentQuantize = q;
    adjustSize();
}

TNotesArea::DrawMode TNotesArea::drawMode() const {
    return m_drawMode;
}

void TNotesArea::setDrawMode(DrawMode drawMode) {
    m_drawMode = drawMode;
}

TNotesArea::AddPointMode TNotesArea::pointMode() const {
    return m_pointMode;
}

void TNotesArea::setPointMode(const AddPointMode &pointMode) {
    m_pointMode = pointMode;
}

void TNotesArea::adjustSize() {
    setSceneRect(QRectF(0, 0, m_currentWidth * 4 * (m_sectionCount + 1 + m_blankSections),
                        m_currentHeight * 84));
}

void TNotesArea::adjustBackground() {
    int curWidth = m_currentWidth;
    int curHeight = m_currentHeight;
    int curAdsorb = m_currentQuantize * 4;
    double lineWidth = m_styleData.lineWidth;

    // Draw Background Unit
    double totalWidth = curWidth * 4;
    double totalHeight = curHeight * 12;

    if (totalWidth == 0 || totalHeight == 0) {
        return;
    }

    QPixmap pixmap(totalWidth, totalHeight);

    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.translate(0, 0);

    const QSet<short> deep{1, 3, 5, 8, 10};
    for (int i = 0; i < 12; ++i) {
        painter.setBrush((deep.contains(i % 12)) ? m_styleData.backDark : m_styleData.backLight);
        painter.drawRect(QRectF(0, i * curHeight, totalWidth, curHeight));
    }

    double w = (curAdsorb == 0) ? Q_INFINITY : (totalWidth / curAdsorb);
    int threshold = 6;
    while (w < threshold && curAdsorb > 1) {
        curAdsorb--;
        while (curAdsorb % 4 != 0 && curAdsorb > 1) {
            curAdsorb--;
        }
        w = totalWidth / curAdsorb;
    }
    for (int i = 1; i < curAdsorb; ++i) {
        if ((i * 4) % curAdsorb == 0) {
            painter.setBrush(m_styleData.quarterLine);
        } else {
            painter.setBrush(m_styleData.timeLine);
        }
        painter.drawRect(QRectF(w * i - lineWidth / 2, 0, lineWidth, totalHeight));
    }

    painter.setBrush(m_styleData.sectionLine);
    painter.drawRect(QRectF(0, 0, lineWidth / 2, totalHeight));
    painter.drawRect(QRectF(totalWidth - lineWidth / 2, 0, lineWidth / 2, totalHeight));

    painter.setBrush(m_styleData.levelLine);
    painter.drawRect(QRectF(0, 0, totalWidth, lineWidth / 2));
    painter.drawRect(QRectF(0, totalHeight - lineWidth / 2, totalWidth, lineWidth / 2));

    painter.setBrush(m_styleData.pitchLine);
    painter.drawRect(QRectF(0, curHeight * 7 - lineWidth / 2, totalWidth, lineWidth));

    setBackgroundBrush(pixmap);
}

