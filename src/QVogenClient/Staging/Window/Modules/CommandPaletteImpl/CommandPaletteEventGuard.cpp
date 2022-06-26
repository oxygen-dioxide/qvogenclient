#include "CommandPaletteEventGuard.h"
#include "../QCommandPalette.h"

#include <QApplication>
#include <QMouseEvent>

#include "../QCommandPalette_p.h"

CommandPaletteEventGuard::CommandPaletteEventGuard(QCommandPalette *parent)
    : QObject(parent), cp(parent) {
    qApp->installEventFilter(this);
}

CommandPaletteEventGuard::~CommandPaletteEventGuard() {
}

bool CommandPaletteEventGuard::eventFilter(QObject *obj, QEvent *event) {
    if (cp->isVisible()) {
        if (event->type() == QEvent::ShortcutOverride) {
            auto keyEvent = static_cast<QKeyEvent *>(event);
            int key = keyEvent->key();
            if (key == Qt::Key_Return || key == Qt::Key_Enter) {
                cp->d_func()->activateItem(cp->d_func()->listWidget->currentItem());
                return true;
            } else if (key == Qt::Key_Escape) {
                cp->d_func()->abandon();
                return true;
            }
        }
        if (!cp->asStdin()) {
            if (event->type() == QEvent::MouseButtonPress) {
                auto e = static_cast<QMouseEvent *>(event);
                if (!cp->rect().contains(cp->mapFromGlobal(e->globalPos()))) {
                    cp->d_func()->abandon();
                    return true;
                }
            }
        }
    }
    return false;
}
