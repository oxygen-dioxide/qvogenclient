#include "TNNoteGroup.h"
#include "../TNotesArea.h"
#include "../TNotesScroll.h"

#include "Types/Events.h"

#include <QApplication>

TNNoteGroup::TNNoteGroup(TNotesArea *area, QObject *parent) : TNNoteList(parent), m_area(area) {
    m_firstNote = nullptr;
    connect(this, &TNNoteList::inserted, this, &TNNoteGroup::_q_inserted);
    connect(this, &TNNoteList::removed, this, &TNNoteGroup::_q_removed);
    connect(this, &TNNoteList::beginChanged, this, &TNNoteGroup::_q_beginChanged);
}

TNNoteGroup::~TNNoteGroup() {
}

void TNNoteGroup::install() {
    m_area->installEventFilter(this);

    m_hintItem = new TNGroupHint(this);
    m_area->addItem(m_hintItem);
    m_hintItem->setZValue(TNotesArea::GroupHint);

    adjustHintPos();
}

void TNNoteGroup::uninstall() {
    m_area->removeEventFilter(this);

    m_area->removeItem(m_hintItem);
    delete m_hintItem;
}

TNGroupHint *TNNoteGroup::hintItem() const {
    return m_hintItem;
}

void TNNoteGroup::adjustHintGeometry() {
    QEvent e(static_cast<QEvent::Type>(QEventImpl::ItemGeometryUpdate));
    QApplication::sendEvent(m_hintItem, &e);
}

void TNNoteGroup::adjustHintPos() {
    if (!m_firstNote) {
        m_hintItem->hide();
        return;
    }

    QRectF rect = m_area->view()->viewportRect();
    m_hintItem->setPos(m_firstNote->geometry().x(), rect.bottom() - m_hintItem->height() * 1.2);

    if (!m_hintItem->isVisible()) {
        m_hintItem->show();
        m_hintItem->update();
    }
}

bool TNNoteGroup::eventFilter(QObject *obj, QEvent *event) {
    if (obj == m_area) {
        switch (event->type()) {
        case QEvent::GraphicsSceneMove:
        case QEvent::GraphicsSceneResize:
        case QEventImpl::SceneRectChange:
            adjustHintPos();
            break;

        default:
            break;
        }
    }
    return TNNoteList::eventFilter(obj, event);
}

void TNNoteGroup::_q_inserted(int beginIndex, int endIndex, TNRectNote *p) {
    Q_UNUSED(endIndex);
    Q_UNUSED(p);

    if (beginIndex == 0) {
        if (!m_begins.isEmpty()) {
            m_firstNote = *m_begins.front().second.begin();
        } else {
            m_firstNote = nullptr;
        }
        adjustHintPos();
    }
}

void TNNoteGroup::_q_removed(int beginIndex, int endIndex, TNRectNote *p) {
    Q_UNUSED(endIndex);
    Q_UNUSED(p);

    if (beginIndex == 0) {
        if (!m_begins.isEmpty()) {
            m_firstNote = *m_begins.front().second.begin();
        } else {
            m_firstNote = nullptr;
        }
        adjustHintPos();
    }
}

void TNNoteGroup::_q_beginChanged(int index, int oldIndex, TNRectNote *p) {
    Q_UNUSED(p);

    if (index == 0 || oldIndex == 0) {
        if (!m_begins.isEmpty()) {
            m_firstNote = *m_begins.front().second.begin();
        } else {
            m_firstNote = nullptr;
        }
        adjustHintPos();
    }
}
