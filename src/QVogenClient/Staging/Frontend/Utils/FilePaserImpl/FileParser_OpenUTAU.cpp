#include "../FileParser.h"

#include "DataManager.h"

bool FileParser::parseOpenUTAUFile(const QString &filename, CommonScore &notes) {
    QMessageBox::information(qobject_cast<QWidget *>(parent()), qData->mainTitle(),
                             tr("Feature is to be implemented."));
    return false;
}
