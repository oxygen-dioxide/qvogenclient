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

protected:
    TNotesArea *m_area;

signals:
};

#endif // TNRECTOBJECT_H
