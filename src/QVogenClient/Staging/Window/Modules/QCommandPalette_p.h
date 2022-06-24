#ifndef QCOMMANDPALETTEPRIVATE_H
#define QCOMMANDPALETTEPRIVATE_H

#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QtGlobal>

#include "QCommandPalette.h"
#include "CommandPaletteImpl/CommandPaletteListWidget.h"

class QCommandPalettePrivate {
    Q_DECLARE_PUBLIC(QCommandPalette)
public:
    QCommandPalettePrivate();
    ~QCommandPalettePrivate();

    void init();

    QCommandPalette *q_ptr;

    QVBoxLayout *layout;
    QLineEdit *lineEdit;
    QCommandPaletteListWidget *listWidget;

    QList<QListWidgetItem *> quantizationItems;

    // Status
    QCommandPalette::CommandType curCmdType;

    enum CommandItemTypes {
        Quantization = Qt::UserRole + 1000,
    };

    void activateItem(QListWidgetItem *item);

    // Item Manager
    QListWidgetItem *createItem(const QIcon &icon, const QSize &size, int type,
                                const QString &filename, const QString &location,
                                const QString &date);
    QList<QListWidgetItem *> cachedItems;
};

#endif // QCOMMANDPALETTEPRIVATE_H
