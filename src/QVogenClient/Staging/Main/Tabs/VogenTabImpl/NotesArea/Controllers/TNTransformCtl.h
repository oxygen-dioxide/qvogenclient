#ifndef TNTRANSFORMCTL_H
#define TNTRANSFORMCTL_H

#include "TNController.h"

class TNTransformCtl : public TNController {
    Q_OBJECT
public:
    explicit TNTransformCtl(TNotesArea *parent = nullptr);
    ~TNTransformCtl();

public:
    void install() override;

    int sectionCount() const;
    void setSectionCount(int sectionCount);

    int currentWidth() const;
    void setCurrentWidth(int currentWidth);

    int currentHeight() const;
    void setCurrentHeight(int currentHeight);

    int currentQuantize() const;
    void setCurrentQuantize(int currentQuantize);

    int blankSections() const;
    void setBlankSections(int blankSections);

    void setCurrentSizes(int w, int h, int q);

    double zeroLine() const;

    bool scrollDrag() const;
    bool zoomDrag() const;

protected:
    int m_sectionCount;

    int m_currentWidth;
    int m_currentHeight;
    int m_currentQuantize;

    int m_blankSections;

    // Drag or zoom
    bool m_scrollDrag;
    bool m_zoomDrag;
    QPoint m_zoomAnchor;

    void adjustSize();

    void setScrollDrag(bool scrollDrag);
    void setZoomDrag(bool zoomDrag);

    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
};

#endif // TNTRANSFORMCTL_H
