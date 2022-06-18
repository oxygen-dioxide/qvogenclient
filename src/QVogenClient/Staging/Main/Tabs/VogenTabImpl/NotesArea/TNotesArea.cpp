#include "TNotesArea.h"
#include "TNotesArea_p.h"
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

TNotesArea::TNotesArea(TNotesScroll *parent) : TNotesArea(*new TNotesAreaPrivate(), parent) {
}

TNotesArea::~TNotesArea() {
}

TNotesArea::TNotesArea(TNotesAreaPrivate &d, TNotesScroll *parent)
    : CGraphicsScene(parent), d_ptr(&d), m_view(parent) {
    d.q_ptr = this;

    d.init();

    m_moving = false;

    m_drawMode = PlainSelect;
    m_pointMode = SingleClick;

    m_transCtl = new TNTransformCtl(this);
    m_selectCtl = new TNSelectCtl(this);
    m_playCtl = new TNPlayheadCtl(this);
    m_spriteCtl = new TNSpriteCtl(this);
    m_screenCtl = new TNScreenCtl(this);
    m_notesCtl = new TNNotesCtl(this);

    m_transCtl->install();
    m_selectCtl->install();
    m_playCtl->install();
    m_spriteCtl->install();
    m_screenCtl->install();
    m_notesCtl->install();

    connect(this, &QGraphicsScene::sceneRectChanged, this, &TNotesArea::_q_sceneRectChanged);
}

TNotesScroll *TNotesArea::view() const {
    return m_view;
}

QPointF TNotesArea::mousePosition() const {
    return m_view->mapToScene(m_view->mapFromGlobal(QCursor::pos()));
}

TNotesArea::StyleData TNotesArea::styleData() const {
    return m_styleData;
}

void TNotesArea::setStyleData(const StyleData &data) {
    m_styleData = data;
    adjustBackground();
}

int TNotesArea::sectionCount() const {
    return m_transCtl->sectionCount();
}

void TNotesArea::setSectionCount(int sectionCount) {
    m_transCtl->setSectionCount(sectionCount);
}

int TNotesArea::currentWidth() const {
    return m_transCtl->currentWidth();
}

void TNotesArea::setCurrentWidth(int currentWidth) {
    m_transCtl->setCurrentWidth(currentWidth);
}

int TNotesArea::currentHeight() const {
    return m_transCtl->currentHeight();
}

void TNotesArea::setCurrentHeight(int currentHeight) {
    m_transCtl->setCurrentHeight(currentHeight);
}

int TNotesArea::currentQuantize() const {
    return m_transCtl->currentQuantize();
}

void TNotesArea::setCurrentQuantize(int currentQuantize) {
    m_transCtl->setCurrentQuantize(currentQuantize);
}

int TNotesArea::blankSections() const {
    return m_transCtl->blankSections();
}

void TNotesArea::setBlankSections(int blankSections) {
    m_transCtl->setBlankSections(blankSections);
}

double TNotesArea::zeroLine() const {
    return m_transCtl->zeroLine();
}

void TNotesArea::setCurrentSizes(int w, int h, int q) {
    m_transCtl->setCurrentSizes(w, h, q);
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

void TNotesArea::adjustBackground() {
    int curWidth = currentWidth();
    int curHeight = currentHeight();
    int curAdsorb = currentQuantize() * 4;
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

bool TNotesArea::mouseMoving() const {
    return m_moving;
}

bool TNotesArea::visionMoving() const {
    return m_transCtl->scrollDrag() || m_transCtl->zoomDrag();
}
