#include "QCommandPalette_p.h"

#include "Types/Actions.h"

#include "Logs/CRecordHolder.h"

QCommandPalettePrivate::QCommandPalettePrivate() {
}

QCommandPalettePrivate::~QCommandPalettePrivate() {
    for (auto item : qAsConst(cachedItems)) {
        delete item;
    }
}

void QCommandPalettePrivate::init() {
    Q_Q(QCommandPalette);

    curCmdType = QCommandPalette::All;

    lineEdit = new QLineEdit();
    listWidget = new QCommandPaletteListWidget();

    layout = new QVBoxLayout();
    layout->addWidget(lineEdit);
    layout->addWidget(listWidget);
    q->setLayout(layout);

    q->setFocusPolicy(Qt::ClickFocus);
    q->setFocusProxy(lineEdit);

    lineEdit->setFocusPolicy(Qt::ClickFocus);
    listWidget->setFocusPolicy(Qt::ClickFocus);

    lineEdit->installEventFilter(q);
    listWidget->installEventFilter(q);

    q->connect(lineEdit, &QLineEdit::textChanged, q, &QCommandPalette::_q_textChanged);
    q->connect(listWidget, &QListWidget::currentRowChanged, q,
               &QCommandPalette::_q_currentRowChanged);
    q->connect(listWidget, &QListWidget::itemClicked, q, &QCommandPalette::_q_itemClicked);

    // Init Actions
    QList<int> quantizes{1, 2, 4, 6, 8, 12, 16, 24, 32 /*, 0*/};
    for (auto quantize : qAsConst(quantizes)) {
        QString desc;
        switch (quantize) {
        case 0:
            desc = QObject::tr("None");
            break;
        case 1:
            desc = QObject::tr("Quarter");
            break;
        default:
            desc = QObject::tr("%1 Quarter").arg(QString::number(quantize));
            break;
        }
        auto item =
            createItem(QIcon(), QSize(), ActionImpl::View_Quantization, desc, QString(), QString());
        item->setData(CommandItemTypes::Quantization, quantize);
        quantizationItems.append(item);
    }
}

void QCommandPalettePrivate::activateItem(QListWidgetItem *item) {
    Q_Q(QCommandPalette);

    switch (curCmdType) {
    case QCommandPalette::Quantization: {
        int quantize = item->data(CommandItemTypes::Quantization).toInt();
        qRecord->commitEState(CRecordHolder::Quantization, quantize);
        break;
    }
    default:
        break;
    }

    emit q->activated(item);
}

QListWidgetItem *QCommandPalettePrivate::createItem(const QIcon &icon, const QSize &size, int type,
                                                    const QString &filename,
                                                    const QString &location, const QString &date) {
    auto item = new QListWidgetItem();
    item->setData(QCommandPaletteItemDelegate::Filename, filename);
    item->setData(QCommandPaletteItemDelegate::Location, location);
    item->setData(QCommandPaletteItemDelegate::Date, date);
    item->setData(QCommandPaletteItemDelegate::Icon, icon);
    item->setData(QCommandPaletteItemDelegate::IconSize, size);
    item->setData(QCommandPaletteItemDelegate::Type, type);
    cachedItems.append(item);
    return item;
}
