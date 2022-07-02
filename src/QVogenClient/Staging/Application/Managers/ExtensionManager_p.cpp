#include "ExtensionManager_p.h"

#include "DataManager.h"

#include <QApplication>
#include <QDialog>
#include <QMessageBox>
#include <QStyle>
#include <QWidget>

#include <QHBoxLayout>
#include <QLabel>

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

    QDialog msgbox;
    {
        auto label = new QLabel(ExtensionManager::tr("Connecting to server..."));
        label->setAlignment(Qt::AlignCenter);

        auto layout = new QHBoxLayout();
        layout->setMargin(0);
        layout->setSpacing(0);
        layout->addWidget(label);
        msgbox.setLayout(layout);
        msgbox.resize(300, 100);

        msgbox.setWindowTitle(qData->mainTitle());
        msgbox.setWindowModality(Qt::ApplicationModal);
        msgbox.setWindowFlags((msgbox.windowFlags() | Qt::WindowMinimizeButtonHint) &
                              ~Qt::WindowCloseButtonHint);
        msgbox.show();
    }

    host->setTimeout(5000);
    bool res = host->launch(qApp->arguments());
    host->setTimeout(30000);

    msgbox.close();

    if (!res) {
        QMessageBox::critical(nullptr, "Fatal Error", "Server is not started yet!");
        ::exit(-1);
    }
}

void ExtensionManagerPrivate::reloadAppFont() {
    QWidget w;
    qApp->style()->polish(&w);
    qApp->setFont(w.font());
}
