#ifndef TNOTESAREA_H
#define TNOTESAREA_H

#include "CGraphicsScene.h"

#include "Elements/TNEdgeSprite.h"
#include "Elements/TNPlayhead.h"
#include "Elements/TNRectScreen.h"
#include "Elements/TNRubber.h"

#include "Controllers/TNNotesCtl.h"
#include "Controllers/TNPlayheadCtl.h"
#include "Controllers/TNPriorCtl.h"
#include "Controllers/TNScreenCtl.h"
#include "Controllers/TNSelectCtl.h"
#include "Controllers/TNSpriteCtl.h"
#include "Controllers/TNTransformCtl.h"

#include <QSet>

class TNotesScroll;
class TNotesAreaPrivate;

class TNotesArea : public CGraphicsScene {
    Q_OBJECT
    Q_DECLARE_PRIVATE(TNotesArea)
public:
    TNotesArea(TNotesScroll *parent = nullptr);
    ~TNotesArea();

protected:
    TNotesArea(TNotesAreaPrivate &d, TNotesScroll *parent = nullptr);

    QScopedPointer<TNotesAreaPrivate> d_ptr;

public:
    enum ElementZIndex {
        Sprite = 1,
        Note,
        Point,
        Lines,
        Vibrato,
        LyricEdit,
        RubberBand,
        Playhead,
        Bar,
    };

    struct StyleData {
        double lineWidth;

        QColor backDark;
        QColor backLight;
        QColor levelLine;
        QColor pitchLine;
        QColor sectionLine;
        QColor quarterLine;
        QColor timeLine;

        StyleData();
    };

    enum DrawMode {
        PlainSelect,
        DrawNote,
        DrawCurves,
    };

    enum AddPointMode {
        SingleClick,
        DoubleClick,
    };

    TNotesScroll *view() const;
    QPointF mousePosition() const;

public:
    StyleData styleData() const;
    void setStyleData(const StyleData &data);

protected:
    TNPriorCtl *m_priorCtl;

    TNTransformCtl *m_transCtl;
    TNSelectCtl *m_selectCtl;

    TNPlayheadCtl *m_playCtl;
    TNSpriteCtl *m_spriteCtl;
    TNScreenCtl *m_screenCtl;

    TNNotesCtl *m_notesCtl;

    TNotesScroll *m_view;
    TNotesArea::StyleData m_styleData;

    TNotesArea::DrawMode m_drawMode;
    TNotesArea::AddPointMode m_pointMode;

    bool m_moving;
    QPair<QSize, int> m_oldSizes;

    void adjustBackground();

    // ----------------------------------------  Transform  ----------------------------------------
public:
    int sectionCount() const;
    void setSectionCount(int sectionCount);
    void setSectionCountHint(int totalLength);

    int currentWidth() const;
    void setCurrentWidth(int currentWidth);

    int currentHeight() const;
    void setCurrentHeight(int currentHeight);

    int currentQuantize() const;
    void setCurrentQuantize(int currentQuantize);

    void setCurrentSizes(int w, int h, int q);

    int blankSections() const;
    void setBlankSections(int blankSections);

    double zeroLine() const;

    // ----------------------------------------  Props  ----------------------------------------
public:
    DrawMode drawMode() const;
    void setDrawMode(DrawMode drawMode);

    AddPointMode pointMode() const;
    void setPointMode(const AddPointMode &pointMode);

    // ----------------------------------------  Assist  ----------------------------------------
public:
    QPointF convertValueToPosition(int tick, int noteNum) const;
    QRectF convertValueToGeometry(int tick, int noteNum, int length) const;

    QPair<int, int> convertPositionToValue(QPointF pos) const;
    double convertWidthToLength(int width) const;

    // ----------------------------------------  Select  ----------------------------------------
public:
    bool isSelecting() const;
    bool mouseMoving() const;
    bool visionMoving() const;
    bool scrollZoomAllowed() const;

    QGraphicsItem *itemUnderMouse() const;

    // ----------------------------------------  Sprite  ----------------------------------------
public:
    void loadSprite(const QString &path);

    bool spriteVisible() const;
    void setSpriteVisible(bool spriteVisible);

    Qt::Corner spritePosition() const;
    void setSpritePosition(const Qt::Corner &spritePosition);

    double spriteAlpha() const;
    void setSpriteAlpha(double spriteAlpha);

    // ----------------------------------------  Notes  ----------------------------------------
public:
    void setNotesFromCommon(const QList<CommonNote> &notes);

    // ----------------------------------------  Events  ----------------------------------------

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    void focusOutEvent(QFocusEvent *event) override;

private:
    void _q_sceneRectChanged(const QRectF &rect);
};

#endif // TNOTESAREA_H
