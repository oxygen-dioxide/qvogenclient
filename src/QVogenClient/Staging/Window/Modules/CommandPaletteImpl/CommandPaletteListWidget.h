#ifndef COMMANDPALETTELISTWIDGET_H
#define COMMANDPALETTELISTWIDGET_H

#include <QAction>
#include <QContextMenuEvent>
#include <QDebug>
#include <QListWidget>
#include <QMenu>

#include "CommandPaletteItemDelegate.h"
#include "QColorList.h"
#include "QTypeList.h"

class QCommandPaletteListWidget : public QListWidget {
    Q_OBJECT
    Q_PROPERTY(QTypeList styleData READ styleData WRITE setStyleData NOTIFY styleDataChanged)

public:
    explicit QCommandPaletteListWidget(QWidget *parent = nullptr);
    ~QCommandPaletteListWidget();

private:
    void init();

public:
    QTypeList styleData() const;
    void setStyleData(const QTypeList &list);

    QCommandPaletteItemDelegate *delegate() const;

    QSize contentsSize() const;

protected:
    QCommandPaletteItemDelegate *m_delegate;
    QSize oldContentsSize;

    void updateGeometries() override;

    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void contentsSizeUpdated(const QSize &size);
    void styleDataChanged();
};

#endif // COMMANDPALETTELISTWIDGET_H
