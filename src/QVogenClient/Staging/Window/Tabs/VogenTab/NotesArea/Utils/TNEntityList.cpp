#include "TNEntityList.h"

TNEntityList::TNEntityList(QObject *parent) : QObject(parent) {
}

TNEntityList::~TNEntityList() {
}

bool TNEntityList::insert(TNRectSelectable *item) {
    // Insert to set
    if (m_set.contains(item)) {
        return false;
    }
    m_set.insert(item);

    // Insert to starts
    insert_begin(item, item->begin());

    // Insert to ends
    insert_end(item, item->end());

    // Connect
    connect(item, &TNRectSelectable::beginChanged, this, &TNEntityList::_q_beginChanged);
    connect(item, &TNRectSelectable::endChanged, this, &TNEntityList::_q_endChanged);

    return true;
}

bool TNEntityList::remove(TNRectSelectable *item) {
    auto it = m_set.find(item);
    if (it == m_set.end()) {
        return false;
    }

    // Disconnect
    disconnect(item, &TNRectSelectable::beginChanged, this, &TNEntityList::_q_beginChanged);
    disconnect(item, &TNRectSelectable::endChanged, this, &TNEntityList::_q_endChanged);

    // Remove from ends
    remove_end(item, item->end());

    // Remove from starts
    remove_begin(item, item->begin());

    // Remove from set
    m_set.erase(it);

    return true;
}

void TNEntityList::clear() {
    for (auto it = m_set.begin(); it != m_set.end(); ++it) {
        auto item = *it;
        disconnect(item, &TNRectSelectable::beginChanged, this, &TNEntityList::_q_beginChanged);
        disconnect(item, &TNRectSelectable::endChanged, this, &TNEntityList::_q_endChanged);
    }
    m_ends.clear();
    m_begins.clear();
    m_set.clear();
}

bool TNEntityList::contains(TNRectSelectable *item) const {
    return m_set.contains(item);
}

bool TNEntityList::isEmpty() const {
    return m_set.isEmpty();
}

int TNEntityList::findBegin(TNRectSelectable *item, int *pos) {
    int index = lowerBound_begin(item->begin());
    auto &set = m_begins[index].second;
    if (pos) {
        *pos = index;
    }
    if (set.contains(item)) {
        return index;
    }
    return -1;
}

int TNEntityList::findEnd(TNRectSelectable *item, int *pos) {
    int index = lowerBound_end(item->end());
    auto &set = m_ends[index].second;
    if (pos) {
        *pos = index;
    }
    if (set.contains(item)) {
        return index;
    }
    return -1;
}

const QList<QPair<int, QSet<TNRectSelectable *>>> &TNEntityList::begins() const {
    return m_begins;
}

const QList<QPair<int, QSet<TNRectSelectable *>>> &TNEntityList::ends() const {
    return m_ends;
}

int TNEntityList::firstBegin() const {
    if (m_begins.isEmpty()) {
        return 0;
    }
    return m_begins.front().first;
}

int TNEntityList::lastEnd() const {
    if (m_ends.isEmpty()) {
        return 0;
    }
    return m_ends.back().first;
}

int TNEntityList::lowerBound_begin(int val) const {
    int i, j, mid;
    i = 0;
    j = m_begins.size() - 1;
    while (i <= j) {
        mid = i + (j - i) / 2;
        if (m_begins.at(mid).first < val) {
            i = mid + 1;
        } else {
            j = mid - 1;
        }
    }
    return i;
}

int TNEntityList::lowerBound_end(int val) const {
    int i, j, mid;
    i = 0;
    j = m_ends.size() - 1;
    while (i <= j) {
        mid = i + (j - i) / 2;
        if (m_ends.at(mid).first < val) {
            i = mid + 1;
        } else {
            j = mid - 1;
        }
    }
    return i;
}

int TNEntityList::insert_begin(TNRectSelectable *item, int val) {
    int index = lowerBound_begin(val);
    if (index < m_begins.size()) {
        auto &pair = m_begins[index];
        auto &set = pair.second;
        if (pair.first == val) {
            set.insert(item);
            return index;
        }
    }
    m_begins.insert(index, qMakePair(val, QSet({item})));
    return index;
}

int TNEntityList::insert_end(TNRectSelectable *item, int val) {
    int index = lowerBound_end(val);
    if (index < m_ends.size()) {
        auto &pair = m_ends[index];
        auto &set = pair.second;
        if (pair.first == val) {
            set.insert(item);
            return index;
        }
    }
    m_ends.insert(index, qMakePair(val, QSet({item})));
    return index;
}

int TNEntityList::remove_begin(TNRectSelectable *item, int val) {
    int index = lowerBound_begin(val);
    auto &set = m_begins[index].second;
    set.remove(item);
    if (set.isEmpty()) {
        m_begins.removeAt(index);
    }
    return index;
}

int TNEntityList::remove_end(TNRectSelectable *item, int val) {
    int index = lowerBound_end(val);
    auto &set = m_ends[index].second;
    set.remove(item);
    if (set.isEmpty()) {
        m_ends.removeAt(index);
    }
    return index;
}

void TNEntityList::_q_beginChanged(int val, int oldVal) {
    auto item = qobject_cast<TNRectSelectable *>(sender());
    remove_begin(item, oldVal);

    int index = insert_begin(item, val);
    emit beginChanged(index, val);
}

void TNEntityList::_q_endChanged(int val, int oldVal) {
    auto item = qobject_cast<TNRectSelectable *>(sender());
    remove_end(item, oldVal);

    int index = insert_end(item, val);
    emit endChanged(index, val);
}
