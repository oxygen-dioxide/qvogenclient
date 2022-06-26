#ifndef SELECTSDIALOGV2_H
#define SELECTSDIALOGV2_H

#include <QButtonGroup>
#include <QCheckBox>
#include <QDebug>
#include <QDialog>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QValidator>

#include "BaseDialog.h"
#include "Macros.h"
#include "QPixelSize.h"

#include "CCheckBox.h"
#include "CPushButton.h"

class SelectsDialogV2 : public BaseDialog {
    Q_OBJECT
    Q_INNER_LAYOUT_PROPERTY_DELCARE(Box, box, m_boxLayout)
    Q_INNER_LAYOUT_PROPERTY_DELCARE(Main, main, m_mainLayout)
    Q_INNER_LAYOUT_PROPERTY_DELCARE(Button, button, m_buttonLayout)
public:
    SelectsDialogV2(const QString &title, const QString &caption, const QStringList &list,
                    QList<bool> &result, bool single, QWidget *parent = nullptr);
    ~SelectsDialogV2();

private:
    void init();

protected:
    QList<bool> &m_result;

    bool m_single;
    QStringList m_options;

    QString m_title;
    QString m_caption;

    QVBoxLayout *m_mainLayout;
    QVBoxLayout *m_boxLayout;
    QHBoxLayout *m_buttonLayout;

    QLabel *lbCaption;
    QButtonGroup *boxGroup;

    CPushButton *btnCancel, *btnOK;

private:
    void onOKClicked();
    void onCancelClicked();
};

#endif // SELECTSDIALOGV2_H
