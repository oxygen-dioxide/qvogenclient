#include "SelectsDialogV2.h"
#include "Managers/DataManager.h"

SelectsDialogV2::SelectsDialogV2(const QString &title, const QString &caption,
                                 const QStringList &list, QList<bool> &result, bool single,
                                 QWidget *parent)
    : BaseDialog(parent), m_result(result), m_single(single), m_options(list),
      m_title(title), m_caption(caption) {
    init();
}

SelectsDialogV2::~SelectsDialogV2() {
}

void SelectsDialogV2::init() {
    setWindowTitle(m_title);

    // Add label and buttons
    lbCaption = new QLabel(m_caption);
    lbCaption->setObjectName("dialog-caption-label");
    lbCaption->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    btnCancel = new CPushButton(tr("Cancel"));
    btnCancel->setObjectName("dialog-button");

    btnOK = new CPushButton(tr("OK"));
    btnOK->setObjectName("dialog-button");

    boxGroup = new QButtonGroup(this);
    for (auto it = m_options.begin(); it != m_options.end(); ++it) {
        CCheckBox *box = new CCheckBox(*it, this);
        box->setObjectName("dialog-check-box");
        boxGroup->addButton(box);
    }
    boxGroup->setExclusive(m_single);

    btnOK->setFocus();
    btnOK->setDefault(true);

    btnOK->setShortcut(QKeySequence(Qt::Key_Return));

    connect(btnOK, &QPushButton::clicked, this, &SelectsDialogV2::onOKClicked);
    connect(btnCancel, &QPushButton::clicked, this, &SelectsDialogV2::onCancelClicked);

    m_mainLayout = new QVBoxLayout();
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->setSpacing(0);
    m_buttonLayout->setMargin(0);

    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(btnOK);
    m_buttonLayout->addWidget(btnCancel);

    m_boxLayout = new QVBoxLayout();
    m_boxLayout->setSpacing(0);
    m_boxLayout->setMargin(0);

    const auto &boxes = boxGroup->buttons();
    for (int i = 0; i < boxes.size(); ++i) {
        m_boxLayout->addWidget(boxes.at(i));
    }

    m_mainLayout->addWidget(lbCaption);
    m_mainLayout->addLayout(m_boxLayout);
    m_mainLayout->addLayout(m_buttonLayout);

    setLayout(m_mainLayout);

    adjustSize();

    QSize size = this->size();
    resize(size.width() < 500 ? 500 : size.width(), size.height());

    btnOK->setFocus();
}

void SelectsDialogV2::onOKClicked() {
    bool isChecked = false;
    m_result.clear();
    const auto &boxes = boxGroup->buttons();
    for (int i = 0; i < boxes.size(); ++i) {
        if (boxes.at(i)->isChecked()) {
            isChecked = true;
        }
        m_result.append(boxes.at(i)->isChecked());
    }

    if (!isChecked) {
        Q_WARNING(this, qData->mainTitle(), tr("Please select at least one!"));
        return;
    }

    done(1);
}

void SelectsDialogV2::onCancelClicked() {
    done(0);
}
