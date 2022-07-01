#include "ExtensionManager_p.h"

#include "DataManager.h"

#include <QApplication>
#include <QMessageBox>
#include <QStyle>
#include <QWidget>

ExtensionManagerPrivate::ExtensionManagerPrivate() {
}

ExtensionManagerPrivate::~ExtensionManagerPrivate() {
    delete host;
}

void ExtensionManagerPrivate::init() {
    Q_Q(ExtensionManager);
    // Init Client
    host = new RH::RenderHost(14251, q);
    host->setTempDir(qData->getStandardPath(DataManager::AppTemp) + "/server");

    if (!host->launch(qApp->arguments())) {
        QMessageBox::critical(nullptr, "Fetal Error", "Server is not started yet!");
        ::exit(-1);
    }
}

void ExtensionManagerPrivate::reloadAppFont() {
    QWidget w;
    qApp->style()->polish(&w);
    qApp->setFont(w.font());
}
