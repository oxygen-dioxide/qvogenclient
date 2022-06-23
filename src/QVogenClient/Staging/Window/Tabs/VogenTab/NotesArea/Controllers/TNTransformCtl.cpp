#include "TNTransformCtl.h"
#include "../TNotesArea.h"
#include "../TNotesScroll.h"

#include <QApplication>
#include <QGraphicsSceneMouseEvent>

#define SECTION_MIN 40
#define SECTION_UNIT 20

TNTransformCtl::TNTransformCtl(TNotesArea *parent) : TNController(parent) {
}

TNTransformCtl::~TNTransformCtl() {
}

void TNTransformCtl::install() {
    m_sectionCount = SECTION_MIN;
    m_currentWidth = 60;
    m_currentHeight = 30;
    m_currentQuantize = 4;

    m_blankSections = 1;

    m_scrollDrag = false;
    m_zoomDrag = false;
    m_zoomAnchor = QPoint(0, 0);

    a->installEventFilter(this);
    adjustSize();
}

int TNTransformCtl::sectionCount() const {
    return m_sectionCount;
}

void TNTransformCtl::setSectionCount(int sectionCount) {
    m_sectionCount = sectionCount;
    adjustSize();
}

void TNTransformCtl::setSectionCountHint(int totalLength) {
    int sections = totalLength / 480 / 4;
    int count = int((sections + (SECTION_UNIT * 1.5)) / SECTION_UNIT) * SECTION_UNIT;

    count = qMax(count, SECTION_MIN);

    setSectionCount(count);
}

int TNTransformCtl::currentWidth() const {
    return m_currentWidth;
}

void TNTransformCtl::setCurrentWidth(int currentWidth) {
    m_currentWidth = currentWidth;
    adjustSize();
}

int TNTransformCtl::currentHeight() const {
    return m_currentHeight;
}

void TNTransformCtl::setCurrentHeight(int currentHeight) {
    m_currentHeight = currentHeight;
    adjustSize();
}

int TNTransformCtl::currentQuantize() const {
    return m_currentQuantize;
}

void TNTransformCtl::setCurrentQuantize(int currentQuantize) {
    m_currentQuantize = currentQuantize;
    adjustSize();
}

void TNTransformCtl::setCurrentSizes(int w, int h, int q) {
    m_currentWidth = w;
    m_currentHeight = h;
    m_currentQuantize = q;
    adjustSize();
}

int TNTransformCtl::blankSections() const {
    return m_blankSections;
}

void TNTransformCtl::setBlankSections(int blankSections) {
    m_blankSections = blankSections;
    adjustSize();
}

double TNTransformCtl::zeroLine() const {
    return m_currentWidth * 4 * m_blankSections;
}

bool TNTransformCtl::scrollDrag() const {
    return m_scrollDrag;
}

bool TNTransformCtl::zoomDrag() const {
    return m_zoomDrag;
}

void TNTransformCtl::adjustSize() {
    a->setSceneRect(QRectF(0, 0, m_currentWidth * 4 * (m_sectionCount + 1 + m_blankSections),
                           m_currentHeight * 84));
}

void TNTransformCtl::setScrollDrag(bool scrollDrag) {
    m_scrollDrag = scrollDrag;
    a->view()->setDragMode(m_scrollDrag ? QGraphicsView::ScrollHandDrag : QGraphicsView::NoDrag);
}

void TNTransformCtl::setZoomDrag(bool zoomDrag) {
    if (zoomDrag) {
        m_zoomAnchor = QCursor::pos();
    } else {
        m_zoomAnchor = QPoint(0, 0);
    }
    m_zoomDrag = zoomDrag;
}

bool TNTransformCtl::eventFilter(QObject *obj, QEvent *event) {
    if (obj == a) {
        switch (event->type()) {

            // Mouse Press Event
            case QEvent::GraphicsSceneMousePress: {
                auto item = a->itemUnderMouse();
                if (!item) {
                    // No Item Under Mouse
                    auto e = static_cast<QGraphicsSceneMouseEvent *>(event);
                    auto button = e->button();
                    const auto &data = a->view()->controlData();
                    const auto &modifiers = qApp->keyboardModifiers();

                    if (button == Qt::LeftButton) {
                        if (modifiers == data.dragM) {
                            // Decision: Move
                            setScrollDrag(true);
                        } else if (modifiers == data.dragZ) {
                            // Decision: Zoom
                            setZoomDrag(true);
                        }
                    }
                }
                break;
            }

                // Mouse Move Event
            case QEvent::GraphicsSceneMouseMove: {
                auto e = static_cast<QGraphicsSceneMouseEvent *>(event);
                if (!a->mouseMoving()) {
                    // Mouse About To Move
                } else {
                    // Mouse Moving
                    if (m_scrollDrag) {
                        // Give control to QT
                    } else if (m_zoomDrag) {
                        auto sub = e->screenPos() - e->lastScreenPos();
                        sub /= 2;
                        if (!sub.isNull()) {
                            if (qAbs(sub.x()) > qAbs(sub.y())) {
                                a->view()->emitZoomRequest(Qt::Horizontal, sub.x(), m_zoomAnchor);
                            } else {
                                a->view()->emitZoomRequest(Qt::Vertical, sub.y(), m_zoomAnchor);
                            }
                        }
                        // return true; // Intercept
                    }
                }
                break;
            }

                // Mouse Release Event / Focus Out Event
            case QEvent::GraphicsSceneMouseRelease: {
                if (m_scrollDrag) {
                    setScrollDrag(false);
                }
                if (m_zoomDrag) {
                    setZoomDrag(false);
                }
                break;
            }

            default:
                break;
        }
    }
    return TNController::eventFilter(obj, event);
}
