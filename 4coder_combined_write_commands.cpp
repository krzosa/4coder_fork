/*
4coder_combined_write_commands.cpp - Commands for writing text specialized for particular contexts.
*/

// TOP

function void
write_string(App *app, View_ID view, Buffer_ID buffer, String_Const_u8 string){
    i64 pos = view_get_cursor_pos(app, view);
    buffer_replace_range(app, buffer, Ii64(pos), string);
    view_set_cursor_and_preferred_x(app, view, seek_pos(pos + string.size));
}

function void
write_string(App *app, String_Const_u8 string){
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    write_string(app, view, buffer, string);
}

function void
write_named_comment_string(App *app, char *type_string){
    Scratch_Block scratch(app);
    String_Const_u8 name = debug_config_user_name;
    String_Const_u8 str = {};
    if (name.size > 0){
        str = push_u8_stringf(scratch, "// %s(%.*s): ", type_string, string_expand(name));
    }
    else{
        str = push_u8_stringf(scratch, "// %s: ", type_string);
    }
    write_string(app, str);
}

function void
long_braces(App *app, char *text, i32 size){
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    i64 pos = view_get_cursor_pos(app, view);
    buffer_replace_range(app, buffer, Ii64(pos), SCu8(text, size));
    view_set_cursor_and_preferred_x(app, view, seek_pos(pos + 2));
    auto_indent_buffer(app, buffer, Ii64_size(pos, size));
    move_past_lead_whitespace(app, view, buffer);
}

CUSTOM_COMMAND_SIG(open_long_braces)
CUSTOM_DOC("At the cursor, insert a '{' and '}' separated by a blank line.")
{
    char text[] = "{\n\n}";
    i32 size = sizeof(text) - 1;
    long_braces(app, text, size);
}

CUSTOM_COMMAND_SIG(open_long_braces_semicolon)
CUSTOM_DOC("At the cursor, insert a '{' and '};' separated by a blank line.")
{
    char text[] = "{\n\n};";
    i32 size = sizeof(text) - 1;
    long_braces(app, text, size);
}

CUSTOM_COMMAND_SIG(open_long_braces_break)
CUSTOM_DOC("At the cursor, insert a '{' and '}break;' separated by a blank line.")
{
    char text[] = "{\n\n}break;";
    i32 size = sizeof(text) - 1;
    long_braces(app, text, size);
}

CUSTOM_COMMAND_SIG(if0_off)
CUSTOM_DOC("Surround the range between the cursor and mark with an '#if 0' and an '#endif'")
{
    place_begin_and_end_on_own_lines(app, "#if 0", "#endif");
}

CUSTOM_COMMAND_SIG(write_todo)
CUSTOM_DOC("At the cursor, insert a '// TODO' comment, includes user name if it was specified in config.4coder.")
{
    write_named_comment_string(app, "TODO");
}

CUSTOM_COMMAND_SIG(write_hack)
CUSTOM_DOC("At the cursor, insert a '// HACK' comment, includes user name if it was specified in config.4coder.")
{
    write_named_comment_string(app, "HACK");
}

CUSTOM_COMMAND_SIG(write_note)
CUSTOM_DOC("At the cursor, insert a '// NOTE' comment, includes user name if it was specified in config.4coder.")
{
    write_named_comment_string(app, "NOTE");
}

CUSTOM_COMMAND_SIG(write_block)
CUSTOM_DOC("At the cursor, insert a block comment.")
{
    place_begin_and_end_on_own_lines(app, "/* ", " */");
}

CUSTOM_COMMAND_SIG(write_zero_struct)
CUSTOM_DOC("At the cursor, insert a ' = {};'.")
{
    write_string(app, string_u8_litexpr(" = {};"));
}

function i64
get_start_of_line_at_cursor(App *app, View_ID view, Buffer_ID buffer){
    i64 pos = view_get_cursor_pos(app, view);
    i64 line = get_line_number_from_pos(app, buffer, pos);
    return(get_pos_past_lead_whitespace_from_line_number(app, buffer, line));
}

function b32
c_line_comment_starts_at_position(App *app, Buffer_ID buffer, i64 pos){
    b32 alread_has_comment = false;
    u8 check_buffer[2];
    if (buffer_read_range(app, buffer, Ii64(pos, pos + 2), check_buffer)){
        if (check_buffer[0] == '/' && check_buffer[1] == '/'){
            alread_has_comment = true;
        }
    }
    return(alread_has_comment);
}

struct Selected_Lines {
    Buffer_ID buffer;
    View_ID view;

    i64 cursor_pos;
    i64 mark_pos;

    i64 min_pos;
    i64 max_pos;

    i64 min_line;
    i64 max_line;

    // WHOLE selected lines
    Range_i64 entire_selected_lines_pos;
};

function Selected_Lines
get_selected_lines_for_active_view(App *app) {
    Selected_Lines result = {};

    View_ID   view   = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    result.view = view;
    result.buffer = buffer;

    result.cursor_pos = view_get_cursor_pos(app, view);
    result.mark_pos   = view_get_mark_pos(app, view);

    result.min_pos = Min(result.cursor_pos, result.mark_pos);
    result.max_pos = Max(result.cursor_pos, result.mark_pos);

    result.min_line = get_line_number_from_pos(app, buffer, result.min_pos);
    result.max_line = get_line_number_from_pos(app, buffer, result.max_pos);

    i64 min_line = get_line_side_pos(app, buffer, result.min_line, Side_Min);
    i64 max_line = get_line_side_pos(app, buffer, result.max_line, Side_Max);

    result.entire_selected_lines_pos.min = min_line;
    result.entire_selected_lines_pos.max = max_line;

    return result;
}


CUSTOM_COMMAND_SIG(comment_lines)
CUSTOM_DOC("Comment out multiple lines"){
    Selected_Lines lines = get_selected_lines_for_active_view(app);
    if(lines.min_line == lines.max_line) lines.max_line += 1;

    History_Group history_group = history_group_begin(app, lines.buffer);
    i64 first_line = get_pos_past_lead_whitespace_from_line_number(app, lines.buffer, lines.min_line);
    b32 comment_is_first_so_we_should_decomment = c_line_comment_starts_at_position(app, lines.buffer, first_line);

    for(i64 line = lines.min_line; line < lines.max_line; line++){
        i64 line_start = get_pos_past_lead_whitespace_from_line_number(app, lines.buffer, line);
        b32 there_is_a_comment = c_line_comment_starts_at_position(app, lines.buffer, line_start);
        if(!comment_is_first_so_we_should_decomment && !there_is_a_comment){
            buffer_replace_range(app, lines.buffer, Ii64(line_start), string_u8_litexpr("// "));
            buffer_post_fade(app, lines.buffer, 0.667f, Ii64_size(line_start,3), finalize_color(defcolor_paste, 0));
        }
        if(comment_is_first_so_we_should_decomment && there_is_a_comment){
            buffer_replace_range(app, lines.buffer, Ii64_size(line_start,2), string_u8_empty);
        }
    }
    history_group_end(history_group);
}

CUSTOM_COMMAND_SIG(put_new_line_below)
CUSTOM_DOC("Pust a new line bellow cursor line")
{
    Scratch_Block scratch(app);
    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    
    i64 pos = view_get_cursor_pos(app, view);
    i64 line = get_line_number_from_pos(app, buffer, pos) + 1;
    String_Const_u8 s = push_buffer_line(app, scratch, buffer, line);
    s = push_u8_stringf(scratch, "\n");
    pos = get_line_side_pos(app, buffer, line, Side_Min);
    buffer_replace_range(app, buffer, Ii64(pos), s);
    view_set_cursor_and_preferred_x(app, view, seek_line_col(line, 0));
}
////////////////////////////////

static Snippet default_snippets[] = {
    // general (for Allen's style)
    {"if",     "if (){\n\n}\n", 4, 7},
    {"ifelse", "if (){\n\n}\nelse{\n\n}", 4, 7},
    {"forn",   "for (node = ;\nnode != 0;\nnode = node->next){\n\n}\n", 5, 38},
    {"fori",   "for (i = 0; i < ; i += 1){\n\n}\n", 5, 16},
    {"forj",   "for (j = 0; j < ; j += 1){\n\n}\n", 5, 16},
    {"fork",   "for (k = 0; k < ; k += 1){\n\n}\n", 5, 16},
    {"for",    "for (;;){\n\n}\n", 5, 10},
    {"///",    "////////////////////////////////", 32, 32},
    {"#guard", "#if !defined(Z)\n#define Z\n#endif\n", 0, 26},

    {"op+",  "Z\noperator+(Z a, Z b){\n,\n}\n", 0, 23},
    {"op-",  "Z\noperator-(Z a, Z b){\n,\n}\n", 0, 23},
    {"op*",  "Z\noperator*(Z a, Z b){\n,\n}\n", 0, 23},
    {"op/",  "Z\noperator/(Z a, Z b){\n,\n}\n", 0, 23},
    {"op+=", "Z&\noperator+=(Z &a, Z b){\n,\n}\n", 0, 26},
    {"op-=", "Z&\noperator-=(Z &a, Z b){\n,\n}\n", 0, 26},
    {"op*=", "Z&\noperator*=(Z &a, Z b){\n,\n}\n", 0, 26},
    {"op/=", "Z&\noperator/=(Z &a, Z b){\n,\n}\n", 0, 26},

    // for 4coder development
    {"4command", "CUSTOM_COMMAND_SIG()\nCUSTOM_DOC()\n{\n\n}\n", 19, 32},
    {"4app", "App *app", 22, 22},

#if defined(SNIPPET_EXPANSION)
#include SNIPPET_EXPANSION
#endif
};

function void
write_snippet(App *app, View_ID view, Buffer_ID buffer,
              i64 pos, Snippet *snippet){
    if (snippet != 0){
        String_Const_u8 snippet_text = SCu8(snippet->text);
        buffer_replace_range(app, buffer, Ii64(pos), snippet_text);
        i64 new_cursor = pos + snippet->cursor_offset;
        view_set_cursor_and_preferred_x(app, view, seek_pos(new_cursor));
        i64 new_mark = pos + snippet->mark_offset;
        view_set_mark(app, view, seek_pos(new_mark));
        auto_indent_buffer(app, buffer, Ii64_size(pos, snippet_text.size));
    }
}

function Snippet*
get_snippet_from_user(App *app, Snippet *snippets, i32 snippet_count,
                      String_Const_u8 query){
    Scratch_Block scratch(app);
    Lister_Block lister(app, scratch);
    lister_set_query(lister, query);
    lister_set_default_handlers(lister);

    Snippet *snippet = snippets;
    for (i32 i = 0; i < snippet_count; i += 1, snippet += 1){
        lister_add_item(lister, SCu8(snippet->name), SCu8(snippet->text), snippet, 0);
    }
    Lister_Result l_result = run_lister(app, lister);
    Snippet *result = 0;
    if (!l_result.canceled){
        result = (Snippet*)l_result.user_data;
    }
    return(result);
}


function Snippet*
get_snippet_from_user(App *app, Snippet *snippets, i32 snippet_count,
                      char *query){
    return(get_snippet_from_user(app, snippets, snippet_count, SCu8(query)));
}

CUSTOM_UI_COMMAND_SIG(snippet_lister)
CUSTOM_DOC("Opens a snippet lister for inserting whole pre-written snippets of text.")
{
    View_ID view = get_this_ctx_view(app, Access_ReadWrite);
    if (view != 0){
        Snippet *snippet = get_snippet_from_user(app, default_snippets,
                                                 ArrayCount(default_snippets),
                                                 "Snippet:");

        Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
        i64 pos = view_get_cursor_pos(app, view);
        write_snippet(app, view, buffer, pos, snippet);
    }
}

// BOTTOM

