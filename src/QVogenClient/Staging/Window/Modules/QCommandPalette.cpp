#include "QCommandPalette.h"
#include "QCommandPalette_p.h"

#include "DataManager.h"
#include "Logs/CRecordHolder.h"
#include "QsApplication.h"

#include <QApplication>
#include <QEvent>
#include <QKeyEvent>

QCommandPalette::QCommandPalette(QWidget *parent)
    : QCommandPalette(*new QCommandPalettePrivate(), parent) {
}

QCommandPalette::~QCommandPalette() {
}

void QCommandPalette::reloadStrings() {
    Q_D(QCommandPalette);
    d->reloadStrings_helper();
}

void QCommandPalette::showCommands(QCommandPalette::CommandType type) {
    Q_D(QCommandPalette);

    if (isVisible()) {
        d->abandon();
        d->reset();
    }

    d->curCmdType = type;

    QSignalBlocker sb1(d->listWidget);
    QSignalBlocker sb2(d->lineEdit);

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
        d->lineEdit->setPlaceholderText(tr("Select quantization unit"));
        break;
    }
    case Languages: {
        // Show actions
        QListWidgetItem *curItem = nullptr;
        for (auto item : qAsConst(d->languageItems)) {
            d->listWidget->addItem(item);
            if (item->data(QCommandPalettePrivate::LanguageIndex).toInt() ==
                qRecordCData.translationIndex) {
                curItem = item;
            }
        }

        // Select Current
        if (curItem) {
            d->listWidget->setCurrentItem(curItem);
        }

        // Show hint
        d->lineEdit->setPlaceholderText(tr("Select translation (Up/down keys to preview)"));
        break;
    }

    case ColorThemes: {
        // Show actions
        QListWidgetItem *curItem = nullptr;
        for (auto item : qAsConst(d->themeItems)) {
            d->listWidget->addItem(item);
            if (item->data(QCommandPalettePrivate::ThemeIndex).toInt() == qRecordCData.themeIndex) {
                curItem = item;
            }
        }

        // Select Current
        if (curItem) {
            d->listWidget->setCurrentItem(curItem);
        }

        // Show hint
        d->lineEdit->setPlaceholderText(tr("Select color theme (Up/down keys to preview)"));
        break;
    }
    default:
        break;
    }

    d->listWidget->show();
    show();
}

void QCommandPalette::showLineEdit(Hint *hint) {
    Q_D(QCommandPalette);

    Q_ASSERT(hint);

    if (isVisible()) {
        d->abandon();
        d->reset();
    }

    d->curCmdType = NoCommands;
    d->hold = hint->hold;

    d->lineEdit->setPlaceholderText(hint->placeholder);
    d->lineEdit->setText(hint->text);
    d->listWidget->hide();

    show();
}

void QCommandPalette::showList(const QStringList &list, int current, const QString &placeholder) {
    Q_D(QCommandPalette);

    if (isVisible()) {
        d->abandon();
        d->reset();
    }

    d->curCmdType = Custom;

    d->lineEdit->setPlaceholderText(placeholder);

    for (int i = 0; i < list.size(); ++i) {
        auto item = d->createItem(QIcon(), QSize(), 0, QString(), QString(), QString());
        item->setData(QCommandPalettePrivate::TempIndex, i);
        item->setText(list.at(i));
        d->listWidget->addItem(item);
        if (i == current) {
            d->listWidget->setCurrentItem(item);
        }
    }

    d->listWidget->show();
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

int QCommandPalette::indexOf(QListWidgetItem *item) const {
    Q_D(const QCommandPalette);
    for (int i = 0; i < d->listWidget->count(); ++i) {
        if (d->listWidget->item(i) == item) {
            return i;
        }
    }
    return -1;
}

void QCommandPalette::showEvent(QShowEvent *event) {
    Q_UNUSED(event);
    setFocus();
}

bool QCommandPalette::eventFilter(QObject *obj, QEvent *event) {
    if (/*event->type() == QEvent::KeyPress || */ event->type() == QEvent::ShortcutOverride) {
        Q_D(QCommandPalette);
        if (isVisible()) {
            auto keyEvent = static_cast<QKeyEvent *>(event);
            int key = keyEvent->key();
            if (key == Qt::Key_Up || key == Qt::Key_Down) {
                if (obj != d->listWidget) {
                    d->listWidget->setFocus();
                    QApplication::sendEvent(d->listWidget, keyEvent);
                    return true;
                }
            } else if (key == Qt::Key_Tab) {
                return true;
            } else {
                if (obj != d->lineEdit) {
                    d->lineEdit->setFocus();
                    QApplication::sendEvent(d->lineEdit, keyEvent);
                    return true;
                }
            }
        }
    }

    return QWidget::eventFilter(obj, event);
}

QCommandPalette::QCommandPalette(QCommandPalettePrivate &d, QWidget *parent)
    : BaseContainer(parent), d_ptr(&d) {
    d.q_ptr = this;
    d.init();

    Q_TR_NOTIFY(QCommandPalette);
}

void QCommandPalette::_q_textChanged(const QString &text) {
    emit textChanged(text);
}

void QCommandPalette::_q_currentRowChanged(int row) {
    Q_D(QCommandPalette);
    d->previewItem(d->listWidget->currentItem());
    emit indexChanged(row);
}

void QCommandPalette::_q_itemClicked(QListWidgetItem *item) {
    Q_D(QCommandPalette);
    d->activateItem(item);
}
