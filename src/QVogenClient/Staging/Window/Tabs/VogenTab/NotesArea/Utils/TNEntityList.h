#ifndef TNENTITYLIST_H
#define TNENTITYLIST_H

#include <QList>
#include <QMap>
#include <QSet>

#include "../Elements/TNRectSelectable.h"

class TNEntityList : public QObject {
    Q_OBJECT
public:
    explicit TNEntityList(QObject *parent = nullptr);
    ~TNEntityList();

public:
    bool insert(TNRectSelectable *item);
    bool remove(TNRectSelectable *item);
    void clear();

    bool contains(TNRectSelectable *item) const;
    bool isEmpty() const;

    int findBegin(TNRectSelectable *item, int *pos = nullptr);
    int findEnd(TNRectSelectable *item, int *pos = nullptr);

    const QList<QPair<int, QSet<TNRectSelectable *>>> &begins() const;
    const QList<QPair<int, QSet<TNRectSelectable *>>> &ends() const;

    int firstBegin() const;
    int lastEnd() const;

signals:
    void beginChanged(int index, int val);
    void endChanged(int index, int val);

protected:
    QList<QPair<int, QSet<TNRectSelectable *>>> m_begins;
    QList<QPair<int, QSet<TNRectSelectable *>>> m_ends;
    QSet<TNRectSelectable *> m_set;

    int lowerBound_begin(int val) const;
    int lowerBound_end(int val) const;

    int insert_begin(TNRectSelectable *item, int val);
    int insert_end(TNRectSelectable *item, int val);

    int remove_begin(TNRectSelectable *item, int val);
    int remove_end(TNRectSelectable *item, int val);

private:
    void _q_beginChanged(int val, int oldVal);
    void _q_endChanged(int val, int oldVal);
};

#endif // TNENTITYLIST_H
