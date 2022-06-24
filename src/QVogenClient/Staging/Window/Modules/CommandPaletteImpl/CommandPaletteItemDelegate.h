#ifndef COMMANDPALETTEITEMDELEGATE_H
#define COMMANDPALETTEITEMDELEGATE_H

#include <QDebug>
#include <QEvent>
#include <QListWidget>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QWidget>

#include "QTypeFace.h"

// Recent list items
class QCommandPaletteItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit QCommandPaletteItemDelegate(QObject *parent = nullptr);
    ~QCommandPaletteItemDelegate();

    friend class QCommandPaletteListWidget;

    enum ItemDataUserRole {
        Filename = Qt::UserRole + 1,
        Location,
        Date,
        Icon,
        IconSize,
        Type,
    };

public:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

protected:
    QTypeFace m_focusType;
    QTypeFace m_selectType;
    QTypeFace m_underline;

    QTypeFace m_fileType;
    QTypeFace m_locType;
    QTypeFace m_dateType;

    QMargins m_fileMargins;
    QMargins m_locMargins;
    QMargins m_dateMargins;

    QMargins m_iconMargins;

protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                     const QModelIndex &index) override;

signals:
    void leftClick(const QModelIndex &index);
    void rightClick(const QModelIndex &index);
};

#endif // COMMANDPALETTEITEMDELEGATE_H
