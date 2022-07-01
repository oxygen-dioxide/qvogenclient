#ifndef TNNOTELIST_H
#define TNNOTELIST_H

#include <QList>
#include <QMap>
#include <QPair>
#include <QSet>

#include <list>

#include "../Elements/TNRectNote.h"
#include "QNavigatableList.h"

class TNNoteList : public QObject {
    Q_OBJECT
public:
    explicit TNNoteList(QObject *parent = nullptr);
    ~TNNoteList();

    struct TimeData {
        int first;
        QSet<TNRectNote *> second;

        TimeData();
        TimeData(int tick, const std::initializer_list<TNRectNote *> &list);

        void insert(TNRectNote *item);
        void remove(TNRectNote *item);
    };

public:
    bool insert(TNRectNote *item);
    bool remove(TNRectNote *item);
    void clear();
    void copy(TNNoteList *other);
    int count() const;

    bool contains(TNRectNote *item) const;
    bool isEmpty() const;

    int findBegin(TNRectNote *item, int *pos = nullptr);
    int findEnd(TNRectNote *item, int *pos = nullptr);

    const QList<TimeData> &begins() const;
    const QList<TimeData> &ends() const;

    int firstBegin() const;
    int lastEnd() const;

    bool isBeginSerialized() const;
    bool isEndSerialized() const;

signals:
    void inserted(int beginIndex, int endIndex, TNRectNote *p);
    void removed(int beginIndex, int endIndex, TNRectNote *p);

    void beginChanged(int index, int oldIndex, TNRectNote *p);
    void endChanged(int index, int oldIndex, TNRectNote *p);

protected:
    QList<TimeData> m_begins;
    QList<TimeData> m_ends;
    QSet<TNRectNote *> m_set;

    int lowerBound_begin(int val) const;
    int lowerBound_end(int val) const;

    int insert_begin(TNRectNote *item, int val);
    int insert_end(TNRectNote *item, int val);

    int remove_begin(TNRectNote *item, int val);
    int remove_end(TNRectNote *item, int val);

private:
    void _q_beginChanged(int val, int oldVal);
    void _q_endChanged(int val, int oldVal);
};

#endif // TNNOTELIST_H
