#ifndef TNRECTOBJECT_H
#define TNRECTOBJECT_H

#include <QGraphicsRectItem>
#include <QObject>

class TNotesArea;

class TNRectObject : public QObject, public QGraphicsRectItem {
    Q_OBJECT
public:
    explicit TNRectObject(TNotesArea *area, QGraphicsItem *parent = nullptr);
    ~TNRectObject();

private:
    void init();

public:
    TNotesArea *area() const;

    QSizeF size() const;
    double width() const;
    double height() const;

    friend QDebug operator<<(QDebug debug, const TNRectObject *obj);

protected:
    TNotesArea *m_area;

    virtual void layoutRequestEvent(QEvent *event);

    bool event(QEvent *event) override;

signals:
};

#endif // TNRECTOBJECT_H
