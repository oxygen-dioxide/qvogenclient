#ifndef QCOMMANDPALETTE_H
#define QCOMMANDPALETTE_H

#include <QLayout>
#include <QListWidgetItem>
#include <QWidget>

#include "QPixelSize.h"

#include "Widgets/BaseContainer.h"

class QCommandPalettePrivate;

class QCommandPalette : public BaseContainer {
    Q_OBJECT
    Q_DECLARE_PRIVATE(QCommandPalette)
    Q_LAYOUT_PROPERTY_DELCARE
public:
    explicit QCommandPalette(QWidget *parent = nullptr);
    ~QCommandPalette();

    void reloadStrings();

    enum CommandType {
        NoCommands,
        All,
        RecentFiles,
        RecentDirs,
        ColorThemes,
        Languages,
        Quantization,
        Playhead,
        BuildIns,
        Plugins,
    };

    struct Hint {
        QString text;
        QString placeholder;
        bool hold;
        Hint(const QString &text, const QString &placeholder, bool hold = false)
            : text(text), placeholder(placeholder), hold(hold){};
        virtual void preview(const QString &text){Q_UNUSED(text)};
    };

    friend class CommandPaletteEventGuard;

public:
    void showCommands(CommandType type);
    void showLineEdit(Hint *hint);
    void finish();

    int count() const;
    bool asStdin() const;
    QString text() const;

signals:
    void activated(QListWidgetItem *item);
    void abandoned();

    void textChanged(const QString &text);
    void indexChanged(int index);

protected:
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

protected:
    QCommandPalette(QCommandPalettePrivate &d, QWidget *parent = nullptr);

    QScopedPointer<QCommandPalettePrivate> d_ptr;

private:
    void _q_textChanged(const QString &text);
    void _q_currentRowChanged(int row);
    void _q_itemClicked(QListWidgetItem *item);

signals:
};

#endif // QCOMMANDPALETTE_H
