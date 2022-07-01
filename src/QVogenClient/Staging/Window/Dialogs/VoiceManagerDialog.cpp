#include "VoiceManagerDialog.h"
#include "VoiceManagerDialog_p.h"

#include "DataManager.h"
#include "ExtensionManager.h"
#include "SystemHelper.h"

#include "RHProtocol.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>

static const char FLAG_VOICE[] = "%VOICE_PACKAGE%";

VoiceManagerDialog::VoiceManagerDialog(QWidget *parent)
    : VoiceManagerDialog(*new VoiceManagerDialogPrivate(), parent) {
}

VoiceManagerDialog::~VoiceManagerDialog() {
}

void VoiceManagerDialog::reloadStrings() {
    Q_D(VoiceManagerDialog);
    d->reloadStrings_helper();
}

VoiceManagerDialog::VoiceManagerDialog(VoiceManagerDialogPrivate &d, QWidget *parent)
    : BaseDialog(d, parent) {
    d.init();

    Q_TR_NOTIFY(VoiceManagerDialog);

    setWindowTitle(tr("Voice Database Manager"));

    auto desktop = qApp->desktop();
    resize(desktop->width() / 2, desktop->height() / 2);

    // Set average size for each column only at beginning
    d.updateDefaultColumnWidth();
}

void VoiceManagerDialog::_q_installVoice() {
    Q_D(VoiceManagerDialog);

    QStringList paths = qData->openFiles(
        tr("Open"), qData->getFileFilter(DataManager::VoicePackage), FLAG_VOICE, this);
    if (paths.isEmpty()) {
        return;
    }

    for (auto it = paths.begin(); it != paths.end(); ++it) {
        const auto &filename = *it;

        RH::VoiceLibMetadata meta;
        int code;
        bool res = qTheme->server()->install(filename, &meta, &code);

        if (!res || code == RH::INSTALL_FAILED) {
            QMessageBox::critical(
                this, qData->errorTitle(),
                tr("Failed to install voice package \"%1\".").arg(Sys::PathFindFileName(filename)));
            continue;
        }

        switch (code) {
        case RH::INSTALL_REPEATED: {
            QMessageBox::warning(
                this, qData->mainTitle(),
                tr("Voice library \"%1\" has already been installed, please remove it first.")
                    .arg(meta.id));
            break;
        }
        case RH::INSTALL_INVALID_PACKAGE: {
            QMessageBox::critical(
                this, qData->errorTitle(),
                tr("Voice package \"%1\" is invalid.").arg(Sys::PathFindFileName(filename)));
            break;
        }
        default:
            break;
        }
    }

    qTheme->reloadVoiceList();
    d->reloadVoices();
}

void VoiceManagerDialog::_q_removeVoice() {
    Q_D(VoiceManagerDialog);

    auto selection = d->tree->selectedItems();
    if (selection.isEmpty()) {
        return;
    }

    for (auto it = selection.begin(); it != selection.end(); ++it) {
        auto item = *it;
        if (item->type() != VoiceManagerDialogPrivate::VoiceItem) {
            continue;
        }
        QString id = item->data(0, VoiceManagerDialogPrivate::VoiceIdentity).toString();

        int code;
        bool res = qTheme->server()->uninstall(id, &code);

        if (!res || code == RH::UNINSTALL_FAILED) {
            QMessageBox::critical(this, qData->errorTitle(),
                                  tr("Failed to uninstall voice library \"%1\".").arg(id));
            continue;
        }

        switch (code) {
        case RH::UNINSTALL_NOT_FOUND: {
            QMessageBox::critical(this, qData->errorTitle(),
                                  tr("Voice library \"%1\" not found.").arg(id));
            break;
        }
        default:
            break;
        }
    }

    qTheme->reloadVoiceList();
    d->reloadVoices();
}
