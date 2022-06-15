#include "CommonNote.h"

CommonNote::CommonNote(int start, int length, int noteNum, const QString lyric, double tempo)
    : start(start), length(length), noteNum(noteNum), lyric(lyric), tempo(tempo) {
}

CommonNote::~CommonNote() {
}
