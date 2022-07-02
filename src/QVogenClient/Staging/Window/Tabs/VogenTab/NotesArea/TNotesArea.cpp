#include "TNotesArea.h"
#include "TNotesArea_p.h"
#include "TNotesScroll.h"

#include "Logs/CRecordHolder.h"

#include "../Utils/Events/SceneStateQuery/TSSQCursorModeEvent.h"

#include <QApplication>
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

QObjectPrivate *TNotesArea::priv() const {
    auto d = QObject::d_ptr.data();
    return static_cast<QObjectPrivate *>(d);
}

TNotesArea::TNotesArea(TNotesAreaPrivate &d, TNotesScroll *parent)
    : CGraphicsScene(parent), d_ptr(&d), m_view(parent) {
    d.q_ptr = this;

    d.init();

    m_moving = false;
    m_oldSizes = qMakePair(QSize(32, 32), 1);

    m_drawMode = PlainSelect;
    m_pointMode = SingleClick;

    m_tempo = 120.0;
    m_timeSig = qMakePair(4, 4);

    m_player = new TMultiPlayer(this);
    m_playerTimerId = 0;

    m_priorCtl = new TNPriorCtl(this);
    m_transCtl = new TNTransformCtl(this);
    m_selectCtl = new TNSelectCtl(this);
    m_playCtl = new TNPlayheadCtl(this);
    m_spriteCtl = new TNSpriteCtl(this);
    m_screenCtl = new TNScreenCtl(this);
    m_notesCtl = new TNNotesCtl(this);

    m_notesCtl->install();

    m_screenCtl->install();

    m_spriteCtl->install();

    m_playCtl->install();

    m_selectCtl->install();

    m_transCtl->install();

    m_priorCtl->install();

    connect(this, &QGraphicsScene::sceneRectChanged, this, &TNotesArea::_q_sceneRectChanged);

    connect(qRecord, &CRecordHolder::eStateCommited, this, &TNotesArea::_q_eStateCommited);
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

void TNotesArea::setSectionCountHint(int totalLength) {
    m_transCtl->setSectionCountHint(totalLength);
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
    adjustBackground();
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

    m_notesCtl->deselect();

    QEventImpl::SceneStateChangeEvent e(QEventImpl::SceneStateChangeEvent::CursorMode);
    QApplication::sendEvent(this, &e);
}

TNotesArea::AddPointMode TNotesArea::pointMode() const {
    return m_pointMode;
}

void TNotesArea::setPointMode(const AddPointMode &pointMode) {
    m_pointMode = pointMode;
}

QPointF TNotesArea::convertValueToPosition(int tick, int noteNum) const {
    int curWidth = currentWidth();
    int curHeight = currentHeight();

    double toX = double(tick) / 480 * curWidth + zeroLine(); // -1 section is the leftmost
    double toY = curHeight * (107 - noteNum);

    return QPointF(toX, toY);
}

QRectF TNotesArea::convertValueToGeometry(int tick, int noteNum, int length) const {
    int curWidth = currentWidth();
    int curHeight = currentHeight();

    QPointF p = convertValueToPosition(tick, noteNum);
    double toW = double(length) / 480 * double(curWidth);
    double toH = curHeight;

    return QRectF(p.x(), p.y(), toW, toH);
}

QPair<int, int> TNotesArea::convertPositionToValue(QPointF pos) const {
    int curWidth = currentWidth();
    int curHeight = currentHeight();

    int tick = (pos.x() - zeroLine()) / curWidth * 480;
    int noteNum = 107 - pos.y() / curHeight;

    return qMakePair(tick, noteNum);
}

double TNotesArea::convertWidthToLength(int width) const {
    int curWidth = currentWidth();
    return double(width) / curWidth * 480;
}

double TNotesArea::tickToTime(int tick) const {
    double unit = 120.0 / m_tempo / 0.96;
    return tick * unit;
}

int TNotesArea::timeToTick(double time) const {
    double unit = 120.0 / m_tempo / 0.96;
    return time / unit;
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

    // Draw background
    const QSet<short> deep{1, 3, 5, 8, 10};
    for (int i = 0; i < 12; ++i) {
        painter.setBrush((deep.contains(i % 12)) ? m_styleData.backDark : m_styleData.backLight);
        painter.drawRect(QRectF(0, i * curHeight, totalWidth, curHeight));
    }


    // Draw horizontal lines
    painter.setBrush(m_styleData.levelLine);
    painter.drawRect(QRectF(0, 0, totalWidth, lineWidth / 2));
    painter.drawRect(QRectF(0, totalHeight - lineWidth / 2, totalWidth, lineWidth / 2));

    painter.setBrush(m_styleData.pitchLine);
    painter.drawRect(QRectF(0, curHeight * 7 - lineWidth / 2, totalWidth, lineWidth));


    // Move by offset
    int offset = int(zeroLine()) % curWidth;
    painter.translate(offset, 0);

    // Draw vertical lines
    double w = (curAdsorb == 0) ? Q_INFINITY : (totalWidth / curAdsorb);
    int threshold = 6;
    while (w < threshold && curAdsorb > 1) {
        curAdsorb--;
        while (curAdsorb % 4 != 0 && curAdsorb > 1) {
            curAdsorb--;
        }
        w = totalWidth / curAdsorb;
    }
    for (int j = 0; j < 2; ++j) {
        for (int i = 1; i < curAdsorb; ++i) {
            if ((i * 4) % curAdsorb == 0) {
                painter.setBrush(m_styleData.quarterLine);
            } else {
                painter.setBrush(m_styleData.timeLine);
            }
            painter.drawRect(
                QRectF(totalWidth * j + w * i - lineWidth / 2, 0, lineWidth, totalHeight));
        }
    }

    // Draw one more
    for (int i = 0; i < 3; ++i) {
        painter.setBrush(m_styleData.sectionLine);
        painter.drawRect(QRectF(totalWidth * i - lineWidth / 2, 0, lineWidth, totalHeight));
    }

    setBackgroundBrush(pixmap);
}

void TNotesArea::setTempo(double tempo) {
    m_tempo = tempo;

    QEventImpl::SceneStateChangeEvent e(QEventImpl::SceneStateChangeEvent::Tempo);
    QApplication::sendEvent(view()->window(), &e);
}

void TNotesArea::setTimeSig(int a, int b) {
    m_timeSig = qMakePair(a, b);

    m_notesCtl->adjustAllGeometry();
    m_notesCtl->adjustAllGroupHintPos();
    m_notesCtl->adjustCanvas();
    m_notesCtl->updateScreen();
    adjustBackground();

    QEventImpl::SceneStateChangeEvent e(QEventImpl::SceneStateChangeEvent::TimeSig);
    QApplication::sendEvent(view()->window(), &e);
}

bool TNotesArea::isSelecting() const {
    return m_selectCtl->isSelecting();
}

bool TNotesArea::mouseMoving() const {
    return m_moving;
}

bool TNotesArea::visionMoving() const {
    return m_transCtl->scrollDrag() || m_transCtl->zoomDrag();
}

bool TNotesArea::scrollZoomAllowed() const {
    return !isSelecting() && !visionMoving() && !itemOperating();
}

bool TNotesArea::itemMoving() const {
    return m_notesCtl->isMoving();
}

bool TNotesArea::itemStretching() const {
    return m_notesCtl->isStretching();
}

bool TNotesArea::itemDrawing() const {
    return m_notesCtl->isDrawing();
}

bool TNotesArea::itemOperating() const {
    return itemMoving() || itemStretching() || itemDrawing();
}

bool TNotesArea::hasSelection() const {
    return m_notesCtl->hasSelection();
}

quint64 TNotesArea::currentGroupId() const {
    return m_notesCtl->currentGroupId();
}

QList<quint64> TNotesArea::groupIdList() const {
    return m_notesCtl->groupIdList();
}

void TNotesArea::setGroupCache(quint64 id, const QList<double> &pitches, const QString &waveFile) {
    m_notesCtl->setGroupCache(id, pitches, waveFile);
}

void TNotesArea::removeGroupCache(quint64 id) {
    m_notesCtl->removeGroupCache(id);
}

void TNotesArea::removeAllCache() {
    m_notesCtl->removeAllCache();
}

bool TNotesArea::hasCache(quint64 id) const {
    return m_notesCtl->hasCache(id);
}

QList<QPair<qint64, QWaveInfo *>> TNotesArea::audioData() const {
    return m_notesCtl->audioData();
}

TWProject::Utterance TNotesArea::currentValidUtterance() const {
    return m_notesCtl->currentValidUtterance();
}

TWProject::Utterance TNotesArea::validUtterance(quint64 gid) const {
    return m_notesCtl->validUtterance(gid);
}

QPair<int, int> TNotesArea::timeSig() const {
    return m_timeSig;
}

double TNotesArea::tempo() const {
    return m_tempo;
}

QGraphicsItem *TNotesArea::itemUnderMouse() const {
    return m_priorCtl->itemUnderMouse();
}
