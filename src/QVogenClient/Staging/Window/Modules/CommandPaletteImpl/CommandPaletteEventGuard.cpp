#include "CommandPaletteEventGuard.h"
#include "../QCommandPalette.h"

#include <QApplication>
#include <QMouseEvent>

CommandPaletteEventGuard::CommandPaletteEventGuard(QCommandPalette *parent)
    : QObject(parent), cp(parent) {
    qApp->installEventFilter(this);
}

CommandPaletteEventGuard::~CommandPaletteEventGuard() {
}

bool CommandPaletteEventGuard::eventFilter(QObject *obj, QEvent *event) {
    if (obj->isWidgetType() && cp->isVisible() && !cp->asStdin()) {
        if (event->type() == QEvent::MouseButtonPress) {
            auto e = static_cast<QMouseEvent *>(event);
            if (!cp->rect().contains(cp->mapFromGlobal(e->globalPos()))) {
                emit cp->abandoned();
                return true;
            }
        }
    }
    return false;
}
