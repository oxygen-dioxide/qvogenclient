#include "TNotesArea.h"
#include "TNotesScroll.h"

#include "../Utils/Operations/TOGroupChange.h"
#include "../Utils/Operations/TOLyricsChange.h"
#include "../Utils/Operations/TONoteInsDel.h"
#include "../Utils/Operations/TONoteMove.h"
#include "../Utils/Operations/TONoteStretch.h"
#include "../Utils/Operations/TOTempoTimeSig.h"

#include "Types/Events.h"

#include <QApplication>

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
    m_notesCtl->addUtterances(data.utterances);

    setTimeSig(data.beat.x(), data.beat.y());
    setTempo(data.tempo);
}

TWProject TNotesArea::projectData() const {
    TWProject data;
    data.utterances = m_notesCtl->utterances();
    data.beat = QPoint(m_timeSig.first, m_timeSig.second);
    data.tempo = m_tempo;
    data.accomOffset = 0;
    return data;
}

bool TNotesArea::processOperation(TBaseOperation *op, bool undo) {
    switch (op->type()) {
    case TBaseOperation::NoteMove: {
        auto op1 = static_cast<TONoteMove *>(op);
        int f = undo ? -1 : 1;

        QList<TWNote::Movement> moves;
        foreach (const auto &move, op1->data) {
            moves.append(TWNote::Movement{move.id, f * move.hMove, f * move.vMove});
        }

        m_notesCtl->moveNotes(moves);
        break;
    }
    case TBaseOperation::NoteStretch: {
        auto op1 = static_cast<TONoteStretch *>(op);
        int f = undo ? -1 : 1;

        QList<TWNote::Stretch> stretches;
        foreach (const auto &stretch, op1->data) {
            stretches.append(TWNote::Stretch{stretch.id, f * stretch.hStretch});
        }

        m_notesCtl->stretchNotes(stretches);
        break;
    }
    case TBaseOperation::LyricsChange: {
        auto op1 = static_cast<TOLyricsChange *>(op);

        QList<TWNote::Lyric> lyrics;
        foreach (const auto &change, op1->data) {
            lyrics.append(TWNote::Lyric{change.id, undo ? change.oldLrc : change.lrc});
        }

        m_notesCtl->changeLyrics(lyrics);
        break;
    }
    case TBaseOperation::NoteAddDelete: {
        auto op1 = static_cast<TONoteInsDel *>(op);

        int f1 = (op1->cType() == TONoteInsDel::Remove) ? -1 : 1;
        int f2 = undo ? -1 : 1;

        if (f1 * f2 > 0) {
            for (const auto &utt : qAsConst(op1->data)) {
                QList<TWNote::NoteAll> notes;
                foreach (const auto &note, utt.notes) {
                    notes.append(TWNote::NoteAll{
                        note.id,      //
                        note.start,   //
                        note.length,  //
                        note.noteNum, //
                        note.lyric,   //
                        note.gid,     //
                    });
                }
                TWNote::Group group{
                    utt.group.id,
                    utt.group.name,
                    utt.group.singer,
                    utt.group.rom,
                };
                m_notesCtl->addNotes(notes, group);
            }
        } else {
            for (const auto &utt : qAsConst(op1->data)) {
                QList<quint64> ids;
                foreach (const auto &note, utt.notes) {
                    ids.append(note.id);
                    ;
                }
                m_notesCtl->removeNotes(ids);
            }
        }
        break;
    }
    case TBaseOperation::GroupChange: {
        auto op1 = static_cast<TOGroupChange *>(op);
        auto g = undo ? op1->oldGroup : op1->group;
        m_notesCtl->changeGroup(op1->ids, TWNote::Group{g.id, g.name, g.singer, g.rom});
        break;
    }
    case TBaseOperation::TempoTimeSig: {
        auto op1 = static_cast<TOTempoTimeSig *>(op);
        const auto &val = undo ? op1->oldVal : op1->val;
        setTimeSig(val.timeSig.first, val.timeSig.second);
        setTempo(val.tempo);
        break;
    }
    default:
        break;
    }
    return true;
}
