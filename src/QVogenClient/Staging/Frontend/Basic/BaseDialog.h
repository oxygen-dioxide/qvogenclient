#ifndef BASEDIALOG_H
#define BASEDIALOG_H

#include <QDialog>

class BaseDialogPrivate;

class BaseDialog : public QDialog {
    Q_OBJECT
    Q_DECLARE_PRIVATE(BaseDialog)
public:
    BaseDialog(QWidget *parent = nullptr);
    ~BaseDialog();

protected:
    BaseDialog(BaseDialogPrivate &d, QWidget *parent = nullptr);

    QScopedPointer<BaseDialogPrivate> d_ptr;
};

#endif // BASEDIALOG_H
