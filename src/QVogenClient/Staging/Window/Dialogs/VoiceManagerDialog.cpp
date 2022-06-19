#include "VoiceManagerDialog.h"
#include "VoiceManagerDialog_p.h"

#include "DataManager.h"
#include "ExtensionManager.h"
#include "SystemHelper.h"

#include "QVogenVoiceFile.h"

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
    d->reloadStrings();
}

VoiceManagerDialog::VoiceManagerDialog(VoiceManagerDialogPrivate &d, QWidget *parent)
    : BaseDialog(d, parent) {
    d.init();

    Q_TR_NOTIFY(VoiceManagerDialog);

    setWindowTitle(tr("Voice Database Manager"));

    auto desktop = qApp->desktop();
    resize(desktop->width() / 2, desktop->height() / 2);
    d.updateDefaultColumnWidth();
}

void VoiceManagerDialog::_q_installVoice() {
    Q_D(VoiceManagerDialog);

    QStringList paths = qData->openFiles(
        tr("Open"), qData->getFileFilter(DataManager::VoicePackage), FLAG_VOICE, this);
    for (auto it = paths.begin(); it != paths.end(); ++it) {
        QVogenVoiceFile voice(*it);
        if (!voice.load()) {
            QMessageBox::critical(this, qData->errorTitle(), tr("Failed to load voice package."));
            continue;
        }
        if (!voice.install()) {
            QMessageBox::critical(this, qData->errorTitle(),
                                  tr("Failed to install voice package."));
            continue;
        }
    }

    qTheme->reloadVoiceList();
    d->reloadVoices();
}

void VoiceManagerDialog::_q_removeVoice() {
    Q_D(VoiceManagerDialog);

    auto selection = d->tree->selectedItems();
    for (auto it = selection.begin(); it != selection.end(); ++it) {
        auto item = *it;
        if (item->type() != VoiceManagerDialogPrivate::VoiceItem) {
            continue;
        }
        QString path = item->data(0, VoiceManagerDialogPrivate::VoicePath).toString();
        if (!path.isEmpty()) {
            Sys::rmDir(path);
        }
    }

    qTheme->reloadVoiceList();
    d->reloadVoices();
}
