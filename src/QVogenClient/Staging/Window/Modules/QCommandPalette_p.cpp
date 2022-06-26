#include "QCommandPalette_p.h"

#include "Types/Actions.h"

#include "DataManager.h"
#include "Logs/CRecordHolder.h"

static const QChar ZH_CN_UTFCODE[] = {0x7B80, 0x4F53, 0x4E2D, 0x6587};
static const QChar ZH_CHT_UTFCODE[] = {0x7E41, 0x9AD4, 0x4E2D, 0x6587};
static const QChar JA_JP_UTFCODE[] = {0x65E5, 0x672C, 0x8A9E};

#define Q_FROM_UTFCODE(Name)                                                                       \
    QString::fromRawData(Name##_UTFCODE, sizeof(Name##_UTFCODE) / sizeof(QChar))

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

    // Init Event Guard
    guard = new CommandPaletteEventGuard(q);

    // Init Actions
    QList<int> quantizes{1, 2, 4, 6, 8, 12, 16, 24, 32 /*, 0*/};
    for (auto quantize : qAsConst(quantizes)) {
        auto item = createItem(QIcon(), QSize(), ActionImpl::View_Quantization, QString(),
                               QString(), QString());
        item->setData(CommandItemTypes::Quantization, quantize);
        quantizationItems.append(item);
    }

    for (int i = 0; i <= QCommandPalette::SimplifiedChinese; ++i) {
        auto item = createItem(QIcon(), QSize(), ActionImpl::File_Languages, QString(), QString(),
                               QString());
        item->setData(CommandItemTypes::LanguageIndex, i);
        languageItems.append(item);
    }
}

void QCommandPalettePrivate::activateItem(QListWidgetItem *item) {
    Q_Q(QCommandPalette);

    if (item) {
        switch (curCmdType) {
        case QCommandPalette::Quantization: {
            int quantize = item->data(CommandItemTypes::Quantization).toInt();
            qRecord->commitEState(CRecordHolder::Quantization, quantize);
            break;
        }
        case QCommandPalette::Languages: {
            int index = item->data(CommandItemTypes::LanguageIndex).toInt();
            qRecordData.translationIndex = index;
            qData->localeLoad(index);
            break;
        }
        default:
            break;
        }
    }

    emit q->activated(item);
}

void QCommandPalettePrivate::previewItem(QListWidgetItem *item) {
    if (item) {
        switch (curCmdType) {
        case QCommandPalette::Languages: {
            int index = item->data(CommandItemTypes::LanguageIndex).toInt();
            qData->localeLoad(index);
            break;
        }
        default:
            break;
        }
    }
}

void QCommandPalettePrivate::abandon() {
    Q_Q(QCommandPalette);

    switch (curCmdType) {
    case QCommandPalette::Languages: {
        qData->localeLoad(qRecordData.translationIndex);
        break;
    }
    default:
        break;
    }

    emit q->abandoned();
}

void QCommandPalettePrivate::reset() {
    // Remove all items
    QSignalBlocker sb1(listWidget);
    QSignalBlocker sb2(lineEdit);

    while (listWidget->count() > 0) {
        listWidget->takeItem(0);
    }
    lineEdit->clear();
    lineEdit->setPlaceholderText(QString());
}

QListWidgetItem *QCommandPalettePrivate::createItem(const QIcon &icon, const QSize &size, int type,
                                                    const QString &filename,
                                                    const QString &location, const QString &date) {
    auto item = new QListWidgetItem();
    item->setText(filename);
    item->setData(QCommandPaletteItemDelegate::Filename, filename); // Deprecated
    item->setData(QCommandPaletteItemDelegate::Location, location);
    item->setData(QCommandPaletteItemDelegate::Date, date);
    item->setData(QCommandPaletteItemDelegate::Icon, icon);
    item->setData(QCommandPaletteItemDelegate::IconSize, size);
    item->setData(QCommandPaletteItemDelegate::Type, type);
    cachedItems.append(item);
    return item;
}

void QCommandPalettePrivate::reloadStrings_helper() {
    // Quantizations
    for (auto item : qAsConst(quantizationItems)) {
        QString desc;
        int quantize = item->data(CommandItemTypes::Quantization).toInt();
        switch (quantize) {
        case 0:
            desc = QCommandPalette::tr("None");
            break;
        case 1:
            desc = QCommandPalette::tr("Quarter");
            break;
        default:
            desc = QCommandPalette::tr("1/%1 Quarter").arg(QString::number(quantize));
            break;
        }
        item->setText(desc);
    }

    QStringList locales = {
        "English",              // English
        Q_FROM_UTFCODE(ZH_CN),  // 简体中文
        Q_FROM_UTFCODE(ZH_CHT), // 繁體中文
        Q_FROM_UTFCODE(JA_JP),  // 日本語
    };

    // Languages
    for (auto item : qAsConst(languageItems)) {
        auto index = item->data(CommandItemTypes::LanguageIndex).toInt();
        item->setText(locales.at(index));
    }
}
