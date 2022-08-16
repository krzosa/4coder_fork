/*
4coder_keyboard_macro.cpp - Keyboard macro recording and replaying commands.
*/

// TOP

function Buffer_ID
get_keyboard_log_buffer(App *app){
    return(get_buffer_by_name(app, string_u8_litexpr("*keyboard*"), Access_Always));
}

function void
keyboard_macro_play_single_line(App *app, String_Const_u8 macro_line){
    Scratch_Block scratch(app);
    Input_Event event = parse_keyboard_event(scratch, macro_line);
    if (event.kind != InputEventKind_None){
        enqueue_virtual_event(app, &event);
    }
}

function void
keyboard_macro_play(App *app, String_Const_u8 macro){
    Scratch_Block scratch(app);
    List_String_Const_u8 lines = string_split(scratch, macro, (u8*)"\n", 1);
    for (Node_String_Const_u8 *node = lines.first;
         node != 0;
         node = node->next){
        String_Const_u8 line = string_skip_chop_whitespace(node->string);
        keyboard_macro_play_single_line(app, line);
    }
}

function b32
get_current_input_is_virtual(App *app){
    User_Input input = get_current_input(app);
    return(input.event.virtual_event);
}

////////////////////////////////

function void
macro_begin_recording(App *app, Range_i64 *range){
    Buffer_ID buffer                          = get_keyboard_log_buffer(app);
    i64           end                         = buffer_get_size(app, buffer);
    Buffer_Cursor cursor                      = buffer_compute_cursor(app, buffer, seek_pos(end));
    Buffer_Cursor back_cursor                 = buffer_compute_cursor(app, buffer, seek_line_col(cursor.line - 1, 1));
    range->one_past_last = back_cursor.pos;
}

function void
macro_end_recording(App *app, Range_i64 *range){
    Buffer_ID buffer = get_keyboard_log_buffer(app);
    range->first     = buffer_get_size(app, buffer);
}

function void
macro_play_recording(App *app, Range_i64 range){
    Buffer_ID buffer = get_keyboard_log_buffer(app);
    Scratch_Block scratch(app);
    String_Const_u8 macro = push_buffer_range(app, scratch, buffer, range);
    keyboard_macro_play(app, macro);
}

CUSTOM_COMMAND_SIG(keyboard_macro_switch_recording)
CUSTOM_DOC("Record a key sequence for later playback") {
    if (get_current_input_is_virtual(app))
        return;
    if (global_keyboard_macro_is_recording) {
        global_keyboard_macro_is_recording = false;
        macro_begin_recording(app, &global_keyboard_macro_range);
    } else {
        global_keyboard_macro_is_recording = true;
        macro_end_recording(app, &global_keyboard_macro_range);
    }
}

CUSTOM_COMMAND_SIG(keyboard_macro_replay)
CUSTOM_DOC("Playback a recorded key sequence")
{
    if (global_keyboard_macro_is_recording || get_current_input_is_virtual(app)){
        return;
    }
    macro_play_recording(app, global_keyboard_macro_range);
}

// BOTTOM

