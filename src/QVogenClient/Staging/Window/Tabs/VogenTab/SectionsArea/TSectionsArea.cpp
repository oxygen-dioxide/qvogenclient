#include "TSectionsArea.h"

#include <QPaintEvent>
#include <QPainter>

#include <QtMath>

#include "../Utils/Events/TPianoRollEvent.h"
#include "../VogenTab.h"

Q_FIND_PARENT_WIDGET_DECLARE(TSectionsArea, VogenTab, tab)

const int USE_HEIGHT_FACTOR = 2;

TSectionsArea::Data::Data() {
    lineWidth = 1;
    shortRatio = 0.5;
    numberRatio = 0.3;

    line = Qt::lightGray;
    number = QColor(0x333333);
    beat = Qt::black;

    globalTempo = Qt::black;
    tempo = Qt::red;
}

TSectionsArea::TSectionsArea(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    m_horizontalValue = 0;
    m_beat = QPoint(4, 4);
    m_tempo = 120.0;
    m_blankSections = 1;
}

TSectionsArea::~TSectionsArea() {
}

TSectionsArea::Data TSectionsArea::styleData() const {
    return m_styleData;
}

void TSectionsArea::setStyleData(const Data &data) {
    m_styleData = data;
    update();
}

int TSectionsArea::currentWidth() const {
    return m_currentWidth;
}

void TSectionsArea::setCurrentWidth(int currentWidth) {
    m_currentWidth = currentWidth;
    update();
}

QPoint TSectionsArea::beat() const {
    return m_beat;
}

void TSectionsArea::setBeat(const QPoint &beat) {
    m_beat = beat;
    update();
}

double TSectionsArea::tempo() const {
    return m_tempo;
}

void TSectionsArea::setTempo(double tempo) {
    m_tempo = tempo;
    update();
}

int TSectionsArea::horizontalValue() const {
    return m_horizontalValue;
}

void TSectionsArea::setHorizontalValue(int horizontalValue) {
    m_horizontalValue = horizontalValue;
    update();
}

int TSectionsArea::blankSections() const {
    return m_blankSections;
}

void TSectionsArea::setBlankSections(int blankSections) {
    m_blankSections = blankSections;
    update();
}

QPixmap TSectionsArea::backgroundBrush() const {
    int curWidth = m_currentWidth;
    double lineWidth = m_styleData.lineWidth;

    double totalWidth = double(curWidth * 4) / m_beat.y() * m_beat.x();
    double totalHeight = height();

    double usedHeight = totalHeight / USE_HEIGHT_FACTOR;

    if (totalWidth == 0) {
        return QPixmap(QSize(totalWidth, totalHeight));
    }

    QPixmap pixmap(totalWidth, totalHeight);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.translate(0, 0);

    painter.setBrush(m_styleData.line);

    int curAdsorb = m_beat.x();
    int threshold = font().pixelSize() * 2;
    double w = totalWidth / curAdsorb;
    while (w < threshold && curAdsorb > 1) {
        if (curAdsorb % 2 == 0) {
            curAdsorb /= 2;
        } else if (curAdsorb % 3 == 0) {
            curAdsorb /= 3;
        } else if (curAdsorb % 5 == 0) {
            curAdsorb /= 5;
        } else if (curAdsorb % 7 == 0) {
            curAdsorb /= 7;
        } else {
            curAdsorb = 1;
        }
        w = totalWidth / curAdsorb;
    }

    double sh = usedHeight * m_styleData.shortRatio;
    for (int i = 1; i < curAdsorb; ++i) {
        painter.drawRect(QRectF(w * i - lineWidth / 2, usedHeight - sh, lineWidth, sh));
    }

    painter.setBrush(m_styleData.line);
    painter.drawRect(QRectF(0, 0, lineWidth / 2, usedHeight));
    painter.drawRect(QRectF(totalWidth - lineWidth / 2, 0, lineWidth / 2, usedHeight));

    painter.drawRect(QRectF(0, usedHeight - lineWidth / 2, totalWidth, lineWidth));

    return pixmap;
}

QPair<QRect, QRect> TSectionsArea::textRects() const {
    QPair<QRect, QRect> res;

    const QFont &font = this->font();
    QFontMetrics fm(font);

    double halfHeight = height() / USE_HEIGHT_FACTOR;
    double fontHeight = fm.height();
    double fontOffset = font.pixelSize() * m_styleData.numberRatio;

    // Time sig
    {
        QString text =
            QString("%1/%2").arg(QString::number(m_beat.x()), QString::number(m_beat.y()));
        double fontWidth = fm.horizontalAdvance(text);
        QRect textRect(fontOffset, halfHeight + (halfHeight - fontHeight) / 2, fontWidth,
                       fontHeight);
        res.first = textRect;
    }

    // Tempo
    {
        QString text = QString::number(m_tempo);
        double fontWidth = fm.horizontalAdvance(text);
        QRect textRect =
            QRect(0, halfHeight + (halfHeight - fontHeight) / 2, fontWidth, fontHeight);
        res.second = textRect;
    }

    res.second.moveLeft(res.first.right() + fontHeight / 2);

    return res;
}

void TSectionsArea::paintEvent(QPaintEvent *event) {
    QPixmap bp = backgroundBrush();

    int bpw = bp.width();
    int first = m_horizontalValue / bpw;
    int offset = m_horizontalValue % bpw;
    int bpcnt = qCeil(double(width() + offset) / bpw);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setClipRegion(event->region());

    const QFont &font = this->font();
    QFontMetrics fm(font);

    double halfHeight = bp.height() / USE_HEIGHT_FACTOR;
    double fontHeight = fm.height();
    double fontOffset = font.pixelSize() * m_styleData.numberRatio;

    {
        // Draw time sig
        QString text =
            QString("%1/%2").arg(QString::number(m_beat.x()), QString::number(m_beat.y()));
        double fontWidth = fm.horizontalAdvance(text);
        QRect textRect(fontOffset, halfHeight + (halfHeight - fontHeight) / 2, fontWidth,
                       fontHeight);
        painter.setPen(m_styleData.beat);
        painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text);

        // Draw bpm
        text = QString::number(m_tempo);
        fontWidth = fm.horizontalAdvance(text);
        textRect = QRect(textRect.right() + fontHeight / 2,
                         halfHeight + (halfHeight - fontHeight) / 2, fontWidth, fontHeight);

        painter.setPen(m_styleData.tempo);
        painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text);
    }

    // Draw Number
    painter.translate(-offset, 0);
    painter.setPen(m_styleData.number);
    for (int i = 0; i < bpcnt; ++i) {
        QString text = QString::number(i + first - m_blankSections);
        double fontWidth = fm.horizontalAdvance(text);
        QRect textRect(i * bpw + fontOffset, (halfHeight - fontHeight) / 2, fontWidth, fontHeight);

        painter.drawPixmap(QPointF(i * bpw, 0), bp);
        painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text);
    }
}

void TSectionsArea::mousePressEvent(QMouseEvent *event) {
    auto rects = textRects();
    auto pos = event->pos();

    if (rects.first.contains(pos)) {
        TPianoRollEvent e(TPianoRollEvent::ChangeTimeSig);
        e.dispatch(this);
    } else if (rects.second.contains(pos)) {
        TPianoRollEvent e(TPianoRollEvent::ChangeTempo);
        e.dispatch(this);
    }

    QWidget::mousePressEvent(event);
}
