/*
4coder_default_map.cpp - Instantiate default bindings.
*/

// TOP

function void
setup_default_mapping(Mapping *mapping, i64 global_id, i64 file_id){
    MappingScope();
    SelectMapping(mapping);
    SelectMap(global_id);
    Bind(keyboard_macro_switch_recording , KeyCode_M, KeyCode_Alt);
    Bind(keyboard_macro_replay,           KeyCode_M, KeyCode_Control);
    Bind(change_active_panel,           KeyCode_1, KeyCode_Control);
    Bind(change_active_panel_backwards, KeyCode_2, KeyCode_Control);
    Bind(command_lister,                KeyCode_P, KeyCode_Control, KeyCode_Shift);
    Bind(interactive_switch_buffer,     KeyCode_P, KeyCode_Control);
    Bind(interactive_open_or_new,       KeyCode_O, KeyCode_Control);
    Bind(goto_next_jump,                KeyCode_N, KeyCode_Control);
    Bind(goto_prev_jump,                KeyCode_N, KeyCode_Control, KeyCode_Shift);
    Bind(save_all_dirty_buffers,        KeyCode_S, KeyCode_Control, KeyCode_Shift);
    Bind(quick_swap_buffer,             KeyCode_Tab, KeyCode_Control);
    Bind(jump_to_definition, KeyCode_I, KeyCode_Control);

    // TODO(Krzosa): We want to move this into Fkey bindings as default
    Bind(build_in_build_panel,          KeyCode_F1);

    // Remedy
    Bind(remedy_start_debug,          KeyCode_F5);
    Bind(remedy_stop_debug,          KeyCode_F5, KeyCode_Shift);

    Bind(remedy_open_file_at_cursor,    KeyCode_F8);
    Bind(remedy_breakpoint_at_cursor,    KeyCode_F9);
    Bind(remedy_remove_breakpoint_at_cursor,    KeyCode_F9, KeyCode_Shift);
    Bind(remedy_run_to_cursor,          KeyCode_F10);

    Bind(maximize_build_panel, KeyCode_Escape);


    Bind(open_in_other,                 KeyCode_O, KeyCode_Alt);
    Bind(jump_to_last_point,            KeyCode_Q, KeyCode_Alt);
    Bind(exit_4coder,          KeyCode_F4, KeyCode_Alt);

    SelectMap(file_id);
    Bind(evaluate_comment, KeyCode_L, KeyCode_Control);
    Bind(evaluate_all_marked_comments, KeyCode_L, KeyCode_Control, KeyCode_Shift);

    Bind(put_new_line_below,  KeyCode_Return, KeyCode_Control);
    Bind(move_line_up, KeyCode_Up, KeyCode_Alt);
    Bind(move_line_down, KeyCode_Down, KeyCode_Alt);
    Bind(duplicate_line_up, KeyCode_Up, KeyCode_Alt, KeyCode_Shift);
    Bind(duplicate_line_down, KeyCode_Down, KeyCode_Alt, KeyCode_Shift);
    Bind(redo_last_command_search_backward, KeyCode_Comma, KeyCode_Control, KeyCode_Shift);
    Bind(redo_last_command_search_forward, KeyCode_Comma, KeyCode_Control);
    Bind(redo_last_command_forward, KeyCode_Period, KeyCode_Control);
    Bind(redo_last_command_backward, KeyCode_Period, KeyCode_Control, KeyCode_Shift);

    Bind(delete_char,            KeyCode_Delete);
    Bind(backspace_char,         KeyCode_Backspace);
    Bind(move_up,                KeyCode_Up);
    Bind(move_down,              KeyCode_Down);
    Bind(move_left,              KeyCode_Left);
    Bind(move_right,             KeyCode_Right);
    Bind(seek_end_of_line,       KeyCode_End);
    Bind(seek_beginning_of_line, KeyCode_Home);
    Bind(page_up,                KeyCode_PageUp);
    Bind(page_down,              KeyCode_PageDown);
    Bind(goto_beginning_of_file, KeyCode_PageUp, KeyCode_Control);
    Bind(goto_end_of_file,       KeyCode_PageDown, KeyCode_Control);
    Bind(move_up_to_blank_line_end,        KeyCode_Up, KeyCode_Control);
    Bind(move_down_to_blank_line_end,      KeyCode_Down, KeyCode_Control);
    Bind(move_left_whitespace_boundary,    KeyCode_Left, KeyCode_Control);
    Bind(move_right_whitespace_boundary,   KeyCode_Right, KeyCode_Control);
    Bind(backspace_alpha_numeric_boundary, KeyCode_Backspace, KeyCode_Control);
    Bind(delete_alpha_numeric_boundary,    KeyCode_Delete, KeyCode_Control);
    Bind(snipe_backward_whitespace_or_token_boundary, KeyCode_Backspace, KeyCode_Alt);
    Bind(snipe_forward_whitespace_or_token_boundary,  KeyCode_Delete, KeyCode_Alt);
    Bind(set_mark,                    KeyCode_Space, KeyCode_Control);
    Bind(replace_in_range,            KeyCode_A, KeyCode_Control);
    Bind(copy,                        KeyCode_C, KeyCode_Control);
    Bind(delete_range,                KeyCode_X, KeyCode_Control, KeyCode_Shift);
    Bind(cut_lines,                 KeyCode_K, KeyCode_Control, KeyCode_Shift);
    Bind(delete_line,                 KeyCode_K, KeyCode_Control);
    Bind(replace_in_buffer, KeyCode_A, KeyCode_Alt);
    Bind(replace_in_all_buffers, KeyCode_H, KeyCode_Control);
    Bind(undo_all_buffers, KeyCode_H, KeyCode_Alt);


    Bind(left_adjust_view,            KeyCode_E, KeyCode_Control, KeyCode_Shift);
    Bind(search,                      KeyCode_F, KeyCode_Control);
    Bind(list_all_locations,          KeyCode_F, KeyCode_Control, KeyCode_Shift);
    Bind(goto_line,                   KeyCode_G, KeyCode_Control);
    Bind(list_all_locations_of_selection,  KeyCode_G, KeyCode_Control, KeyCode_Shift);
    Bind(cursor_mark_swap,            KeyCode_E, KeyCode_Control);
    Bind(reopen,                      KeyCode_O, KeyCode_Control, KeyCode_Shift);

    Bind(query_replace,               KeyCode_R, KeyCode_Control);
    Bind(query_replace_identifier,    KeyCode_R, KeyCode_Alt);
    Bind(save,                        KeyCode_S, KeyCode_Control);
    Bind(save_all_dirty_buffers,      KeyCode_S, KeyCode_Control, KeyCode_Shift);
    Bind(search_identifier,           KeyCode_F, KeyCode_Alt);
    Bind(paste_and_indent,            KeyCode_V, KeyCode_Control);
    Bind(cut,                         KeyCode_X, KeyCode_Control);
    Bind(redo,                        KeyCode_Z, KeyCode_Control, KeyCode_Shift);
    Bind(undo,                        KeyCode_Z, KeyCode_Control);
    Bind(jump_to_hiperlink_at_cursor, KeyCode_Q, KeyCode_Control);
    Bind(jump_to_hiperlink_at_cursor_other_panel, KeyCode_Q, KeyCode_Control, KeyCode_Shift);
    BindMouse(jump_to_hiperlink_at_cursor, MouseCode_Left, KeyCode_Control);
    BindMouse(jump_to_hiperlink_at_cursor_other_panel, MouseCode_Left, KeyCode_Control, KeyCode_Shift);

    Bind(move_left_alpha_numeric_boundary,           KeyCode_Left, KeyCode_Control);
    Bind(move_right_alpha_numeric_boundary,          KeyCode_Right, KeyCode_Control);
    Bind(move_left_alpha_numeric_or_camel_boundary,  KeyCode_Left, KeyCode_Alt);
    Bind(move_right_alpha_numeric_or_camel_boundary, KeyCode_Right, KeyCode_Alt);
    Bind(comment_lines,        KeyCode_ForwardSlash, KeyCode_Control);
    Bind(word_complete,              KeyCode_Tab);
    Bind(auto_indent_line_at_cursor, KeyCode_Tab, KeyCode_Shift);
    Bind(write_note,                 KeyCode_Y, KeyCode_Alt);
    Bind(list_all_locations_of_type_definition,               KeyCode_D, KeyCode_Alt);
    Bind(list_all_locations_of_type_definition_of_identifier, KeyCode_T, KeyCode_Alt, KeyCode_Shift);
    Bind(open_long_braces,           KeyCode_LeftBracket, KeyCode_Control);
    Bind(open_long_braces_semicolon, KeyCode_LeftBracket, KeyCode_Control, KeyCode_Shift);
    Bind(open_long_braces_break,     KeyCode_RightBracket, KeyCode_Control, KeyCode_Shift);
    Bind(select_surrounding_scope,   KeyCode_LeftBracket, KeyCode_Alt);
    Bind(select_surrounding_scope_maximal, KeyCode_LeftBracket, KeyCode_Alt, KeyCode_Shift);
    Bind(select_prev_scope_absolute, KeyCode_RightBracket, KeyCode_Alt);
    Bind(select_prev_top_most_scope, KeyCode_RightBracket, KeyCode_Alt, KeyCode_Shift);
    Bind(select_next_scope_absolute, KeyCode_Quote, KeyCode_Alt);
    Bind(select_next_scope_after_current, KeyCode_Quote, KeyCode_Alt, KeyCode_Shift);
    Bind(place_in_scope,             KeyCode_ForwardSlash, KeyCode_Alt);
    Bind(delete_current_scope,       KeyCode_Minus, KeyCode_Alt);
    Bind(if0_off,                    KeyCode_I, KeyCode_Alt);
    Bind(open_matching_file_cpp,     KeyCode_2, KeyCode_Alt);
    Bind(write_zero_struct,          KeyCode_0, KeyCode_Control);
}

// BOTTOM

