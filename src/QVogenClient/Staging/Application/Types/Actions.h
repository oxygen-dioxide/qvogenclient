#ifndef ACTIONS_H
#define ACTIONS_H

#include <QtGlobal>

namespace ActionImpl {

    enum MainMenu {
        File,
        Edit,
        View,
        Modify,
        Playback,
        Help,
        IndexMax,
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

        View_Cursor_Select,
        View_Cursor_Sketch,
        View_Cursor_Freehand,

        View_Quantization,
        View_Playhead,

        Modify_InsertLyrics,
        Modify_FindReplace,
        Modify_Transpose,
        Modify_OctaveUp,
        Modify_OctaveDown,

        Modify_BuildIn,
        Modify_Plugins,

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

        Special_Recent,
    };

    enum StateType {
        FileState = 0x1,
        EditState = 0x2,
        SelectState = 0x4,
        StateMask = FileState | EditState | SelectState,
    };
    Q_DECLARE_FLAGS(StateTypes, StateType);

    enum State {
        NoFlag = 0x0,

        TabFlag = 0x1,
        DocumentFlag = TabFlag * 2,
        TypeMask = TabFlag | DocumentFlag,

        EditedFlag = 0x100,
        UntitledFlag = EditedFlag * 2,
        DeletedFlag = UntitledFlag * 2,
        FileMask = EditedFlag | UntitledFlag | DeletedFlag,

        UndoFlag = 0x1000,
        RedoFlag = UndoFlag * 2,
        EditMask = UndoFlag | RedoFlag,

        SelectionFlag = 0x10000,
        SelectNoteFlag = SelectionFlag * 2,
        SelectionMask = SelectionFlag | SelectNoteFlag,
    };

    Q_DECLARE_FLAGS(States, State);

}; // namespace ActionImpl

#endif // ACTIONS_H
