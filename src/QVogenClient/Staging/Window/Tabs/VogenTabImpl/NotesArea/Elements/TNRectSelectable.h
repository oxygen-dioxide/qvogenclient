#ifndef TNRECTSELECTABLE_H
#define TNRECTSELECTABLE_H

#include "TNRectObject.h"

class TNRectSelectable : public TNRectObject {
    Q_OBJECT
public:
    explicit TNRectSelectable(TNotesArea *area, QGraphicsItem *parent = nullptr);
    ~TNRectSelectable();

public:
    enum Behavior {
        NoBehavior,
        SelectOne,
        SelectOnly,
        SelectContinuously,
        DeselectOne,
        DeselectAll,
        IndependentStretch,
        RelativeStretch,
        AbsoluteStretch,
    };

    bool isSelected() const;
    void setSelected(bool selected);

protected:
    bool m_selected;

signals:
};

#endif // TNRECTSELECTABLE_H
