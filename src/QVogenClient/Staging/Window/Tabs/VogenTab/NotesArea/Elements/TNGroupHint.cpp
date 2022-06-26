#include "TNGroupHint.h"
#include "../TNotesArea.h"
#include "../Utils/TNNoteGroup.h"

#include "Types/Graphics.h"

#include <QFontMetrics>
#include <QPainter>

TNGroupHint::TNGroupHint(TNNoteGroup *g, QGraphicsItem *parent)
    : TNRectObject(g->m_area, parent), g(g) {
    m_padding = QSize(5, 5);
    setAcceptHoverEvents(true);
}

TNGroupHint::~TNGroupHint() {
}

int TNGroupHint::type() const {
    return GraphicsImpl::GroupHintItem;
}

void TNGroupHint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                        QWidget *widget) {
    double padding = 0.5;
    QRectF rect = this->rect();
    QRectF originRect(padding, padding, rect.width() - 2 * padding, rect.height() - 2 * padding);

    double border = rect.height() / 10;
    border = border > 6 ? 6 : border;

    QColor textColor = Qt::white;
    QColor backColor = isUnderMouse() ? 0x606060 : 0x474747;

    painter->setBrush(QBrush(backColor));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(originRect, border * 1.5, border * 1.5);

    QFont font = m_area->font();
    font.setPixelSize(font.pixelSize() * 3 / 4);

    painter->setPen(textColor);
    painter->setFont(font);
    painter->drawText(originRect, Qt::AlignCenter, hintText());
}

QString TNGroupHint::hintText() const {
    return QString("%1 | %2").arg(g->singer.isEmpty() ? "(None)" : g->singer,
                                  g->rom.isEmpty() ? "(None)" : g->rom);
}

void TNGroupHint::layoutRequestEvent(QEvent *event) {
    Q_UNUSED(event);

    QFont font = m_area->font();
    font.setPixelSize(font.pixelSize() * 3 / 4);

    QFontMetrics f(font);

    QSize fontSize = QSize(f.horizontalAdvance(hintText()), f.height());

    QSize sz = fontSize + m_padding * 2;
    setRect(0, 0, sz.width(), sz.height());
}

void TNGroupHint::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    update();
    TNRectObject::hoverEnterEvent(event);
}

void TNGroupHint::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    update();
    TNRectObject::hoverLeaveEvent(event);
}
