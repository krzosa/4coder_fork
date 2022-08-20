
/*
When designing config api:
* Config should notify when value is not set
* Config should provide defaults everywhere
* Should have easy to read messages
* Should noticeably notify when error
* Shouldn't require indentation, shouldn't require a lot of syntax
*

*/
// Command Mapping
// ""                - Leave the bindings unaltered - use this when writing your own customization!
// "choose"          - Ask 4coder to choose based on platform.
// "default"         - Use the default keybindings 4coder has always had.
// "mac-default"     - Use keybindings similar to those found in other Mac applications.
String8 debug_config_mapping = string_u8_litexpr("");


// MODE
// "4coder"       - The default 4coder mode that has been around since the beginning of time (2015)
// "notepad-like" - Single "thin" cursor and highlight ranges like in notepad, sublime, notepad++, etc
String8 debug_config_mode = string_u8_litexpr("4coder");
b32 debug_config_bind_by_physical_key = false;

// UI
b32 debug_config_use_file_bars = true;
b32 debug_config_hide_file_bar_in_ui = true;
b32 debug_config_use_error_highlight = true;
b32 debug_config_use_jump_highlight = true;
b32 debug_config_use_scope_highlight = true;
b32 debug_config_use_paren_helper = true;
b32 debug_config_use_comment_keywords = true;
b32 debug_config_lister_whole_word_backspace_when_modified = true;
b32 debug_config_show_line_number_margins = false;
b32 debug_config_enable_output_wrapping = false;
b32 debug_config_highlight_line_at_cursor = true;
b32 debug_config_enable_undo_fade_out = true;


f32 debug_config_background_margin_width = 0.f;

// cursor_roundess is a value [0,50] setting the radius of
// the cursor and mark's roundness as a percentage of their width
// (At 50 the left and right corners will be so round they form a semi-circle,
//  hence 50 is the max)
i64 debug_config_cursor_roundness = 45;

// mark_thickness is a pixel count value setting the
// thickness of the mark wire box in original mode
i64 debug_config_mark_thickness = 2;

// lister_roundess is a value [0,50] setting the radius of
// the lister items' roundness as a percentage of their height
i64 debug_config_lister_roundness = 20;
f32 debug_config_lister_item_height = 1.2f;

// Code Wrapping
String8 debug_config_treat_as_code = string_u8_litexpr(".cpp.c.hpp.h.cc.cs.java.rs.glsl.m.mm");
b32 debug_config_enable_virtual_whitespace = true;
i64 debug_config_virtual_whitespace_regular_indent = 4;
b32 debug_config_enable_code_wrapping = false;

// This only applies to code files in code-wrapping mode.
// Plain text and code files without virtual-whitespace will not be effected.
b32 debug_config_automatically_indent_text_on_save = true;

// When set to true, all unsaved changes will be saved on a build.
b32 debug_config_automatically_save_changes_on_build = true;

// Indentation
b32 debug_config_indent_with_tabs = false;
i64 debug_config_indent_width = 4;
i64 debug_config_default_tab_width = 4;

// Theme
String8 debug_config_default_theme_name = string_u8_litexpr("4coder");

// Font
String8 debug_config_default_font_name = string_u8_litexpr("liberation-mono.ttf");
i64 debug_config_default_font_size = 12;
b32 debug_config_default_font_hinting = false;

// aa modes:
//  8bit - mono-chrome 0-255 opacity channel per pixel
//  1bit - mono-chrome 0/1 opacity channel per pixel
Face_Antialiasing_Mode debug_config_default_font_aa_mode = FaceAntialiasingMode_8BitMono;

// User
String8 debug_config_user_name = string_u8_litexpr("not-set");

// Keyboard AltGr setting
b32 debug_config_lalt_lctrl_is_altgr = false;
