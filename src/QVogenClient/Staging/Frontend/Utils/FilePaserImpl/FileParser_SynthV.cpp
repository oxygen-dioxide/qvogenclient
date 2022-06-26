#include "../FileParser.h"
#include "CommonScore.h"

#include "DataManager.h"

bool FileParser::parseSynthVFile(const QString &filename, CommonScore &proj) {
    QMessageBox::information(qobject_cast<QWidget *>(parent()), qData->mainTitle(),
                             tr("Feature is to be implemented."));
    return false;
}

bool FileParser::parseSynthVOldFile(const QString &filename, CommonScore &proj) {
    QMessageBox::information(qobject_cast<QWidget *>(parent()), qData->mainTitle(),
                             tr("Feature is to be implemented."));
    return false;
}
