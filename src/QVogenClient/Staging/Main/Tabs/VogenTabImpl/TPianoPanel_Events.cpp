#include "TPianoPanel.h"

#include <QApplication>
#include <QEvent>
#include <QMetaType>
#include <QScrollBar>
#include <QWheelEvent>

void TPianoPanel::wheelEvent(QWheelEvent *event) {
    VogenPanel::wheelEvent(event);
}

bool TPianoPanel::eventFilter(QObject *obj, QEvent *event) {
    if (obj == m_pianoScroll) {
        if (event->type() == QEvent::Wheel) {
            QApplication::sendEvent(m_notesScroll->viewport(), event);
            return true;
        }
    }
    return VogenPanel::eventFilter(obj, event);
}
