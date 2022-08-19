/*
4coder_build_commands.cpp - Commands for building.
*/

// TOP

static String_Const_u8
push_build_directory_at_file(App *app, Arena *arena, Buffer_ID buffer){
    String_Const_u8 result = {};
    String_Const_u8 file_name = push_buffer_file_name(app, arena, buffer);
    Temp_Memory restore_point = begin_temp(arena);
    String_Const_u8 base_name = push_buffer_base_name(app, arena, buffer);
    b32 is_match = string_match(file_name, base_name);
    end_temp(restore_point);
    if (!is_match){
        result = push_string_copy(arena, string_remove_last_folder(file_name));
    }
    return(result);
}

#if OS_WINDOWS

global String_Const_u8 standard_build_file_name_array[] = {
    str8_lit("build.bat"),
};
global String_Const_u8 standard_build_cmd_string_array[] = {
    str8_lit("build"),
};

#elif OS_LINUX || OS_MAC

global String_Const_u8 standard_build_file_name_array[] = {
    str8_lit("build.sh"),
    str8_lit("Makefile"),
};
global String_Const_u8 standard_build_cmd_string_array[] = {
    str8_lit("build.sh"),
    str8_lit("make"),
};

#else
#error OS needs standard search and build rules
#endif

static String_Const_u8
push_fallback_command(Arena *arena, String_Const_u8 file_name){
    return(push_u8_stringf(arena, "echo could not find %.*s", string_expand(file_name)));
}

static String_Const_u8
push_fallback_command(Arena *arena){
    return(push_fallback_command(arena, standard_build_file_name_array[0]));
}

global_const Buffer_Identifier standard_build_buffer_identifier = buffer_identifier(string_u8_litexpr("*compilation*"));

global_const u32 standard_build_exec_flags = CLI_OverlapWithConflict|CLI_SendEndSignal;

static void
standard_build_exec_command(App *app, View_ID view, String_Const_u8 dir, String_Const_u8 cmd){
    exec_system_command(app, view, standard_build_buffer_identifier,
                        dir, cmd,
                        standard_build_exec_flags);
}

function b32
standard_search_and_build_from_dir(App *app, View_ID view, String_Const_u8 start_dir){
    Scratch_Block scratch(app);

    // NOTE(allen): Search
    String_Const_u8 full_file_path = {};
    String_Const_u8 cmd_string  = {};
    for (i32 i = 0; i < ArrayCount(standard_build_file_name_array); i += 1){
        full_file_path = push_file_search_up_path(app, scratch, start_dir, standard_build_file_name_array[i]);
        if (full_file_path.size > 0){
            cmd_string = standard_build_cmd_string_array[i];
            break;
        }
    }

    b32 result = (full_file_path.size > 0);
    if (result){
        // NOTE(allen): Build
        String_Const_u8 path = string_remove_last_folder(full_file_path);
        String_Const_u8 command = push_u8_stringf(scratch, "\"%.*s/%.*s\"",
                                                  string_expand(path),
                                                  string_expand(cmd_string));
        b32 auto_save = debug_config_automatically_save_changes_on_build;
        if (auto_save){
            save_all_dirty_buffers(app);
        }
        standard_build_exec_command(app, view, path, command);
        print_message(app, push_u8_stringf(scratch, "Building with: %.*s\n",
                                           string_expand(full_file_path)));
    }

    return(result);
}

// NOTE(allen): This searches first using the active file's directory,
// then if no build script is found, it searches from 4coders hot directory.
static void
standard_search_and_build(App *app, View_ID view, Buffer_ID active_buffer){
    Scratch_Block scratch(app);
    b32 did_build = false;
    String_Const_u8 build_dir = push_build_directory_at_file(app, scratch, active_buffer);
    if (build_dir.size > 0){
        did_build = standard_search_and_build_from_dir(app, view, build_dir);
    }
    if (!did_build){
        build_dir = push_hot_directory(app, scratch);
        if (build_dir.size > 0){
            did_build = standard_search_and_build_from_dir(app, view, build_dir);
        }
    }
    if (!did_build){
        standard_build_exec_command(app, view,
                                    push_hot_directory(app, scratch),
                                    push_fallback_command(scratch));
    }
}

CUSTOM_COMMAND_SIG(build_in_build_panel)
CUSTOM_DOC("Looks for a build.bat, build.sh, or makefile in the current and parent directories.  Runs the first that it finds and prints the output to *compilation*.  Puts the *compilation* buffer in a panel at the footer of the current view.")
{
    View_ID view = global_compilation_view;
    Buffer_ID buffer = view_get_buffer(app, view, Access_Always);

    standard_search_and_build(app, view, buffer);
    block_zero_struct(&prev_location);
    lock_jump_buffer(app, string_u8_litexpr("*compilation*"));
}

CUSTOM_COMMAND_SIG(maximize_build_panel)
CUSTOM_DOC("Make it big")
{
    View_ID view = global_compilation_view;
    Buffer_ID buffer = view_get_buffer(app, view, Access_Always);
    Face_ID face_id = get_face_id(app, buffer);
    Face_Metrics metrics = get_face_metrics(app, face_id);

    // TODO(Krzosa): Adjust size to window size
    if(global_compilation_view_maximized){
        view_set_split_pixel_size(app, view, (i32)(metrics.line_height*4.f));
        view_set_active(app, global_compilation_view_before_maximized_selected_view);
    }
    else{
        view_set_split_pixel_size(app, view, (i32)(metrics.line_height*32.f));
        global_compilation_view_before_maximized_selected_view = get_active_view(app, Access_Always);
        view_set_active(app, view);
    }
    global_compilation_view_maximized = !global_compilation_view_maximized;
}

struct Python_Changed_Buffer{
    Buffer_ID buffer;
    i64 change_size;
    i64 change_pos;
};
global Python_Changed_Buffer changed_buffer_data[1024];
global i64 changed_buffer_data_count;

struct Python_Eval_Data{
    Range_i64 range_to_modify;
    Buffer_ID buffer_to_modify;
};
Child_Process_End_Sig(python_eval_callback){
    Python_Eval_Data *py = (Python_Eval_Data *)data;

    Scratch_Block scratch(app);
    String_Const_u8 string = push_whole_buffer(app, scratch, buffer);
    string = push_stringf(scratch, "\n%.*s", string_expand(string));

    if(py->range_to_modify.max == py->range_to_modify.min){
        string = push_stringf(scratch, "%.*s/*END*/", string_expand(string));
    }

    // NOTE(Krzosa): Modify the range to take into account the previous changes
    Assert(changed_buffer_data_count < ArrayCount(changed_buffer_data));
    for(int i = 0; i < changed_buffer_data_count; i++){
        Python_Changed_Buffer *b = changed_buffer_data + i;
        if(b->buffer == py->buffer_to_modify){
            if(py->range_to_modify.min > b->change_pos){
                py->range_to_modify.min += b->change_size;
                py->range_to_modify.max += b->change_size;
            }
        }
    }

    Range_i64 modified_range = {py->range_to_modify.min, py->range_to_modify.min+(i64)string.size};
    global_history_edit_group_begin(app);
    {
        buffer_replace_range(app, py->buffer_to_modify, py->range_to_modify, string);
    }
    global_history_edit_group_end(app);

    Assert(changed_buffer_data_count + 1 < ArrayCount(changed_buffer_data));
    Python_Changed_Buffer *b = changed_buffer_data + changed_buffer_data_count++;
    b->buffer = py->buffer_to_modify;
    b->change_pos = modified_range.min;
    b->change_size = range_size(modified_range);

    heap_free(&global_heap, data);
}
CUSTOM_COMMAND_SIG(python_interpreter_on_comment)
CUSTOM_DOC("Call python interpreter 'python' and feed it text inside a comment, result ends up in clipboard")
{
    Scratch_Block scratch(app);
    Active_View_Info a = get_active_view_info(app, Access_ReadWriteVisible);
    changed_buffer_data_count = 0;

    Range_i64 range = {};
    seek_string_backward(app, a.buffer, a.cursor.pos, 0, string_u8_litexpr("/*"), &range.min);
    seek_string_forward(app, a.buffer, a.cursor.pos, 0, string_u8_litexpr("*/"), &range.max);
    range.min += 2;
    String_Const_u8 string = push_buffer_range(app, scratch, a.buffer, range);

    Range_i64 mod_range = {range.max+2};
    seek_string_forward(app, a.buffer, range.max, 0, string_u8_litexpr("/*END*/"), &mod_range.max);
    if(mod_range.max >= a.buffer_range.max) mod_range.max = mod_range.min;


    String8 dir = push_hot_directory(app, scratch);//get_hot_dsystem_get_path(scratch, SystemPath_UserDirectory);
    String8 file = push_stringf(scratch, "%.*s/%s\0", string_expand(dir), "__python_gen.py");
    system_save_file(scratch, (char *)file.str, string);

    String8 cmd = push_stringf(scratch, "python %.*s\0", string_expand(file));

    Python_Eval_Data *py = (Python_Eval_Data *)heap_allocate(&global_heap, sizeof(Python_Eval_Data));
    py->range_to_modify = mod_range;
    py->buffer_to_modify = a.buffer;
    Child_Process_ID child_process_id = create_child_process(app, dir, cmd, python_eval_callback, py);
    if (child_process_id != 0){
        Buffer_ID buffer = buffer_identifier_to_id_create_out_buffer(app, standard_build_buffer_identifier);
        if (buffer != 0){
            if (set_buffer_system_command(app, child_process_id, buffer, standard_build_exec_flags)){
                view_set_buffer(app, global_compilation_view, buffer, 0);
            }
        }
    }
}

inline b32
string_match_found(String_Match m){
    return m.buffer != 0;
}

CUSTOM_COMMAND_SIG(python_interpreter_on_all_comments_todo)
CUSTOM_DOC("")
{
    Scratch_Block scratch(app);

    String8 dir = push_hot_directory(app, scratch);
    int code_file_id = 0;

    changed_buffer_data_count = 0;
    for (Buffer_ID buffer = get_buffer_next(app, 0, Access_Always);
         buffer != 0;
         buffer = get_buffer_next(app, buffer, Access_Always)){
        i64 seek_pos = 0;
        for(;;){
            String_Match match = buffer_seek_string(app, buffer, string_u8_litexpr("/*#py"), Scan_Forward, (i32)seek_pos);
            if(string_match_found(match)){
                i64 py_seek_pos = match.range.min + 2; // NOTE(Krzosa): Skip comment

                String_Match end_of_comment_match = buffer_seek_string(app, buffer, string_u8_litexpr("*/"), Scan_Forward, (i32)py_seek_pos);
                if(string_match_found(end_of_comment_match)){
                    i64 close_seek_pos = end_of_comment_match.range.min;
                    seek_pos = close_seek_pos;

                    String_Match end_of_generated_block = buffer_seek_string(app, buffer, string_u8_litexpr("/*END*/"), Scan_Forward, (i32)close_seek_pos);
                    i64 end_block_pos = close_seek_pos;
                    if(string_match_found(end_of_generated_block)){
                        end_block_pos = end_of_generated_block.range.min;
                        seek_pos = end_block_pos;
                    }

                    int current_id = code_file_id++;
                    Range_i64 code_range = {py_seek_pos, close_seek_pos};
                    Range_i64 generated_range = {close_seek_pos+2, end_block_pos+2};

                    String8 code = push_buffer_range(app, scratch, buffer, code_range);
                    String8 name = push_stringf(scratch, "__python_gen%d.py", current_id);
                    String8 file = push_stringf(scratch, "%.*s/%.*s\0", string_expand(dir), string_expand(name));
                    system_save_file(scratch, (char *)file.str, code);

                    String8 cmd = push_stringf(scratch, "python %.*s\0", string_expand(file));

                    Buffer_ID out_buffer = create_buffer(app, name, BufferCreate_AlwaysNew|BufferCreate_NeverAttachToFile);
                    if (out_buffer != 0){
                        buffer_set_setting(app, out_buffer, BufferSetting_ReadOnly, true);
                        buffer_set_setting(app, out_buffer, BufferSetting_Unimportant, true);

                        Python_Eval_Data *py = (Python_Eval_Data *)heap_allocate(&global_heap, sizeof(Python_Eval_Data));
                        py->range_to_modify = generated_range;
                        py->buffer_to_modify = buffer;
                        Child_Process_ID child_process_id = create_child_process(app, dir, cmd, python_eval_callback, py);
                        if (child_process_id != 0){
                            if (set_buffer_system_command(app, child_process_id, out_buffer, standard_build_exec_flags)){
                                view_set_buffer(app, global_compilation_view, out_buffer, 0);
                            }
                        }
                    }
                    else{
                        print_message(app, string_u8_litexpr("Failed to create buffer while pythoning"));
                    }
                }
                else{
                    String8 buffer_name = push_buffer_file_name(app, scratch, buffer);
                    Buffer_Cursor cursor = buffer_compute_cursor(app, buffer, {buffer_seek_pos, py_seek_pos});
                    String8 err = push_stringf(scratch, "Unclosed python comment in file %.*s:%d", string_expand(buffer_name),
                                               (int)cursor.col);
                    print_message(app, err);
                    return;
                }
            }
            else{
                break;
            }
        }
    }
}



Child_Process_End_Sig(clang_callback){
    Python_Eval_Data *py = (Python_Eval_Data *)data;


    Scratch_Block scratch(app);
    String_Const_u8 string = push_whole_buffer(app, scratch, buffer);


    if(py->range_to_modify.max == py->range_to_modify.min){
        buffer_replace_range(app, py->buffer_to_modify, Ii64(py->range_to_modify.min), string_u8_litexpr("/*END*/"));
    }

    buffer_replace_range(app, py->buffer_to_modify, py->range_to_modify, string);
    heap_free(&global_heap, data);
}
CUSTOM_COMMAND_SIG(compile_and_run_comment)
CUSTOM_DOC("Call python interpreter 'python' and feed it text inside a comment, result ends up in clipboard")
{
    Scratch_Block scratch(app);
    Active_View_Info a = get_active_view_info(app, Access_ReadWriteVisible);

    Range_i64 range = {};
    seek_string_backward(app, a.buffer, a.cursor.pos, 0, string_u8_litexpr("/*"), &range.min);
    seek_string_forward(app, a.buffer, a.cursor.pos, 0, string_u8_litexpr("*/"), &range.max);
    range.min += 2;
    String_Const_u8 string = push_buffer_range(app, scratch, a.buffer, range);

    Range_i64 mod_range = {range.max+2};
    seek_string_forward(app, a.buffer, range.max, 0, string_u8_litexpr("/*END*/"), &mod_range.max);
    if(mod_range.max >= buffer_get_size(app, a.buffer)) mod_range.max = mod_range.min;


    String8 dir = push_hot_directory(app, scratch);//get_hot_dsystem_get_path(scratch, SystemPath_UserDirectory);
    String8 file = push_stringf(scratch, "%.*s/%s\0", string_expand(dir), "__gen.cpp");
    system_save_file(scratch, (char *)file.str, string);

    // TODO(Krzosa): Not crossplatform
    String8 cmd = push_stringf(scratch, "clang %.*s -Wno-writable-strings -o __gen.exe && __gen.exe\0", string_expand(file));

    Python_Eval_Data *py = (Python_Eval_Data *)heap_allocate(&global_heap, sizeof(Python_Eval_Data));
    py->range_to_modify = mod_range;
    py->buffer_to_modify = a.buffer;
    Child_Process_ID child_process_id = create_child_process(app, dir, cmd, clang_callback, py);
    if (child_process_id != 0){
        Buffer_ID buffer = buffer_identifier_to_id_create_out_buffer(app, standard_build_buffer_identifier);
        if (buffer != 0){
            if (set_buffer_system_command(app, child_process_id, buffer, standard_build_exec_flags)){
                view_set_buffer(app, global_compilation_view, buffer, 0);
            }
        }
    }
}


// BOTTOM

