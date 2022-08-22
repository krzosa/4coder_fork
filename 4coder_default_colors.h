/*
* Default color slots
*/

// TOP

#if !defined(FCODER_DEFAULT_COLORS_H)
#define FCODER_DEFAULT_COLORS_H

struct Color_Table_Node{
    Color_Table_Node *next;
    String_Const_u8 name;
    Color_Table table;
};

struct Color_Table_List{
    Color_Table_Node *first;
    Color_Table_Node *last;
    i32 count;
};

global Color_Table active_color_table = {};
global Color_Table default_color_table = {};

global Arena global_theme_arena = {};
global Color_Table_List global_theme_list = {};

#endif

// BOTTOM

