#include "QCommandPalette_p.h"

#include "Types/Actions.h"

#include "DataManager.h"
#include "ExtensionManager.h"
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

    hold = false;
    curCmdType = QCommandPalette::NoCommands;

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
    // Quantizations
    QList<int> quantizes{1, 2, 4, 6, 8, 12, 16, 24, 32 /*, 0*/};
    for (auto quantize : qAsConst(quantizes)) {
        auto item = createItem(QIcon(), QSize(), ActionImpl::View_Quantization, QString(),
                               QString(), QString());
        item->setData(CommandItemTypes::Quantization, quantize);
        quantizationItems.append(item);
    }

    // Languages
    for (int i = 0; i < qData->localeCount(); ++i) {
        auto item = createItem(QIcon(), QSize(), ActionImpl::File_Languages, QString(), QString(),
                               QString());
        item->setData(CommandItemTypes::LanguageIndex, i);
        languageItems.append(item);
    }

    // Themes
    for (int i = 0; i < qTheme->themeCount(); ++i) {
        auto item = createItem(QIcon(), QSize(), ActionImpl::File_ColorThemes, QString(), QString(),
                               QString());
        item->setData(CommandItemTypes::ThemeIndex, i);
        themeItems.append(item);
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
        case QCommandPalette::ColorThemes: {
            int index = item->data(CommandItemTypes::ThemeIndex).toInt();
            qRecordData.themeIndex = index;
            qTheme->themeLoad(index);
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
        case QCommandPalette::ColorThemes: {
            int index = item->data(CommandItemTypes::ThemeIndex).toInt();
            qTheme->themeLoad(index);
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
    case QCommandPalette::ColorThemes: {
        qTheme->themeLoad(qRecordData.themeIndex);
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

    hold = false;
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

    // Languages
    for (auto item : qAsConst(languageItems)) {
        auto index = item->data(CommandItemTypes::LanguageIndex).toInt();
        item->setText(qData->localeName(index));
    }

    // Themes
    for (auto item : qAsConst(themeItems)) {
        auto index = item->data(CommandItemTypes::ThemeIndex).toInt();
        item->setText(qTheme->themeName(index));
    }
}
