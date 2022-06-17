#ifndef CMENU_H
#define CMENU_H

#include <QMenu>

class CMenu : public QMenu {
    Q_OBJECT
public:
    explicit CMenu(QWidget *parent = nullptr);
    explicit CMenu(const QString &title, QWidget *parent = nullptr);
    ~CMenu();

protected:
    void paintEvent(QPaintEvent *event) override;
    void initStyleOption(QStyleOptionMenuItem *option, const QAction *action) const;

signals:
};

#endif // CMENU_H
