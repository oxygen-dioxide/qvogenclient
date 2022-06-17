#include "../FileParser.h"
#include "DataManager.h"

#include "SequenceTextFile.h"

bool FileParser::parseUTAUFile(const QString &filename, CommonScore &notes) {
    SequenceTextFile file;

    if (!file.load(filename)) {
        QMessageBox::warning(Q_W(parent()), qData->mainTitle(), tr("Failed to read UTAU file!"));
        return false;
    }

    return true;
}
