#include "CGraphicsView.h"
#include "CScrollBar.h"

#include <QApplication>
#include <QGraphicsSceneMoveEvent>
#include <QGraphicsSceneResizeEvent>
#include <QResizeEvent>

#include <QDebug>

CGraphicsView::CGraphicsView(QWidget *parent) : QGraphicsView(parent) {
    init();
}

CGraphicsView::CGraphicsView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent) {
    init();
}

CGraphicsView::~CGraphicsView() {
}

void CGraphicsView::init() {
    CScrollBar::replaceScrollBars(this);
}

QRectF CGraphicsView::viewportRect() const {
    QRect rect(0, 0, viewport()->width(), viewport()->height());
    QRectF sceneRect = mapToScene(rect).boundingRect();
    return sceneRect;
}

void CGraphicsView::setValueX(int value) {
    horizontalScrollBar()->setValue(value);
}

int CGraphicsView::valueX() const {
    return horizontalScrollBar()->value();
}

void CGraphicsView::setValueY(int value) {
    verticalScrollBar()->setValue(value);
}

int CGraphicsView::valueY() const {
    return verticalScrollBar()->value();
}

void CGraphicsView::scrollContentsBy(int dx, int dy) {
    auto scene = this->scene();
    QRectF rect = viewportRect();

    QGraphicsSceneMoveEvent event;
    event.setOldPos(rect.topLeft() - QPointF(dx, dy));
    event.setNewPos(rect.topLeft());
    event.setWidget(this);

    QGraphicsView::scrollContentsBy(dx, dy);
    if (scene) {
        QApplication::sendEvent(scene, &event);
    }
}

bool CGraphicsView::viewportEvent(QEvent *event) {
    auto scene = this->scene();
    if (event->type() == QEvent::Resize) {
        QResizeEvent *e = static_cast<QResizeEvent *>(event);
        QGraphicsSceneResizeEvent e2;
        e2.setOldSize(e->oldSize());
        e2.setNewSize(e->size());
        e2.setWidget(this);
        if (scene) {
            QApplication::sendEvent(scene, &e2);
        }
    }
    return QGraphicsView::viewportEvent(event);
}
