#include "TSectionsArea.h"

#include <QPaintEvent>
#include <QPainter>

#include <QtMath>

#include "../VogenTab.h"

Q_FIND_PARENT_WIDGET_DECLARE(TSectionsArea, VogenTab, tab)

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

    double totalWidth = curWidth * 4;
    double totalHeight = height();

    double halfHeight = totalHeight / 2;

    if (totalWidth == 0) {
        return QPixmap(QSize(totalWidth, totalHeight));
    }

    QPixmap pixmap(totalWidth, totalHeight);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.translate(0, 0);

    painter.setBrush(m_styleData.line);

    int curAdsorb = 4;
    int threshold = font().pixelSize() * 2;
    double w = totalWidth / curAdsorb;
    while (w < threshold && curAdsorb > 1) {
        curAdsorb--;
        while (curAdsorb % 2 != 0 && curAdsorb > 1) {
            curAdsorb--;
        }
        w = totalWidth / curAdsorb;
    }

    double sh = halfHeight * m_styleData.shortRatio;
    for (int i = 1; i < curAdsorb; ++i) {
        painter.drawRect(QRectF(w * i - lineWidth / 2, halfHeight - sh, lineWidth, sh));
    }

    painter.setBrush(m_styleData.line);
    painter.drawRect(QRectF(0, 0, lineWidth / 2, halfHeight));
    painter.drawRect(QRectF(totalWidth - lineWidth / 2, 0, lineWidth / 2, halfHeight));

    painter.drawRect(QRectF(0, totalHeight / 2 - lineWidth / 2, totalWidth, lineWidth));

    return pixmap;
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

    double halfHeight = bp.height() / 2;
    double fontHeight = fm.height();
    double fontOffset = font.pixelSize() * m_styleData.numberRatio;

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
