#include "QCommandPalette.h"
#include "QCommandPalette_p.h"

#include "Logs/CRecordHolder.h"
#include "QsApplication.h"

#include <QApplication>
#include <QEvent>
#include <QKeyEvent>

class QCommandPalette::NotifyFilter : public CAppNotifyFilter {
public:
    NotifyFilter(QCommandPalette *cp) : CAppNotifyFilter(cp){};
    bool notifyFilter(QObject *obj, QEvent *event) override {
        return qobject_cast<QCommandPalette *>(this->obj)->notifyFilter(obj, event);
    };
};

QCommandPalette::QCommandPalette(QWidget *parent)
    : QCommandPalette(*new QCommandPalettePrivate(), parent) {
}

QCommandPalette::~QCommandPalette() {
    qApp->removeNotifyFilter(this);
}

void QCommandPalette::showCommands(QCommandPalette::CommandType type) {
    Q_D(QCommandPalette);

    d->reset();
    d->curCmdType = type;

    switch (type) {
    case Quantization: {
        // Show actions
        QListWidgetItem *curItem = nullptr;
        for (auto item : qAsConst(d->quantizationItems)) {
            d->listWidget->addItem(item);
            if (item->data(QCommandPalettePrivate::Quantization).toInt() ==
                qRecordCData.currentQuantize) {
                curItem = item;
            }
        }

        // Select Current
        if (curItem) {
            d->listWidget->setCurrentItem(curItem);
        }

        // Show hint
        d->lineEdit->setPlaceholderText(tr("Select quantization (Up/down keys to preview)"));
        break;
    }
    default:
        break;
    }

    d->listWidget->show();
    show();
}

void QCommandPalette::showLineEdit(const QString &hint, const QString &placeholder) {
    Q_D(QCommandPalette);
    d->reset();
    d->lineEdit->setPlaceholderText(placeholder);
    d->lineEdit->setText(hint);
    d->listWidget->hide();
    show();
}

void QCommandPalette::finish() {
    Q_D(QCommandPalette);

    hide();
    d->reset();
}

int QCommandPalette::count() const {
    Q_D(const QCommandPalette);
    return d->listWidget->count();
}

bool QCommandPalette::asStdin() const {
    Q_D(const QCommandPalette);
    return !d->listWidget->isVisible();
}

QString QCommandPalette::text() const {
    Q_D(const QCommandPalette);
    return d->lineEdit->text();
}

void QCommandPalette::showEvent(QShowEvent *event) {
    Q_UNUSED(event);
    setFocus();
}

bool QCommandPalette::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::ShortcutOverride) {
        Q_D(QCommandPalette);
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        int key = keyEvent->key();
        if (key == Qt::Key_Up || key == Qt::Key_Down) {
            if (obj != d->listWidget) {
                d->listWidget->setFocus();
                QApplication::sendEvent(d->listWidget, keyEvent);
                return true;
            }
        } else if (key == Qt::Key_Return || key == Qt::Key_Enter) {
            auto item = d->listWidget->currentItem();
            d->activateItem(item);
            return true;
        } else if (key == Qt::Key_Tab) {
            return true;
        } else if (key == Qt::Key_Escape) {
            emit abandoned();
            return true;
        } else {
            if (obj != d->lineEdit) {
                d->lineEdit->setFocus();
                QApplication::sendEvent(d->lineEdit, keyEvent);
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

bool QCommandPalette::notifyFilter(QObject *obj, QEvent *event) {
    Q_UNUSED(obj)
    if (obj->isWidgetType()) {
        if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *e = static_cast<QMouseEvent *>(event);
            if (isVisible() && !rect().contains(mapFromGlobal(e->globalPos()))) {
                emit abandoned();
            }
        }
    }
    return false;
}

QCommandPalette::QCommandPalette(QCommandPalettePrivate &d, QWidget *parent)
    : BaseContainer(parent), d_ptr(&d) {
    d.q_ptr = this;
    d.init();

    qApp->installNotifyFilter(new NotifyFilter(this));
}

void QCommandPalette::_q_textChanged(const QString &text) {
    emit textChanged(text);
}

void QCommandPalette::_q_currentRowChanged(int row) {
    emit indexChanged(row);
}

void QCommandPalette::_q_itemClicked(QListWidgetItem *item) {
    Q_D(QCommandPalette);
    d->activateItem(item);
}
