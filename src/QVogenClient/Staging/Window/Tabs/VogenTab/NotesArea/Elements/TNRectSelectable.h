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

    bool isEnabled() const;
    void setEnabled(bool enabled);

    int begin() const;
    void setBegin(int begin);

    int end() const;
    void setEnd(int end);

protected:
    int m_begin;
    int m_end;

    bool m_selected;
    bool m_enabled;

signals:
    void beginChanged(int val, int oldVal);
    void endChanged(int val, int oldVal);
};

#endif // TNRECTSELECTABLE_H
