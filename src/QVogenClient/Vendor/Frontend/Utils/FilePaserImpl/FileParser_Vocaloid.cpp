#include "../FileParser.h"
#include "CommonScore.h"

bool FileParser::parseVocaloidFile(const QString &filename, CommonScore &notes) {
    QMessageBox::information(qobject_cast<QWidget *>(parent()), m_title,
                             tr("Feature is to be implemented."));
    return false;
}

bool FileParser::parseVocaloidOldFile(const QString &filename, CommonScore &notes) {
    QMessageBox::information(qobject_cast<QWidget *>(parent()), m_title,
                             tr("Feature is to be implemented."));
    return false;
}
