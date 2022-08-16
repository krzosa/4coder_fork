/*
* Default color slots
*/

// TOP

function Color_Array
finalize_color_array(Color_Table table, u64 id){
    return(table.arrays[id % table.count]);
}

function ARGB_Color
finalize_color(Color_Array array, i32 sub_index){
    ARGB_Color result = 0xFFFFFFFF;
    if (array.count > 0){
        result = array.vals[sub_index % array.count];
    }
    return(result);
}

function ARGB_Color
finalize_color(Color_Table color_table, u64 id, i32 sub_index){
    Color_Array array = finalize_color_array(color_table, id);
    return(finalize_color(array, sub_index));
}

function Color_Array
finalize_color_array(u64 id){
	return(finalize_color_array(active_color_table, id));
}

function ARGB_Color
finalize_color(u64 id, i32 sub_index){
	return(finalize_color(active_color_table, id, sub_index));
}

////////////////////////////////

function Color_Array
make_colors(Arena *arena, ARGB_Color color){
    Color_Array result = {};
    result.count = 1;
    result.vals = push_array(arena, ARGB_Color, 1);
    result.vals[0] = color;
    return(result);
}

function Color_Array
make_colors(Arena *arena, ARGB_Color c1, ARGB_Color c2){
    Color_Array result = {};
    result.count = 2;
    result.vals = push_array(arena, ARGB_Color, 2);
    result.vals[0] = c1;
    result.vals[1] = c2;
    return(result);
}

function Color_Array
make_colors(Arena *arena, ARGB_Color c1, ARGB_Color c2, ARGB_Color c3){
    Color_Array result = {};
    result.count = 3;
    result.vals = push_array(arena, ARGB_Color, 3);
    result.vals[0] = c1;
    result.vals[1] = c2;
    result.vals[2] = c3;
    return(result);
}

function Color_Array
make_colors(Arena *arena, ARGB_Color c1, ARGB_Color c2, ARGB_Color c3, ARGB_Color c4){
    Color_Array result = {};
    result.count = 4;
    result.vals = push_array(arena, ARGB_Color, 4);
    result.vals[0] = c1;
    result.vals[1] = c2;
    result.vals[2] = c3;
    result.vals[3] = c4;
    return(result);
}

function Color_Array
make_colors(Arena *arena, ARGB_Color *colors, i32 count){
    Color_Array result = {};
    result.count = count;
    result.vals = push_array_write(arena, ARGB_Color, count, colors);
    return(result);
}

function Color_Table
make_color_table(App *app, Arena *arena){
    Managed_ID highest_color_id = managed_id_group_highest_id(app, string_u8_litexpr("colors"));
    Color_Table result = {};
    result.count = (u32)(clamp_top(highest_color_id + 1, max_u32));
    result.arrays = push_array(arena, Color_Array, result.count);
    u32 *dummy = push_array(arena, u32, 1);
    *dummy = 0xFF990099;
    for (i32 i = 0; i < result.count; i += 1){
        result.arrays[i].vals = dummy;
        result.arrays[i].count = 1;
    }
    return(result);
}

function void
set_default_color_scheme(App *app){
    if (global_theme_arena.base_allocator == 0){
        global_theme_arena = make_arena_system();
    }

    Arena *arena = &global_theme_arena;

    default_color_table = make_color_table(app, arena);

    u32 background=0xff1E1E1E;
    // u32 background_darker=0xff0E0E0E;
    u32 background_ligher=0xff9E9E9E;

    u32 caret=0xffDCDCDC;
    u32 foreground=0xffDCDCDC;
<<<<<<< Updated upstream:4coder_default_colors.cpp
    // u32 invisibles=0xFFFFFF40;
=======
    u32 invisibles=0xFFFFFF40;
>>>>>>> Stashed changes:custom/4coder_default_colors.cpp
    u32 line_highlight=0xFF0F0F0F;
    u32 selection=0xFF264F78;
    u32 comment=0xFF608B4E;
    // u32 variable=0xFFDCDCDC;
    u32 keyword=0xFF569CD6;
    u32 test=0xFFd6c556;

    // u32 vs_bar=0xFF0E70C0;
    u32 border=0xFF333337;
    u32 number=0xFFB5CEA8;
    u32 string=0xFFD69D85;
    u32 preproc=0xFF9B9B9B;
    // u32 macro=0xFFc586c0;
    u32 type=0xFF4EC9B0;
    u32 green=0xFFA6E22E;
    u32 purple=0xFF967EFB;
    u32 red=0xFFA72822;

    default_color_table.arrays[0] = make_colors(arena, 0xFF90B080);
    default_color_table.arrays[defcolor_bar] = make_colors(arena, border);
    default_color_table.arrays[defcolor_base] = make_colors(arena, foreground);
    default_color_table.arrays[defcolor_pop1] = make_colors(arena, foreground);
    default_color_table.arrays[defcolor_pop2] = make_colors(arena, keyword);
    default_color_table.arrays[defcolor_back] = make_colors(arena, background);
    default_color_table.arrays[defcolor_margin] = make_colors(arena, background);
    default_color_table.arrays[defcolor_margin_hover] = make_colors(arena, 0xffff0000); // ?
    default_color_table.arrays[defcolor_margin_active] = make_colors(arena, background_ligher);
    default_color_table.arrays[defcolor_list_item] = make_colors(arena, defcolor_margin, defcolor_back);
    default_color_table.arrays[defcolor_list_item_hover] = make_colors(arena, 0x22ffffff, 0x22ffffff);
    default_color_table.arrays[defcolor_list_item_active] = make_colors(arena, 0x33ffffff, 0x33ffffff);
    default_color_table.arrays[defcolor_cursor] = make_colors(arena, caret, red);
    default_color_table.arrays[defcolor_at_cursor] = make_colors(arena, 0xff000000);
    default_color_table.arrays[defcolor_highlight_cursor_line] = make_colors(arena, line_highlight);
    default_color_table.arrays[defcolor_highlight] = make_colors(arena, 0x11264F78);
    default_color_table.arrays[defcolor_at_highlight] = make_colors(arena, selection);
    default_color_table.arrays[defcolor_mark] = make_colors(arena, caret);
    default_color_table.arrays[defcolor_text_default] = make_colors(arena, foreground);
    default_color_table.arrays[defcolor_comment] = make_colors(arena, comment);
    default_color_table.arrays[defcolor_comment_pop] = make_colors(arena, keyword, keyword);
    default_color_table.arrays[defcolor_keyword] = make_colors(arena, keyword);
    default_color_table.arrays[defcolor_function] = make_colors(arena, type);
    default_color_table.arrays[defcolor_macro] = make_colors(arena, type);
    default_color_table.arrays[defcolor_type] = make_colors(arena, type);
    default_color_table.arrays[defcolor_str_constant] = make_colors(arena, string);
    default_color_table.arrays[defcolor_char_constant] = make_colors(arena, string);
    default_color_table.arrays[defcolor_int_constant] = make_colors(arena, number);
    default_color_table.arrays[defcolor_float_constant] = make_colors(arena, number);
    default_color_table.arrays[defcolor_bool_constant] = make_colors(arena, keyword);
    default_color_table.arrays[defcolor_preproc] = make_colors(arena, preproc);
    default_color_table.arrays[defcolor_include] = make_colors(arena, foreground);
    default_color_table.arrays[defcolor_special_character] = make_colors(arena, foreground);
    default_color_table.arrays[defcolor_ghost_character] = make_colors(arena, foreground);
    default_color_table.arrays[defcolor_highlight_junk] = make_colors(arena, 0xFF3A0000); // ?
    default_color_table.arrays[defcolor_highlight_white] = make_colors(arena, 0xFF003A3A); // ?
    default_color_table.arrays[defcolor_paste] = make_colors(arena, 0xFFDDEE00);
    default_color_table.arrays[defcolor_undo] = make_colors(arena, 0xFF00DDEE);
    default_color_table.arrays[defcolor_back_cycle] = make_colors(arena, 0x0C4EC9B0, 0x08569CD6, 0x080000A0, 0x08A0A000);
    default_color_table.arrays[defcolor_text_cycle] = make_colors(arena, string, type, green, purple);
    default_color_table.arrays[defcolor_line_numbers_back] = make_colors(arena, defcolor_back);
    default_color_table.arrays[defcolor_line_numbers_text] = make_colors(arena, defcolor_base);

    active_color_table = default_color_table;
}

////////////////////////////////

function void
set_active_color(Color_Table *table){
    if (table != 0){
        active_color_table = *table;
    }
}

// TODO(allen): Need to make this nicer.
function void
set_single_active_color(u64 id, ARGB_Color color){
    active_color_table.arrays[id] = make_colors(&global_theme_arena, color);
}

function void
save_theme(Color_Table table, String_Const_u8 name){
    Color_Table_Node *node = push_array(&global_theme_arena, Color_Table_Node, 1);
    sll_queue_push(global_theme_list.first, global_theme_list.last, node);
    global_theme_list.count += 1;
    node->name = push_string_copy(&global_theme_arena, name);
    node->table = table;
}

////////////////////////////////

function Color_Table*
get_color_table_by_name(String_Const_u8 name){
    Color_Table *result = 0;
    for (Color_Table_Node *node = global_theme_list.first;
         node != 0;
         node = node->next){
        if (string_match(node->name, name)){
            result = &node->table;
            break;
        }
    }
    return(result);
}

// BOTTOM
