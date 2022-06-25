#ifndef COMMANDPALETTEEVENTGUARD_H
#define COMMANDPALETTEEVENTGUARD_H

#include <QObject>

class QCommandPalette;

class CommandPaletteEventGuard : public QObject {
    Q_OBJECT
public:
    explicit CommandPaletteEventGuard(QCommandPalette *parent = nullptr);
    ~CommandPaletteEventGuard();

protected:
    QCommandPalette *cp;

    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
};

#endif // COMMANDPALETTEEVENTGUARD_H
