#ifndef VOGENTAB_H
#define VOGENTAB_H

#include "../DocumentTab.h"
#include "CommonScore.h"

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
    bool saveAs(const QString &filename) override;

    void undo() override;
    void redo() override;

    void initAfterLayout() override;
    void handleSpecificAction(ActionImpl::Action a) override;

    void makeUntitled(const QString &name = QString());

    void import(const CommonScore &proj);
    void append(const CommonScore &proj);

protected:
    void setUntitled(bool untitled) override;
    void setDeleted(bool deleted) override;

    void customEvent(QEvent *event) override;
};

#endif // VOGENTAB_H
