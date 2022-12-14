#if !defined(FCODER_TYPES_H)
#define FCODER_TYPES_H


struct Thread_Context_Extra_Info{
    void *coroutine;
    void *async_thread;
};

struct Models;
struct App{
    Thread_Context *tctx;
    Models *cmd_context;
};
typedef App Application_Links;

////////////////////////////////


typedef u16 ID_Color;


union FColor{
    struct{
        u8 padding__[3];
        u8 a_byte;
    };
    ARGB_Color argb;
    struct{
        ID_Color id;
        u8 sub_index;
        u8 padding_;
    };
};


struct Theme_Color{
    ID_Color tag;
    ARGB_Color color;
};


struct Color_Array{
    ARGB_Color *vals;
    i32 count;
};


struct Color_Table{
    Color_Array *arrays;
    i32 count;
};


struct Color_Picker{
    String_Const_u8 title;
    ARGB_Color *dest;
    b32 *finished;
};

////////////////////////////////


typedef u32 Face_ID;


struct Fancy_String{
    Fancy_String *next;
    String_Const_u8 value;
    Face_ID face;
    FColor fore;
    f32 pre_margin;
    f32 post_margin;
};


struct Fancy_Line{
    Fancy_Line *next;
    Face_ID face;
    FColor fore;
    Fancy_String *first;
    Fancy_String *last;
};


struct Fancy_Block{
    Fancy_Line *first;
    Fancy_Line *last;
    i32 line_count;
};

////////////////////////////////


typedef i32 Buffer_ID;


typedef i32 View_ID;


typedef i32 Panel_ID;


typedef u32 Text_Layout_ID;


typedef u32 Child_Process_ID;


typedef i32 UI_Highlight_Level;
enum{
    UIHighlight_None,
    UIHighlight_Hover,
    UIHighlight_Active,
};


struct Buffer_Point{
    i64 line_number;
    Vec2_f32 pixel_shift;
};


struct Line_Shift_Vertical{
    i64 line;
    f32 y_delta;
};


struct Line_Shift_Character{
    i64 line;
    i64 character_delta;
};


typedef u32 Child_Process_Set_Target_Flags;
enum{
    ChildProcessSet_FailIfBufferAlreadyAttachedToAProcess = 1,
    ChildProcessSet_FailIfProcessAlreadyAttachedToABuffer = 2,
    ChildProcessSet_NeverOverrideExistingAttachment = 3,
    ChildProcessSet_CursorAtEnd = 4,
};


typedef u32 Memory_Protect_Flags;
enum{
    MemProtect_Read    = 0x1,
    MemProtect_Write   = 0x2,
    MemProtect_Execute = 0x4,
};


typedef i32 Wrap_Indicator_Mode;
enum{
    WrapIndicator_Hide,
    WrapIndicator_Show_After_Line,
    WrapIndicator_Show_At_Wrap_Edge,
};


typedef i32 Global_Setting_ID;
enum{
    GlobalSetting_Null,
    GlobalSetting_LAltLCtrlIsAltGr,
};


typedef i32 Buffer_Setting_ID;
enum{
    BufferSetting_Null,
    BufferSetting_Unimportant,
    BufferSetting_ReadOnly,
    BufferSetting_RecordsHistory,
    BufferSetting_Unkillable,
};


struct Character_Predicate{
    u8 b[32];
};


struct Frame_Info{
    i32 index;
    f32 literal_dt;
    f32 animation_dt;
};


typedef i32 View_Setting_ID;
enum{
    ViewSetting_Null,
    ViewSetting_ShowWhitespace,
    ViewSetting_ShowScrollbar,
    ViewSetting_ShowFileBar,
    ViewSetting_ColorModification,
};


typedef u32 Buffer_Create_Flag;
enum{
    BufferCreate_Background = 0x1,
    BufferCreate_AlwaysNew  = 0x2,
    BufferCreate_NeverNew   = 0x4,
    BufferCreate_JustChangedFile = 0x8,
    BufferCreate_MustAttachToFile = 0x10,
    BufferCreate_NeverAttachToFile = 0x20,
    BufferCreate_SuppressNewFileHook = 0x40,
};


typedef u32 Buffer_Save_Flag;
enum{
    BufferSave_IgnoreDirtyFlag = 0x1,
};


typedef u32 Buffer_Kill_Flag;
enum{
    BufferKill_AlwaysKill  = 0x2,
};


typedef u32 Buffer_Reopen_Flag;
enum{
    BufferReopenFlag_OnlyHereToPreventWarning,
};


typedef u32 Buffer_Kill_Result;
enum{
    BufferKillResult_Killed = 0,
    BufferKillResult_Dirty = 1,
    BufferKillResult_Unkillable = 2,
    BufferKillResult_DoesNotExist = 3,
};


typedef u32 Buffer_Reopen_Result;
enum{
    BufferReopenResult_Reopened = 0,
    BufferReopenResult_Failed = 1,
};


typedef u32 Access_Flag;
enum{
    Access_Write = 0x1,
    Access_Read = 0x2,
    Access_Visible = 0x4,
};
enum{
    Access_Always = 0,
    Access_ReadWrite = Access_Write|Access_Read,
    Access_ReadVisible = Access_Read|Access_Visible,
    Access_ReadWriteVisible = Access_Write|Access_Read|Access_Visible,
};


typedef i32 Dirty_State;
enum{
    DirtyState_UpToDate = 0,
    DirtyState_UnsavedChanges = 1,
    DirtyState_UnloadedChanges = 2,
    DirtyState_UnsavedChangesAndUnloadedChanges = 3,
};


typedef u32 Command_Line_Interface_Flag;
enum{
    CLI_OverlapWithConflict = 0x1,
    CLI_AlwaysBindToView    = 0x2,
    CLI_CursorAtEnd         = 0x4,
    CLI_SendEndSignal       = 0x8,
};


typedef u32 Set_Buffer_Flag;
enum{
    SetBuffer_KeepOriginalGUI = 0x1
};


typedef i32 Mouse_Cursor_Show_Type;
enum{
    MouseCursorShow_Never,
    MouseCursorShow_Always,
};


typedef i32 View_Split_Position;
enum{
    ViewSplit_Top,
    ViewSplit_Bottom,
    ViewSplit_Left,
    ViewSplit_Right,
};


typedef i32 Panel_Split_Kind;
enum{
    PanelSplitKind_Ratio_Min = 0,
    PanelSplitKind_Ratio_Max = 1,
    PanelSplitKind_FixedPixels_Min = 2,
    PanelSplitKind_FixedPixels_Max = 3,
};


typedef u8 Key_Modifier;


struct Mouse_State{
    b8 l;
    b8 r;
    b8 press_l;
    b8 press_r;
    b8 release_l;
    b8 release_r;
    b8 out_of_window;
    i32 wheel;
    union{
        struct{
            i32 x;
            i32 y;
        };
        Vec2_i32 p;
    };
};


struct Parser_String_And_Type{
    char *str;
    u32 length;
    u32 type;
};


typedef u32 File_Attribute_Flag;
enum{
    FileAttribute_IsDirectory = 1,
};


struct File_Attributes{
    u64 size;
    u64 last_write_time;
    File_Attribute_Flag flags;
};


struct File_Info{
    File_Info *next;
    String_Const_u8 file_name;
    File_Attributes attributes;
};


struct File_List{
    File_Info **infos;
    u32 count;
};


struct Buffer_Identifier{
    char *name;
    i32 name_len;
    Buffer_ID id;
};


typedef i32 Set_Buffer_Scroll_Rule;
enum{
    SetBufferScroll_NoCursorChange,
    SetBufferScroll_SnapCursorIntoView,
};


struct Buffer_Scroll{
    Buffer_Point position;
    Buffer_Point target;
};


struct Basic_Scroll{
    Vec2_f32 position;
    Vec2_f32 target;
};


typedef i32 Buffer_Seek_Type;
enum{
    buffer_seek_pos,
    buffer_seek_line_col,
};


struct Buffer_Seek{
    Buffer_Seek_Type type;
    union{
        struct{
            i64 pos;
        };
        struct{
            i64 line;
            i64 col;
        };
    };
};


struct Buffer_Cursor{
    i64 pos;
    i64 line;
    i64 col;
};


union Range_Cursor{
    struct{
        Buffer_Cursor min;
        Buffer_Cursor max;
    };
    struct{
        Buffer_Cursor start;
        Buffer_Cursor end;
    };
    struct{
        Buffer_Cursor first;
        Buffer_Cursor one_past_last;
    };
};


struct Marker{
    i64 pos;
    b32 lean_right;
};


typedef i32 Managed_Object_Type;
enum{
    ManagedObjectType_Error = 0,
    ManagedObjectType_Memory = 1,
    ManagedObjectType_Markers = 2,

    ManagedObjectType_COUNT = 4,
};


typedef u64 Managed_ID;
typedef u64 Managed_Scope;
typedef u64 Managed_Object;


struct Marker_Visual{
    Managed_Scope scope;
    u32 slot_id;
    u32 gen_id;
};


typedef u32 Glyph_Flag;
enum{
    GlyphFlag_None = 0x0,
};


struct Query_Bar{
    String_Const_u8 prompt;
    String_Const_u8 string;
    u64 string_capacity;
};


struct Query_Bar_Ptr_Array{
    Query_Bar **ptrs;
    i32 count;
};


struct Query_Bar_Group{
    App *app;
    View_ID view;

    Query_Bar_Group(App *app);
    Query_Bar_Group(App *app, View_ID view);
    ~Query_Bar_Group();
};


struct Font_Load_Location{
    String_Const_u8 file_name;
};


typedef u32 Face_Antialiasing_Mode;
enum{
    FaceAntialiasingMode_8BitMono,
    FaceAntialiasingMode_1BitMono,
};


struct Face_Load_Parameters{
    u32 pt_size;
    Face_Antialiasing_Mode aa_mode;
    b8 bold;
    b8 italic;
    b8 underline;
    b8 hinting;
};


struct Face_Description{
    Font_Load_Location font;
    Face_Load_Parameters parameters;
};


struct Face_Metrics{
    f32 text_height;
    f32 line_height;
    f32 ascent;
    f32 descent;
    f32 line_skip;

    f32 underline_yoff1;
    f32 underline_yoff2;

    f32 max_advance;
    f32 space_advance;
    f32 decimal_digit_advance;
    f32 hex_digit_advance;
    f32 byte_advance;
    f32 byte_sub_advances[3];
    f32 normal_lowercase_advance;
    f32 normal_uppercase_advance;
    f32 normal_advance;
};


struct Codepoint_Index_Map{
    b32 has_zero_index;
    u16 zero_index;
    u16 max_index;
    Table_u32_u16 table;
};


struct Face_Advance_Map{
    Codepoint_Index_Map codepoint_to_index;
    f32 *advance;
    u16 index_count;
};


struct Edit{
    String_Const_u8 text;
    Range_i64 range;
};


struct Batch_Edit{
    Batch_Edit *next;
    Edit edit;
};


typedef i32 Record_Kind;
enum{
    RecordKind_Single,
    RecordKind_Group,
};


typedef i32 Record_Error;
enum{
    RecordError_NoError,
    RecordError_InvalidBuffer,
    RecordError_NoHistoryAttached,
    RecordError_IndexOutOfBounds,
    RecordError_SubIndexOutOfBounds,
    RecordError_InitialStateDummyRecord,
    RecordError_WrongRecordTypeAtIndex,
};


typedef u32 Record_Merge_Flag;
enum{
    RecordMergeFlag_StateInRange_MoveStateForward = 0x0,
    RecordMergeFlag_StateInRange_MoveStateBackward = 0x1,
    RecordMergeFlag_StateInRange_ErrorOut = 0x2,
};


typedef i32 History_Record_Index;


struct Record_Info{
    Record_Error error;
    Record_Kind kind;
    i64 pos_before_edit;
    i32 edit_number;
    union{
        struct{
            String_Const_u8 single_string_forward;
            String_Const_u8 single_string_backward;
            i64 single_first;
        };
        struct{
            i32 group_count;
        };
    };
};

#if defined(CUSTOM_COMMAND_SIG) || defined(CUSTOM_UI_COMMAND_SIG) || defined(CUSTOM_DOC) || defined(CUSTOM_COMMAND)
#error Please do not define CUSTOM_COMMAND_SIG, CUSTOM_DOC, CUSTOM_UI_COMMAND_SIG, or CUSTOM_COMMAND
#endif

#if !defined(META_PASS)
#define CUSTOM_COMMAND_SIG(name) internal void name(struct App *app)
#define CUSTOM_UI_COMMAND_SIG(name) internal void name(struct App *app)
#define CUSTOM_DOC(str)
#define CUSTOM_ID(group, name) global Managed_ID name;
#else
#define CUSTOM_COMMAND_SIG(name) CUSTOM_COMMAND(name, __FILE__, __LINE__, Normal)
#define CUSTOM_UI_COMMAND_SIG(name) CUSTOM_COMMAND(name, __FILE__, __LINE__, UI)
#define CUSTOM_DOC(str) CUSTOM_DOC(str)
#define CUSTOM_ID(group, name) CUSTOM_ID(group, name)
#endif


struct User_Input{
    Input_Event event;
    b32 abort;
};


typedef i32 Hook_ID;
enum{
    HookID_Tick,
    HookID_RenderCaller,
    HookID_WholeScreenRenderCaller,
    HookID_DeltaRule,
    HookID_BufferViewerUpdate,
    HookID_ViewEventHandler,
    HookID_BufferNameResolver,
    HookID_BeginBuffer,
    HookID_EndBuffer,
    HookID_NewFile,
    HookID_SaveFile,
    HookID_BufferEditRange,
    HookID_BufferRegion,
    HookID_Layout,
    HookID_ViewChangeBuffer,
};


typedef i32 Hook_Function(App *app);
#define HOOK_SIG(name) i32 name(App *app)


struct Buffer_Name_Conflict_Entry{
    Buffer_ID buffer_id;
    String_Const_u8 file_name;
    String_Const_u8 base_name;
    u8 *unique_name_in_out;
    u64 unique_name_len_in_out;
    u64 unique_name_capacity;
};


typedef void Buffer_Name_Resolver_Function(App *app, Buffer_Name_Conflict_Entry *conflicts, i32 conflict_count);
#define BUFFER_NAME_RESOLVER_SIG(n) void n(App *app, Buffer_Name_Conflict_Entry *conflicts, i32 conflict_count)


typedef i32 Buffer_Hook_Function(App *app, Buffer_ID buffer_id);
#define BUFFER_HOOK_SIG(name) i32 name(App *app, Buffer_ID buffer_id)


typedef i32 Buffer_Edit_Range_Function(App *app, Buffer_ID buffer_id,
                                       Range_i64 new_range, Range_Cursor old_range);
#define BUFFER_EDIT_RANGE_SIG(name) i32 name(App *app, Buffer_ID buffer_id, Range_i64 new_range, Range_Cursor old_cursor_range)


typedef Vec2_f32 Delta_Rule_Function(Vec2_f32 pending, b32 is_new_target, f32 dt, void *data);
#define DELTA_RULE_SIG(name) Vec2_f32 name(Vec2_f32 pending, b32 is_new_target, f32 dt, void *data)


typedef Rect_f32 Buffer_Region_Function(App *app, View_ID view_id, Rect_f32 region);


typedef void New_Clipboard_Contents_Function(App *app, String_Const_u8 contents);
#define NEW_CLIPBOARD_CONTENTS_SIG(name) void name(App *app, String_Const_u8 contents)


typedef void Tick_Function(App *app, Frame_Info frame_info);


typedef void Render_Caller_Function(App *app, Frame_Info frame_info, View_ID view);


typedef void Whole_Screen_Render_Caller_Function(App *app, Frame_Info frame_info);


typedef void View_Change_Buffer_Function(App *app, View_ID view_id,
                                         Buffer_ID old_buffer_id, Buffer_ID new_buffer_id);


typedef u32 Layout_Item_Flag;
enum{
    LayoutItemFlag_Special_Character = (1 << 0),
    LayoutItemFlag_Ghost_Character = (1 << 1)
};


struct Layout_Item{
    i64 index;
    u32 codepoint;
    Layout_Item_Flag flags;
    Rect_f32 rect;
    f32 padded_y1;
};


struct Layout_Item_Block{
    Layout_Item_Block *next;
    Layout_Item *items;
    i64 item_count;
    i64 character_count;
    Face_ID face;
};


struct Layout_Item_List{
    Layout_Item_Block *first;
    Layout_Item_Block *last;
    i64 item_count;
    i64 character_count;
    i32 node_count;
    f32 height;
    f32 bottom_padding;
    Range_i64 input_index_range;
    Range_i64 manifested_index_range;
};


typedef Layout_Item_List Layout_Function(App *app, Arena *arena, Buffer_ID buffer, Range_i64 range, Face_ID face, f32 width);


struct View_Context{
    Render_Caller_Function *render_caller;
    Delta_Rule_Function *delta_rule;
    u64 delta_rule_memory_size;
    b32 hides_buffer;
    struct Mapping *mapping;
    i64 map_id;
};


typedef u32 String_Match_Flag;
enum{
    StringMatch_CaseSensitive = 1,
    StringMatch_LeftSideSloppy = 2,
    StringMatch_RightSideSloppy = 4,
    StringMatch_Straddled = 8,
};


struct String_Match{
    String_Match *next;
    Buffer_ID buffer;
    i32 string_id;
    String_Match_Flag flags;
    Range_i64 range;
};


struct String_Match_List{
    String_Match *first;
    String_Match *last;
    i32 count;
};


struct Process_State{
    b32 valid;
    b32 is_updating;
    i64 return_code;
};

#endif
