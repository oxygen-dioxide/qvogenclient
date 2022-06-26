#include "TNRectNote.h"
#include "../TNotesArea.h"
#include "../TNotesScroll.h"
#include "Types/Graphics.h"

#include "CommonTuneStd.h"

#include <QApplication>
#include <QDebug>
#include <QPainter>

static void _q_hoverFromMouseEvent(QGraphicsSceneHoverEvent *hover,
                                   const QGraphicsSceneMouseEvent *mouseEvent) {
    hover->setWidget(mouseEvent->widget());
    hover->setPos(mouseEvent->pos());
    hover->setScenePos(mouseEvent->scenePos());
    hover->setScreenPos(mouseEvent->screenPos());
    hover->setLastPos(mouseEvent->lastPos());
    hover->setLastScenePos(mouseEvent->lastScenePos());
    hover->setLastScreenPos(mouseEvent->lastScreenPos());
    hover->setModifiers(mouseEvent->modifiers());
    hover->setAccepted(mouseEvent->isAccepted());
}

TNRectNote::TNRectNote(TNotesArea *area, QGraphicsItem *parent) : TNRectSelectable(area, parent) {
    m_movable = true;
    m_stretch = false;

    // Initialize Default Properties
    start = 0;
    length = CommonTuneStd::TIME_BASE;
    tone = CommonTuneStd::StandardToneNum();

    group = nullptr;

    setAcceptHoverEvents(true);
}

TNRectNote::~TNRectNote() {
}

bool TNRectNote::movable() const {
    return m_movable;
}

void TNRectNote::setMovable(bool movable) {
    m_movable = movable;
    if (!m_movable) {
        setStretch(false);
    }
    update();
}

QRectF TNRectNote::geometry() const {
    return m_area->convertValueToGeometry(start, tone, length);
}

TNRectSelectable::Behavior TNRectNote::mousePressBehavior() const {
    Behavior res = NoBehavior;

    if (m_enabled) {
        Qt::KeyboardModifiers modifiers = qApp->keyboardModifiers();
        const auto &data = m_area->view()->controlData();

        // Stretch
        if (m_movable) {
            if (m_stretch) {
                // Before stretching
                if (modifiers == data.stretchR) {
                    res = RelativeStretch;
                } else if (modifiers == data.stretchA) {
                    res = AbsoluteStretch;
                } else {
                    res = IndependentStretch;
                }
                return res;
            }
        }

        bool selected = isSelected();

        // Shift
        if (modifiers == data.selectC) {
            res = SelectContinuously;
            return res;
        }

        // Ctrl
        if (modifiers == data.selectS) {
            // Add or Remove
            if (selected) {
                res = DeselectOne;
            } else {
                res = SelectOne;
            }
        } else {
            if (!selected) {
                res = SelectOnly;
            }
        }
    }
    return res;
}

void TNRectNote::setStretch(bool stretch) {
    if (stretch == m_stretch) {
        return;
    }
    m_stretch = stretch;
    if (m_stretch) {
        m_oldCursor = cursor();
        setCursor(Qt::SizeHorCursor);
    } else {
        setCursor(m_oldCursor);
    }
}

void TNRectNote::layoutRequestEvent(QEvent *event) {
    Q_UNUSED(event);

    QRectF geometry = m_area->convertValueToGeometry(start, tone, length);
    setPos(geometry.topLeft());
    setRect(0, 0, geometry.width(), geometry.height());

    update();

    setBegin(start);
    setEnd(start + length);
}

void TNRectNote::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        qDebug() << this;
    } else {
        qDebug() << pos();
    }
    TNRectSelectable::mouseDoubleClickEvent(event);
}

void TNRectNote::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    TNRectSelectable::hoverEnterEvent(event);
}

void TNRectNote::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    if (m_movable) {
        if (m_enabled) {
            setStretch(event->pos().x() > width() - 5);
        } else {
            setStretch(false);
        }
    }
    TNRectSelectable::hoverMoveEvent(event);
}

void TNRectNote::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    setStretch(false);
    TNRectSelectable::hoverLeaveEvent(event);
}

int TNRectNote::type() const {
    return GraphicsImpl::NoteItem;
}

void TNRectNote::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    //    qDebug() << option << widget;

    QRectF rect = this->rect();

    double padding = 0.5;
    double innerPadding = 2;
    double border = rect.height() / 10;

    border = border > 6 ? 6 : border;

    QColor lineColor = isEnabled() ? 0x78AD35 : 0x516834;
    QColor fillColor = 0xB0E56D;

    QRectF originRect(padding, padding, rect.width() - 2 * padding, rect.height() - 2 * padding);
    QRectF entityRect(originRect);
    QRectF textRect(originRect);
    textRect.setX(entityRect.x() + 5 + m_area->currentWidth() / 128.0);

    // Draw Entity
    painter->setBrush(lineColor);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(entityRect, border * 1.5, border * 1.5);

    // Draw Line
    if (isSelected() && isEnabled()) {
        painter->setBrush(fillColor);
        QRectF innerRect(entityRect.left() + innerPadding, entityRect.top() + innerPadding,
                         entityRect.width() - 2 * innerPadding,
                         entityRect.height() - 2 * innerPadding);
        painter->drawRoundedRect(innerRect, border * 1.5, border * 1.5);
    }

    painter->setPen(Qt::black);
    painter->setBrush(Qt::NoBrush);

    QFont font = m_area->font();
    font.setPixelSize(font.pixelSize() * 3 / 4);
    painter->setFont(font);
    painter->drawText(textRect, Qt::AlignVCenter, lyric, &entityRect);
}

QDebug operator<<(QDebug debug, TNRectNote *p) {
    debug.noquote() << QString("%1(%2)").arg(
        p->staticMetaObject.className(),
        p                                                              //
            ? QString("id=%1,start=%2,len=%3,tone=%4,lyric=%5,gid=%6") //
                  .arg(QString::number(p->id),                         //
                       QString::number(p->start),                      //
                       QString::number(p->length),                     //
                       QString::number(p->tone),                       //
                       p->lyric,                                       //
                       QString::number(p->group->id))                  //
            : "0x0"                                                    //
    );
    return debug;
}
