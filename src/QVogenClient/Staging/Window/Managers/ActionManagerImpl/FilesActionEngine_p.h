#ifndef FILESACTIONENGINEPRIVATE_H
#define FILESACTIONENGINEPRIVATE_H

#include "ActionManagerEngine_p.h"

#include "FilesActionEngine.h"

#include "Types/Actions.h"

class FilesActionEnginePrivate : public ActionManagerEnginePrivate {
    Q_DECLARE_PUBLIC(FilesActionEngine)
public:
    FilesActionEnginePrivate();
    ~FilesActionEnginePrivate();

    void init();

    void setup() override;
    void reloadStrings() override;
    void reloadShortcuts() override;

    QAction *NewAction(ActionImpl::Action a, QObject *parent = nullptr);
    void registerHandler(QMenu *menu);

    void reloadRecentActions();
    void reloadRecentActionStrings();
    void handleRecentAction(QAction *action);

    QMap<QAction *, ActionImpl::Action> map;

    QMap<QAction::MenuRole, QAction *> roleMap;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *modifyMenu;
    QMenu *playMenu;
    QMenu *helpMenu;

    QActionGroup *cursorGroup;

    // File
    QMenu *recentMenu; // *
    QMenu *exportMenu;
    QMenu *preferencesMenu;

    QAction *file_newFile;
    QAction *file_newWindow;
    QAction *file_openFile;
    QAction *file_saveFile;
    QAction *file_saveAs;
    QAction *file_saveAll;
    QAction *file_importFile;
    QAction *file_appendFile;
    QAction *file_fileSettings;
    QAction *file_closeFile;
    QAction *file_closeWindow;

    QAction *file_exportSelection;
    QAction *file_exportTrack;

    QAction *preference_settings;
    QAction *preference_keyShortcuts;
    QAction *preference_themes;
    QAction *preference_languages;

    // Edit
    QMenu *pasteMenu;
    QMenu *removeMenu;

    QAction *edit_undo;
    QAction *edit_redo;

    QAction *edit_cut;
    QAction *edit_copy;
    QAction *edit_paste;
    QAction *edit_pastePitch;

    QAction *edit_remove;
    QAction *edit_removePitch;

    QAction *edit_selectAll;
    QAction *edit_deselect;

    // View
    QMenu *appearanceMenu;
    QMenu *panelsMenu;
    QMenu *displayMenu;
    QMenu *cursorMenu;

    QAction *view_quantization;
    QAction *view_playhead;

    QAction *appearance_toolBar;
    QAction *appearance_navBar;
    QAction *appearance_statusBar;
    QAction *appearance_panelBars;
    QAction *appearance_menuBar;

    QAction *display_note;
    QAction *display_pitch;

    QAction *cursor_select;
    QAction *cursor_sketch;
    QAction *cursor_free;

    // Modify
    QAction *modify_insertLyrics;
    QAction *modify_findReplace;
    QAction *modify_transpose;
    QAction *modify_octaveUp;
    QAction *modify_octaveDown;

    QAction *modify_buildIn;
    QAction *modify_plugins;

    // Play
    QAction *play_play;
    QAction *play_stop;
    QAction *play_render;
    QAction *play_renderAll;
    QAction *play_moveStart;
    QAction *play_moveEnd;
    QAction *play_removeCache;
    QAction *play_removeAllCache;
    QAction *play_exportAudio;

    // Help
    QAction *help_welcome;
    QAction *help_showActions;
    QAction *help_voiceManager;
    QAction *help_instructions;
    QAction *help_checkUpdate;
    QAction *help_aboutApp;
    QAction *help_aboutQt;
};

#endif // FILESACTIONENGINEPRIVATE_H
