#ifndef TNOTESSCROLL_H
#define TNOTESSCROLL_H

#include "CGraphicsView.h"

#include "QColorList.h"
#include "QLineStyle.h"

#include <QDebug>

class TNotesArea;

class TNotesScroll : public CGraphicsView {
    Q_OBJECT
public:
    explicit TNotesScroll(QWidget *parent = nullptr);
    ~TNotesScroll();

    friend class TuningScope;
    friend class TNotesArea;

    struct ControlData {
        Qt::KeyboardModifiers moveH; // move horizontally
        Qt::KeyboardModifiers zoomH; // zoom horizontally
        Qt::KeyboardModifiers zoomV; // zoom vertically

        Qt::KeyboardModifiers selectS; // select singlely
        Qt::KeyboardModifiers selectC; // select continuously

        Qt::KeyboardModifiers dragS; // drag select
        Qt::KeyboardModifiers dragM; // drag move
        Qt::KeyboardModifiers dragZ; // drag zoom

        ControlData();
    };

    // Events
public:
    ControlData controlData() const;
    void setControlData(const ControlData &data);

    TNotesArea *area() const;

protected:
    ControlData m_controlData;

    void wheelEvent(QWheelEvent *event) override;
    void scrollContentsBy(int dx, int dy) override;

public:
    void emitZoomRequest(Qt::Orientation orient, int offset, const QPoint &anchor);

signals:
    void viewMoved(int dx, int dy);
    void zoomRequested(Qt::Orientation orient, int offset, const QPoint &anchor);
};

#endif // TNOTESSCROLL_H
