#include "BaseDialog.h"
#include "BaseDialog_p.h"

BaseDialog::BaseDialog(QWidget *parent) : BaseDialog(*new BaseDialogPrivate(), parent) {
}

BaseDialog::~BaseDialog() {
}

BaseDialog::BaseDialog(BaseDialogPrivate &d, QWidget *parent)
    : QDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint), d_ptr(&d) {
    d.q_ptr = this;

    d.init();
}
