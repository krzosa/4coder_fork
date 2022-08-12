/*
4coder_code_index_listers.cpp - Listers for exploring the contents of the code index.
*/

// TOP

struct Tiny_Jump{
    Buffer_ID buffer;
    i64 pos;
};

CUSTOM_UI_COMMAND_SIG(jump_to_definition)
CUSTOM_DOC("List all definitions in the code index and jump to one chosen by the user.")
{
    char *query = "Definition:";

    Scratch_Block scratch(app);
    Lister_Block lister(app, scratch);
    lister_set_query(lister, query);
    lister_set_default_handlers(lister);

    code_index_lock();
    for (Buffer_ID buffer = get_buffer_next(app, 0, Access_Always);
         buffer != 0;
         buffer = get_buffer_next(app, buffer, Access_Always)){
        Code_Index_File *file = code_index_get_file(buffer);
        if (file != 0){
            for (i32 i = 0; i < file->note_array.count; i += 1){
                Code_Index_Note *note = file->note_array.ptrs[i];
                Tiny_Jump *jump = push_array(scratch, Tiny_Jump, 1);
                jump->buffer = buffer;
                jump->pos = note->pos.first;

                String_Const_u8 sort = {};
                switch (note->note_kind){
                    case CodeIndexNote_Type:
                    {
                        sort = string_u8_litexpr("type");
                    }break;
                    case CodeIndexNote_Function:
                    {
                        sort = string_u8_litexpr("function");
                    }break;
                    case CodeIndexNote_Macro:
                    {
                        sort = string_u8_litexpr("macro");
                    }break;
                }
                lister_add_item(lister, note->text, sort, jump, 0);
            }
        }
    }
    code_index_unlock();

    Lister_Result l_result = run_lister(app, lister);
    Tiny_Jump result = {};
    if (!l_result.canceled && l_result.user_data != 0){
        block_copy_struct(&result, (Tiny_Jump*)l_result.user_data);
    }

    if (result.buffer != 0){
        View_ID view = get_this_ctx_view(app, Access_Always);
        point_stack_push_view_cursor(app, view);
        jump_to_location(app, view, result.buffer, result.pos);
    }
}

function b32
jump_to_definition_at_cursor(Application_Links *app, b32 same_panel) {
  View_ID view = get_active_view(app, Access_Visible);
  b32 result = false;

  if (view != 0){
    Scratch_Block scratch(app);
    String_Const_u8 query = push_token_or_word_under_active_cursor(app, scratch);

    code_index_lock();
    for (Buffer_ID buffer = get_buffer_next(app, 0, Access_Always);
         buffer != 0;
         buffer = get_buffer_next(app, buffer, Access_Always)){
      Code_Index_File *file = code_index_get_file(buffer);
      if (file != 0){
        for (i32 i = 0; i < file->note_array.count; i += 1){
          Code_Index_Note *note = file->note_array.ptrs[i];
          if (string_match(note->text, query)){
            // @Todo: Is this ok? This way other panel inherits the jump stack
            // so alt q works, not sure if thats intuitive
            if(same_panel) {
              point_stack_push_view_cursor(app, view);
            }
            View_ID target_view = view;
            if(!same_panel) {
              change_active_panel(app);
              target_view = get_active_view(app, Access_Always);
              point_stack_push_view_cursor(app, target_view);
              switch_to_existing_view(app, target_view, buffer);
            }
            jump_to_location(app, target_view, buffer, note->pos.first);
            result = true;
            goto done;
          }
        }
      }
    }
    done:;
    code_index_unlock();
  }

  return result;
}

function b32
jump_to_file_in_quotes_at_cursor(Application_Links *app, b32 same_panel) {
  b32 result = false;
  View_ID view = get_active_view(app, Access_ReadVisible);
  Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
  if (buffer_exists(app, buffer)){
    Scratch_Block scratch(app);

    i64 pos = view_get_cursor_pos(app, view);

    Range_i64 range = enclose_pos_inside_quotes(app, buffer, pos);

    String_Const_u8 quoted_name = push_buffer_range(app, scratch, buffer, range);

    String_Const_u8 file_name = push_buffer_file_name(app, scratch, buffer);
    String_Const_u8 path = string_remove_last_folder(file_name);

    if (character_is_slash(string_get_character(path, path.size - 1))){
      path = string_chop(path, 1);
    }

    String_Const_u8 new_file_name = push_u8_stringf(scratch, "%.*s/%.*s", string_expand(path), string_expand(quoted_name));
    if(same_panel) {
      point_stack_push_view_cursor(app, view);
    }
    else {
      view = get_next_view_looped_primary_panels(app, view, Access_Always);
      point_stack_push_view_cursor(app, view);
    }
    if (view != 0){
      if (view_open_file(app, view, new_file_name, true)){
        result = true;
        view_set_active(app, view);
      }
    }
  }
  return result;
}

#if 0
// Bind(jump_to_definition_at_cursor, KeyCode_W, KeyCode_Control);
// Bind(jump_to_definition_at_cursor, KeyCode_W, KeyCode_Command);
CUSTOM_UI_COMMAND_SIG(jump_to_definition_at_cursor)
CUSTOM_DOC("Jump to the first definition in the code index matching an identifier at the cursor")
{
    View_ID view = get_active_view(app, Access_Visible);

    if (view != 0){
        Scratch_Block scratch(app);
        String_Const_u8 query = push_token_or_word_under_active_cursor(app, scratch);

        code_index_lock();
        for (Buffer_ID buffer = get_buffer_next(app, 0, Access_Always);
             buffer != 0;
             buffer = get_buffer_next(app, buffer, Access_Always)){
            Code_Index_File *file = code_index_get_file(buffer);
            if (file != 0){
                for (i32 i = 0; i < file->note_array.count; i += 1){
                    Code_Index_Note *note = file->note_array.ptrs[i];
                    if (string_match(note->text, query)){
                        point_stack_push_view_cursor(app, view);
                        jump_to_location(app, view, buffer, note->pos.first);
                        goto done;
                    }
                }
            }
        }
        done:;
        code_index_unlock();
    }
}
#endif

CUSTOM_UI_COMMAND_SIG(jump_to_thing_at_cursor_other_panel)
CUSTOM_DOC("Jump to the first definition in the code index matching an identifier at the cursor")
{
  if(!jump_to_definition_at_cursor(app, false)) {
    if(!jump_to_file_in_quotes_at_cursor(app, false)) {
      goto_jump_at_cursor(app);
    }
  }
}

CUSTOM_UI_COMMAND_SIG(jump_to_thing_at_cursor)
CUSTOM_DOC("Jump to the first definition in the code index matching an identifier at the cursor")
{
  if(!jump_to_definition_at_cursor(app, true)) {
    if(!jump_to_file_in_quotes_at_cursor(app, true)) {
      goto_jump_at_cursor_same_panel(app);
    }
  }
}


// BOTTOM

