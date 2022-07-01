#ifndef TSECTIONSAREA_H
#define TSECTIONSAREA_H

#include <QWidget>

#include "Macros.h"

class VogenTab;

class TSectionsArea : public QWidget {
    Q_OBJECT
public:
    explicit TSectionsArea(QWidget *parent = nullptr);
    ~TSectionsArea();

    Q_FIND_PARENT_WIDGET(VogenTab, tab)

    struct Data {
        double lineWidth;
        double shortRatio;
        double numberRatio;

        QColor line;
        QColor number;
        QColor beat;

        QColor globalTempo;
        QColor tempo;

        Data();
    };

public:
    Data styleData() const;
    void setStyleData(const Data &data);

    int currentWidth() const;
    void setCurrentWidth(int currentWidth);

    QPoint beat() const;
    void setBeat(const QPoint &beat);

    double tempo() const;
    void setTempo(double tempo);

    int horizontalValue() const;
    void setHorizontalValue(int horizontalValue);

    int blankSections() const;
    void setBlankSections(int blankSections);

protected:
    Data m_styleData;
    int m_currentWidth;

    double m_tempo;
    QPoint m_beat;

    int m_horizontalValue;

    int m_blankSections;

    QPixmap backgroundBrush() const;

    QPair<QRect, QRect> textRects() const;

    void paintEvent(QPaintEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // TSECTIONSAREA_H
