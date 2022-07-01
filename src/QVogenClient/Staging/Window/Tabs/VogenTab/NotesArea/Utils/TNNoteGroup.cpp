#include "TNNoteGroup.h"
#include "../TNotesArea.h"
#include "../TNotesScroll.h"

#include "Types/Events.h"

#include <QApplication>

TNNoteGroup::TNNoteGroup(TNotesArea *area, QObject *parent) : TNNoteList(parent), m_area(area) {
    m_firstNote = nullptr;
    m_cache = nullptr;

    connect(this, &TNNoteList::inserted, this, &TNNoteGroup::_q_inserted);
    connect(this, &TNNoteList::removed, this, &TNNoteGroup::_q_removed);
    connect(this, &TNNoteList::beginChanged, this, &TNNoteGroup::_q_beginChanged);
}

TNNoteGroup::~TNNoteGroup() {
    removeCache();
}

void TNNoteGroup::install() {
    m_hintItem = new TNGroupHint(this);
    m_area->addItem(m_hintItem);
    m_hintItem->setZValue(TNotesArea::GroupHint);
    m_hintItem->installEventFilter(this);
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

TWAudio::Audio *TNNoteGroup::cache() const {
    return m_cache;
}

void TNNoteGroup::setCache(const TWAudio::Audio &cache) {
    removeCache();
    m_cache = new TWAudio::Audio(cache);
}

void TNNoteGroup::removeCache() {
    if (m_cache) {
        delete m_cache;
        m_cache = nullptr;
    }
}

bool TNNoteGroup::eventFilter(QObject *obj, QEvent *event) {
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
