#include "FilesActionEngine_p.h"

#include "CMenu.h"
#include "DataManager.h"

FilesActionEnginePrivate::FilesActionEnginePrivate() {
}

FilesActionEnginePrivate::~FilesActionEnginePrivate() {
    for (auto it = map.begin(); it != map.end(); ++it) {
        delete it.key();
    }
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
    recentMenu = new CMenu(); // *
    exportMenu = new CMenu();
    preferencesMenu = new CMenu();

    registerHandler(recentMenu);
    registerHandler(exportMenu);
    registerHandler(preferencesMenu);

    file_newFile = NewAction(ActionImpl::File_New);
    file_newWindow = NewAction(ActionImpl::File_NewWindow);
    file_openFile = NewAction(ActionImpl::File_Open);
    file_saveFile = NewAction(ActionImpl::File_Save);
    file_saveAs = NewAction(ActionImpl::File_SaveAs);
    file_saveAll = NewAction(ActionImpl::File_SaveAll);
    file_importFile = NewAction(ActionImpl::File_Import);
    file_appendFile = NewAction(ActionImpl::File_Append);
    file_fileSettings = NewAction(ActionImpl::File_FileSettings);
    file_closeFile = NewAction(ActionImpl::File_Close);
    file_closeWindow = NewAction(ActionImpl::File_CloseWindow);

    file_exportSelection = NewAction(ActionImpl::File_ExportSelection);
    file_exportTrack = NewAction(ActionImpl::File_ExportTrack);

    preference_settings = NewAction(ActionImpl::File_Settings);
    preference_keyShortcuts = NewAction(ActionImpl::File_KeyboardShortcuts);
    preference_themes = NewAction(ActionImpl::File_ColorThemes);
    preference_languages = NewAction(ActionImpl::File_Languages);

    // Edit
    pasteMenu = new CMenu();
    removeMenu = new CMenu();

    registerHandler(pasteMenu);
    registerHandler(removeMenu);

    edit_undo = NewAction(ActionImpl::Edit_Undo);
    edit_redo = NewAction(ActionImpl::Edit_Redo);

    edit_cut = NewAction(ActionImpl::Edit_Cut);
    edit_copy = NewAction(ActionImpl::Edit_Copy);
    edit_paste = NewAction(ActionImpl::Edit_Paste);
    edit_pastePitch = NewAction(ActionImpl::Edit_PastePitch);

    edit_remove = NewAction(ActionImpl::Edit_Remove);
    edit_removePitch = NewAction(ActionImpl::Edit_RemovePitch);

    edit_selectAll = NewAction(ActionImpl::Edit_SelectAll);
    edit_deselect = NewAction(ActionImpl::Edit_Deselect);

    // View
    appearanceMenu = new CMenu();
    panelsMenu = new CMenu();
    quantizationMenu = new CMenu();
    playheadMenu = new CMenu();
    displayMenu = new CMenu();
    cursorMenu = new CMenu();

    registerHandler(appearanceMenu);
    registerHandler(panelsMenu);
    registerHandler(quantizationMenu);
    registerHandler(playheadMenu);
    registerHandler(displayMenu);
    registerHandler(cursorMenu);

    appearance_toolBar = NewAction(ActionImpl::View_Appearance_ToolBar);
    appearance_navBar = NewAction(ActionImpl::View_Appearance_NavigationBar);
    appearance_statusBar = NewAction(ActionImpl::View_Appearance_StatusBar);
    appearance_panelBars = NewAction(ActionImpl::View_Appearance_ToolWindowsBar);
    appearance_menuBar = NewAction(ActionImpl::View_Appearance_MenuBar);

    panels_notePanel = new QAction();
    panels_ctrlPanel = new QAction();
    panels_pitchPanel = new QAction();
    panels_trackPanel = new QAction();
    panels_lyricsPanel = new QAction();
    panels_statePanel = new QAction();

    playhead_normal = new QAction();
    playhead_center = new QAction();

    display_note = new QAction();
    display_pitch = new QAction();
    display_env = new QAction();

    cursor_select = new QAction();
    cursor_sketch = new QAction();
    cursor_free = new QAction();

    // Modify
    buildInMenu = new CMenu(); // *
    pluginsMenu = new CMenu(); // *

    registerHandler(buildInMenu);
    registerHandler(pluginsMenu);

    modify_insertLyrics = NewAction(ActionImpl::Modify_InsertLyrics);
    modify_findReplace = NewAction(ActionImpl::Modify_FindReplace);
    modify_transpose = NewAction(ActionImpl::Modify_Transpose);
    modify_octaveUp = NewAction(ActionImpl::Modify_OctaveUp);
    modify_octaveDown = NewAction(ActionImpl::Modify_OctaveDown);

    // Play
    play_play = NewAction(ActionImpl::Playback_Play);
    play_stop = NewAction(ActionImpl::Playback_Stop);
    play_replay = NewAction(ActionImpl::Playback_Replay);
    play_moveStart = NewAction(ActionImpl::Playback_MoveStart);
    play_moveEnd = NewAction(ActionImpl::Playback_MoveEnd);
    play_removeCache = NewAction(ActionImpl::Playback_RemoveCache);
    play_exportAudio = NewAction(ActionImpl::Playback_ExportRecent);

    // Help
    help_welcome = NewAction(ActionImpl::Help_Welcome);
    help_showActions = NewAction(ActionImpl::Help_ShowActions);
    help_voiceManager = NewAction(ActionImpl::Help_VoiceManager);
    help_instructions = NewAction(ActionImpl::Help_Instructions);
    help_checkUpdate = NewAction(ActionImpl::Help_CheckUpdate);
    help_aboutApp = NewAction(ActionImpl::Help_AboutApplication);
    help_aboutQt = NewAction(ActionImpl::Help_AboutQt);

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
    fileMenu->addMenu(recentMenu);
    fileMenu->addSeparator();
    fileMenu->addAction(file_saveFile);
    fileMenu->addAction(file_saveAs);
    fileMenu->addAction(file_saveAll);
    fileMenu->addSeparator();
    fileMenu->addAction(file_importFile);
    fileMenu->addAction(file_appendFile);
    fileMenu->addMenu(exportMenu);
    fileMenu->addSeparator();
    fileMenu->addAction(file_fileSettings);
    fileMenu->addMenu(preferencesMenu);
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

    panelsMenu->addAction(panels_notePanel);
    panelsMenu->addAction(panels_ctrlPanel);
    panelsMenu->addAction(panels_pitchPanel);
    panelsMenu->addAction(panels_trackPanel);
    panelsMenu->addAction(panels_lyricsPanel);
    panelsMenu->addAction(panels_statePanel);

    // Play
    playheadMenu->addAction(playhead_normal);
    playheadMenu->addAction(playhead_center);

    displayMenu->addAction(display_note);
    displayMenu->addAction(display_pitch);
    displayMenu->addAction(display_env);

    cursorMenu->addAction(cursor_select);
    cursorMenu->addAction(cursor_sketch);
    cursorMenu->addAction(cursor_free);

    viewMenu->addMenu(appearanceMenu);
    viewMenu->addMenu(panelsMenu);
    viewMenu->addSeparator();
    viewMenu->addMenu(quantizationMenu);
    viewMenu->addMenu(playheadMenu);
    viewMenu->addMenu(displayMenu);
    viewMenu->addSeparator();
    viewMenu->addMenu(cursorMenu);

    // Modify
    modifyMenu->addAction(modify_insertLyrics);
    modifyMenu->addAction(modify_findReplace);
    modifyMenu->addSeparator();
    modifyMenu->addAction(modify_transpose);
    modifyMenu->addAction(modify_octaveUp);
    modifyMenu->addAction(modify_octaveDown);
    modifyMenu->addSeparator();
    modifyMenu->addSeparator();
    modifyMenu->addMenu(buildInMenu);
    modifyMenu->addMenu(pluginsMenu);
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
    file_importFile->setText(QObject::tr("Import"));
    file_appendFile->setText(QObject::tr("Append"));
    file_fileSettings->setText(QObject::tr("File Settings"));
    file_closeFile->setText(QObject::tr("Close File"));
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
    quantizationMenu->setTitle(QObject::tr("Quantization"));
    playheadMenu->setTitle(QObject::tr("Playhead"));
    displayMenu->setTitle(QObject::tr("Note Display"));
    cursorMenu->setTitle(QObject::tr("Cursor Mode"));

    appearance_toolBar->setText(QObject::tr("Tool Bar"));
    appearance_navBar->setText(QObject::tr("Navigation Bar"));
    appearance_statusBar->setText(QObject::tr("Status Bar"));
    appearance_panelBars->setText(QObject::tr("Tool Panel Bars"));
    appearance_menuBar->setText(QObject::tr("Menu Bar"));

    panels_notePanel->setText(QObject::tr("Note"));
    panels_ctrlPanel->setText(QObject::tr("Control"));
    panels_pitchPanel->setText(QObject::tr("Pitch"));
    panels_trackPanel->setText(QObject::tr("Track"));
    panels_lyricsPanel->setText(QObject::tr("Lyrics"));
    panels_statePanel->setText(QObject::tr("State"));

    playhead_normal->setText(QObject::tr("Page"));
    playhead_center->setText(QObject::tr("Smooth"));

    display_note->setText(QObject::tr("Show Note"));
    display_pitch->setText(QObject::tr("Show Pitch"));
    display_env->setText(QObject::tr("Show Envelope"));

    cursor_select->setText(QObject::tr("Select Mode"));
    cursor_sketch->setText(QObject::tr("Sketch Mode"));
    cursor_free->setText(QObject::tr("Freehand Mode"));

    // Modify
    buildInMenu->setTitle(QObject::tr("Build-in Tools")); // *
    pluginsMenu->setTitle(QObject::tr("Plugins"));        // *

    modify_insertLyrics->setText(QObject::tr("Insert Lyrics..."));
    modify_findReplace->setText(QObject::tr("Find/Replace"));
    modify_transpose->setText(QObject::tr("Transpose..."));
    modify_octaveUp->setText(QObject::tr("Shift Up by an Octave"));
    modify_octaveDown->setText(QObject::tr("Shift Down by an Octave"));

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
    file_newWindow->setShortcut(QObject::tr("Ctrl+Shift+N"));
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
    panels_notePanel->setShortcut(QKeySequence("Ctrl+E"));
    panels_ctrlPanel->setShortcut(QKeySequence("Ctrl+Y"));
    panels_pitchPanel->setShortcut(QKeySequence("Ctrl+P"));
    panels_trackPanel->setShortcut(QKeySequence("Ctrl+T"));
    panels_lyricsPanel->setShortcut(QKeySequence("Ctrl+L"));
    panels_statePanel->setShortcut(QKeySequence("Ctrl+I"));

    display_note->setShortcut(QKeySequence("Show Note"));
    display_pitch->setShortcut(QKeySequence("Show Pitch"));
    display_env->setShortcut(QKeySequence("Show Envelope"));

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

QAction *FilesActionEnginePrivate::NewAction(ActionImpl::Action a) {
    auto action = new QAction();
    map.insert(action, a);
    return action;
}

void FilesActionEnginePrivate::registerHandler(QMenu *menu) {
    Q_Q(FilesActionEngine);
    q->connect(menu, &QMenu::triggered, q, &FilesActionEngine::q_actionTriggered);
}
