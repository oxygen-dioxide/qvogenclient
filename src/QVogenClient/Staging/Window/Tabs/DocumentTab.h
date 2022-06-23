#ifndef DOCUMENTTAB_H
#define DOCUMENTTAB_H

#include "CentralTab.h"

class DocumentTabPrivate;

class DocumentTab : public CentralTab {
    Q_OBJECT
    Q_DECLARE_PRIVATE(DocumentTab)
public:
    explicit DocumentTab(QWidget *parent = nullptr);
    ~DocumentTab();

    Type type() const override;

protected:
    DocumentTab(DocumentTabPrivate &d, QWidget *parent = nullptr);

public:
    bool isUntitled() const;
    bool isDeleted() const;

protected:
    virtual void setUntitled(bool untitled);
    virtual void setDeleted(bool deleted);

    void setEdited(bool edited) override;
    void filenameSet(const QString &filename) override;

signals:
};

#endif // DOCUMENTTAB_H
