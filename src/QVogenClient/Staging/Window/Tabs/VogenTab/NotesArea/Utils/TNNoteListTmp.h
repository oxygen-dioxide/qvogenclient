#ifndef TNNOTELIST_H
#define TNNOTELIST_H

#include <QList>
#include <QMap>
#include <QPair>
#include <QSet>

#include "../Elements/TNRectNote.h"

class TNNoteList : public QObject {
    Q_OBJECT
public:
    explicit TNNoteList(QObject *parent = nullptr);
    ~TNNoteList();

public:
    bool insert(TNRectNote *item);
    bool remove(TNRectNote *item);
    void clear();

    bool contains(TNRectNote *item) const;
    bool isEmpty() const;

    int findBegin(TNRectNote *item, int *pos = nullptr);
    int findEnd(TNRectNote *item, int *pos = nullptr);

    const QList<QPair<int, QSet<TNRectNote *>>> &begins() const;
    const QList<QPair<int, QSet<TNRectNote *>>> &ends() const;

    int firstBegin() const;
    int lastEnd() const;

signals:
    void beginChanged(int index, int val);
    void endChanged(int index, int val);

protected:
    QList<QPair<int, QSet<TNRectNote *>>> m_begins;
    QList<QPair<int, QSet<TNRectNote *>>> m_ends;
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
