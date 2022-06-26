#ifndef QCOMMANDPALETTEPRIVATE_H
#define QCOMMANDPALETTEPRIVATE_H

#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QtGlobal>

#include "CommandPaletteImpl/CommandPaletteEventGuard.h"
#include "CommandPaletteImpl/CommandPaletteListWidget.h"
#include "QCommandPalette.h"

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
    QList<QListWidgetItem *> languageItems;

    // Status
    QCommandPalette::CommandType curCmdType;

    enum CommandItemTypes {
        Quantization = Qt::UserRole + 1000,
        LanguageIndex,
    };

    void activateItem(QListWidgetItem *item);
    void previewItem(QListWidgetItem *item);
    void abandon();

    void reset();

    // Item Manager
    QListWidgetItem *createItem(const QIcon &icon, const QSize &size, int type,
                                const QString &filename, const QString &location,
                                const QString &date);
    QList<QListWidgetItem *> cachedItems;

    // Event Guard
    CommandPaletteEventGuard *guard;

    void reloadStrings_helper();
};

#endif // QCOMMANDPALETTEPRIVATE_H
