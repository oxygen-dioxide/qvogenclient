#include "FilesActionEngine_p.h"

#include "CMenu.h"
#include "DataManager.h"

#include "Logs/CRecordHolder.h"

struct ActionData {
    enum ActionRole {
        Null,
        OpenFile,
        OpenDir,
        MoreFiles,
        MoreDirs,
        Clear,
    };
    ActionRole type;
    QString data;
    ActionData() = default;
    ActionData(ActionRole type) : type(type) {
    }
    ActionData(ActionRole type, const QString &data) : type(type), data(data) {
    }
    QVariant toVariant() const {
        QVariant var;
        var.setValue(*this);
        return var;
    }
    static ActionData fromVariant(const QVariant &var) {
        return var.value<ActionData>();
    }
};
Q_DECLARE_METATYPE(ActionData)

FilesActionEnginePrivate::FilesActionEnginePrivate() {
}

FilesActionEnginePrivate::~FilesActionEnginePrivate() {
}

void FilesActionEnginePrivate::init() {
}

void FilesActionEnginePrivate::setup() {
    // ----------------- Allocate -----------------
    fileMenu = new CMenu(menuBar);
    editMenu = new CMenu(menuBar);
    viewMenu = new CMenu(menuBar);
    modifyMenu = new CMenu(menuBar);
    playMenu = new CMenu(menuBar);
    helpMenu = new CMenu(menuBar);

    registerHandler(fileMenu);
    registerHandler(editMenu);
    registerHandler(viewMenu);
    registerHandler(modifyMenu);
    registerHandler(playMenu);
    registerHandler(helpMenu);

    cursorGroup = new QActionGroup(menuBar);

    // File
    recentMenu = new CMenu(fileMenu); // *
    exportMenu = new CMenu(fileMenu);
    preferencesMenu = new CMenu(fileMenu);

    registerHandler(recentMenu);
    registerHandler(exportMenu);
    registerHandler(preferencesMenu);

    file_newFile = NewAction(ActionImpl::File_New, fileMenu);
    file_newWindow = NewAction(ActionImpl::File_NewWindow, fileMenu);
    file_openFile = NewAction(ActionImpl::File_Open, fileMenu);
    file_saveFile = NewAction(ActionImpl::File_Save, fileMenu);
    file_saveAs = NewAction(ActionImpl::File_SaveAs, fileMenu);
    file_saveAll = NewAction(ActionImpl::File_SaveAll, fileMenu);
    file_importFile = NewAction(ActionImpl::File_Import, fileMenu);
    file_appendFile = NewAction(ActionImpl::File_Append, fileMenu);
    file_fileSettings = NewAction(ActionImpl::File_FileSettings, fileMenu);
    file_closeFile = NewAction(ActionImpl::File_Close, fileMenu);
    file_closeWindow = NewAction(ActionImpl::File_CloseWindow, fileMenu);

    file_exportSelection = NewAction(ActionImpl::File_ExportSelection, exportMenu);
    file_exportTrack = NewAction(ActionImpl::File_ExportTrack, exportMenu);

    preference_settings = NewAction(ActionImpl::File_Settings, preferencesMenu);
    preference_keyShortcuts = NewAction(ActionImpl::File_KeyboardShortcuts, preferencesMenu);
    preference_themes = NewAction(ActionImpl::File_ColorThemes, preferencesMenu);
    preference_languages = NewAction(ActionImpl::File_Languages, preferencesMenu);

    // Edit
    pasteMenu = new CMenu(editMenu);
    removeMenu = new CMenu(editMenu);

    registerHandler(pasteMenu);
    registerHandler(removeMenu);

    edit_undo = NewAction(ActionImpl::Edit_Undo, editMenu);
    edit_redo = NewAction(ActionImpl::Edit_Redo, editMenu);

    edit_cut = NewAction(ActionImpl::Edit_Cut, editMenu);
    edit_copy = NewAction(ActionImpl::Edit_Copy, editMenu);
    edit_paste = NewAction(ActionImpl::Edit_Paste, editMenu);
    edit_remove = NewAction(ActionImpl::Edit_Remove, editMenu);

    edit_pastePitch = NewAction(ActionImpl::Edit_PastePitch, pasteMenu);
    edit_removePitch = NewAction(ActionImpl::Edit_RemovePitch, removeMenu);

    edit_selectAll = NewAction(ActionImpl::Edit_SelectAll, editMenu);
    edit_deselect = NewAction(ActionImpl::Edit_Deselect, editMenu);

    // View
    appearanceMenu = new CMenu(viewMenu);
    panelsMenu = new CMenu(viewMenu);
    displayMenu = new CMenu(viewMenu);
    cursorMenu = new CMenu(viewMenu);

    registerHandler(appearanceMenu);
    registerHandler(panelsMenu);
    registerHandler(displayMenu);
    registerHandler(cursorMenu);

    view_quantization = NewAction(ActionImpl::View_Quantization, viewMenu);
    view_playhead = NewAction(ActionImpl::View_Playhead, viewMenu);

    appearance_toolBar = NewAction(ActionImpl::View_Appearance_ToolBar, appearanceMenu);
    appearance_navBar = NewAction(ActionImpl::View_Appearance_NavigationBar, appearanceMenu);
    appearance_statusBar = NewAction(ActionImpl::View_Appearance_StatusBar, appearanceMenu);
    appearance_panelBars = NewAction(ActionImpl::View_Appearance_ToolWindowsBar, appearanceMenu);
    appearance_menuBar = NewAction(ActionImpl::View_Appearance_MenuBar, appearanceMenu);

    display_note = new QAction(displayMenu);
    display_pitch = new QAction(displayMenu);

    cursor_select = NewAction(ActionImpl::View_Cursor_Select, cursorMenu);
    cursor_sketch = NewAction(ActionImpl::View_Cursor_Sketch, cursorMenu);
    cursor_free = NewAction(ActionImpl::View_Cursor_Freehand, cursorMenu);

    // Modify
    modify_insertLyrics = NewAction(ActionImpl::Modify_InsertLyrics, modifyMenu);
    modify_findReplace = NewAction(ActionImpl::Modify_FindReplace, modifyMenu);
    modify_transpose = NewAction(ActionImpl::Modify_Transpose, modifyMenu);
    modify_octaveUp = NewAction(ActionImpl::Modify_OctaveUp, modifyMenu);
    modify_octaveDown = NewAction(ActionImpl::Modify_OctaveDown, modifyMenu);

    modify_buildIn = NewAction(ActionImpl::Modify_BuildIn, modifyMenu);
    modify_plugins = NewAction(ActionImpl::Modify_Plugins, modifyMenu);

    // Play
    play_play = NewAction(ActionImpl::Playback_Play, playMenu);
    play_stop = NewAction(ActionImpl::Playback_Stop, playMenu);
    play_replay = NewAction(ActionImpl::Playback_Replay, playMenu);
    play_moveStart = NewAction(ActionImpl::Playback_MoveStart, playMenu);
    play_moveEnd = NewAction(ActionImpl::Playback_MoveEnd, playMenu);
    play_removeCache = NewAction(ActionImpl::Playback_RemoveCache, playMenu);
    play_exportAudio = NewAction(ActionImpl::Playback_ExportRecent, playMenu);

    // Help
    help_welcome = NewAction(ActionImpl::Help_Welcome, helpMenu);
    help_showActions = NewAction(ActionImpl::Help_ShowActions, helpMenu);
    help_voiceManager = NewAction(ActionImpl::Help_VoiceManager, helpMenu);
    help_instructions = NewAction(ActionImpl::Help_Instructions, helpMenu);
    help_checkUpdate = NewAction(ActionImpl::Help_CheckUpdate, helpMenu);
    help_aboutApp = NewAction(ActionImpl::Help_AboutApplication, helpMenu);
    help_aboutQt = NewAction(ActionImpl::Help_AboutQt, helpMenu);

    // ----------------- Setup -----------------
    // File
    exportMenu->addAction(file_exportSelection);
    exportMenu->addAction(file_exportTrack);

    preferencesMenu->addAction(preference_settings);
    preferencesMenu->addAction(preference_keyShortcuts);
    preferencesMenu->addAction(preference_themes);
    preferencesMenu->addAction(preference_languages);

    fileMenu->addAction(file_newFile);
    fileMenu->addAction(file_newWindow);
    fileMenu->addSeparator();
    fileMenu->addAction(file_openFile);
    fileMenu->addAction(file_importFile);
    fileMenu->addMenu(recentMenu);
    fileMenu->addSeparator();
    fileMenu->addAction(file_saveFile);
    fileMenu->addAction(file_saveAs);
    fileMenu->addAction(file_saveAll);
    fileMenu->addSeparator();
    fileMenu->addAction(file_appendFile);
    fileMenu->addMenu(exportMenu);
    fileMenu->addSeparator();
    fileMenu->addAction(file_fileSettings);

    auto file_preferences = fileMenu->addMenu(preferencesMenu);

    fileMenu->addSeparator();
    fileMenu->addAction(file_closeFile);
    fileMenu->addAction(file_closeWindow);

    // Edit
    pasteMenu->addAction(edit_pastePitch);
    removeMenu->addAction(edit_removePitch);

    editMenu->addAction(edit_undo);
    editMenu->addAction(edit_redo);
    editMenu->addSeparator();
    editMenu->addAction(edit_cut);
    editMenu->addAction(edit_copy);
    editMenu->addAction(edit_paste);
    editMenu->addMenu(pasteMenu);
    editMenu->addSeparator();
    editMenu->addAction(edit_remove);
    editMenu->addMenu(removeMenu);
    editMenu->addSeparator();
    editMenu->addAction(edit_selectAll);
    editMenu->addAction(edit_deselect);

    // View
    appearanceMenu->addAction(appearance_toolBar);
    appearanceMenu->addAction(appearance_navBar);
    appearanceMenu->addAction(appearance_statusBar);
    appearanceMenu->addAction(appearance_panelBars);
    appearanceMenu->addAction(appearance_menuBar);

    //    panelsMenu->addAction(panels_notePanel);
    //    panelsMenu->addAction(panels_ctrlPanel);
    //    panelsMenu->addAction(panels_pitchPanel);
    //    panelsMenu->addAction(panels_trackPanel);
    //    panelsMenu->addAction(panels_lyricsPanel);
    //    panelsMenu->addAction(panels_statePanel);

    // Play
    displayMenu->addAction(display_note);
    displayMenu->addAction(display_pitch);

    cursorMenu->addAction(cursor_select);
    cursorMenu->addAction(cursor_sketch);
    cursorMenu->addAction(cursor_free);

    viewMenu->addMenu(appearanceMenu);
    viewMenu->addMenu(panelsMenu);
    viewMenu->addSeparator();
    viewMenu->addMenu(displayMenu);
    viewMenu->addSeparator();
    viewMenu->addMenu(cursorMenu);
    viewMenu->addSeparator();
    viewMenu->addAction(view_quantization);
    viewMenu->addAction(view_playhead);

    // Modify
    modifyMenu->addAction(modify_insertLyrics);
    modifyMenu->addAction(modify_findReplace);
    modifyMenu->addSeparator();
    modifyMenu->addAction(modify_transpose);
    modifyMenu->addAction(modify_octaveUp);
    modifyMenu->addAction(modify_octaveDown);
    modifyMenu->addSeparator();
    modifyMenu->addSeparator();
    modifyMenu->addAction(modify_buildIn);
    modifyMenu->addAction(modify_plugins);
    modifyMenu->addSeparator();

    // Play
    playMenu->addAction(play_play);
    playMenu->addAction(play_stop);
    playMenu->addAction(play_replay);
    playMenu->addSeparator();
    playMenu->addAction(play_moveStart);
    playMenu->addAction(play_moveEnd);
    playMenu->addSeparator();
    playMenu->addAction(play_removeCache);
    playMenu->addAction(play_exportAudio);

    // Help
    helpMenu->addAction(help_welcome);
    helpMenu->addAction(help_showActions);
    helpMenu->addSeparator();
    helpMenu->addAction(help_voiceManager);
    helpMenu->addSeparator();
    helpMenu->addAction(help_instructions);
    helpMenu->addAction(help_checkUpdate);
    helpMenu->addSeparator();
    helpMenu->addAction(help_aboutApp);
    helpMenu->addAction(help_aboutQt);

    menuBar->addMenu(fileMenu);
    menuBar->addMenu(editMenu);
    menuBar->addMenu(viewMenu);
    menuBar->addMenu(modifyMenu);
    menuBar->addMenu(playMenu);
    menuBar->addMenu(helpMenu);

    help_aboutApp->setMenuRole(QAction::AboutRole);
    help_aboutQt->setMenuRole(QAction::AboutQtRole);
    file_preferences->setMenuRole(QAction::PreferencesRole);

    // Signals
    Q_Q(FilesActionEngine);
    q->connect(qRecord, &CRecordHolder::recentCommited, q, &FilesActionEngine::_q_recentCommited);
    reloadRecentActions();
}

void FilesActionEnginePrivate::reloadStrings() {
    fileMenu->setTitle(QObject::tr("File(&F)"));
    editMenu->setTitle(QObject::tr("Edit(&E)"));
    viewMenu->setTitle(QObject::tr("View(&V)"));
    modifyMenu->setTitle(QObject::tr("Modify(&M)"));
    playMenu->setTitle(QObject::tr("Playback(&P)"));
    helpMenu->setTitle(QObject::tr("Help(&H)"));

    // File
    recentMenu->setTitle(QObject::tr("Recent")); // *
    exportMenu->setTitle(QObject::tr("Export"));
    preferencesMenu->setTitle(QObject::tr("Preferences"));

    file_newFile->setText(QObject::tr("New File"));
    file_newWindow->setText(QObject::tr("New Window"));
    file_openFile->setText(QObject::tr("Open..."));
    file_saveFile->setText(QObject::tr("Save"));
    file_saveAs->setText(QObject::tr("Save As..."));
    file_saveAll->setText(QObject::tr("Save All"));
    file_importFile->setText(QObject::tr("Import..."));
    file_appendFile->setText(QObject::tr("Append..."));
    file_fileSettings->setText(QObject::tr("File Settings"));
    file_closeFile->setText(QObject::tr("Close"));
    file_closeWindow->setText(QObject::tr("Close Window"));

    file_exportSelection->setText(QObject::tr("Export Selection..."));
    file_exportTrack->setText(QObject::tr("Export Track..."));

    preference_settings->setText(QObject::tr("Settings"));
    preference_keyShortcuts->setText(QObject::tr("Keyboard Shortcuts"));
    preference_themes->setText(QObject::tr("Color Themes"));
    preference_languages->setText(QObject::tr("Multilingual"));

    // Edit
    pasteMenu->setTitle(QObject::tr("More Paste Options"));
    removeMenu->setTitle(QObject::tr("More Remove Options"));

    edit_undo->setText(QObject::tr("Undo"));
    edit_redo->setText(QObject::tr("Redo"));

    edit_cut->setText(QObject::tr("Cut"));
    edit_copy->setText(QObject::tr("Copy"));
    edit_paste->setText(QObject::tr("Paste"));
    edit_pastePitch->setText(QObject::tr("Paste Pitch"));

    edit_remove->setText(QObject::tr("Remove"));
    edit_removePitch->setText(QObject::tr("Remove Points"));

    edit_selectAll->setText(QObject::tr("Select All"));
    edit_deselect->setText(QObject::tr("Deselect"));

    // View
    appearanceMenu->setTitle(QObject::tr("Appearance"));
    panelsMenu->setTitle(QObject::tr("Tool Panels"));
    displayMenu->setTitle(QObject::tr("Note Display"));
    cursorMenu->setTitle(QObject::tr("Cursor Mode"));

    view_quantization->setText(QObject::tr("Quantization..."));
    view_playhead->setText(QObject::tr("Playhead..."));

    appearance_toolBar->setText(QObject::tr("Tool Bar"));
    appearance_navBar->setText(QObject::tr("Navigation Bar"));
    appearance_statusBar->setText(QObject::tr("Status Bar"));
    appearance_panelBars->setText(QObject::tr("Tool Panel Bars"));
    appearance_menuBar->setText(QObject::tr("Menu Bar"));

    //    panels_notePanel->setText(QObject::tr("Note"));
    //    panels_ctrlPanel->setText(QObject::tr("Control"));
    //    panels_pitchPanel->setText(QObject::tr("Pitch"));
    //    panels_trackPanel->setText(QObject::tr("Track"));
    //    panels_lyricsPanel->setText(QObject::tr("Lyrics"));
    //    panels_statePanel->setText(QObject::tr("State"));

    display_note->setText(QObject::tr("Show Note"));
    display_pitch->setText(QObject::tr("Show Pitch"));

    cursor_select->setText(QObject::tr("Select Mode"));
    cursor_sketch->setText(QObject::tr("Sketch Mode"));
    cursor_free->setText(QObject::tr("Freehand Mode"));

    // Modify
    modify_insertLyrics->setText(QObject::tr("Insert Lyrics..."));
    modify_findReplace->setText(QObject::tr("Find/Replace"));
    modify_transpose->setText(QObject::tr("Transpose..."));
    modify_octaveUp->setText(QObject::tr("Shift Up by an Octave"));
    modify_octaveDown->setText(QObject::tr("Shift Down by an Octave"));

    modify_buildIn->setText(QObject::tr("Build-in Tools"));
    modify_plugins->setText(QObject::tr("Plugins"));

    // Play
    play_play->setText(QObject::tr("Play"));
    play_stop->setText(QObject::tr("Stop"));
    play_replay->setText(QObject::tr("Replay"));
    play_moveStart->setText(QObject::tr("Move to Start"));
    play_moveEnd->setText(QObject::tr("Move to End"));
    play_removeCache->setText(QObject::tr("Remove Cache"));
    play_exportAudio->setText(QObject::tr("Export Recent Audio..."));

    // Help
    help_welcome->setText(QObject::tr("Welcome"));
    help_showActions->setText(QObject::tr("Show All Actions"));
    help_voiceManager->setText(QObject::tr("Voice Database Manager"));
    help_instructions->setText(QObject::tr("Instructions"));
    help_checkUpdate->setText(QObject::tr("Check Update"));
    help_aboutApp->setText(QObject::tr("About %1").arg(qData->appName()));
    help_aboutQt->setText(QObject::tr("About %1").arg("Qt"));
}

void FilesActionEnginePrivate::reloadShortcuts() {
    // File
    file_newFile->setShortcut(QKeySequence("Ctrl+N"));
    file_newWindow->setShortcut(QKeySequence("Ctrl+Shift+N"));
    file_openFile->setShortcut(QKeySequence("Ctrl+O"));
    file_saveFile->setShortcut(QKeySequence("Ctrl+S"));
    file_saveAs->setShortcut(QKeySequence("Ctrl+Shift+S"));
    file_closeFile->setShortcut(QKeySequence("Ctrl+W"));

    preference_settings->setShortcut(QKeySequence("Ctrl+,"));
    preference_keyShortcuts->setShortcut(QKeySequence("Ctrl+K,Ctrl+S"));
    preference_themes->setShortcut(QKeySequence("Ctrl+K,Ctrl+T"));
    preference_languages->setShortcut(QKeySequence("Ctrl+K,Ctrl+L"));

    // Edit
    edit_undo->setShortcut(QKeySequence("Ctrl+Z"));
    edit_redo->setShortcut(QKeySequence("Ctrl+Shift+Z"));

    edit_copy->setShortcut(QKeySequence("Ctrl+C"));
    edit_cut->setShortcut(QKeySequence("Ctrl+X"));
    edit_paste->setShortcut(QKeySequence("Ctrl+V"));

    edit_remove->setShortcut(QKeySequence("Del"));

    edit_selectAll->setShortcut(QKeySequence("Ctrl+A"));

    // View
    //    panels_notePanel->setShortcut(QKeySequence("Ctrl+E"));
    //    panels_ctrlPanel->setShortcut(QKeySequence("Ctrl+Y"));
    //    panels_pitchPanel->setShortcut(QKeySequence("Ctrl+P"));
    //    panels_trackPanel->setShortcut(QKeySequence("Ctrl+T"));
    //    panels_lyricsPanel->setShortcut(QKeySequence("Ctrl+L"));
    //    panels_statePanel->setShortcut(QKeySequence("Ctrl+I"));

    display_note->setShortcut(QKeySequence("Show Note"));
    display_pitch->setShortcut(QKeySequence("Show Pitch"));

    cursor_select->setShortcut(QKeySequence("G"));
    cursor_sketch->setShortcut(QKeySequence("H"));
    cursor_free->setShortcut(QKeySequence("F"));

    // Modify
    modify_insertLyrics->setShortcut(QKeySequence("Ctrl+Shift+L"));
    modify_findReplace->setShortcut(QKeySequence("Ctrl+F"));

    // Play
    play_play->setShortcut(QKeySequence("Space"));
    play_stop->setShortcut(QKeySequence("F6"));
    play_replay->setShortcut(QKeySequence("Shift+F5"));
    play_exportAudio->setShortcut(QKeySequence("Ctrl+Shift+P"));

    // Help
    help_showActions->setShortcut(QKeySequence("Ctrl+Shift+A"));
}

QAction *FilesActionEnginePrivate::NewAction(ActionImpl::Action a, QObject *parent) {
    auto action = new QAction(parent);
    map.insert(action, a);
    return action;
}

void FilesActionEnginePrivate::registerHandler(QMenu *menu) {
    Q_Q(FilesActionEngine);
    q->connect(menu, &QMenu::triggered, q, &FilesActionEngine::_q_actionTriggered);
}

void FilesActionEnginePrivate::reloadRecentActions() {
    QStringList projects = qRecordCData.projects.valid();

    // Remove all Actions
    {
        const auto &actions = recentMenu->actions();
        for (auto action : qAsConst(actions)) {
            map.remove(action);
            recentMenu->removeAction(action);
            action->deleteLater(); // Cannot delete directly
        }
        recentMenu->clear();
    }

    QVector<QAction *> vogFiles;

    int fileActions = 0;
    for (auto it = projects.begin(); it != projects.end(); ++it) {
        if (fileActions == 10) {
            break;
        }
        QString name = *it;
        auto action = NewAction(ActionImpl::Special_Recent, recentMenu);
        action->setText(QDir::toNativeSeparators(name));
        action->setData(ActionData(ActionData::OpenFile, name).toVariant());
        vogFiles.append(action);
        fileActions++;
    }

    int i;
    for (i = 0; i < vogFiles.size(); ++i) {
        auto action = vogFiles.at(i);
        recentMenu->addAction(action);
    }
    if (i > 0) {
        recentMenu->addSeparator();
    }

    if (vogFiles.isEmpty()) {
        auto action = NewAction(ActionImpl::Special_Recent, recentMenu);
        action->setData(ActionData(ActionData::Null).toVariant());
        recentMenu->addAction(action);
    } else {
        bool more = false;
        if (fileActions < projects.size()) {
            auto action = NewAction(ActionImpl::Special_Recent, recentMenu);
            action->setData(ActionData(ActionData::MoreFiles).toVariant());
            recentMenu->addAction(action);
            more = true;
        }
        if (more) {
            recentMenu->addSeparator();
        }
        auto clearRecentAction = NewAction(ActionImpl::Special_Recent, recentMenu);
        clearRecentAction->setData(ActionData(ActionData::Clear).toVariant());
        recentMenu->addAction(clearRecentAction);
    }

    reloadRecentActionStrings();
}

void FilesActionEnginePrivate::reloadRecentActionStrings() {
    const auto &actions = recentMenu->actions();
    for (auto it = actions.begin(); it != actions.end(); ++it) {
        auto action = *it;
        QVariant var = action->data();
        if (var.convert(qMetaTypeId<ActionData>())) {
            ActionData data = ActionData::fromVariant(var);
            switch (data.type) {
            case ActionData::Null:
                action->setText(QObject::tr("Null"));
                break;
            case ActionData::MoreFiles:
                action->setText(QObject::tr("More Files..."));
                break;
            case ActionData::MoreDirs:
                action->setText(QObject::tr("More Folders..."));
                break;
            case ActionData::Clear:
                action->setText(QObject::tr("Clear Recent List"));
                break;
            default:
                break;
            }
        }
    }
}

void FilesActionEnginePrivate::handleRecentAction(QAction *action) {
    QVariant var = action->data();
    if (var.convert(qMetaTypeId<ActionData>())) {
        ActionData data = ActionData::fromVariant(var);
        switch (data.type) {
        case ActionData::Null:
            break;
        case ActionData::OpenFile:
            eventMgr->openFile(data.data);
            break;
        case ActionData::OpenDir:
            break;
        case ActionData::MoreFiles:
            break;
        case ActionData::MoreDirs:
            break;
        case ActionData::Clear:
            qRecord->commitRecent(CRecordHolder::Project, CRecordHolder::Clear);
            qRecord->commitRecent(CRecordHolder::Folder, CRecordHolder::Clear);
            break;
        }
    }
}
