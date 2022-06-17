#ifndef TPIANOSCROLL_H
#define TPIANOSCROLL_H

#include "Widgets/NoneScrollArea.h"

class TPianoArea;

class TPianoScroll : public NoneScrollArea {
    Q_OBJECT
public:
    explicit TPianoScroll(QWidget *parent = nullptr);
    ~TPianoScroll();

    friend class TuningScope;

public:
    TPianoArea *area() const;
    void setArea(TPianoArea *area);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    TPianoArea *m_area;

signals:
};

#endif // TPIANOSCROLL_H
