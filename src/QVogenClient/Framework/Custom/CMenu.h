#ifndef CMENU_H
#define CMENU_H

#include <QMenu>

#include "QSvgUri.h"

class CMenu : public QMenu {
    Q_OBJECT
    Q_PROPERTY(QSvgUri subIcon READ subIcon WRITE setSubIcon NOTIFY styleChanged)
    Q_PROPERTY(QSvgUri subIconHover READ subIconHover WRITE setSubIconHover NOTIFY styleChanged)
    Q_PROPERTY(QSize subIconMargins READ subIconMargins WRITE setSubIconMargins NOTIFY styleChanged)
public:
    explicit CMenu(QWidget *parent = nullptr);
    explicit CMenu(const QString &title, QWidget *parent = nullptr);
    ~CMenu();

public:
    QSvgUri subIcon() const;
    void setSubIcon(const QSvgUri &icon);

    QSvgUri subIconHover() const;
    void setSubIconHover(const QSvgUri &icon);

    QSize subIconMargins() const;
    void setSubIconMargins(const QSize &margins);

protected:
    QSvgUri m_subIcon;
    QSvgUri m_subIconHover;
    QSize m_subIconMargins;

    void paintEvent(QPaintEvent *event) override;

    void initStyleOption(QStyleOptionMenuItem *option, const QAction *action) const;

signals:
    void styleChanged();
};

#endif // CMENU_H
