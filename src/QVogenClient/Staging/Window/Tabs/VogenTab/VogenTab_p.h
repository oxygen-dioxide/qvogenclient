#ifndef VOGENTABPRIVATE_H
#define VOGENTABPRIVATE_H

#include "../DocumentTab_p.h"

#include "TPianoPanel.h"
#include "VogenTab.h"

#include "QVogenFile.h"

#include "Utils/Events/PianoRoll/TChangeVoiceEvent.h"
#include "Utils/Events/TPianoRollEvent.h"

#include "Utils/Operations/TBaseOperation.h"

#include <QEventLoop>

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

    QString tempDir;

    bool saveFile(const QString &filename);

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
    void dispatchEvent(TPianoRollEvent *event);

    void inputLyrics();
    void inputBeat();
    void inputTempo();

    void inputTranspose();
    void transpose(int val);

    void changeVoice(TChangeVoiceEvent *event);

    void violentRender();
    void violentRenderAll();
    void violentExportAudio();

    bool violentRender_helper(quint64 gid, const TWProject::Utterance &utter);

    QString setTabNameProxy(const QString &tabName) override;
};

#endif // VOGENTABPRIVATE_H
