#include "TPianoScroll.h"
#include "TPianoArea.h"

#include <QEvent>
#include <QResizeEvent>
#include <QScrollBar>

TPianoScroll::TPianoScroll(QWidget *parent) : NoneScrollArea(parent) {
    m_area = nullptr;
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
}

TPianoScroll::~TPianoScroll() {
}

TPianoArea *TPianoScroll::area() const {
    return m_area;
}

void TPianoScroll::setArea(TPianoArea *area) {
    if (m_area) {
        m_area->removeEventFilter(this);
    }
    m_area = area;
    m_area->installEventFilter(this);

    setWidget(area);
}

QSize TPianoScroll::sizeHint() const {
    QSize sz = NoneScrollArea::sizeHint();
    return m_area ? QSize(m_area->width(), sz.height()) : sz;
}

QSize TPianoScroll::minimumSizeHint() const {
    return sizeHint();
}

