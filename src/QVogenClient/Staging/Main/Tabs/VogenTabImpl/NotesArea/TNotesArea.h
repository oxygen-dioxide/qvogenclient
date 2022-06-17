#ifndef TNOTESAREA_H
#define TNOTESAREA_H

#include "CGraphicsScene.h"

#include "Elements/TNEdgeSprite.h"
#include "Elements/TNPlayhead.h"
#include "Elements/TNRectScreen.h"
#include "Elements/TNRubber.h"

#include <QSet>

class TNotesScroll;

class TNotesArea : public CGraphicsScene {
    Q_OBJECT
public:
    explicit TNotesArea(TNotesScroll *parent = nullptr);
    ~TNotesArea();

    const double spriteZIndex = 1;
    const double noteZIndex = 2;
    const double pointZIndex = 3;
    const double linesZIndex = 4;
    const double vibratoZIndex = 5;
    const double lyricEditZIndex = 6;
    const double rubberBandZIndex = 7;
    const double playHeadZIndex = 8;
    const double barZIndex = 9;

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

private:
    void init();

public:
    StyleData styleData() const;
    void setStyleData(const StyleData &data);

    int sectionCount() const;
    void setSectionCount(int sectionCount);

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

    DrawMode drawMode() const;
    void setDrawMode(DrawMode drawMode);

    AddPointMode pointMode() const;
    void setPointMode(const AddPointMode &pointMode);

protected:
    TNotesScroll *m_view;
    StyleData m_styleData;

    int m_sectionCount;

    int m_currentWidth;
    int m_currentHeight;
    int m_currentQuantize;

    int m_blankSections;

    DrawMode m_drawMode;
    AddPointMode m_pointMode;

    void adjustSize();
    void adjustBackground();

public:
    // ========================================  Elements ========================================
    // ----------------------------------------  Screen  ----------------------------------------
protected:
    TNRectScreen *m_screen;

    void updateScreen();

private:
    void initScreenElements();

    // ----------------------------------------  Select  ----------------------------------------
protected:
    TNRubber *m_rubber;

    bool m_moving;

    bool m_scrollDrag;
    bool m_zoomDrag;
    QPoint m_zoomAnchor;

    void stopSelecting();

    void setScrollDrag(bool scrollDrag);
    void setZoomDrag(bool zoomDrag);

private:
    void initSelectElements();

public:
    bool isSelecting() const;
    bool isScrollDragging() const;
    bool isZoomDragging() const;

    bool scrollZoomAllowed() const;

    // ----------------------------------------  Sprite  ----------------------------------------
protected:
    TNEdgeSprite *m_sprite;

    bool m_spriteVisible;
    Qt::Corner m_spritePosition;

    void updateSprite();

private:
    void initSpriteElements();

public:
    void loadSprite(const QString &path);

    bool spriteVisible() const;
    void setSpriteVisible(bool spriteVisible);

    Qt::Corner spritePosition() const;
    void setSpritePosition(const Qt::Corner &spritePosition);

    double spriteAlpha() const;
    void setSpriteAlpha(double spriteAlpha);

    // =========================================================================================
    // Events
protected:
    TNPlayhead *m_playhead;

    int m_playToNote;
    qint64 m_playToPosition;
    double m_playToTick;

    void updatePlayhead();

private:
    void initPlayElements();

public:
    void setPlayPosition(double x);

    // ----------------------------------------  Events  ----------------------------------------
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

    void viewMoveEvent(QGraphicsSceneMoveEvent *event) override;
    void viewResizeEvent(QGraphicsSceneResizeEvent *event) override;

private:
    void handleSceneRectChanged(const QRectF &rect);
    void rearrangeElements();
    void settleMouseUp(); // Focus out or mouse release
};

#endif // TNOTESAREA_H
