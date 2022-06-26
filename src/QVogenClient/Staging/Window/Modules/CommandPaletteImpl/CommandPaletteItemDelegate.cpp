#include "CommandPaletteItemDelegate.h"

#include <QDir>

QCommandPaletteItemDelegate::QCommandPaletteItemDelegate(QObject *pParent)
    : QStyledItemDelegate(pParent) {
    m_selectType = QTypeFace(Qt::transparent, 1);
    m_focusType = QTypeFace(Qt::transparent, 1);
    m_underline = QTypeFace(Qt::lightGray, 1);

    m_fileMargins = QMargins(5, 5, 5, 5);
    m_locMargins = QMargins(5, 5, 5, 5);
    m_dateMargins = QMargins(5, 5, 5, 5);

    m_iconMargins = QMargins(5, 5, 5, 5);
}

QCommandPaletteItemDelegate::~QCommandPaletteItemDelegate() {
}

QSize QCommandPaletteItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const {
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    QString location = QDir::toNativeSeparators(index.data(Location).toString());

    int iconHeight =
        index.data(IconSize).toSize().height() + m_iconMargins.top() + m_iconMargins.bottom();
    int midHeight = QFontMetrics(m_fileType.font()).height() + m_fileMargins.top() +
                    m_fileMargins.bottom() +
                    (location.isEmpty() ? 0
                                        : (QFontMetrics(m_locType.font()).height() +
                                           m_locMargins.top() + m_locMargins.bottom()));
    int dateHeight =
        QFontMetrics(m_dateType.font()).height() + m_dateMargins.top() + m_dateMargins.bottom();
    int h = qMax(iconHeight, qMax(midHeight, dateHeight));
    if (size.height() < h) {
        size.setHeight(h);
    }

    return size;
}

void QCommandPaletteItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const {
    QRect rect = option.rect;

    // Fetch data
    QString filename = index.data(Qt::DisplayRole).toString();
    QString location = QDir::toNativeSeparators(index.data(Location).toString());
    QString date = index.data(Date).toString();

    QIcon icon = index.data(Icon).value<QIcon>();
    QSize iconSize = index.data(IconSize).toSize();

    // Calculate size
    QFont fileFont = m_fileType.font();
    QFont locFont = m_locType.font();
    QFont dateFont = m_dateType.font();

    QFontMetrics fileFontM(fileFont);
    QFontMetrics locFontM(locFont);
    QFontMetrics dateFontM(dateFont);

    int fileFontHeight = fileFontM.height();
    int locFontHeight = locFontM.height();
    int dateFontHeight = dateFontM.height();

    int dateWidth = dateFontM.horizontalAdvance(date);

    int iconHeight =
        index.data(IconSize).toSize().height() + m_iconMargins.top() + m_iconMargins.bottom();
    int midHeight =
        fileFontHeight + m_fileMargins.top() + m_fileMargins.bottom() +
        (location.isEmpty() ? 0 : (locFontHeight + m_locMargins.top() + m_locMargins.bottom()));
    int dateHeight = dateFontHeight + m_dateMargins.top() + m_dateMargins.bottom();

    int realHeight = rect.height();

    QRect iconRect;
    iconRect.setTop(rect.top() + (realHeight - iconHeight) / 2 + m_iconMargins.top());
    iconRect.setLeft(m_iconMargins.left());
    iconRect.setSize(iconSize);

    int iconRight = icon.isNull() ? 0 : (iconRect.right() + m_iconMargins.right());
    int wordsWidth = rect.width() - iconRight;

    QRect dateRect;
    dateRect.setTop(rect.top() + (realHeight - dateHeight) / 2 + m_dateMargins.top());
    dateRect.setRight(rect.width() - m_dateMargins.right());
    dateRect.setLeft(qMax(iconRect.right() + wordsWidth / 2 + m_dateMargins.left(),
                          dateRect.right() - dateWidth));
    dateRect.setHeight(dateFontHeight);

    int dateLeft = dateRect.left() - m_dateMargins.left();

    QRect fileRect;
    fileRect.setTop(rect.top() + (realHeight - midHeight) / 2 + m_fileMargins.top());
    fileRect.setLeft(iconRight + m_fileMargins.left());
    fileRect.setRight(dateLeft - m_fileMargins.right());
    fileRect.setHeight(fileFontHeight);

    int fileBottom = fileRect.bottom() + m_fileMargins.bottom();

    QRect locRect;
    locRect.setTop(fileBottom + m_locMargins.top());
    locRect.setLeft(iconRight + m_locMargins.left());
    locRect.setRight(dateLeft - m_locMargins.right());
    locRect.setHeight(locFontHeight);

    painter->setRenderHint(QPainter::Antialiasing);

    // Status
    painter->setPen(Qt::NoPen);
    if (option.state & QStyle::State_Selected) {
        painter->setBrush(m_selectType.color());
        painter->drawRect(rect);
    } else if (option.state & QStyle::State_MouseOver) {
        painter->setBrush(m_focusType.color());
        painter->drawRect(rect);
    }
    painter->setBrush(Qt::NoBrush);

    // Icon
    if (!icon.isNull()) {
        painter->drawPixmap(iconRect, icon.pixmap(iconSize));
    }

    painter->setFont(dateFont);
    painter->setPen(m_dateType.color());

    // Last open time
    {
        QString text = date;
        const QRect &dst = dateRect;
        const QFontMetrics &fm = dateFontM;

        int width = fm.horizontalAdvance(text);
        if (width > dst.width()) {
            text = fm.elidedText(text, Qt::ElideRight, dst.width());
        }
        painter->drawText(dst, Qt::AlignLeft | Qt::AlignVCenter, text);
    }

    painter->setFont(fileFont);
    painter->setPen(m_fileType.color());

    // Filename
    {
        QString text = filename;
        const QRect &dst = fileRect;
        const QFontMetrics &fm = fileFontM;

        int width = fm.horizontalAdvance(text);
        if (width > dst.width()) {
            text = fm.elidedText(text, Qt::ElideRight, dst.width());
        }
        painter->drawText(dst, Qt::AlignLeft | Qt::AlignVCenter, text);
    }

    painter->setFont(locFont);
    painter->setPen(m_locType.color());

    // Address
    if (!location.isEmpty()) {
        QString text = location;
        const QRect &dst = locRect;
        const QFontMetrics &fm = locFontM;

        int width = fm.horizontalAdvance(text);
        if (width > dst.width()) {
            text = fm.elidedText(text, Qt::ElideRight, dst.width());
        }
        painter->drawText(dst, Qt::AlignLeft | Qt::AlignVCenter, text);
    }

    painter->setPen(QPen(m_underline.color(), m_underline.widthF()));
    painter->drawLine(rect.bottomLeft(), rect.bottomRight());
}

bool QCommandPaletteItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                              const QStyleOptionViewItem &option,
                                              const QModelIndex &index) {
    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

        if (mouseEvent->button() == Qt::RightButton) {
            emit rightClick(index);
        } else if (mouseEvent->button() == Qt::LeftButton) {
            emit leftClick(index);
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
