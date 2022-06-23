#include "TNotesArea.h"
#include "TNotesScroll.h"

#include <QScrollBar>

void TNotesArea::setVisionFitToItem(QGraphicsItem *item, Qt::AnchorPoint side, bool tween) {
    QRectF rect = item->boundingRect();

    double x1 = item->x() + rect.x();
    double x2 = x1 + rect.width() / 2;
    double x3 = x1 + rect.width();
    double y1 = item->y() + rect.y();
    double y2 = y1 + rect.height() / 2;
    double y3 = y1 + rect.height();

    double pos;
    switch (side) {
    case Qt::AnchorLeft:
        pos = x1;
        break;
    case Qt::AnchorHorizontalCenter:
        pos = x2;
        break;
    case Qt::AnchorRight:
        pos = x3;
        break;
    case Qt::AnchorTop:
        pos = y1;
        break;
    case Qt::AnchorVerticalCenter:
        pos = y2;
        break;
    default:
        pos = y3;
        break;
    }
    setVisionFitToPos(pos, side, tween);
}

void TNotesArea::setVisionFitToItem(QGraphicsItem *item, Qt::Orientations orients, bool tween) {
    auto view = this->view();
    if (!view) {
        return;
    }
    QRectF viewportRect = view->viewportRect();

    QRectF rect = item->boundingRect();

    double x1 = item->x() + rect.x();
    double x3 = x1 + rect.width();
    double y1 = item->y() + rect.y();
    double y3 = y1 + rect.height();

    if (orients & Qt::Horizontal) {
        if (x1 < viewportRect.left()) {
            setVisionFitToPos(x1, Qt::AnchorLeft, tween);
        } else if (x3 > viewportRect.right()) {
            setVisionFitToPos(x3, Qt::AnchorRight, tween);
        }
    }
    if (orients & Qt::Vertical) {
        if (y1 < viewportRect.top()) {
            setVisionFitToPos(y1, Qt::AnchorTop, tween);
        } else if (y3 > viewportRect.bottom()) {
            setVisionFitToPos(y3, Qt::AnchorBottom, tween);
        }
    }
}

void TNotesArea::setVisionFitToPos(double pos, Qt::AnchorPoint side, bool tween) {
    auto view = this->view();
    if (!view) {
        return;
    }
    QRectF viewportRect = view->viewportRect();

    int toValue = 0;
    Qt::Orientation orient = Qt::Horizontal;

    switch (side) {
    case Qt::AnchorLeft:
        toValue = pos;
        break;
    case Qt::AnchorHorizontalCenter:
        toValue = pos - viewportRect.width() / 2;
        break;
    case Qt::AnchorRight:
        toValue = pos - viewportRect.width();
        break;
    case Qt::AnchorTop:
        orient = Qt::Vertical;
        toValue = pos;
        break;
    case Qt::AnchorVerticalCenter:
        orient = Qt::Vertical;
        toValue = pos - viewportRect.height() / 2;
        break;
    default:
        orient = Qt::Vertical;
        toValue = pos - viewportRect.height();
        break;
    }
    if (tween) {
        (orient == Qt::Horizontal) ? view->horizontalTween(toValue) : view->verticalTween(toValue);
    } else {
        (orient == Qt::Horizontal) ? view->setValueX(toValue) : view->setValueY(toValue);
    }
}
