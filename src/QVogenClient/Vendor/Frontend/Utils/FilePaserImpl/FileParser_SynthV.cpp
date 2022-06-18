#include "../FileParser.h"
#include "CommonScore.h"

bool FileParser::parseSynthVFile(const QString &filename, CommonScore &notes) {
    QMessageBox::information(qobject_cast<QWidget *>(parent()), m_title,
                             tr("Feature is to be implemented."));
    return false;
}

bool FileParser::parseSynthVOldFile(const QString &filename, CommonScore &notes) {
    QMessageBox::information(qobject_cast<QWidget *>(parent()), m_title,
                             tr("Feature is to be implemented."));
    return false;
}
