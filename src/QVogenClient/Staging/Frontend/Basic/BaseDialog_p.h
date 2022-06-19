#ifndef BASEDIALOGPRIVATE_H
#define BASEDIALOGPRIVATE_H

#include "BaseDialog.h"

class BaseDialogPrivate {
    Q_DECLARE_PUBLIC(BaseDialog)
public:
    BaseDialogPrivate();
    virtual ~BaseDialogPrivate();

    void init();

    BaseDialog *q_ptr;
};

#endif // BASEDIALOGPRIVATE_H