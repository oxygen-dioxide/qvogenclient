#ifndef VOGENTAB_H
#define VOGENTAB_H

#include "DocumentTab.h"

class VogenTabPrivate;

class VogenTab : public DocumentTab {
    Q_OBJECT
    Q_DECLARE_PRIVATE(VogenTab)
public:
    explicit VogenTab(QWidget *parent = nullptr);
    ~VogenTab();

    void reloadStrings();

    Type type() const override;

protected:
    VogenTab(VogenTabPrivate &d, QWidget *parent = nullptr);

public:
    bool load() override;
    bool save() override;

    void makeUntitled(const QString &name = QString());

protected:
    void setUntitled(bool untitled) override;
    void setDeleted(bool deleted) override;
};

#endif // VOGENTAB_H
