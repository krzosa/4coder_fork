/*
4coder_clipboard.cpp - Copy paste commands and clipboard related setup.
*/

// TOP

CUSTOM_COMMAND_SIG(clipboard_record_clip)
CUSTOM_DOC("In response to a new clipboard contents events, saves the new clip onto the clipboard history")
{
    User_Input in = get_current_input(app);
    if (in.event.kind == InputEventKind_Core && in.event.core.code == CoreCode_NewClipboardContents){
        clipboard0.clip = in.event.core.string;
    }
}

////////////////////////////////

function void
try_initializing_clipboard_and_clear(){
    if(clipboard0.arena.base_allocator == 0){
        Base_Allocator *a = get_base_allocator_system();
        clipboard0.arena = make_arena(a, KB(8), 8);
    }
    linalloc_clear(&clipboard0.arena);
}

function b32
clipboard_post_buffer_range(App *app, Buffer_ID buffer, Range_i64 range){
    try_initializing_clipboard_and_clear();

    b32 success = false;
    String_Const_u8 string = push_buffer_range(app, &clipboard0.arena, buffer, range);
    if (string.size > 0){
        system_post_clipboard(string, 0); // TODO(Krzosa): 0 seems to be unused in the future would be nice to clean that up in OS layer
        clipboard0.clip = string;
        success = true;
    }
    return(success);
}

function b32
clipboard_update_history_from_system(App *app){
    b32 result = false;

    Scratch_Block scratch(app);
    String_Const_u8 string = system_get_clipboard(scratch, 0);
    if (string.str != 0){
        try_initializing_clipboard_and_clear();
        clipboard0.clip = push_string_copy(&clipboard0.arena, string);
        result = true;
    }
    return(result);
}

CUSTOM_COMMAND_SIG(copy)
CUSTOM_DOC("Copy the text in the range from the cursor to the mark onto the clipboard.")
{
    View_ID view = get_active_view(app, Access_ReadVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
    Range_i64 range = get_view_range(app, view);
    clipboard_post_buffer_range(app, buffer, range);
}

CUSTOM_COMMAND_SIG(cut)
CUSTOM_DOC("Cut the text in the range from the cursor to the mark onto the clipboard.")
{
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    Range_i64 range = get_view_range(app, view);
    if (clipboard_post_buffer_range(app, buffer, range)){
        buffer_replace_range(app, buffer, range, string_u8_empty);
    }
}

CUSTOM_COMMAND_SIG(cut_lines)
CUSTOM_DOC("Cut selected lines of text")
{
    Active_View_Info a = get_active_view_info(app, Access_ReadWriteVisible);
    if (clipboard_post_buffer_range(app, a.buffer, a.cursor_to_mark_lines_pos_range)){
        buffer_replace_range(app, a.buffer, a.cursor_to_mark_lines_pos_range, string_u8_empty);
    }
}

CUSTOM_COMMAND_SIG(delete_lines)
CUSTOM_DOC("Cut selected lines of text")
{
    Active_View_Info a = get_active_view_info(app, Access_ReadWriteVisible);
    buffer_replace_range(app, a.buffer, a.cursor_to_mark_lines_pos_range, string_u8_empty);
}

CUSTOM_COMMAND_SIG(paste)
CUSTOM_DOC("At the cursor, insert the text at the top of the clipboard.")
{
    clipboard_update_history_from_system(app);


    View_ID view = get_active_view(app, Access_ReadWriteVisible);

    Scratch_Block scratch(app);

    String_Const_u8 string = clipboard0.clip;
    if (string.size > 0){
        Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);

        i64 pos = view_get_cursor_pos(app, view);
        buffer_replace_range(app, buffer, Ii64(pos), string);
        view_set_mark(app, view, seek_pos(pos));
        view_set_cursor_and_preferred_x(app, view, seek_pos(pos + (i32)string.size));

        ARGB_Color argb = theme_paste;
        buffer_post_fade(app, buffer, 0.667f, Ii64_size(pos, string.size), argb);
    }
}

CUSTOM_COMMAND_SIG(paste_and_indent)
CUSTOM_DOC("Paste from the top of clipboard and run auto-indent on the newly pasted text.")
{
    paste(app);
    auto_indent_range(app);
}

// BOTTOM

