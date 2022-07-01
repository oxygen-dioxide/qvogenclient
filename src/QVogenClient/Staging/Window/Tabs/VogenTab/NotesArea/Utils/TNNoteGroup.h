#ifndef TNNOTEGROUP_H
#define TNNOTEGROUP_H

#include "../../Utils/TWrappedData.h"
#include "../Elements/TNGroupHint.h"

#include "TNNoteListTmp.h"

class TNNoteGroup : public TNNoteList {
    Q_OBJECT
public:
    explicit TNNoteGroup(TNotesArea *area, QObject *parent = nullptr);
    ~TNNoteGroup();

    friend class TNGroupHint;

public:
    void install();
    void uninstall();

public:
    quint64 id;

    QString name;
    QString singerId;
    QString rom;

    TNGroupHint *hintItem() const;

    void adjustHintGeometry();
    void adjustHintPos();

    TWAudio::Audio *cache() const;
    void setCache(const TWAudio::Audio &cache);
    void removeCache();

    QPixmap waveform() const;

protected:
    TNotesArea *m_area;

    TNGroupHint *m_hintItem;
    TNRectNote *m_firstNote;

    TWAudio::Audio *m_cache;

    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void _q_inserted(int beginIndex, int endIndex, TNRectNote *p);
    void _q_removed(int beginIndex, int endIndex, TNRectNote *p);
    void _q_beginChanged(int index, int oldIndex, TNRectNote *p);
};

#endif // TNNOTEGROUP_H
