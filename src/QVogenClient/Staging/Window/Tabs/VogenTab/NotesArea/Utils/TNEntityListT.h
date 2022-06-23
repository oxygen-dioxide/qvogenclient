#ifndef TNENTITYLISTT_H
#define TNENTITYLISTT_H

#include <QList>
#include <QMap>
#include <QSet>

#define Q_DECLARE_ENTITY_LIST(TNEntityListT, TNRectSelectableT)                                    \
    class TNEntityListT : public QObject {                                                         \
        Q_OBJECT                                                                                   \
    public:                                                                                        \
        explicit TNEntityListT(QObject *parent = nullptr);                                         \
        ~TNEntityListT();                                                                          \
                                                                                                   \
    public:                                                                                        \
        bool insert(TNRectSelectableT *item);                                                      \
        bool remove(TNRectSelectableT *item);                                                      \
        void clear();                                                                              \
                                                                                                   \
        bool contains(TNRectSelectableT *item) const;                                              \
        bool isEmpty() const;                                                                      \
                                                                                                   \
        int findBegin(TNRectSelectableT *item, int *pos = nullptr);                                \
        int findEnd(TNRectSelectableT *item, int *pos = nullptr);                                  \
                                                                                                   \
        const QList<QSet<TNRectSelectableT *>> &begins() const;                                    \
        const QList<QSet<TNRectSelectableT *>> &ends() const;                                      \
                                                                                                   \
        int firstBegin() const;                                                                    \
        int lastEnd() const;                                                                       \
                                                                                                   \
    Q_SIGNALS:                                                                                     \
        void beginChanged(int index, int val);                                                     \
        void endChanged(int index, int val);                                                       \
                                                                                                   \
    protected:                                                                                     \
        QList<QSet<TNRectSelectableT *>> m_begins;                                                 \
        QList<QSet<TNRectSelectableT *>> m_ends;                                                   \
        QSet<TNRectSelectableT *> m_set;                                                           \
                                                                                                   \
        int lowerBound_begin(int val) const;                                                       \
        int lowerBound_end(int val) const;                                                         \
                                                                                                   \
        int insert_begin(TNRectSelectableT *item, int val);                                        \
        int insert_end(TNRectSelectableT *item, int val);                                          \
                                                                                                   \
        int remove_begin(TNRectSelectableT *item, int val);                                        \
        int remove_end(TNRectSelectableT *item, int val);                                          \
                                                                                                   \
    private:                                                                                       \
        void _q_beginChanged(int val, int oldVal);                                                 \
        void _q_endChanged(int val, int oldVal);                                                   \
    };




#define Q_DECLARE_ENTITY_LIST_IMPLEMENT(TNEntityListT, TNRectSelectableT)                          \
    TNEntityListT::TNEntityListT(QObject *parent) : QObject(parent) {                              \
    }                                                                                              \
                                                                                                   \
    TNEntityListT::~TNEntityListT() {                                                              \
    }                                                                                              \
                                                                                                   \
    bool TNEntityListT::insert(TNRectSelectableT *item) {                                          \
        if (m_set.contains(item)) {                                                                \
            return false;                                                                          \
        }                                                                                          \
        m_set.insert(item);                                                                        \
        insert_begin(item, item->begin());                                                         \
        insert_end(item, item->end());                                                             \
        connect(item, &TNRectSelectableT::beginChanged, this, &TNEntityListT::_q_beginChanged);    \
        connect(item, &TNRectSelectableT::endChanged, this, &TNEntityListT::_q_endChanged);        \
        return true;                                                                               \
    }                                                                                              \
                                                                                                   \
    bool TNEntityListT::remove(TNRectSelectableT *item) {                                          \
        auto it = m_set.find(item);                                                                \
        if (it == m_set.end()) {                                                                   \
            return false;                                                                          \
        }                                                                                          \
        disconnect(item, &TNRectSelectableT::beginChanged, this, &TNEntityListT::_q_beginChanged); \
        disconnect(item, &TNRectSelectableT::endChanged, this, &TNEntityListT::_q_endChanged);     \
        m_set.erase(it);                                                                           \
        remove_end(item, item->end());                                                             \
        remove_begin(item, item->begin());                                                         \
        return true;                                                                               \
    }                                                                                              \
                                                                                                   \
    void TNEntityListT::clear() {                                                                  \
        for (auto it = m_set.begin(); it != m_set.end(); ++it) {                                   \
            auto item = *it;                                                                       \
            disconnect(item, &TNRectSelectableT::beginChanged, this,                               \
                       &TNEntityListT::_q_beginChanged);                                           \
            disconnect(item, &TNRectSelectableT::endChanged, this, &TNEntityListT::_q_endChanged); \
        }                                                                                          \
        m_ends.clear();                                                                            \
        m_begins.clear();                                                                          \
        m_set.clear();                                                                             \
    }                                                                                              \
                                                                                                   \
    bool TNEntityListT::contains(TNRectSelectableT *item) const {                                  \
        return m_set.contains(item);                                                               \
    }                                                                                              \
                                                                                                   \
    bool TNEntityListT::isEmpty() const {                                                          \
        return m_set.isEmpty();                                                                    \
    }                                                                                              \
                                                                                                   \
    int TNEntityListT::findBegin(TNRectSelectableT *item, int *pos) {                              \
        int index = lowerBound_begin(item->begin());                                               \
        auto &set = m_begins[index];                                                               \
        if (pos) {                                                                                 \
            *pos = index;                                                                          \
        }                                                                                          \
        if (set.contains(item)) {                                                                  \
            return index;                                                                          \
        }                                                                                          \
        return -1;                                                                                 \
    }                                                                                              \
                                                                                                   \
    int TNEntityListT::findEnd(TNRectSelectableT *item, int *pos) {                                \
        int index = lowerBound_end(item->end());                                                   \
        auto &set = m_ends[index];                                                                 \
        if (pos) {                                                                                 \
            *pos = index;                                                                          \
        }                                                                                          \
        if (set.contains(item)) {                                                                  \
            return index;                                                                          \
        }                                                                                          \
        return -1;                                                                                 \
    }                                                                                              \
                                                                                                   \
    const QList<QSet<TNRectSelectableT *>> &TNEntityListT::ends() const {                          \
        return m_ends;                                                                             \
    }                                                                                              \
                                                                                                   \
    int TNEntityListT::firstBegin() const {                                                        \
        if (m_begins.isEmpty()) {                                                                  \
            return 0;                                                                              \
        }                                                                                          \
        return (*(m_begins.front()).begin())->begin();                                             \
    }                                                                                              \
                                                                                                   \
    int TNEntityListT::lastEnd() const {                                                           \
        if (m_ends.isEmpty()) {                                                                    \
            return 0;                                                                              \
        }                                                                                          \
        return (*(m_ends.back()).begin())->end();                                                  \
    }                                                                                              \
                                                                                                   \
    int TNEntityListT::lowerBound_begin(int val) const {                                           \
        int i, j, mid;                                                                             \
        i = 0;                                                                                     \
        j = m_begins.size() - 1;                                                                   \
        while (i <= j) {                                                                           \
            mid = i + (j - i) / 2;                                                                 \
            if ((*m_begins.at(mid).begin())->begin() < val) {                                      \
                i = mid + 1;                                                                       \
            } else {                                                                               \
                j = mid - 1;                                                                       \
            }                                                                                      \
        }                                                                                          \
        return i;                                                                                  \
    }                                                                                              \
                                                                                                   \
    int TNEntityListT::lowerBound_end(int val) const {                                             \
        int i, j, mid;                                                                             \
        i = 0;                                                                                     \
        j = m_ends.size() - 1;                                                                     \
        while (i <= j) {                                                                           \
            mid = i + (j - i) / 2;                                                                 \
            if ((*m_ends.at(mid).begin())->end() < val) {                                          \
                i = mid + 1;                                                                       \
            } else {                                                                               \
                j = mid - 1;                                                                       \
            }                                                                                      \
        }                                                                                          \
        return i;                                                                                  \
    }                                                                                              \
                                                                                                   \
    int TNEntityListT::insert_begin(TNRectSelectableT *item, int val) {                            \
        int index = lowerBound_begin(val);                                                         \
        if (index < m_begins.size()) {                                                             \
            auto &set = m_begins[index];                                                           \
            if ((*set.begin())->begin() == val) {                                                  \
                set.insert(item);                                                                  \
                return index;                                                                      \
            }                                                                                      \
        }                                                                                          \
        m_begins.insert(index, {item});                                                            \
        return index;                                                                              \
    }                                                                                              \
                                                                                                   \
    int TNEntityListT::insert_end(TNRectSelectableT *item, int val) {                              \
        int index = lowerBound_end(val);                                                           \
        if (index < m_ends.size()) {                                                               \
            auto &set = m_ends[index];                                                             \
            if ((*set.begin())->end() == val) {                                                    \
                set.insert(item);                                                                  \
                return index;                                                                      \
            }                                                                                      \
        }                                                                                          \
        m_ends.insert(index, {item});                                                              \
        return index;                                                                              \
    }                                                                                              \
                                                                                                   \
    int TNEntityListT::remove_begin(TNRectSelectableT *item, int val) {                            \
        int index = lowerBound_begin(val);                                                         \
        auto &set = m_begins[index];                                                               \
        set.remove(item);                                                                          \
        if (set.isEmpty()) {                                                                       \
            m_begins.removeAt(index);                                                              \
        }                                                                                          \
        return index;                                                                              \
    }                                                                                              \
                                                                                                   \
    int TNEntityListT::remove_end(TNRectSelectableT *item, int val) {                              \
        int index = lowerBound_end(val);                                                           \
        auto &set = m_ends[index];                                                                 \
        set.remove(item);                                                                          \
        if (set.isEmpty()) {                                                                       \
            m_ends.removeAt(index);                                                                \
        }                                                                                          \
        return index;                                                                              \
    }                                                                                              \
                                                                                                   \
    const QList<QSet<TNRectSelectableT *>> &TNEntityListT::begins() const {                        \
        return m_begins;                                                                           \
    }                                                                                              \
                                                                                                   \
    void TNEntityListT::_q_beginChanged(int val, int oldVal) {                                     \
        auto item = qobject_cast<TNRectSelectableT *>(sender());                                   \
        remove_begin(item, oldVal);                                                                \
                                                                                                   \
        int index = insert_begin(item, val);                                                       \
        emit beginChanged(index, val);                                                             \
    }                                                                                              \
                                                                                                   \
    void TNEntityListT::_q_endChanged(int val, int oldVal) {                                       \
        auto item = qobject_cast<TNRectSelectableT *>(sender());                                   \
        remove_end(item, oldVal);                                                                  \
                                                                                                   \
        int index = insert_end(item, val);                                                         \
        emit endChanged(index, val);                                                               \
    }

#endif // TNENTITYLISTT_H
