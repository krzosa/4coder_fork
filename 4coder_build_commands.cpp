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
        b32 auto_save = config_automatically_save_changes_on_build;
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


//
// TODO(Krzosa): We want this to be configurable!!
// TODO(Krzosa): Probably want some injected into code variables like file
// TODO(Krzosa): Maybe add something like presets? you could bind a command into a number
//               then you would specify it using something like #1
// TODO(Krzosa): Probably would want also to add something like precise commands
//               so you could specify a command to run it inside the comment
//
struct Python_Eval_Data{
    Range_i64 range_to_modify;
    Buffer_ID buffer_to_modify;
};
struct Python_Changed_Buffer{
    Buffer_ID buffer;
    i64 change_size;
    i64 change_pos;
};
global Python_Changed_Buffer python_changed_buffer_data[1024];
global i64 python_buffer_data_count;

Child_Process_End_Sig(python_eval_callback){
    Scratch_Block scratch(app);
    Python_Eval_Data *py = (Python_Eval_Data *)data;

    String_Const_u8 string = push_whole_buffer(app, scratch, buffer);
    string = push_stringf(scratch, "\n%.*s/*END*/", string_expand(string));

    // NOTE(Krzosa): Modify the range to take into account the previous changes
    Assert(python_buffer_data_count < ArrayCount(python_changed_buffer_data));
    for(int i = 0; i < python_buffer_data_count; i++){
        Python_Changed_Buffer *b = python_changed_buffer_data + i;
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

    Assert(python_buffer_data_count + 1 < ArrayCount(python_changed_buffer_data));
    Python_Changed_Buffer *b = python_changed_buffer_data + python_buffer_data_count++;
    b->buffer = py->buffer_to_modify;
    b->change_pos = modified_range.min;
    b->change_size = range_size(modified_range) - range_size(py->range_to_modify);

    heap_free(&global_heap, data);
}

function void
create_eval_process_and_set_a_callback_to_insert_code_block(App *app, Arena *scratch, int id, Buffer_ID buffer_with_code, Range_i64 code_range, Range_i64 range_to_modify){
    //
    // Dump comment code to file
    //
    String_Const_u8 string = push_buffer_range(app, scratch, buffer_with_code, code_range);

    // Skip the run all comments marker
    if(string.size && string.str[0] == '#'){
        string.str  += 1;
        string.size -= 1;
    }

    String8 ext = config_comment_runner_filename_extension;
    String8 buffer_name = push_stringf(scratch, "__buffer_gen%d.%.*s", id, string_expand(ext));
    String8 name = push_stringf(scratch, "__gen%d.%.*s", id, string_expand(ext));
    String8 dir = push_hot_directory(app, scratch);
    String8 file = push_stringf(scratch, "%.*s/%.*s\0", string_expand(dir), string_expand(name));
    system_save_file(scratch, (char *)file.str, string);

    String8 command_to_run = config_comment_runner_command;
    command_to_run = string_replace(scratch, command_to_run, string_u8_litexpr("{file}"), file);
    command_to_run = string_replace(scratch, command_to_run, string_u8_litexpr("{id}"), push_stringf(scratch, "%d", id));

    //
    // Call python with the code
    //
    Python_Eval_Data *py = (Python_Eval_Data *)heap_allocate(&global_heap, sizeof(Python_Eval_Data));
    py->range_to_modify = range_to_modify;
    py->buffer_to_modify = buffer_with_code;

    print_message(app, command_to_run);
    exec_system_command(app, global_compilation_view, {(char *)buffer_name.str, (i32)buffer_name.size}, dir, command_to_run, CLI_SendEndSignal|CLI_OverlapWithConflict, python_eval_callback, py);
}

CUSTOM_COMMAND_SIG(python_interpreter_on_comment)
CUSTOM_DOC("Call python interpreter 'python' and feed it text inside a comment")
{
    Scratch_Block scratch(app);
    Active_View_Info a = get_active_view_info(app, Access_ReadWriteVisible);
    python_buffer_data_count = 0;

    Range_i64 range = {};
    seek_string_backward(app, a.buffer, a.cursor.pos, 0, string_u8_litexpr("/*"), &range.min);
    seek_string_forward(app, a.buffer, a.cursor.pos, 0, string_u8_litexpr("*/"), &range.max);
    range.min += 2;

    Range_i64 mod_range = {range.max+2, range.max+2};
    String_Match end_block_match = buffer_seek_string(app, a.buffer, string_u8_litexpr("/*END*/"), Scan_Forward, a.cursor.pos);
    String_Match next_comment_match = buffer_seek_string(app, a.buffer, string_u8_litexpr("/*"), Scan_Forward, a.cursor.pos);
    if(string_match_found(end_block_match)){
        if(string_match_found(next_comment_match)){
            if(end_block_match.range.min == next_comment_match.range.min){
                mod_range.max = end_block_match.range.max;
            }
        }
    }

    create_eval_process_and_set_a_callback_to_insert_code_block(app, scratch, 0, a.buffer, range, mod_range);
}

CUSTOM_COMMAND_SIG(python_interpreter_on_all_marked_comments)
CUSTOM_DOC("Run python interpreter on all comments that start with /*#py")
{
    Scratch_Block scratch(app);

    int code_file_id = 0;
    python_buffer_data_count = 0; // Zero global
    for (Buffer_ID buffer = get_buffer_next(app, 0, Access_Always); buffer != 0; buffer = get_buffer_next(app, buffer, Access_Always)){
        i32 seek_pos = 0;
        for(;;){
            //
            // Find the 2 ranges: one with code to run, one optional with previous output
            //
            Range_i64 code_range  = {};
            Range_i64 gen_range   = {};

            String_Match code_begin = buffer_seek_string(app, buffer, string_u8_litexpr("/*#"), Scan_Forward, seek_pos);
            if(string_match_found(code_begin)){
                seek_pos = code_begin.range.max;
                const i32 comment_token_size = 3;
                code_range.min = code_begin.range.min + comment_token_size;
            }
            else break; // No more code sections, go next

            String_Match code_end = buffer_seek_string(app, buffer, string_u8_litexpr("*/"), Scan_Forward, seek_pos);
            if(string_match_found(code_end)){
                seek_pos = code_end.range.max;
                code_range.max = code_end.range.min;
            }
            else{ // Unclose comment go next buffer, post error
                String8 buffer_name = push_buffer_file_name(app, scratch, buffer);
                Buffer_Cursor cursor = buffer_compute_cursor(app, buffer, {seek_pos, seek_pos});
                String8 err = push_stringf(scratch, "Unclosed code comment in file %.*s:%d", string_expand(buffer_name), (int)cursor.col);
                print_message(app, err);
                // TODO(Krzosa): Add error messages
                break;
            }

            gen_range = {code_end.range.max, code_end.range.max};
            String_Match gen_end = buffer_seek_string(app, buffer, string_u8_litexpr("/*END*/"), Scan_Forward, seek_pos);
            if(string_match_found(gen_end)){
                //
                // Should make sure that this generated block belongs to this code comment
                //
                b32 gen_block_belongs_to_the_code_block = false;
                String_Match next_code = buffer_seek_string(app, buffer, string_u8_litexpr("/*"), Scan_Forward, seek_pos);
                if(string_match_found(next_code)){
                    if(gen_end.range.min == next_code.range.min){
                        gen_block_belongs_to_the_code_block = true;
                    }
                }
                else gen_block_belongs_to_the_code_block = true;

                if(gen_block_belongs_to_the_code_block){
                    gen_range.max = gen_end.range.max;
                    seek_pos = gen_range.max;
                }
            }

            create_eval_process_and_set_a_callback_to_insert_code_block(app, scratch, code_file_id++, buffer, code_range, gen_range);
        }
    }
}


// BOTTOM

