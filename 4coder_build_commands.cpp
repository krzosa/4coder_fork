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

struct Python_Eval_Data{
    Range_i64 range_to_modify;
    Buffer_ID buffer_to_modify;
};
Child_Process_End_Sig(python_eval_callback){
    Python_Eval_Data *py = (Python_Eval_Data *)data;

    Scratch_Block scratch(app);
    String_Const_u8 string = push_whole_buffer(app, scratch, buffer);

    if(py->range_to_modify.max == py->range_to_modify.min){
        buffer_replace_range(app, py->buffer_to_modify, Ii64(py->range_to_modify.min), string_u8_litexpr("/*END*/"));
    }

    buffer_replace_range(app, py->buffer_to_modify, py->range_to_modify, string);
    heap_free(&global_heap, data);
}
CUSTOM_COMMAND_SIG(python_interpreter_on_comment)
CUSTOM_DOC("Call python interpreter 'python' and feed it text inside a comment, result ends up in clipboard")
{
    Scratch_Block scratch(app);
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);

    Range_i64 range = {};
    i64 cursor = view_get_cursor_pos(app, view);
    seek_string_backward(app, buffer, cursor, 0, string_u8_litexpr("/*"), &range.min);
    seek_string_forward(app, buffer, cursor, 0, string_u8_litexpr("*/"), &range.max);
    range.min += 2;
    String_Const_u8 string = push_buffer_range(app, scratch, buffer, range);

    Range_i64 mod_range = {range.max+2};
    seek_string_forward(app, buffer, cursor, 0, string_u8_litexpr("/*END*/"), &mod_range.max);
    if(mod_range.max >= buffer_get_size(app, buffer)) mod_range.max = mod_range.min;


    String8 dir = push_hot_directory(app, scratch);//get_hot_dsystem_get_path(scratch, SystemPath_UserDirectory);
    String8 file = push_stringf(scratch, "%.*s/%s\0", string_expand(dir), "__python_gen.py");
    system_save_file(scratch, (char *)file.str, string);

    String8 cmd = push_stringf(scratch, "python %.*s\0", string_expand(file));

    Heap *heap = &global_heap;
    Python_Eval_Data *py = (Python_Eval_Data *)heap_allocate(&global_heap, sizeof(Python_Eval_Data));
    py->range_to_modify = mod_range;
    py->buffer_to_modify = buffer;
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

// BOTTOM

