#ifndef VOGENTABPRIVATE_H
#define VOGENTABPRIVATE_H

#include "../DocumentTab_p.h"

#include "TPianoPanel.h"
#include "VogenTab.h"

#include "QVogenFile.h"

#include "Types/Events.h"

#include "Utils/Operations/TBaseOperation.h"

class VogenTabPrivate : public DocumentTabPrivate {
    Q_DECLARE_PUBLIC(VogenTab)
public:
    VogenTabPrivate();
    ~VogenTabPrivate();

    void init();

    // UI
    TPianoPanel *piano;
    QGridLayout *layout;

    // Tab
    int untitledIndex;

    static int s_untitledIndex;

    // Data
    QVogenFile vog;

    // Operations
    QList<TBaseOperation *> historyList;
    int historyIndex;
    int savedHistoryIndex;

    bool earliest() const override;
    bool latest() const override;

    void addHistory(TBaseOperation *op);
    void clearHistory();

    void updateSaveStatus();

    // Events
    void dispatchEvent(QEventImpl::PianoRollChangeEvent *event);

    void inputLyrics();

    void inputTranspose();
    void transpose(int val);

    QString setTabNameProxy(const QString &tabName) override;
};

#endif // VOGENTABPRIVATE_H
