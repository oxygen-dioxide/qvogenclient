#include "VoiceManagerDialog_p.h"

#include "ExtensionManager.h"

#include <QHeaderView>

VoiceManagerDialogPrivate::VoiceManagerDialogPrivate() {
}

VoiceManagerDialogPrivate::~VoiceManagerDialogPrivate() {
}

void VoiceManagerDialogPrivate::init() {
    Q_Q(VoiceManagerDialog);

    installButton = new CPushButton();
    removeButton = new CPushButton();

    topLayout = new QHBoxLayout();
    topLayout->setMargin(0);
    topLayout->setSpacing(0);

    topLayout->addWidget(installButton);
    topLayout->addWidget(removeButton);
    topLayout->addStretch();

    headerItem = new QTreeWidgetItem();

    tree = new CTreeWidget(q);
    tree->setColumnCount(4);
    tree->setHeaderItem(headerItem);

    auto header = tree->header();
    header->setHighlightSections(false);
    header->setSectionResizeMode(QHeaderView::Interactive);

    tree->setSelectionBehavior(QAbstractItemView::SelectRows);
    tree->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // Voice Item
    voiceItem = new QTreeWidgetItem();
    tree->addTopLevelItem(voiceItem);

    layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addLayout(topLayout);
    layout->addWidget(tree);
    q->setLayout(layout);

    q->connect(installButton, &QPushButton::clicked, q, &VoiceManagerDialog::_q_installVoice);
    q->connect(removeButton, &QPushButton::clicked, q, &VoiceManagerDialog::_q_removeVoice);

    // Load Voices
    reloadVoices();
}

void VoiceManagerDialogPrivate::reloadVoices() {
    // Remove All
    QList<QTreeWidgetItem *> items;
    for (int i = 0; i < voiceItem->childCount(); ++i) {
        items.append(voiceItem->child(i));
    }
    for (auto it = items.begin(); it != items.end(); ++it) {
        voiceItem->removeChild(*it);
    }

    // Add All
    const auto &voices = qTheme->voiceList();
    for (auto it = voices.begin(); it != voices.end(); ++it) {
        const auto &info = *it;

        auto item = new QTreeWidgetItem(ItemType::VoiceItem);

        item->setText(0, info.name);
        item->setText(1, info.version);
        item->setText(2, info.builtBy);
        item->setText(3, info.voiceBy);

        item->setData(0, DataRole::VoicePath, info.dirname());
        item->setData(0, DataRole::VoiceIdentity, info.id);

        voiceItem->addChild(item);
    }

    tree->expandItem(voiceItem);
}

void VoiceManagerDialogPrivate::reloadStrings() {
    installButton->setText(QObject::tr("Install"));
    removeButton->setText(QObject::tr("Remove"));

    headerItem->setText(0, QObject::tr("Voice Name"));
    headerItem->setText(1, QObject::tr("Version"));
    headerItem->setText(2, QObject::tr("Author"));
    headerItem->setText(3, QObject::tr("Origin"));

    voiceItem->setText(0, QObject::tr("Default"));
}

void VoiceManagerDialogPrivate::updateDefaultColumnWidth() {
    Q_Q(VoiceManagerDialog);

    int w = q->width();
    QList<double> ws{0.2, 0.2, 0.2};

    for (int i = 0; i < tree->columnCount(); ++i) {
        tree->setColumnWidth(i, w * ws[i]);
    }
}
