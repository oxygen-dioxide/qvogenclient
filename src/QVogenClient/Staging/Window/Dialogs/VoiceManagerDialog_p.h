#ifndef VOICEMANAGERDIALOGPRIVATE_H
#define VOICEMANAGERDIALOGPRIVATE_H

#include "BaseDialog_p.h"

#include "VoiceManagerDialog.h"

#include "CPushButton.h"
#include "CTreeWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

class VoiceManagerDialogPrivate : public BaseDialogPrivate {
    Q_DECLARE_PUBLIC(VoiceManagerDialog)
public:
    VoiceManagerDialogPrivate();
    ~VoiceManagerDialogPrivate();

    enum ItemType {
        VoiceItem = QTreeWidgetItem::UserType + 1,
    };

    enum DataRole {
        VoicePath = Qt::UserRole + 1,
        VoiceIdentity,
    };

    void init();

    void reloadVoices();
    void reloadStrings();

    void updateDefaultColumnWidth();

    QVBoxLayout *layout;

    QHBoxLayout *topLayout;
    CPushButton *installButton;
    CPushButton *removeButton;

    CTreeWidget *tree;

    QTreeWidgetItem *headerItem;
    QTreeWidgetItem *voiceItem;
};

#endif // VOICEMANAGERDIALOGPRIVATE_H
