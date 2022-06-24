#include "TNotesArea.h"
#include "TNotesScroll.h"

#include "../Utils/Operations/TOLyricsChange.h"
#include "../Utils/Operations/TONoteMove.h"
#include "../Utils/Operations/TONoteStretch.h"

void TNotesArea::loadSprite(const QString &path) {
    m_spriteCtl->loadSprite(path);
}

bool TNotesArea::spriteVisible() const {
    return m_spriteCtl->spriteVisible();
}

void TNotesArea::setSpriteVisible(bool spriteVisible) {
    m_spriteCtl->setSpriteVisible(spriteVisible);
}

Qt::Corner TNotesArea::spritePosition() const {
    return m_spriteCtl->spritePosition();
}

void TNotesArea::setSpritePosition(const Qt::Corner &spritePosition) {
    m_spriteCtl->setSpritePosition(spritePosition);
}

double TNotesArea::spriteAlpha() const {
    return m_spriteCtl->spriteAlpha();
}

void TNotesArea::setSpriteAlpha(double spriteAlpha) {
    m_spriteCtl->setSpriteAlpha(spriteAlpha);
}

void TNotesArea::setProjectData(const TWProject &data) {
    m_notesCtl->setUtterances(data.utterances);
}

TWProject TNotesArea::projectData() const {
    TWProject data;
    data.utterances = m_notesCtl->utterances();
    return data;
}

bool TNotesArea::processOperation(TBaseOperation *op, bool undo) {
    switch (op->type()) {
    case TBaseOperation::NoteMove: {
        auto m = static_cast<TONoteMove *>(op);
        int f = undo ? -1 : 1;

        QList<TWNote::Movement> moves;
        foreach (const auto &move, m->data) {
            moves.append(TWNote::Movement{move.id, f * move.hMove, f * move.vMove});
        }

        m_notesCtl->moveNotes(moves);
        break;
    }
    case TBaseOperation::NoteStretch: {
        auto s = static_cast<TONoteStretch *>(op);
        int f = undo ? -1 : 1;

        QList<TWNote::Stretch> stretches;
        foreach (const auto &stretch, s->data) {
            stretches.append(TWNote::Stretch{stretch.id, f * stretch.hStretch});
        }

        m_notesCtl->stretchNotes(stretches);
        break;
    }
    case TBaseOperation::LyricsChange: {
        auto l = static_cast<TOLyricsChange *>(op);

        QList<TWNote::Lyric> lyrics;
        foreach (const auto &change, l->data) {
            lyrics.append(TWNote::Lyric{change.id, undo ? change.oldLrc : change.lrc});
        }

        m_notesCtl->changeLyrics(lyrics);
        break;
    }
    default:
        break;
    }
    return true;
}
