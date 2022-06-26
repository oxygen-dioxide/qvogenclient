#include "../FileParser.h"
#include "CommonScore.h"
#include "SequenceTextFile.h"

#include "DataManager.h"

bool FileParser::parseUTAUFile(const QString &filename, CommonScore &proj) {
    SequenceTextFile file;

    if (!file.load(filename)) {
        QMessageBox::warning(qobject_cast<QWidget *>(parent()), qData->errorTitle(),
                             tr("Failed to read UTAU file!"));
        return false;
    }

    proj.name = file.sectionSettings.projectName;
    proj.tempo = file.sectionSettings.globalTempo;

    QList<CommonNote> notes;
    int currentLength = 0;
    for (const auto &note : qAsConst(file.sectionNotes)) {
        if (!Utau::isRestLyric(note.lyric)) {
            CommonNote n;
            n.start = currentLength;
            n.length = note.length;
            n.noteNum = note.noteNum;
            n.lyric = note.lyric;
            n.tempo = (note.tempo != NODEF_DOUBLE) ? note.tempo : Utau::DEFAULT_VALUE_TEMPO;
            notes.append(n);
        }
        currentLength += note.length;
    }

    proj.tracks.append(CommonScore::Track{proj.name, notes});
    return true;
}
