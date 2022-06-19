#ifndef VOICEMANAGERDIALOG_H
#define VOICEMANAGERDIALOG_H

#include "BaseDialog.h"

class VoiceManagerDialogPrivate;

class VoiceManagerDialog : public BaseDialog {
    Q_OBJECT
    Q_DECLARE_PRIVATE(VoiceManagerDialog)
public:
    explicit VoiceManagerDialog(QWidget *parent = nullptr);
    ~VoiceManagerDialog();

    void reloadStrings();

protected:
    VoiceManagerDialog(VoiceManagerDialogPrivate &d, QWidget *parent = nullptr);

private:
    void _q_installVoice();
    void _q_removeVoice();
};

#endif // VOICEMANAGERDIALOG_H
