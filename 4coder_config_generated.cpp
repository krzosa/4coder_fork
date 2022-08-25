/*#
#define _CRT_SECURE_NO_WARNINGS
#include "4coder_base_types.h"
#include "4coder_token.h"
#include "generated/lexer_cpp.h"

#include "4coder_base_types.cpp"
#include "4coder_stringf.cpp"
#include "4coder_malloc_allocator.cpp"

#include "4coder_token.cpp"
#include "generated/lexer_cpp.cpp"

#include "4coder_file.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef u32 Face_Antialiasing_Mode;
enum{
    FaceAntialiasingMode_8BitMono,
    FaceAntialiasingMode_1BitMono,
};

#include "4coder_config_parse.cpp"

int main(){
  Arena arena_ = make_arena_malloc(MB(1), 8);
  Arena *arena = &arena_;

  String_Const_u8 text = file_dump(arena, "config.txt");
  Loaded_Config config = parse_config(arena, arena, text);
  Config_Value *values = config.values;
  i64 values_count = config.count;

  String8 section_loaders[] = {
    string_u8_litexpr("config"),
    string_u8_litexpr("theme"),
  };

  for(int i = 0; i < values_count; i++){
    Config_Value *v = values + i;
    if(!string_match(v->section, string_u8_litexpr("binding"))){
      continue;
    }

    printf("%.*s ", string_expand(v->type));
    printf("%.*s_%.*s[] = {", string_expand(v->section), string_expand(v->name));
    for(int j = 0; j < v->value_str_count; j++){
      printf("string_u8_litexpr(\"%.*s\"),", string_expand(v->value_str[j]));
    }
    printf("};\n");
  }
  printf("\n");

  for(i32 i = 0; i < values_count; i++){
    Config_Value *v = values + i;

    if(string_match(v->section, string_u8_litexpr("binding"))){
      continue;
    }

    b32 is_string = false;
    if(string_match(string_u8_litexpr("String8"), v->type)) is_string = true;

    printf("%.*s ", string_expand(v->type));
    printf("%.*s_%.*s = ", string_expand(v->section), string_expand(v->name));
    if(is_string == true) printf("string_u8_litexpr(\"");
    printf("%.*s", string_expand(v->value_str[0]));
    if(is_string == true) printf("\")");
    printf(";\n");
  }

  printf("function void\nset_config_value(Config_Value *record){\n");
  for(int section_index = 0; section_index < ArrayCount(section_loaders); section_index++){
    String8 section = section_loaders[section_index];

    printf("if(string_match(string_u8_litexpr(\"%.*s\"), record->section)){\n", string_expand(section));
    for(i32 i = 0; i < values_count; i++){
      Config_Value *v = values + i;
      if(!string_match(section, v->section)) continue;

      printf("\nif(string_match(record->name, string_u8_litexpr(\"%.*s\"))){\n", string_expand(v->name));
      if(string_match(v->section, string_u8_litexpr("theme"))){
        printf("Vec4_f32 color = unpack_color(record->%.*s);\n", string_expand(v->value_specifier));
        printf("color.r = powf(color.r, 2.2f);\n");
        printf("color.g = powf(color.g, 2.2f);\n");
        printf("color.b = powf(color.b, 2.2f);\n");
        printf("u32 packed_color = pack_color(color);\n");

        printf("%.*s_%.*s = packed_color;\n", string_expand(v->section), string_expand(v->name));
      }
      else{
        printf("%.*s_%.*s = ", string_expand(v->section), string_expand(v->name));
        printf("record->%.*s;", string_expand(v->value_specifier));
      }
      printf("\nreturn;\n}");
    }
    printf("}\n");
  }
  printf("}\n");

}
*/
String8 binding_keyboard_macro_switch_recording[] = {string_u8_litexpr("M"),string_u8_litexpr("Alt"),};
String8 binding_keyboard_macro_replay[] = {string_u8_litexpr("M"),string_u8_litexpr("Control"),};
String8 binding_change_active_panel[] = {string_u8_litexpr("1"),string_u8_litexpr("Control"),};
String8 binding_change_active_panel_backwards[] = {string_u8_litexpr("2"),string_u8_litexpr("Control"),};
String8 binding_command_lister[] = {string_u8_litexpr("P"),string_u8_litexpr("Control"),string_u8_litexpr("Shift"),};
String8 binding_interactive_switch_buffer[] = {string_u8_litexpr("P"),string_u8_litexpr("Control"),};
String8 binding_interactive_open_or_new[] = {string_u8_litexpr("O"),string_u8_litexpr("Control"),};
String8 binding_goto_next_jump[] = {string_u8_litexpr("N"),string_u8_litexpr("Control"),};
String8 binding_goto_prev_jump[] = {string_u8_litexpr("N"),string_u8_litexpr("Control"),string_u8_litexpr("Shift"),};
String8 binding_save_all_dirty_buffers[] = {string_u8_litexpr("S"),string_u8_litexpr("Control"),string_u8_litexpr("Shift"),};
String8 binding_quick_swap_buffer[] = {string_u8_litexpr("Tab"),string_u8_litexpr("Control"),};
String8 binding_jump_to_definition[] = {string_u8_litexpr("I"),string_u8_litexpr("Control"),};
String8 binding_exit_4coder[] = {string_u8_litexpr("F4"),string_u8_litexpr("Alt"),};

String8 config_mapping = string_u8_litexpr("");
b32 config_bind_by_physical_key = false;
b32 config_use_file_bars = true;
b32 config_hide_file_bar_in_ui = true;
b32 config_use_error_highlight = true;
b32 config_use_jump_highlight = true;
b32 config_use_scope_highlight = true;
b32 config_use_paren_helper = true;
b32 config_use_comment_keywords = true;
b32 config_lister_whole_word_backspace_when_modified = true;
b32 config_show_line_number_margins = false;
b32 config_enable_output_wrapping = false;
b32 config_highlight_line_at_cursor = true;
b32 config_enable_undo_fade_out = true;
f32 config_background_margin_width = 0.0;
b32 config_smooth_scroll = true;
i64 config_cursor_roundness = 45;
i64 config_mark_thickness = 2;
i64 config_lister_roundness = 20;
f32 config_lister_item_height = 1.2;
String8 config_treat_as_code = string_u8_litexpr(".cpp.c.hpp.h.cc.cs.java.rs.glsl.m.mm");
b32 config_enable_virtual_whitespace = true;
i64 config_virtual_whitespace_regular_indent = 4;
b32 config_enable_code_wrapping = false;
b32 config_automatically_indent_text_on_save = true;
b32 config_automatically_save_changes_on_build = true;
b32 config_indent_with_tabs = false;
i64 config_indent_width = 4;
i64 config_default_tab_width = 4;
String8 config_default_font_name = string_u8_litexpr("liberation-mono.ttf");
i64 config_default_font_size = 12;
b32 config_default_font_hinting = false;
i64 config_default_font_aa_mode = 0;
String8 config_user_name = string_u8_litexpr("Krzosa");
b32 config_lalt_lctrl_is_altgr = false;
String8 build_comment_runner_filename_extension = string_u8_litexpr("cpp");
String8 build_comment_runner_command = string_u8_litexpr("clang {file} -Wno-writable-strings -g -o __gen{id}.exe && __gen{id}.exe");
u32 theme_bar = 0xFF333337;
u32 theme_base = 0xFFDCDCDC;
u32 theme_pop1 = 0xFFDCDCDC;
u32 theme_pop2 = 0xFF569CD6;
u32 theme_back = 0xFF1E1E1E;
u32 theme_margin = 0xFF1E1E1E;
u32 theme_margin_hover = 0xffff0000;
u32 theme_margin_active = 0xFF1E1E1E;
u32 theme_list_item_1 = 0xFF1E1E1E;
u32 theme_list_item_2 = 0xFF1E1E1E;
u32 theme_list_item_hover_1 = 0x22ffffff;
u32 theme_list_item_hover_2 = 0x22ffffff;
u32 theme_list_item_active_1 = 0x33ffffff;
u32 theme_list_item_active_2 = 0x33ffffff;
u32 theme_cursor_1 = 0xFFDCDCDC;
u32 theme_cursor_2 = 0xFFA72822;
u32 theme_at_cursor = 0xff000000;
u32 theme_highlight_cursor_line = 0xFF0F0F0F;
u32 theme_highlight = 0x11264F78;
u32 theme_at_highlight = 0xFF264F78;
u32 theme_mark = 0xFFDCDCDC;
u32 theme_text_default = 0xFFDCDCDC;
u32 theme_inactive_panel_overlay = 0x22000000;
u32 theme_comment = 0xFF608B4E;
u32 theme_comment_note = 0xFF569CD6;
u32 theme_comment_todo = 0xFF569CD6;
u32 theme_keyword = 0xFF569CD6;
u32 theme_function = 0xFF4EC9B0;
u32 theme_macro = 0xFFc586c0;
u32 theme_type = 0xFF4EC9B0;
u32 theme_str_constant = 0xFFD69D85;
u32 theme_char_constant = 0xFFD69D85;
u32 theme_int_constant = 0xFFB5CEA8;
u32 theme_float_constant = 0xFFB5CEA8;
u32 theme_bool_constant = 0xFF569CD6;
u32 theme_preproc = 0xFF9B9B9B;
u32 theme_include = 0xFFDCDCDC;
u32 theme_special_character = 0xFFDCDCDC;
u32 theme_ghost_character = 0xFFDCDCDC;
u32 theme_highlight_junk = 0xFF3A0000;
u32 theme_highlight_white = 0xFF003A3A;
u32 theme_paste = 0xFFDDEE00;
u32 theme_undo = 0xFF00DDEE;
u32 theme_text_cycle_1 = 0xFFD69D85;
u32 theme_text_cycle_2 = 0xFF4EC9B0;
u32 theme_text_cycle_3 = 0xFFA6E22E;
u32 theme_text_cycle_4 = 0xFF967EFB;
u32 theme_back_cycle_1 = 0x0C4EC9B0;
u32 theme_back_cycle_2 = 0x08569CD6;
u32 theme_back_cycle_3 = 0x080000A0;
u32 theme_back_cycle_4 = 0x08A0A000;
u32 theme_line_numbers_back = 0xFF1E1E1E;
u32 theme_line_numbers_text = 0xFFDCDCDC;
function void
set_config_value(Config_Value *record){
    if(string_match(string_u8_litexpr("config"), record->section)){

        if(string_match(record->name, string_u8_litexpr("mapping"))){
            config_mapping = record->value_str[0];
            return;
        }
        if(string_match(record->name, string_u8_litexpr("bind_by_physical_key"))){
            config_bind_by_physical_key = record->value_bool;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("use_file_bars"))){
            config_use_file_bars = record->value_bool;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("hide_file_bar_in_ui"))){
            config_hide_file_bar_in_ui = record->value_bool;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("use_error_highlight"))){
            config_use_error_highlight = record->value_bool;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("use_jump_highlight"))){
            config_use_jump_highlight = record->value_bool;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("use_scope_highlight"))){
            config_use_scope_highlight = record->value_bool;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("use_paren_helper"))){
            config_use_paren_helper = record->value_bool;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("use_comment_keywords"))){
            config_use_comment_keywords = record->value_bool;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("lister_whole_word_backspace_when_modified"))){
            config_lister_whole_word_backspace_when_modified = record->value_bool;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("show_line_number_margins"))){
            config_show_line_number_margins = record->value_bool;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("enable_output_wrapping"))){
            config_enable_output_wrapping = record->value_bool;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("highlight_line_at_cursor"))){
            config_highlight_line_at_cursor = record->value_bool;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("enable_undo_fade_out"))){
            config_enable_undo_fade_out = record->value_bool;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("background_margin_width"))){
            config_background_margin_width = record->value_float;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("smooth_scroll"))){
            config_smooth_scroll = record->value_bool;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("cursor_roundness"))){
            config_cursor_roundness = record->value_int;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("mark_thickness"))){
            config_mark_thickness = record->value_int;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("lister_roundness"))){
            config_lister_roundness = record->value_int;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("lister_item_height"))){
            config_lister_item_height = record->value_float;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("treat_as_code"))){
            config_treat_as_code = record->value_str[0];
            return;
        }
        if(string_match(record->name, string_u8_litexpr("enable_virtual_whitespace"))){
            config_enable_virtual_whitespace = record->value_bool;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("virtual_whitespace_regular_indent"))){
            config_virtual_whitespace_regular_indent = record->value_int;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("enable_code_wrapping"))){
            config_enable_code_wrapping = record->value_bool;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("automatically_indent_text_on_save"))){
            config_automatically_indent_text_on_save = record->value_bool;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("automatically_save_changes_on_build"))){
            config_automatically_save_changes_on_build = record->value_bool;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("indent_with_tabs"))){
            config_indent_with_tabs = record->value_bool;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("indent_width"))){
            config_indent_width = record->value_int;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("default_tab_width"))){
            config_default_tab_width = record->value_int;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("default_font_name"))){
            config_default_font_name = record->value_str[0];
            return;
        }
        if(string_match(record->name, string_u8_litexpr("default_font_size"))){
            config_default_font_size = record->value_int;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("default_font_hinting"))){
            config_default_font_hinting = record->value_bool;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("default_font_aa_mode"))){
            config_default_font_aa_mode = record->value_int;
            return;
        }
        if(string_match(record->name, string_u8_litexpr("user_name"))){
            config_user_name = record->value_str[0];
            return;
        }
        if(string_match(record->name, string_u8_litexpr("lalt_lctrl_is_altgr"))){
            config_lalt_lctrl_is_altgr = record->value_bool;
            return;
        }}
    if(string_match(string_u8_litexpr("theme"), record->section)){

        if(string_match(record->name, string_u8_litexpr("bar"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_bar = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("base"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_base = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("pop1"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_pop1 = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("pop2"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_pop2 = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("back"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_back = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("margin"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_margin = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("margin_hover"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_margin_hover = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("margin_active"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_margin_active = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("list_item_1"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_list_item_1 = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("list_item_2"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_list_item_2 = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("list_item_hover_1"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_list_item_hover_1 = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("list_item_hover_2"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_list_item_hover_2 = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("list_item_active_1"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_list_item_active_1 = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("list_item_active_2"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_list_item_active_2 = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("cursor_1"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_cursor_1 = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("cursor_2"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_cursor_2 = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("at_cursor"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_at_cursor = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("highlight_cursor_line"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_highlight_cursor_line = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("highlight"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_highlight = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("at_highlight"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_at_highlight = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("mark"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_mark = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("text_default"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_text_default = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("inactive_panel_overlay"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_inactive_panel_overlay = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("comment"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_comment = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("comment_note"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_comment_note = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("comment_todo"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_comment_todo = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("keyword"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_keyword = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("function"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_function = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("macro"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_macro = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("type"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_type = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("str_constant"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_str_constant = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("char_constant"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_char_constant = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("int_constant"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_int_constant = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("float_constant"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_float_constant = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("bool_constant"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_bool_constant = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("preproc"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_preproc = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("include"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_include = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("special_character"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_special_character = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("ghost_character"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_ghost_character = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("highlight_junk"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_highlight_junk = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("highlight_white"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_highlight_white = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("paste"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_paste = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("undo"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_undo = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("text_cycle_1"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_text_cycle_1 = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("text_cycle_2"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_text_cycle_2 = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("text_cycle_3"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_text_cycle_3 = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("text_cycle_4"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_text_cycle_4 = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("back_cycle_1"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_back_cycle_1 = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("back_cycle_2"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_back_cycle_2 = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("back_cycle_3"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_back_cycle_3 = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("back_cycle_4"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_back_cycle_4 = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("line_numbers_back"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_line_numbers_back = packed_color;

            return;
        }
        if(string_match(record->name, string_u8_litexpr("line_numbers_text"))){
            Vec4_f32 color = unpack_color(record->value_u32);
            color.r = powf(color.r, 2.2f);
            color.g = powf(color.g, 2.2f);
            color.b = powf(color.b, 2.2f);
            u32 packed_color = pack_color(color);
            theme_line_numbers_text = packed_color;

            return;
        }}
}
/*END*/


// TODO(Krzosa); Arrays in config?
function ARGB_Color
theme_text_cycle(i32 i){
    ARGB_Color text_cycle[] = {theme_text_cycle_1, theme_text_cycle_2, theme_text_cycle_3, theme_text_cycle_4};
    return text_cycle[i % ArrayCount(text_cycle)];
}

global ARGB_Color theme_black = 0xFF000000;
global ARGB_Color theme_white = 0xFFFFFFFF;
global ARGB_Color theme_light_gray = 0xFFCCCCCC;
global ARGB_Color theme_dark_gray = 0xFF555555;
global ARGB_Color theme_gray = 0xFF999999;
