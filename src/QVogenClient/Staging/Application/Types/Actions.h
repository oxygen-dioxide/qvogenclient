#ifndef ACTIONS_H
#define ACTIONS_H

namespace ActionImpl {
    enum MainMenu {
        File,
        Edit,
        View,
        Modify,
        Playback,
        Help,
    };

    enum Action {
        File_New,
        File_NewWindow,
        File_Open,
        File_Save,
        File_SaveAs,
        File_SaveAll,
        File_Import,
        File_Append,
        File_ExportSelection,
        File_ExportTrack,
        File_FileSettings,
        File_Settings,
        File_KeyboardShortcuts,
        File_ColorThemes,
        File_Languages,
        File_Close,
        File_CloseWindow,

        Edit_Undo,
        Edit_Redo,
        Edit_Cut,
        Edit_Copy,
        Edit_Paste,
        Edit_PastePitch,
        Edit_Remove,
        Edit_RemovePitch,
        Edit_SelectAll,
        Edit_Deselect,

        View_Appearance_ToolBar,
        View_Appearance_NavigationBar,
        View_Appearance_StatusBar,
        View_Appearance_ToolWindowsBar,
        View_Appearance_MenuBar,

        Modify_InsertLyrics,
        Modify_FindReplace,
        Modify_Transpose,
        Modify_OctaveUp,
        Modify_OctaveDown,

        Playback_Play,
        Playback_Stop,
        Playback_Replay,
        Playback_MoveStart,
        Playback_MoveEnd,
        Playback_RemoveCache,
        Playback_ExportRecent,

        Help_Welcome,
        Help_ShowActions,
        Help_VoiceManager,
        Help_Instructions,
        Help_CheckUpdate,
        Help_AboutApplication,
        Help_AboutQt,
    };

}; // namespace ActionImpl

#endif // ACTIONS_H
