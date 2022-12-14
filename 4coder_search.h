/*
4coder_search.h - Types that are used in the search accross all buffers procedures.
*/

// TOP

#if !defined(FCODER_SEARCH_H)
#define FCODER_SEARCH_H

typedef u32 List_All_Locations_Flag;
enum{
    ListAllLocationsFlag_CaseSensitive = 1,
    ListAllLocationsFlag_MatchSubstring = 2,
};
internal void list_all_locations__generic(App *app, String_Const_u8_Array needle, List_All_Locations_Flag flags);

struct Word_Complete_Iterator{
    App *app;
    Arena *arena;

    Temp_Memory arena_restore;
    Buffer_ID first_buffer;
    Buffer_ID current_buffer;
    b32 scan_all_buffers;
    String_Const_u8 needle;

    List_String_Const_u8 list;
    Node_String_Const_u8 *node;
    Table_Data_u64 already_used_table;
};

function Word_Complete_Iterator*word_complete_get_shared_iter(App *app);
internal Range_i64 get_word_complete_needle_range(App *app, Buffer_ID buffer, i64 pos);
function void word_complete_iter_init(Buffer_ID buffer, Range_i64 range, Word_Complete_Iterator *iter);
function void word_complete_iter_next(Word_Complete_Iterator *it);
function String_Const_u8 word_complete_iter_read(Word_Complete_Iterator *it);
#endif

// BOTOTM

