/*
4coder_search.cpp - Commands that search accross buffers including word complete,
and list all locations.
*/

// TOP

global String_Const_u8 search_name = string_u8_litexpr("*search*");

internal void
print_string_match_list_to_buffer(App *app, Buffer_ID out_buffer_id, String_Match_List matches){
    Scratch_Block scratch(app);
    clear_buffer(app, out_buffer_id);
    Buffer_Insertion out = begin_buffer_insertion_at_buffered(app, out_buffer_id, 0, scratch, KB(64));
    buffer_set_setting(app, out_buffer_id, BufferSetting_ReadOnly, true);
    buffer_set_setting(app, out_buffer_id, BufferSetting_RecordsHistory, false);

    Temp_Memory buffer_name_restore_point = begin_temp(scratch);
    String_Const_u8 current_file_name = {};
    Buffer_ID current_buffer = 0;

    if (matches.first != 0){
        for (String_Match *node = matches.first;
             node != 0;
             node = node->next){
            if (node->buffer != out_buffer_id){
                if (current_buffer != 0 && current_buffer != node->buffer){
                    insertc(&out, '\n');
                }
                if (current_buffer != node->buffer){
                    end_temp(buffer_name_restore_point);
                    current_buffer = node->buffer;
                    current_file_name = push_buffer_file_name(app, scratch, current_buffer);
                    if (current_file_name.size == 0){
                        current_file_name = push_buffer_unique_name(app, scratch, current_buffer);
                    }
                }

                Buffer_Cursor cursor = buffer_compute_cursor(app, current_buffer, seek_pos(node->range.first));
                Temp_Memory line_temp = begin_temp(scratch);
                String_Const_u8 full_line_str = push_buffer_line(app, scratch, current_buffer, cursor.line);
                String_Const_u8 line_str = string_skip_chop_whitespace(full_line_str);
                insertf(&out, "%.*s:%d:%d: %.*s\n",
                        string_expand(current_file_name), cursor.line, cursor.col,
                        string_expand(line_str));
                end_temp(line_temp);
            }
        }
    }
    else{
        insertf(&out, "no matches");
    }

    end_buffer_insertion(&out);
}

internal void
print_all_matches_all_buffers(App *app, String_Const_u8_Array match_patterns, String_Match_Flag must_have_flags, String_Match_Flag must_not_have_flags, Buffer_ID out_buffer_id){
    Scratch_Block scratch(app);
    String_Match_List matches = find_all_matches_all_buffers(app, scratch, match_patterns, must_have_flags, must_not_have_flags);
    string_match_list_filter_remove_buffer(&matches, out_buffer_id);
    string_match_list_filter_remove_buffer_predicate(app, &matches, buffer_has_name_with_star);
    print_string_match_list_to_buffer(app, out_buffer_id, matches);
}

internal void
print_all_matches_all_buffers_to_search(App *app, String_Const_u8_Array match_patterns, String_Match_Flag must_have_flags, String_Match_Flag must_not_have_flags, View_ID default_target_view){
    Buffer_ID search_buffer = create_or_switch_to_buffer_and_clear_by_name(app, search_name, default_target_view);
    print_all_matches_all_buffers(app, match_patterns, must_have_flags, must_not_have_flags, search_buffer);
    lock_jump_buffer(app, search_name);
}

internal String_Const_u8_Array
user_list_definition_array(App *app, Arena *arena, String_Const_u8 base_needle){
    String_Const_u8_Array result = {};
    if (base_needle.size > 0){
        result.count = 12;
        result.vals = push_array(arena, String_Const_u8, result.count);
        i32 i = 0;
        result.vals[i++] = (push_u8_stringf(arena, "struct %.*s{"  , string_expand(base_needle)));
        result.vals[i++] = (push_u8_stringf(arena, "struct %.*s\n{", string_expand(base_needle)));
        result.vals[i++] = (push_u8_stringf(arena, "struct %.*s\r\n{", string_expand(base_needle)));
        result.vals[i++] = (push_u8_stringf(arena, "struct %.*s {" , string_expand(base_needle)));
        result.vals[i++] = (push_u8_stringf(arena, "union %.*s{"   , string_expand(base_needle)));
        result.vals[i++] = (push_u8_stringf(arena, "union %.*s\n{" , string_expand(base_needle)));
        result.vals[i++] = (push_u8_stringf(arena, "union %.*s\r\n{" , string_expand(base_needle)));
        result.vals[i++] = (push_u8_stringf(arena, "union %.*s {"  , string_expand(base_needle)));
        result.vals[i++] = (push_u8_stringf(arena, "enum %.*s{"    , string_expand(base_needle)));
        result.vals[i++] = (push_u8_stringf(arena, "enum %.*s\n{"  , string_expand(base_needle)));
        result.vals[i++] = (push_u8_stringf(arena, "enum %.*s\r\n{"  , string_expand(base_needle)));
        result.vals[i++] = (push_u8_stringf(arena, "enum %.*s {"   , string_expand(base_needle)));
        Assert(i == result.count);
    }
    return(result);
}

internal String_Const_u8_Array
query_user_list_definition_needle(App *app, Arena *arena){
    u8 *space = push_array(arena, u8, KB(1));
    String_Const_u8 base_needle = get_query_string(app, "List Definitions For: ", space, KB(1));
    return(user_list_definition_array(app, arena, base_needle));
}

internal void
list_all_locations__generic(App *app, String_Const_u8_Array needle, List_All_Locations_Flag flags){
    if (needle.count > 0){
        View_ID target_view = global_compilation_view;
        String_Match_Flag must_have_flags = 0;
        String_Match_Flag must_not_have_flags = 0;
        if (HasFlag(flags, ListAllLocationsFlag_CaseSensitive)){
            AddFlag(must_have_flags, StringMatch_CaseSensitive);
        }
        if (!HasFlag(flags, ListAllLocationsFlag_MatchSubstring)){
            AddFlag(must_not_have_flags, StringMatch_LeftSideSloppy);
            AddFlag(must_not_have_flags, StringMatch_RightSideSloppy);
        }
        print_all_matches_all_buffers_to_search(app, needle, must_have_flags, must_not_have_flags, target_view);
    }
}

internal void
list_all_locations__generic(App *app, String_Const_u8 needle, List_All_Locations_Flag flags){
    if (needle.size != 0){
        String_Const_u8_Array array = {&needle, 1};
        list_all_locations__generic(app, array, flags);
    }
}

internal void
list_all_locations__generic_query(App *app, List_All_Locations_Flag flags){
    Scratch_Block scratch(app);
    u8 *space = push_array(scratch, u8, KB(1));
    String_Const_u8 needle = get_query_string(app, "List Locations For: ", space, KB(1));
    list_all_locations__generic(app, needle, flags);
}

internal void
list_all_locations__generic_identifier(App *app, List_All_Locations_Flag flags){
    Scratch_Block scratch(app);
    String_Const_u8 needle = push_token_or_word_under_active_cursor(app, scratch);
    list_all_locations__generic(app, needle, flags);
}

internal void
list_all_locations__generic_view_range(App *app, List_All_Locations_Flag flags){
    Scratch_Block scratch(app);
    String_Const_u8 needle = push_view_range_string(app, scratch);
    list_all_locations__generic(app, needle, flags);
}

CUSTOM_COMMAND_SIG(list_all_locations)
CUSTOM_DOC("Queries the user for a string and lists all exact case-sensitive matches found in all open buffers.")
{
    list_all_locations__generic_query(app, ListAllLocationsFlag_CaseSensitive);
}

CUSTOM_COMMAND_SIG(list_all_substring_locations)
CUSTOM_DOC("Queries the user for a string and lists all case-sensitive substring matches found in all open buffers.")
{
    list_all_locations__generic_query(app, ListAllLocationsFlag_CaseSensitive|ListAllLocationsFlag_MatchSubstring);
}

CUSTOM_COMMAND_SIG(list_all_locations_case_insensitive)
CUSTOM_DOC("Queries the user for a string and lists all exact case-insensitive matches found in all open buffers.")
{
    list_all_locations__generic_query(app, 0);
}

CUSTOM_COMMAND_SIG(list_all_substring_locations_case_insensitive)
CUSTOM_DOC("Queries the user for a string and lists all case-insensitive substring matches found in all open buffers.")
{
    list_all_locations__generic_query(app, ListAllLocationsFlag_MatchSubstring);
}

CUSTOM_COMMAND_SIG(list_all_locations_of_identifier)
CUSTOM_DOC("Reads a token or word under the cursor and lists all exact case-sensitive mathces in all open buffers.")
{
    list_all_locations__generic_identifier(app, ListAllLocationsFlag_CaseSensitive);
}

CUSTOM_COMMAND_SIG(list_all_locations_of_identifier_case_insensitive)
CUSTOM_DOC("Reads a token or word under the cursor and lists all exact case-insensitive mathces in all open buffers.")
{
    list_all_locations__generic_identifier(app, ListAllLocationsFlag_CaseSensitive|ListAllLocationsFlag_MatchSubstring);
}

CUSTOM_COMMAND_SIG(list_all_locations_of_selection)
CUSTOM_DOC("Reads the string in the selected range and lists all exact case-sensitive mathces in all open buffers.")
{
    list_all_locations__generic_view_range(app, ListAllLocationsFlag_CaseSensitive);
}

CUSTOM_COMMAND_SIG(list_all_locations_of_selection_case_insensitive)
CUSTOM_DOC("Reads the string in the selected range and lists all exact case-insensitive mathces in all open buffers.")
{
    list_all_locations__generic_view_range(app, 0);
}

CUSTOM_COMMAND_SIG(list_all_locations_of_type_definition)
CUSTOM_DOC("Queries user for string, lists all locations of strings that appear to define a type whose name matches the input string.")
{
    Scratch_Block scratch(app);
    String_Const_u8_Array array = query_user_list_definition_needle(app, scratch);
    list_all_locations__generic(app, array, ListAllLocationsFlag_CaseSensitive|ListAllLocationsFlag_MatchSubstring);
}

CUSTOM_COMMAND_SIG(list_all_locations_of_type_definition_of_identifier)
CUSTOM_DOC("Reads a token or word under the cursor and lists all locations of strings that appear to define a type whose name matches it.")
{
    Scratch_Block scratch(app);
    String_Const_u8 base_needle = push_token_or_word_under_active_cursor(app, scratch);
    String_Const_u8_Array array = user_list_definition_array(app, scratch, base_needle);
    list_all_locations__generic(app, array, ListAllLocationsFlag_CaseSensitive|ListAllLocationsFlag_MatchSubstring);
}

internal Range_i64
get_word_complete_needle_range(App *app, Buffer_ID buffer, i64 pos){
    Range_i64 needle_range = {};
    needle_range.max = pos;
    needle_range.min = scan(app, boundary_alpha_numeric_underscore_utf8, buffer, Scan_Backward, pos);
    i64 e = scan(app, boundary_alpha_numeric_underscore_utf8, buffer, Scan_Forward, needle_range.min);
    if (pos > e){
        needle_range = Ii64(pos);
    }
    return(needle_range);
}

internal void
string_match_list_enclose_all(App *app, String_Match_List list,
                              Enclose_Function *enclose){
    for (String_Match *node = list.first;
         node != 0;
         node = node->next){
        node->range = enclose(app, node->buffer, node->range);
    }
}

global String_Match_Flag complete_must = (StringMatch_CaseSensitive|
                                          StringMatch_RightSideSloppy);
global String_Match_Flag complete_must_not = StringMatch_LeftSideSloppy;

internal String_Match_List
get_complete_list_raw(App *app, Arena *arena, Buffer_ID buffer,
                      Range_i64 needle_range, String_Const_u8 needle){
    local_persist Character_Predicate *pred =
        &character_predicate_alpha_numeric_underscore_utf8;

    String_Match_List result = {};
    i64 size = buffer_get_size(app, buffer);
    if (range_size(needle_range) > 0){
        String_Match_List up = buffer_find_all_matches(app, arena, buffer, 0,
                                                       Ii64(0, needle_range.min),
                                                       needle, pred, Scan_Backward);
        String_Match_List down = buffer_find_all_matches(app, arena, buffer, 0,
                                                         Ii64(needle_range.max, size),
                                                         needle, pred, Scan_Forward);
        string_match_list_filter_flags(&up, complete_must, complete_must_not);
        string_match_list_filter_flags(&down, complete_must, complete_must_not);
        result = string_match_list_merge_nearest(&up, &down, needle_range);
    }
    else{
        result = buffer_find_all_matches(app, arena, buffer, 0,
                                         Ii64(0, size), needle, pred, Scan_Forward);
        string_match_list_filter_flags(&result, complete_must, complete_must_not);
    }

    string_match_list_enclose_all(app, result,
                                  right_enclose_alpha_numeric_underscore_utf8);
    return(result);
}

function void
word_complete_list_extend_from_raw(App *app, Arena *arena, String_Match_List *matches, List_String_Const_u8 *list, Table_Data_u64 *used_table){
    // NOTE(Krzosa): Deleted profile code: ProfileScope(app, "word complete list extend from raw");
    Scratch_Block scratch(app);
    for (String_Match *node = matches->first; node != 0; node = node->next){
        String_Const_u8 s = push_buffer_range(app, scratch, node->buffer, node->range);
        Table_Lookup lookup = table_lookup(used_table, s);
        if (!lookup.found_match){
            String_Const_u8 data = push_data_copy(arena, s);
            table_insert(used_table, data, 1);
            string_list_push(arena, list, data);
        }
    }
}

function void
word_complete_iter_init__inner(Buffer_ID buffer, String_Const_u8 needle, Range_i64 range, Word_Complete_Iterator *iter){
    App *app = iter->app;
    Arena *arena = iter->arena;

    Base_Allocator *allocator = get_base_allocator_system();
    if (iter->already_used_table.allocator != 0){
        end_temp(iter->arena_restore);
        table_clear(&iter->already_used_table);
    }

    block_zero_struct(iter);
    iter->app = app;
    iter->arena = arena;

    iter->arena_restore = begin_temp(arena);
    iter->needle = push_string_copy(arena, needle);
    iter->first_buffer = buffer;
    iter->current_buffer = buffer;

    Scratch_Block scratch(app, arena);
    String_Match_List list = get_complete_list_raw(app, scratch, buffer, range, needle);

    iter->already_used_table = make_table_Data_u64(allocator, 100);
    word_complete_list_extend_from_raw(app, arena, &list, &iter->list, &iter->already_used_table);

    iter->scan_all_buffers = true;
}

function void
word_complete_iter_init(Buffer_ID buffer, Range_i64 range, Word_Complete_Iterator *iter){
    if (iter->app != 0 && iter->arena != 0){
        App *app = iter->app;
        Arena *arena = iter->arena;
        Scratch_Block scratch(app, arena);
        String_Const_u8 needle = push_buffer_range(app, scratch, buffer, range);
        word_complete_iter_init__inner(buffer, needle, range, iter);
    }
}

function void
word_complete_iter_init(Buffer_ID first_buffer, String_Const_u8 needle, Word_Complete_Iterator *iter){
    if (iter->app != 0 && iter->arena != 0){
        word_complete_iter_init__inner(first_buffer, needle, Ii64(), iter);
    }
}

function void
word_complete_iter_init(String_Const_u8 needle, Word_Complete_Iterator *iter){
    if (iter->app != 0 && iter->arena != 0){
        App *app = iter->app;
        Buffer_ID first_buffer = get_buffer_next(app, 0, Access_Read);
        word_complete_iter_init__inner(first_buffer, needle, Ii64(), iter);
    }
}

function void
word_complete_iter_next(Word_Complete_Iterator *it){
    for (;;){
        if (it->node == 0){
            it->node = it->list.first;
        }
        else{
            it->node = it->node->next;
        }

        if (it->node != 0){
            break;
        }

        if (!it->scan_all_buffers){
            break;
        }

        App *app = it->app;
        Buffer_ID next = get_buffer_next_looped(app, it->current_buffer, Access_Read);
        if (next == it->first_buffer){
            break;
        }

        it->node = it->list.last;
        it->current_buffer = next;
        Scratch_Block scratch(app);
        String_Match_List list = get_complete_list_raw(app, scratch,
                                                       next, Ii64(), it->needle);
        word_complete_list_extend_from_raw(app, it->arena, &list,
                                           &it->list, &it->already_used_table);
    }
}

function String_Const_u8
word_complete_iter_read(Word_Complete_Iterator *it){
    String_Const_u8 result = {};
    if (it->node == 0){
        result = it->needle;
    }
    else{
        result = it->node->string;
    }
    return(result);
}


function b32
word_complete_iter_is_at_base_slot(Word_Complete_Iterator *it){
    return(it->node == 0);
}

function Word_Complete_Iterator*
word_complete_get_shared_iter(App *app){
    local_persist Arena completion_arena = {};
    local_persist Word_Complete_Iterator it = {};
    local_persist b32 first_call = true;
    if (first_call){
        first_call = false;
        completion_arena = make_arena_system();
    }
    it.app = app;
    it.arena = &completion_arena;
    return(&it);
}

#if 0

Ideal word complete

Memes(word_complete)
{
    // API 1

    It it = begin_word_complete();
    for(;;){
        // We detect if needle changed
        String8 result = get_next_word_complete(it, needle, strings);
    }
    end_word_complete(it);


    // API 2
    Command{
        static It it;
        String8 result = next_word_complete_in_buffer(it, buffer, needle, buffer_got_updated());
    }


}


#endif

CUSTOM_COMMAND_SIG(word_complete)
CUSTOM_DOC("Iteratively tries completing the word to the left of the cursor with other words in open buffers that have the same prefix string.")
{
    // NOTE(Krzosa): Deleted profile code: ProfileScope(app, "word complete");

    View_ID view = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID buffer = view_get_buffer(app, view, Access_ReadWriteVisible);
    if (buffer != 0){
        Managed_Scope scope = view_get_managed_scope(app, view);

        b32 first_completion = false;
        Rewrite_Type *rewrite = scope_attachment(app, scope, view_rewrite_loc, Rewrite_Type);
        if (*rewrite != Rewrite_WordComplete){
            first_completion = true;
        }

        set_next_rewrite(app, view, Rewrite_WordComplete);

        Word_Complete_Iterator *it = word_complete_get_shared_iter(app);
        local_persist b32 initialized = false;
        local_persist Range_i64 range = {};

        if (first_completion || !initialized){
            // NOTE(Krzosa): Deleted profile code: ProfileBlock(app, "word complete state init");
            initialized = false;
            i64 pos = view_get_cursor_pos(app, view);
            Range_i64 needle_range = get_word_complete_needle_range(app, buffer, pos);
            if (range_size(needle_range) > 0){
                initialized = true;
                range = needle_range;
                word_complete_iter_init(buffer, needle_range, it);
            }
        }

        if (initialized){
            // NOTE(Krzosa): Deleted profile code: ProfileBlock(app, "word complete apply");

            word_complete_iter_next(it);
            String_Const_u8 str = word_complete_iter_read(it);

            buffer_replace_range(app, buffer, range, str);

            range.max = range.min + str.size;
            view_set_cursor_and_preferred_x(app, view, seek_pos(range.max));
        }
    }
}

// BOTTOM

