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

global_const Buffer_Identifier standard_build_build_buffer_identifier = buffer_identifier(string_u8_litexpr("*compilation*"));

global_const u32 standard_build_exec_flags = CLI_OverlapWithConflict|CLI_SendEndSignal;

static void
standard_build_exec_command(App *app, View_ID view, String_Const_u8 dir, String_Const_u8 cmd){
    exec_system_command(app, view, standard_build_build_buffer_identifier,
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

CUSTOM_COMMAND_SIG(build_search)
CUSTOM_DOC("Looks for a build.bat, build.sh, or makefile in the current and parent directories.  Runs the first that it finds and prints the output to *compilation*.")
{
    View_ID view = get_active_view(app, Access_Always);
    Buffer_ID buffer = view_get_buffer(app, view, Access_Always);
    standard_search_and_build(app, view, buffer);
    block_zero_struct(&prev_location);
    lock_jump_buffer(app, string_u8_litexpr("*compilation*"));
}

static Buffer_ID
get_comp_buffer(App *app){
    return(get_buffer_by_name(app, string_u8_litexpr("*compilation*"), Access_Always));
}

static View_ID
get_or_open_build_panel(App *app){
    View_ID view = 0;
    Buffer_ID buffer = get_comp_buffer(app);
    if (buffer != 0){
        view = get_first_view_with_buffer(app, buffer);
    }
    if (view == 0){
        view = open_build_footer_panel(app);
    }
    return(view);
}

function void
set_fancy_compilation_buffer_font(App *app){
    Scratch_Block scratch(app);
    Buffer_ID buffer = get_comp_buffer(app);
    Font_Load_Location font = {};
    font.file_name = def_search_normal_full_path(scratch, str8_lit("fonts/liberation-mono.ttf"));
    set_buffer_face_by_font_load_location(app, buffer, &font);
}

struct View_Buffer{
  View_ID view;
  Buffer_ID buffer;
  Buffer_Identifier buffer_identifier;
};

//
// Make sure we get compilation buffer that is in inactive window always
//
function View_Buffer
get_compilation_buffer(App *app){
  Buffer_ID compilation = get_buffer_by_name(app, string_u8_litexpr("*compilation*"), Access_Always);
  buffer_kill(app, compilation, BufferKill_AlwaysKill);
  Buffer_ID comp_buffer = create_buffer(app, string_u8_litexpr("*compilation*"), BufferCreate_NeverAttachToFile | BufferCreate_AlwaysNew);
  buffer_set_setting(app, comp_buffer, BufferSetting_Unimportant, true);
  buffer_set_setting(app, comp_buffer, BufferSetting_ReadOnly, true);
  Buffer_Identifier identifier = buffer_identifier(string_u8_litexpr("*compilation*"));
  Buffer_ID comp_id = buffer_identifier_to_id(app, identifier);
  View_ID view = get_next_view_after_active(app, Access_Always);
  view_set_buffer(app, view, comp_id, 0);
  return {view, comp_id, identifier};
}


CUSTOM_COMMAND_SIG(build_in_other_panel)
CUSTOM_DOC("Looks for a build.bat, build.sh, or makefile in the current and parent directories.  Runs the first that it finds and prints the output to *compilation*.")
{
    View_ID view = get_active_view(app, Access_Always);
    Buffer_ID buffer = view_get_buffer(app, view, Access_Always);

    View_ID build_view = get_or_open_build_panel(app);

    standard_search_and_build(app, build_view, buffer);
    set_fancy_compilation_buffer_font(app);

    block_zero_struct(&prev_location);
    lock_jump_buffer(app, string_u8_litexpr("*compilation*"));
}


CUSTOM_COMMAND_SIG(build_in_build_panel)
CUSTOM_DOC("Looks for a build.bat, build.sh, or makefile in the current and parent directories.  Runs the first that it finds and prints the output to *compilation*.  Puts the *compilation* buffer in a panel at the footer of the current view.")
{
    View_ID view = get_active_view(app, Access_Always);
    Buffer_ID buffer = view_get_buffer(app, view, Access_Always);

    View_ID build_view = get_compilation_buffer(app).view;

    standard_search_and_build(app, build_view, buffer);
    set_fancy_compilation_buffer_font(app);

    block_zero_struct(&prev_location);
    lock_jump_buffer(app, string_u8_litexpr("*compilation*"));
}

CUSTOM_COMMAND_SIG(close_build_panel)
CUSTOM_DOC("If the special build panel is open, closes it.")
{
    close_build_footer_panel(app);
}

CUSTOM_COMMAND_SIG(change_to_build_panel)
CUSTOM_DOC("If the special build panel is open, makes the build panel the active panel.")
{
    View_ID view = get_or_open_build_panel(app);
    if (view != 0){
        view_set_active(app, view);
    }
}

// BOTTOM

