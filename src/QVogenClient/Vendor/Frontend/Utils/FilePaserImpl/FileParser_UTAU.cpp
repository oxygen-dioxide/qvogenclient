#include "../FileParser.h"

#include "SequenceTextFile.h"

bool FileParser::parseUTAUFile(const QString &filename, CommonScore &notes) {
    SequenceTextFile file;

    if (!file.load(filename)) {
        QMessageBox::warning(qobject_cast<QWidget *>(parent()), m_title,
                             tr("Failed to read UTAU file!"));
        return false;
    }

    return true;
}
