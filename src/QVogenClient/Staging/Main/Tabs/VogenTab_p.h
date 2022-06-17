#ifndef VOGENTABPRIVATE_H
#define VOGENTABPRIVATE_H

#include "DocumentTab_p.h"

#include "VogenTab.h"
#include "VogenTabImpl/TPianoPanel.h"

#include "QVogenFile.h"

class VogenTabPrivate : public DocumentTabPrivate {
    Q_DECLARE_PUBLIC(VogenTab)
public:
    VogenTabPrivate();
    ~VogenTabPrivate();

    void init();

    QString setTabNameProxy(const QString &tabName) override;

    // UI
    TPianoPanel *piano;
    QGridLayout *layout;

    // Tab
    int untitledIndex;

    static int s_untitledIndex;

    // Data
    QVogenFile vog;
};

#endif // VOGENTABPRIVATE_H
