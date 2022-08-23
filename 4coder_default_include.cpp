/*
4coder_default_include.cpp - Default set of commands and setup used in 4coder.
*/

// TOP

#if !defined(FCODER_DEFAULT_INCLUDE_CPP)
#define FCODER_DEFAULT_INCLUDE_CPP

#if !defined(FCODER_TRANSITION_TO)
#define FCODER_TRANSITION_TO 0
#endif

#include <stdio.h>
#include <stdlib.h>

#if !defined(META_PASS)
#include "generated/command_metadata.h"
#endif


#include "4coder_config_parse.cpp"
#include "4coder_variables.h"
#include "4coder_async_tasks.h"
#include "4coder_string_match.h"
#include "4coder_helper.h"
#include "4coder_delta_rule.h"
#include "4coder_layout_rule.h"
#include "4coder_code_index.h"
#include "4coder_draw.h"
#include "4coder_insertion.h"
#include "4coder_command_map.h"
#include "4coder_lister_base.h"
#include "4coder_clipboard.h"
#include "4coder_default_framework.h"
#include "4coder_auto_indent.h"
#include "4coder_search.h"
#include "4coder_build_commands.h"
#include "4coder_jumping.h"
#include "4coder_jump_sticky.h"
#include "4coder_jump_lister.h"
#include "4coder_function_list.h"
#include "4coder_scope_commands.h"
#include "4coder_combined_write_commands.h"
// #include "4coder_log_parser.h"
#include "4coder_profile_inspect.h"
#include "4coder_search_list.h"

////////////////////////////////

#include "4coder_config_generated.cpp"
#include "4coder_file.cpp"
#include "4coder_async_tasks.cpp"
#include "4coder_default_map.cpp"
#include "4coder_mac_map.cpp"

#include "4coder_default_framework_variables.cpp"
#include "4coder_helper.cpp"
#include "4coder_delta_rule.cpp"
#include "4coder_layout_rule.cpp"
#include "4coder_code_index.cpp"
#include "4coder_fancy.cpp"
#include "4coder_draw.cpp"
#include "4coder_font_helper.cpp"
#include "4coder_search_list.cpp"
#include "4coder_default_framework.cpp"
#include "4coder_clipboard.cpp"
#include "4coder_lister_base.cpp"
#include "4coder_base_commands.cpp"
#include "4coder_insertion.cpp"
#include "4coder_eol.cpp"
#include "4coder_lists.cpp"
#include "4coder_auto_indent.cpp"
#include "4coder_search.cpp"
#include "4coder_jumping.cpp"
#include "4coder_jump_sticky.cpp"
#include "4coder_jump_lister.cpp"
#include "4coder_code_index_listers.cpp"
// #include "4coder_log_parser.cpp"
#include "4coder_keyboard_macro.cpp"
#include "4coder_cli_command.cpp"
#include "4coder_build_commands.cpp"
#include "4coder_function_list.cpp"
#include "4coder_scope_commands.cpp"
#include "4coder_combined_write_commands.cpp"
#include "4coder_profile_inspect.cpp"
#include "4coder_variables.cpp"
#include "4coder_default_hooks.cpp"
#include "4coder_config_load.cpp"
#include "4coder_remedybg_driver.cpp"

#endif

// BOTTOM

