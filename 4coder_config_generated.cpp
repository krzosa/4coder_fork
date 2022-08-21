/*
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

#include "4coder_config1.cpp"

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


    for(i32 i = 0; i < values_count; i++){
      Config_Value *v = values + i;

      b32 is_string = false;
      if(string_match(string_u8_litexpr("String8"), v->type)) is_string = true;

      printf("%.*s ", string_expand(v->type));
      printf("%.*s_%.*s = ", string_expand(v->section), string_expand(v->name));
      if(is_string == true) printf("string_u8_litexpr(\"");
      printf("%.*s", string_expand(v->value_str));
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
      printf(R"==(
      if(string_match(record->name, string_u8_litexpr("%.*s"))){
        %.*s_%.*s = record->%.*s;
        return;
      }
      )==", string_expand(v->name), string_expand(v->section), string_expand(v->name), string_expand(v->value_specifier));
    }
    printf("}\n");
  }
  printf("}\n");

}
*/
String8 config_mapping = string_u8_litexpr("");
String8 config_mode = string_u8_litexpr("4coder");
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
String8 config_default_theme_name = string_u8_litexpr("4coder");
String8 config_default_font_name = string_u8_litexpr("liberation-mono.ttf");
i64 config_default_font_size = 12;
b32 config_default_font_hinting = false;
i64 config_default_font_aa_mode = FaceAntialiasingMode_8BitMono;
String8 config_user_name = string_u8_litexpr("Krzosa");
b32 config_lalt_lctrl_is_altgr = false;
String8 config_comment_runner_filename_extension = string_u8_litexpr("cpp");
String8 config_comment_runner_command = string_u8_litexpr("clang {file} -Wno-writable-strings -g -o __gen{id}.exe && __gen{id}.exe");
u32 theme_defcolor_bar = 0xFF333337;
u32 theme_defcolor_base = 0xFFDCDCDC;
u32 theme_defcolor_pop1 = 0xFFDCDCDC;
u32 theme_defcolor_pop2 = 0xFF569CD6;
u32 theme_defcolor_back = 0xFF1E1E1E;
u32 theme_defcolor_margin = 0xFF1E1E1E;
u32 theme_defcolor_margin_hover = 0xffff0000;
u32 theme_defcolor_margin_active = 0xFF1E1E1E;
u32 theme_defcolor_list_item = 0xFF1E1E1E;
u32 theme_defcolor_list_item_hover = 0x22ffffff;
u32 theme_defcolor_list_item_active = 0x33ffffff;
u32 theme_defcolor_cursor = 0xFFDCDCDC;
u32 theme_defcolor_at_cursor = 0xff000000;
u32 theme_defcolor_highlight_cursor_line = 0xFF0F0F0F;
u32 theme_defcolor_highlight = 0x11264F78;
u32 theme_defcolor_at_highlight = 0xFF264F78;
u32 theme_defcolor_mark = 0xFFDCDCDC;
u32 theme_defcolor_text_default = 0xFFDCDCDC;
u32 theme_defcolor_comment = 0xFF608B4E;
u32 theme_defcolor_comment_pop = 0xFF569CD6;
u32 theme_defcolor_keyword = 0xFF569CD6;
u32 theme_defcolor_function = 0xFF4EC9B0;
u32 theme_defcolor_macro = 0xFFc586c0;
u32 theme_defcolor_type = 0xFF4EC9B0;
u32 theme_defcolor_str_constant = 0xFFD69D85;
u32 theme_defcolor_char_constant = 0xFFD69D85;
u32 theme_defcolor_int_constant = 0xFFB5CEA8;
u32 theme_defcolor_float_constant = 0xFFB5CEA8;
u32 theme_defcolor_bool_constant = 0xFF569CD6;
u32 theme_defcolor_preproc = 0xFF9B9B9B;
u32 theme_defcolor_include = 0xFFDCDCDC;
u32 theme_defcolor_special_character = 0xFFDCDCDC;
u32 theme_defcolor_ghost_character = 0xFFDCDCDC;
u32 theme_defcolor_highlight_junk = 0xFF3A0000;
u32 theme_defcolor_highlight_white = 0xFF003A3A;
u32 theme_defcolor_paste = 0xFFDDEE00;
u32 theme_defcolor_undo = 0xFF00DDEE;
u32 theme_defcolor_back_cycle = 0x0C4EC9B0;
u32 theme_defcolor_text_cycle = 0xFFD69D85;
u32 theme_defcolor_line_numbers_back = 0xFF1E1E1E;
u32 theme_defcolor_line_numbers_text = 0xFFDCDCDC;
function void
set_config_value(Config_Value *record){
    if(string_match(string_u8_litexpr("config"), record->section)){

        if(string_match(record->name, string_u8_litexpr("mapping"))){
            config_mapping = record->value_str;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("mode"))){
            config_mode = record->value_str;
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
            config_treat_as_code = record->value_str;
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

        if(string_match(record->name, string_u8_litexpr("default_theme_name"))){
            config_default_theme_name = record->value_str;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("default_font_name"))){
            config_default_font_name = record->value_str;
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
            config_user_name = record->value_str;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("lalt_lctrl_is_altgr"))){
            config_lalt_lctrl_is_altgr = record->value_bool;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("comment_runner_filename_extension"))){
            config_comment_runner_filename_extension = record->value_str;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("comment_runner_command"))){
            config_comment_runner_command = record->value_str;
            return;
        }
    }
    if(string_match(string_u8_litexpr("theme"), record->section)){

        if(string_match(record->name, string_u8_litexpr("defcolor_bar"))){
            theme_defcolor_bar = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_base"))){
            theme_defcolor_base = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_pop1"))){
            theme_defcolor_pop1 = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_pop2"))){
            theme_defcolor_pop2 = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_back"))){
            theme_defcolor_back = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_margin"))){
            theme_defcolor_margin = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_margin_hover"))){
            theme_defcolor_margin_hover = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_margin_active"))){
            theme_defcolor_margin_active = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_list_item"))){
            theme_defcolor_list_item = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_list_item_hover"))){
            theme_defcolor_list_item_hover = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_list_item_active"))){
            theme_defcolor_list_item_active = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_cursor"))){
            theme_defcolor_cursor = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_at_cursor"))){
            theme_defcolor_at_cursor = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_highlight_cursor_line"))){
            theme_defcolor_highlight_cursor_line = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_highlight"))){
            theme_defcolor_highlight = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_at_highlight"))){
            theme_defcolor_at_highlight = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_mark"))){
            theme_defcolor_mark = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_text_default"))){
            theme_defcolor_text_default = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_comment"))){
            theme_defcolor_comment = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_comment_pop"))){
            theme_defcolor_comment_pop = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_keyword"))){
            theme_defcolor_keyword = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_function"))){
            theme_defcolor_function = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_macro"))){
            theme_defcolor_macro = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_type"))){
            theme_defcolor_type = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_str_constant"))){
            theme_defcolor_str_constant = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_char_constant"))){
            theme_defcolor_char_constant = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_int_constant"))){
            theme_defcolor_int_constant = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_float_constant"))){
            theme_defcolor_float_constant = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_bool_constant"))){
            theme_defcolor_bool_constant = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_preproc"))){
            theme_defcolor_preproc = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_include"))){
            theme_defcolor_include = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_special_character"))){
            theme_defcolor_special_character = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_ghost_character"))){
            theme_defcolor_ghost_character = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_highlight_junk"))){
            theme_defcolor_highlight_junk = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_highlight_white"))){
            theme_defcolor_highlight_white = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_paste"))){
            theme_defcolor_paste = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_undo"))){
            theme_defcolor_undo = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_back_cycle"))){
            theme_defcolor_back_cycle = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_text_cycle"))){
            theme_defcolor_text_cycle = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_line_numbers_back"))){
            theme_defcolor_line_numbers_back = record->value_u32;
            return;
        }

        if(string_match(record->name, string_u8_litexpr("defcolor_line_numbers_text"))){
            theme_defcolor_line_numbers_text = record->value_u32;
            return;
        }
    }
}
/*END*/
