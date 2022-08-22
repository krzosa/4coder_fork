/*
4coder_default_hooks.cpp - Sets up the hooks for the default framework.
*/

// TOP

CUSTOM_COMMAND_SIG(reload_config);
CUSTOM_COMMAND_SIG(load_project_file);


CUSTOM_COMMAND_SIG(default_startup)
CUSTOM_DOC("Default command for responding to a startup event")
{
    ProfileScope(app, "default startup");
    User_Input input = get_current_input(app);
    if (match_core_code(&input, CoreCode_Startup)){
        String_Const_u8_Array file_names = input.event.core.file_names;
        // CLEANUP(Krzosa)
        // load_themes_default_folder(app);


        Scratch_Block scratch(app);


        extern Arena config_string_arena;
        config_string_arena = make_arena_system(MB(1));
        reload_config(app);
        load_project_file(app);

        //
        // Apply config
        //
        String_Const_u8 mode = config_mode;
        change_mode(app, mode);

        b32 lalt_lctrl_is_altgr = config_lalt_lctrl_is_altgr;
        global_set_setting(app, GlobalSetting_LAltLCtrlIsAltGr, lalt_lctrl_is_altgr);

        // Themes
        // String_Const_u8 default_theme_name = def_get_config_string(scratch, vars_save_string_lit("default_theme_name"));
        // Color_Table *colors = get_color_table_by_name(default_theme_name);
        // set_active_color(colors);

        Face_Description description = {};
        description.parameters.pt_size = (i32)config_default_font_size;
        description.parameters.hinting = config_default_font_hinting;
        description.parameters.aa_mode = FaceAntialiasingMode_8BitMono;
        description.font.file_name = config_default_font_name;

        if (!modify_global_face_by_description(app, description)){
            String8 name_in_fonts_folder = push_u8_stringf(scratch, "fonts/%.*s", string_expand(description.font.file_name));
            description.font.file_name = def_search_normal_full_path(scratch, name_in_fonts_folder);
            modify_global_face_by_description(app, description);
        }

        b32 bind_by_physical_key = config_bind_by_physical_key;
        if (bind_by_physical_key){
            system_set_key_mode(KeyMode_Physical);
        }
        else{
            system_set_key_mode(KeyMode_LanguageArranged);
        }


        // CLEANUP(Krzosa)
        // TODO(Krzosa) Load config, load theme, load keybindings they overwrite so can load here
        // Example:
        // String_ID global_map_id = vars_save_string_lit("keys_global");
        // String_ID file_map_id = vars_save_string_lit("keys_file");
        // String_ID code_map_id = vars_save_string_lit("keys_code");
        // setup_built_in_mapping(app, mapping, &framework_mapping, global_map_id, file_map_id, code_map_id);

        // open command line files
        String_Const_u8 hot_directory = push_hot_directory(app, scratch);
        for (i32 i = 0; i < file_names.count; i += 1){
            Temp_Memory_Block temp(scratch);
            String_Const_u8 input_name = file_names.vals[i];
            String_Const_u8 full_name = push_u8_stringf(scratch, "%.*s/%.*s",
                                                        string_expand(hot_directory),
                                                        string_expand(input_name));
            Buffer_ID new_buffer = create_buffer(app, full_name, BufferCreate_NeverNew|BufferCreate_MustAttachToFile);
            if (new_buffer == 0){
                create_buffer(app, input_name, 0);
            }
        }

        if(app->cmd_context->settings.open_code_in_current_dir){
            open_code_files_in_current_directory(app);
        }


        //
        // Set panel layout
        //
        {
            Buffer_Identifier left  = buffer_identifier(string_u8_litexpr("*scratch*"));
            Buffer_Identifier compilation  = buffer_identifier(string_u8_litexpr("*compilation*"));
            Buffer_Identifier right = buffer_identifier(string_u8_litexpr("*messages*"));

            Buffer_ID compilation_id = buffer_identifier_to_id(app, compilation);
            Buffer_ID left_id = buffer_identifier_to_id(app, left);
            Buffer_ID right_id = buffer_identifier_to_id(app, right);

            // Left Panel
            View_ID view = get_active_view(app, Access_Always);
            new_view_settings(app, view);
            view_set_buffer(app, view, left_id, 0);

            View_ID compilation_view = open_view(app, view, ViewSplit_Bottom);
            {
                view_set_setting(app, compilation_view, ViewSetting_ShowFileBar, false);
                view_set_setting(app, compilation_view, ViewSetting_ColorModification, true);
                Buffer_ID buffer = view_get_buffer(app, compilation_view, Access_Always);
                Face_ID face_id = get_face_id(app, buffer);
                Face_Metrics metrics = get_face_metrics(app, face_id);
                view_set_split_pixel_size(app, compilation_view, (i32)(metrics.line_height*4.f));
                view_set_passive(app, compilation_view, true);
                global_compilation_view = compilation_view;
                view_set_buffer(app, compilation_view, compilation_id, 0);
            }


            // Right Panel
            View_ID right_view = open_view(app, view, ViewSplit_Left);
            {
                view_set_buffer(app, right_view, right_id, 0);
            }

            // Restore Active to Left
            view_set_active(app, view);
        }
    }

    {
        def_enable_virtual_whitespace = config_enable_virtual_whitespace;
        clear_all_layouts(app);
    }
}

CUSTOM_COMMAND_SIG(default_try_exit)
CUSTOM_DOC("Default command for responding to a try-exit event")
{
    User_Input input = get_current_input(app);
    if (match_core_code(&input, CoreCode_TryExit)){
        b32 do_exit = true;
        if (!allow_immediate_close_without_checking_for_changes){
            b32 has_unsaved_changes = false;
            for (Buffer_ID buffer = get_buffer_next(app, 0, Access_Always);
                 buffer != 0;
                 buffer = get_buffer_next(app, buffer, Access_Always)){
                Dirty_State dirty = buffer_get_dirty_state(app, buffer);
                if (HasFlag(dirty, DirtyState_UnsavedChanges)){
                    has_unsaved_changes = true;
                    break;
                }
            }
            if (has_unsaved_changes){
                View_ID view = get_active_view(app, Access_Always);
                do_exit = do_4coder_close_user_check(app, view);
            }
        }
        if (do_exit){
            hard_exit(app);
        }
    }
}

function Implicit_Map_Result
default_implicit_map(App *app, String_ID lang, String_ID mode, Input_Event *event){
    Implicit_Map_Result result = {};

    View_ID view = get_this_ctx_view(app, Access_Always);

    Command_Map_ID map_id = default_get_map_id(app, view);
    Command_Binding binding = map_get_binding_recursive(&framework_mapping, map_id, event);

    // TODO(allen): map_id <-> map name?
    result.map = 0;
    result.command = binding.custom;

    return(result);
}

CUSTOM_COMMAND_SIG(default_view_input_handler)
CUSTOM_DOC("Input consumption loop for default view behavior")
{
    Scratch_Block scratch(app);
    default_input_handler_init(app, scratch);

    View_ID view = get_this_ctx_view(app, Access_Always);
    Managed_Scope scope = view_get_managed_scope(app, view);

    for (;;){
        // NOTE(allen): Get input
        User_Input input = get_next_input(app, EventPropertyGroup_Any, 0);
        if (input.abort){
            break;
        }

        ProfileScopeNamed(app, "before view input", view_input_profile);

        // NOTE(allen): Mouse Suppression
        Event_Property event_properties = get_event_properties(&input.event);
        if (suppressing_mouse && (event_properties & EventPropertyGroup_AnyMouseEvent) != 0){
            continue;
        }

        // NOTE(allen): Get binding
        if (implicit_map_function == 0){
            implicit_map_function = default_implicit_map;
        }
        Implicit_Map_Result map_result = implicit_map_function(app, 0, 0, &input.event);
        if (map_result.command == 0){
            leave_current_input_unhandled(app);
            continue;
        }

        // NOTE(allen): Run the command and pre/post command stuff
        default_pre_command(app, scope);
        ProfileCloseNow(view_input_profile);
        map_result.command(app);
        ProfileScope(app, "after view input");
        default_post_command(app, scope);
    }
}

function void
code_index_update_tick(App *app){
    Scratch_Block scratch(app);
    for (Buffer_Modified_Node *node = global_buffer_modified_set.first;
         node != 0;
         node = node->next){
        Temp_Memory_Block temp(scratch);
        Buffer_ID buffer_id = node->buffer;

        String_Const_u8 contents = push_whole_buffer(app, scratch, buffer_id);
        Token_Array tokens = get_token_array_from_buffer(app, buffer_id);
        if (tokens.count == 0){
            continue;
        }

        Arena arena = make_arena_system(KB(16));
        Code_Index_File *index = push_array_zero(&arena, Code_Index_File, 1);
        index->buffer = buffer_id;

        Generic_Parse_State state = {};
        generic_parse_init(app, &arena, contents, &tokens, &state);
        // TODO(allen): Actually determine this in a fair way.
        // Maybe switch to an enum?
        // Actually probably a pointer to a struct that defines the language.
        state.do_cpp_parse = true;
        generic_parse_full_input_breaks(index, &state, max_i32);

        code_index_lock();
        code_index_set_file(buffer_id, arena, index);
        code_index_unlock();
        buffer_clear_layout_cache(app, buffer_id);
    }

    buffer_modified_set_clear();
}

function void
default_tick(App *app, Frame_Info frame_info){
    ////////////////////////////////
    // NOTE(allen): Update code index

    code_index_update_tick(app);

    ////////////////////////////////
    // NOTE(allen): Update fade ranges

    if (tick_all_fade_ranges(app, frame_info.animation_dt)){
        animate_in_n_milliseconds(app, 0);
    }

    ////////////////////////////////
    // NOTE(allen): Clear layouts if virtual whitespace setting changed.

    {
        b32 enable_virtual_whitespace = config_enable_virtual_whitespace;
        if (enable_virtual_whitespace != def_enable_virtual_whitespace){
            def_enable_virtual_whitespace = enable_virtual_whitespace;
            clear_all_layouts(app);
        }
    }
}

function Rect_f32
default_buffer_region(App *app, View_ID view_id, Rect_f32 region){
    Buffer_ID buffer = view_get_buffer(app, view_id, Access_Always);
    Face_ID face_id = get_face_id(app, buffer);
    Face_Metrics metrics = get_face_metrics(app, face_id);
    f32 line_height = metrics.line_height;
    f32 digit_advance = metrics.decimal_digit_advance;

    // NOTE(allen): margins
    region = rect_inner(region, config_background_margin_width);

    // NOTE(allen): file bar
    b64 showing_file_bar = false;
    if (view_get_setting(app, view_id, ViewSetting_ShowFileBar, &showing_file_bar) &&
        showing_file_bar){
        Rect_f32_Pair pair = layout_file_bar_on_top(region, line_height);
        region = pair.max;
    }

    // NOTE(allen): query bars
    {
        Query_Bar *space[32];
        Query_Bar_Ptr_Array query_bars = {};
        query_bars.ptrs = space;
        if (get_active_query_bars(app, view_id, ArrayCount(space), &query_bars)){
            Rect_f32_Pair pair = layout_query_bar_on_top(region, line_height, query_bars.count);
            region = pair.max;
        }
    }

    // NOTE(allen): FPS hud
    if (show_fps_hud){
        Rect_f32_Pair pair = layout_fps_hud_on_bottom(region, line_height);
        region = pair.min;
    }

    // NOTE(allen): line numbers
    b32 show_line_number_margins = config_show_line_number_margins;
    if (show_line_number_margins){
        Rect_f32_Pair pair = layout_line_number_margin(app, buffer, region, digit_advance);
        region = pair.max;
    }

    return(region);
}

function void
recursive_nest_highlight(App *app, Text_Layout_ID layout_id, Range_i64 range,
                         Code_Index_Nest_Ptr_Array *array, i32 counter){
    Code_Index_Nest **ptr = array->ptrs;
    Code_Index_Nest **ptr_end = ptr + array->count;

    for (;ptr < ptr_end; ptr += 1){
        Code_Index_Nest *nest = *ptr;
        if (!nest->is_closed){
            break;
        }
        if (range.first <= nest->close.max){
            break;
        }
    }

    ARGB_Color argb = finalize_color(defcolor_text_cycle, counter);
    argb &= ~0x33000000;

    for (;ptr < ptr_end; ptr += 1){
        Code_Index_Nest *nest = *ptr;
        if (range.max <= nest->open.min){
            break;
        }

        paint_text_color(app, layout_id, nest->open, argb);
        if (nest->is_closed){
            paint_text_color(app, layout_id, nest->close, argb);
        }
        recursive_nest_highlight(app, layout_id, range, &nest->nest_array, counter + 1);
    }
}

function void
recursive_nest_highlight(App *app, Text_Layout_ID layout_id, Range_i64 range, Code_Index_File *file){
    recursive_nest_highlight(app, layout_id, range, &file->nest_array, 0);
}

function void
default_render_buffer(App *app, View_ID view_id, Face_ID face_id,
                      Buffer_ID buffer, Text_Layout_ID text_layout_id,
                      Rect_f32 rect){
    ProfileScope(app, "render buffer");

    Scratch_Block scratch(app);
    View_ID active_view = get_active_view(app, Access_Always);
    b32 is_active_view = (active_view == view_id);
    Rect_f32 prev_clip = draw_set_clip(app, rect);

    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);

    // NOTE(allen): Cursor shape
    Face_Metrics metrics = get_face_metrics(app, face_id);
    u64 cursor_roundness_100 = config_cursor_roundness;
    f32 cursor_roundness = metrics.normal_advance*cursor_roundness_100*0.01f;
    f32 mark_thickness = (f32)config_mark_thickness;

    i64 cursor_pos = view_correct_cursor(app, view_id);
    view_correct_mark(app, view_id);

    Mouse_State mouse_state = get_mouse_state(app);
    Vec2_f32 mouse_screen_pos = {(f32)mouse_state.x, (f32)mouse_state.y};
    i64 mouse_view_pos = view_pos_from_xy(app, active_view, mouse_screen_pos);
    Rect_f32 view_rect = view_get_screen_rect(app, active_view);
    b32 mouse_is_on_active_view = rect_contains_point(view_rect, mouse_screen_pos);


    // NOTE(allen): Line highlight
    b32 highlight_line_at_cursor = config_highlight_line_at_cursor;
    if (highlight_line_at_cursor && is_active_view){
        i64 line_number = get_line_number_from_pos(app, buffer, cursor_pos);
        draw_line_highlight(app, text_layout_id, line_number, fcolor_id(defcolor_highlight_cursor_line));
    }

    // NOTE(allen): Token colorizing
    Token_Array token_array = get_token_array_from_buffer(app, buffer);
    if (token_array.tokens != 0){
        draw_cpp_token_colors(app, text_layout_id, &token_array);

        // NOTE(allen): Scan for TODOs and NOTEs
        b32 use_comment_keyword = config_use_comment_keywords;
        if (use_comment_keyword){
            Comment_Highlight_Pair pairs[] = {
                {string_u8_litexpr("NOTE"), finalize_color(defcolor_comment_pop, 0)},
                {string_u8_litexpr("TODO"), finalize_color(defcolor_comment_pop, 1)},
            };
            draw_comment_highlights(app, buffer, text_layout_id, &token_array, pairs, ArrayCount(pairs));
        }

        // NOTE(allen): Color functions
        Scratch_Block scratch(app);

        ARGB_Color argb = 0xFFFF00FF;
        Token_Iterator_Array it = token_iterator_pos(0, &token_array, visible_range.first);
        for (;;){
            b32 underline_token = false;
            if (!token_it_inc_non_whitespace(&it)){
                break;
            }

            Token *token = token_it_read(&it);
            Range_i64 range = Ii64_size(token->pos, token->size);
            b32 cursor_is_over_token = range_contains(range, cursor_pos);
            b32 mouse_is_over_token = mouse_is_on_active_view && range_contains(range, mouse_view_pos);

            String_Const_u8 lexeme = push_token_lexeme(app, scratch, buffer, token);
            Code_Index_Note *note = code_index_note_from_string(lexeme);
            if(note){
                if(note->note_kind == CodeIndexNote_Function){
                    argb = finalize_color(defcolor_function, 0);
                } else if(note->note_kind == CodeIndexNote_Type){
                    argb = finalize_color(defcolor_type, 0);
                } else if(note->note_kind == CodeIndexNote_Macro){
                    argb = finalize_color(defcolor_macro, 0);
                }

                paint_text_color(app, text_layout_id, range, argb);
                b32 is_definition_of_note = (note->file->buffer == buffer && range_contains(range, note->pos.min));

                // Outline when token is clickable, avoid a case where we outline a definition
                underline_token = !is_definition_of_note;
            }

            if(token->kind == TokenBaseKind_LiteralString){
                if(token->sub_kind == 41){
                    argb = finalize_color(defcolor_str_constant, 0);
                    String_Const_u8 quoted_name = push_buffer_range(app, scratch, buffer, Ii64_size(token->pos+1, token->size-2));
                    if(quoted_name.size){
                        String_Const_u8 new_file_name = construct_relative_path_in_the_same_folder_as_buffer(app, scratch, buffer, quoted_name);
                        if(file_exists(app, new_file_name)){
                            underline_token = true;
                        }
                    }
                }
            }

            if(underline_token && is_active_view){
                // Draw outline below clickable char
                Rect_f32 range_start_rect = text_layout_character_on_screen(app, text_layout_id, range.min);
                Rect_f32 range_end_rect = text_layout_character_on_screen(app, text_layout_id, range.max-1);

                Rect_f32 total_range_rect = {0};
                total_range_rect.y0 = Min(range_start_rect.y0, range_end_rect.y0);
                total_range_rect.x0 = Min(range_start_rect.x0, range_end_rect.x0);
                total_range_rect.x1 = Max(range_start_rect.x1, range_end_rect.x1);
                total_range_rect.y1 = Max(range_start_rect.y1, range_end_rect.y1);

                f32 scale = 0.8f;
                if(cursor_is_over_token){
                    scale += 3.f;
                }

                // Emphesise hiperlink over clickable link
                if(mouse_is_over_token){
                    scale += 3.f;
                }

                if(scale < 1.f){
                    total_range_rect.x1 = total_range_rect.x0 + metrics.space_advance;
                }

                total_range_rect.y0 = total_range_rect.y1 - scale;
                total_range_rect.y1 += scale;

                draw_rectangle(app, total_range_rect, 4.f, argb);
            }
        }
    }
    else{
        paint_text_color_fcolor(app, text_layout_id, visible_range, fcolor_id(defcolor_text_default));
    }


    // NOTE(Krzosa): Color tokens based on the scope
    Code_Index_File *code_index_file = code_index_get_file(buffer);
    if(code_index_file) recursive_nest_highlight(app, text_layout_id, visible_range, code_index_file);

    // NOTE(allen): Scope highlight
    b32 use_scope_highlight = config_use_scope_highlight;
    if (use_scope_highlight){
        Color_Array colors = finalize_color_array(defcolor_back_cycle);
        draw_scope_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
    }

    b32 use_error_highlight = config_use_error_highlight;
    b32 use_jump_highlight = config_use_jump_highlight;
    if (use_error_highlight || use_jump_highlight){
        // NOTE(allen): Error highlight
        String_Const_u8 name = string_u8_litexpr("*compilation*");
        Buffer_ID compilation_buffer = get_buffer_by_name(app, name, Access_Always);
        if (use_error_highlight){
            draw_jump_highlights(app, buffer, text_layout_id, compilation_buffer,
                                 fcolor_id(defcolor_highlight_junk));
        }

        // NOTE(allen): Search highlight
        if (use_jump_highlight){
            Buffer_ID jump_buffer = get_locked_jump_buffer(app);
            if (jump_buffer != compilation_buffer){
                draw_jump_highlights(app, buffer, text_layout_id, jump_buffer,
                                     fcolor_id(defcolor_highlight_white));
            }
        }
    }

    // NOTE(allen): Color parens
    b32 use_paren_helper = config_use_paren_helper;
    if (use_paren_helper){
        Color_Array colors = finalize_color_array(defcolor_text_cycle);
        draw_paren_highlight(app, buffer, text_layout_id, cursor_pos, colors.vals, colors.count);
    }

    // NOTE(allen): Whitespace highlight
    b64 show_whitespace = false;
    view_get_setting(app, view_id, ViewSetting_ShowWhitespace, &show_whitespace);
    if (show_whitespace){
        if (token_array.tokens == 0){
            draw_whitespace_highlight(app, buffer, text_layout_id, cursor_roundness);
        }
        else{
            draw_whitespace_highlight(app, text_layout_id, &token_array, cursor_roundness);
        }
    }

    // NOTE(allen): Cursor
    switch (fcoder_mode){
        case FCoderMode_Original:
        {
            draw_original_4coder_style_cursor_mark_highlight(app, view_id, is_active_view, buffer, text_layout_id, cursor_roundness, mark_thickness);
        }break;
        case FCoderMode_NotepadLike:
        {
            draw_notepad_style_cursor_highlight(app, view_id, buffer, text_layout_id, cursor_roundness);
        }break;
    }

    // NOTE(allen): Fade ranges
    paint_fade_ranges(app, text_layout_id, buffer);

    // NOTE(allen): put the actual text on the actual screen
    draw_text_layout_default(app, text_layout_id);

    draw_set_clip(app, prev_clip);

#if 0
    if(deffered_button_render.size){
        String_Const_u8 text = deffered_button_render;
        Fancy_String *fancy = push_fancy_string(scratch, 0, face_id, fcolor_id(defcolor_text_default), text);

        Face_Metrics metrics = get_face_metrics(app, face_id);
        Vec2_f32 dim = get_fancy_string_dim(app, 0, fancy);
        Rect_f32 button_rect = Rf32_xy_wh({view_rect.x1 - dim.x, metrics.line_height}, dim + V2f32(2,2));

        ARGB_Color background_color = fcolor_resolve(fcolor_id(defcolor_back));
        ARGB_Color border_color = fcolor_resolve(fcolor_id(defcolor_margin_active));

        background_color &= 0x00ffffff;
        background_color |= 0xd0000000;

        border_color &= 0x00ffffff;
        border_color |= 0xd0000000;

        f32 roundness = 4.f;
        draw_rectangle(app, button_rect, roundness, background_color);
        draw_rectangle_outline(app, button_rect, roundness, 3.f, border_color);

        Vec2_f32 p = (button_rect.p0 + button_rect.p1 - dim)*0.5f;
        draw_fancy_string(app, fancy, p);
    }
#endif
}

function Rect_f32
default_draw_query_bars(App *app, Rect_f32 region, View_ID view_id, Face_ID face_id){
    Face_Metrics face_metrics = get_face_metrics(app, face_id);
    f32 line_height = face_metrics.line_height;

    Query_Bar *space[32];
    Query_Bar_Ptr_Array query_bars = {};
    query_bars.ptrs = space;
    if (get_active_query_bars(app, view_id, ArrayCount(space), &query_bars)){
        for (i32 i = 0; i < query_bars.count; i += 1){
            Rect_f32_Pair pair = layout_query_bar_on_top(region, line_height, 1);
            draw_query_bar(app, query_bars.ptrs[i], face_id, pair.min);
            region = pair.max;
        }
    }
    return(region);
}

function void
default_render_caller(App *app, Frame_Info frame_info, View_ID view_id){
    ProfileScope(app, "default render caller");
    Rect_f32 region = draw_background_and_margin(app, view_id);
    Rect_f32 prev_clip = draw_set_clip(app, region);

    Buffer_ID buffer = view_get_buffer(app, view_id, Access_Always);
    Face_ID face_id = get_face_id(app, buffer);
    Face_Metrics face_metrics = get_face_metrics(app, face_id);
    f32 line_height = face_metrics.line_height;
    f32 digit_advance = face_metrics.decimal_digit_advance;

    // NOTE(allen): file bar
    b64 showing_file_bar = false;
    if (view_get_setting(app, view_id, ViewSetting_ShowFileBar, &showing_file_bar) && showing_file_bar){
        Rect_f32_Pair pair = layout_file_bar_on_top(region, line_height);
        draw_file_bar(app, view_id, buffer, face_id, pair.min);
        region = pair.max;
    }

    Buffer_Scroll scroll = view_get_buffer_scroll(app, view_id);

    Buffer_Point_Delta_Result delta = delta_apply(app, view_id,
                                                  frame_info.animation_dt, scroll);
    if (!block_match_struct(&scroll.position, &delta.point)){
        block_copy_struct(&scroll.position, &delta.point);
        view_set_buffer_scroll(app, view_id, scroll, SetBufferScroll_NoCursorChange);
    }
    if (delta.still_animating){
        animate_in_n_milliseconds(app, 0);
    }

    // NOTE(allen): query bars
    region = default_draw_query_bars(app, region, view_id, face_id);

    // NOTE(allen): FPS hud
    if (show_fps_hud){
        Rect_f32_Pair pair = layout_fps_hud_on_bottom(region, line_height);
        draw_fps_hud(app, frame_info, face_id, pair.max);
        region = pair.min;
        animate_in_n_milliseconds(app, 1000);
    }

    // NOTE(allen): layout line numbers
    b32 show_line_number_margins = config_show_line_number_margins;
    Rect_f32 line_number_rect = {};
    if (show_line_number_margins){
        Rect_f32_Pair pair = layout_line_number_margin(app, buffer, region, digit_advance);
        line_number_rect = pair.min;
        region = pair.max;
    }

    // NOTE(allen): begin buffer render
    Buffer_Point buffer_point = scroll.position;
    Text_Layout_ID text_layout_id = text_layout_create(app, buffer, region, buffer_point);

    // NOTE(allen): draw line numbers
    if (show_line_number_margins){
        draw_line_number_margin(app, view_id, buffer, face_id, text_layout_id, line_number_rect);
    }

    // NOTE(allen): draw the buffer
    default_render_buffer(app, view_id, face_id, buffer, text_layout_id, region);

    text_layout_free(app, text_layout_id);
    draw_set_clip(app, prev_clip);
}

function void
default_whole_screen_render_caller(App *app, Frame_Info frame_info){
#if 0
    Rect_f32 region = global_get_screen_rectangle(app);
    Vec2_f32 center = rect_center(region);

    Face_ID face_id = get_face_id(app, 0);
    Scratch_Block scratch(app);
    draw_string_oriented(app, face_id, finalize_color(defcolor_text_default, 0),
                         SCu8("Hello, World!"), center - V2f32(200.f, 300.f),
                         0, V2f32(0.f, -1.f));
    draw_string_oriented(app, face_id, finalize_color(defcolor_text_default, 0),
                         SCu8("Hello, World!"), center - V2f32(240.f, 300.f),
                         0, V2f32(0.f, 1.f));
    draw_string_oriented(app, face_id, finalize_color(defcolor_text_default, 0),
                         SCu8("Hello, World!"), center - V2f32(400.f, 400.f),
                         0, V2f32(-1.f, 0.f));
    draw_string_oriented(app, face_id, finalize_color(defcolor_text_default, 0),
                         SCu8("Hello, World!"), center - V2f32(400.f, -100.f),
                         0, V2f32(cos_f32(pi_f32*.333f), sin_f32(pi_f32*.333f)));
#endif
}

HOOK_SIG(default_view_adjust){
    // NOTE(allen): Called whenever the view layout/sizes have been modified,
    // including by full window resize.
    return(0);
}

BUFFER_NAME_RESOLVER_SIG(default_buffer_name_resolution){
    ProfileScope(app, "default buffer name resolution");
    if (conflict_count > 1){
        // List of unresolved conflicts
        Scratch_Block scratch(app);

        i32 *unresolved = push_array(scratch, i32, conflict_count);
        i32 unresolved_count = conflict_count;
        for (i32 i = 0; i < conflict_count; ++i){
            unresolved[i] = i;
        }

        // Resolution Loop
        i32 x = 0;
        for (;;){
            // Resolution Pass
            ++x;
            for (i32 i = 0; i < unresolved_count; ++i){
                i32 conflict_index = unresolved[i];
                Buffer_Name_Conflict_Entry *conflict = &conflicts[conflict_index];

                u64 size = conflict->base_name.size;
                size = clamp_top(size, conflict->unique_name_capacity);
                conflict->unique_name_len_in_out = size;
                block_copy(conflict->unique_name_in_out, conflict->base_name.str, size);

                if (conflict->file_name.str != 0){
                    Temp_Memory_Block temp(scratch);
                    String_Const_u8 uniqueifier = {};

                    String_Const_u8 file_name = string_remove_last_folder(conflict->file_name);
                    if (file_name.size > 0){
                        file_name = string_chop(file_name, 1);
                        u8 *end = file_name.str + file_name.size;
                        b32 past_the_end = false;
                        for (i32 j = 0; j < x; ++j){
                            file_name = string_remove_last_folder(file_name);
                            if (j + 1 < x){
                                file_name = string_chop(file_name, 1);
                            }
                            if (file_name.size == 0){
                                if (j + 1 < x){
                                    past_the_end = true;
                                }
                                break;
                            }
                        }
                        u8 *start = file_name.str + file_name.size;

                        uniqueifier = SCu8(start, end);
                        if (past_the_end){
                            uniqueifier = push_u8_stringf(scratch, "%.*s~%d",
                                                          string_expand(uniqueifier), i);
                        }
                    }
                    else{
                        uniqueifier = push_u8_stringf(scratch, "%d", i);
                    }

                    String_u8 builder = Su8(conflict->unique_name_in_out,
                                            conflict->unique_name_len_in_out,
                                            conflict->unique_name_capacity);
                    string_append(&builder, string_u8_litexpr(" <"));
                    string_append(&builder, uniqueifier);
                    string_append(&builder, string_u8_litexpr(">"));
                    conflict->unique_name_len_in_out = builder.size;
                }
            }

            // Conflict Check Pass
            b32 has_conflicts = false;
            for (i32 i = 0; i < unresolved_count; ++i){
                i32 conflict_index = unresolved[i];
                Buffer_Name_Conflict_Entry *conflict = &conflicts[conflict_index];
                String_Const_u8 conflict_name = SCu8(conflict->unique_name_in_out,
                                                     conflict->unique_name_len_in_out);

                b32 hit_conflict = false;
                if (conflict->file_name.str != 0){
                    for (i32 j = 0; j < unresolved_count; ++j){
                        if (i == j) continue;

                        i32 conflict_j_index = unresolved[j];
                        Buffer_Name_Conflict_Entry *conflict_j = &conflicts[conflict_j_index];

                        String_Const_u8 conflict_name_j = SCu8(conflict_j->unique_name_in_out,
                                                               conflict_j->unique_name_len_in_out);

                        if (string_match(conflict_name, conflict_name_j)){
                            hit_conflict = true;
                            break;
                        }
                    }
                }

                if (hit_conflict){
                    has_conflicts = true;
                }
                else{
                    --unresolved_count;
                    unresolved[i] = unresolved[unresolved_count];
                    --i;
                }
            }

            if (!has_conflicts){
                break;
            }
        }
    }
}

function void
do_full_lex_async__inner(Async_Context *actx, Buffer_ID buffer_id){
    App *app = actx->app;
    ProfileScope(app, "async lex");
    Scratch_Block scratch(app);

    String_Const_u8 contents = {};
    {
        ProfileBlock(app, "async lex contents (before mutex)");
        acquire_global_frame_mutex(app);
        ProfileBlock(app, "async lex contents (after mutex)");
        contents = push_whole_buffer(app, scratch, buffer_id);
        release_global_frame_mutex(app);
    }

    i32 limit_factor = 10000;

    Token_List list = {};
    b32 canceled = false;

    Lex_State_Cpp state = {};
    lex_full_input_cpp_init(&state, contents);
    for (;;){
        ProfileBlock(app, "async lex block");
        if (lex_full_input_cpp_breaks(scratch, &list, &state, limit_factor)){
            break;
        }
        if (async_check_canceled(actx)){
            canceled = true;
            break;
        }
    }

    if (!canceled){
        ProfileBlock(app, "async lex save results (before mutex)");
        acquire_global_frame_mutex(app);
        ProfileBlock(app, "async lex save results (after mutex)");
        Managed_Scope scope = buffer_get_managed_scope(app, buffer_id);
        if (scope != 0){
            Base_Allocator *allocator = managed_scope_allocator(app, scope);
            Token_Array *tokens_ptr = scope_attachment(app, scope, attachment_tokens, Token_Array);
            base_free(allocator, tokens_ptr->tokens);
            Token_Array tokens = {};
            tokens.tokens = base_array(allocator, Token, list.total_count);
            tokens.count = list.total_count;
            tokens.max = list.total_count;
            token_fill_memory_from_list(tokens.tokens, &list);
            block_copy_struct(tokens_ptr, &tokens);
        }
        buffer_mark_as_modified(buffer_id);
        release_global_frame_mutex(app);
    }
}

function void
do_full_lex_async(Async_Context *actx, String_Const_u8 data){
    if (data.size == sizeof(Buffer_ID)){
        Buffer_ID buffer = *(Buffer_ID*)data.str;
        do_full_lex_async__inner(actx, buffer);
    }
}

function String_Const_u8_Array
parse_extension_line_to_extension_list(App *app, Arena *arena, String_Const_u8 str){
    ProfileScope(app, "parse extension line to extension list");
    i32 count = 0;
    for (u64 i = 0; i < str.size; i += 1){
        if (str.str[i] == '.'){
            count += 1;
        }
    }

    String_Const_u8_Array array = {};
    array.count = count;
    array.strings = push_array(arena, String_Const_u8, count);

    push_align(arena, 1);
    str = string_skip(str, string_find_first(str, '.') + 1);
    for (i32 i = 0; i < count; i += 1){
        u64 next_period = string_find_first(str, '.');
        String_Const_u8 extension = string_prefix(str, next_period);
        str = string_skip(str, next_period + 1);
        array.strings[i] = push_string_copy(arena, extension);
    }
    push_align(arena, 8);

    return(array);
}

BUFFER_HOOK_SIG(default_begin_buffer){
    ProfileScope(app, "begin buffer");

    Scratch_Block scratch(app);

    b32 treat_as_code = false;
    String_Const_u8 file_name = push_buffer_file_name(app, scratch, buffer_id);
    if (file_name.size > 0){
        String_Const_u8 treat_as_code_string = config_treat_as_code;
        String_Const_u8_Array extensions = parse_extension_line_to_extension_list(app, scratch, treat_as_code_string);
        String_Const_u8 ext = string_file_extension(file_name);
        for (i32 i = 0; i < extensions.count; ++i){
            if (string_match(ext, extensions.strings[i])){

                if (string_match(ext, string_u8_litexpr("cpp")) ||
                    string_match(ext, string_u8_litexpr("h")) ||
                    string_match(ext, string_u8_litexpr("c")) ||
                    string_match(ext, string_u8_litexpr("hpp")) ||
                    string_match(ext, string_u8_litexpr("cc"))){
                    treat_as_code = true;
                }
#if 0
                treat_as_code = true;

                if (string_match(ext, string_u8_litexpr("cs"))){
                    if (parse_context_language_cs == 0){
                        init_language_cs(app);
                    }
                    parse_context_id = parse_context_language_cs;
                }

                if (string_match(ext, string_u8_litexpr("java"))){
                    if (parse_context_language_java == 0){
                        init_language_java(app);
                    }
                    parse_context_id = parse_context_language_java;
                }

                if (string_match(ext, string_u8_litexpr("rs"))){
                    if (parse_context_language_rust == 0){
                        init_language_rust(app);
                    }
                    parse_context_id = parse_context_language_rust;
                }

                if (string_match(ext, string_u8_litexpr("cpp")) ||
                    string_match(ext, string_u8_litexpr("h")) ||
                    string_match(ext, string_u8_litexpr("c")) ||
                    string_match(ext, string_u8_litexpr("hpp")) ||
                    string_match(ext, string_u8_litexpr("cc"))){
                    if (parse_context_language_cpp == 0){
                        init_language_cpp(app);
                    }
                    parse_context_id = parse_context_language_cpp;
                }

                // TODO(NAME): Real GLSL highlighting
                if (string_match(ext, string_u8_litexpr("glsl"))){
                    if (parse_context_language_cpp == 0){
                        init_language_cpp(app);
                    }
                    parse_context_id = parse_context_language_cpp;
                }

                // TODO(NAME): Real Objective-C highlighting
                if (string_match(ext, string_u8_litexpr("m"))){
                    if (parse_context_language_cpp == 0){
                        init_language_cpp(app);
                    }
                    parse_context_id = parse_context_language_cpp;
                }
#endif

                break;
            }
        }
    }

    String_ID file_map_id = vars_save_string_lit("keys_file");
    String_ID code_map_id = vars_save_string_lit("keys_code");

    Command_Map_ID map_id = (treat_as_code)?(code_map_id):(file_map_id);
    Managed_Scope scope = buffer_get_managed_scope(app, buffer_id);
    Command_Map_ID *map_id_ptr = scope_attachment(app, scope, buffer_map_id, Command_Map_ID);
    *map_id_ptr = map_id;

    Line_Ending_Kind setting = guess_line_ending_kind_from_buffer(app, buffer_id);
    Line_Ending_Kind *eol_setting = scope_attachment(app, scope, buffer_eol_setting, Line_Ending_Kind);
    *eol_setting = setting;

    // NOTE(allen): Decide buffer settings
    b32 wrap_lines = config_enable_code_wrapping;
    b32 use_lexer = false;
    if (treat_as_code){
        use_lexer = true;
    }

    String_Const_u8 buffer_name = push_buffer_base_name(app, scratch, buffer_id);
    if (buffer_name.size > 0 && buffer_name.str[0] == '*' && buffer_name.str[buffer_name.size - 1] == '*'){
        wrap_lines = config_enable_output_wrapping;
    }

    if (use_lexer){
        ProfileBlock(app, "begin buffer kick off lexer");
        Async_Task *lex_task_ptr = scope_attachment(app, scope, buffer_lex_task, Async_Task);
        *lex_task_ptr = async_task_no_dep(&global_async_system, do_full_lex_async, make_data_struct(&buffer_id));
    }

    {
        b32 *wrap_lines_ptr = scope_attachment(app, scope, buffer_wrap_lines, b32);
        *wrap_lines_ptr = wrap_lines;
    }

    if (use_lexer){
        buffer_set_layout(app, buffer_id, layout_virt_indent_index_generic);
    }
    else{
        if (treat_as_code){
            buffer_set_layout(app, buffer_id, layout_virt_indent_literal_generic);
        }
        else{
            buffer_set_layout(app, buffer_id, layout_generic);
        }
    }

    // no meaning for return
    return(0);
}

BUFFER_HOOK_SIG(default_new_file){
    Scratch_Block scratch(app);
    String_Const_u8 file_name = push_buffer_base_name(app, scratch, buffer_id);
    if (!string_match(string_postfix(file_name, 2), string_u8_litexpr(".h"))) {
        return(0);
    }

    Buffer_Insertion insert = begin_buffer_insertion_at_buffered(app, buffer_id, 0, scratch, KB(16));
    insertf(&insert, "#pragma once\n");
    end_buffer_insertion(&insert);

    return(0);
}

BUFFER_HOOK_SIG(default_file_save){
    ProfileScope(app, "default file save");

    b32 auto_indent = config_automatically_indent_text_on_save;
    b32 is_virtual = config_enable_virtual_whitespace;
    if (auto_indent && is_virtual){
        auto_indent_buffer(app, buffer_id, buffer_range(app, buffer_id));
    }

    clean_all_lines_buffer(app, buffer_id, CleanAllLinesMode_RemoveBlankLines);
    Managed_Scope scope = buffer_get_managed_scope(app, buffer_id);
    Line_Ending_Kind *eol = scope_attachment(app, scope, buffer_eol_setting, Line_Ending_Kind);
    switch (*eol){
        case LineEndingKind_LF:
        {
            rewrite_lines_to_lf(app, buffer_id);
        }break;
        case LineEndingKind_CRLF:
        {
            rewrite_lines_to_crlf(app, buffer_id);
        }break;
    }

    // no meaning for return
    return(0);
}

BUFFER_EDIT_RANGE_SIG(default_buffer_edit_range){
    // buffer_id, new_range, original_size
    ProfileScope(app, "default edit range");

    Range_i64 old_range = Ii64(old_cursor_range.min.pos, old_cursor_range.max.pos);

    buffer_shift_fade_ranges(buffer_id, old_range.max, (new_range.max - old_range.max));

    {
        code_index_lock();
        Code_Index_File *file = code_index_get_file(buffer_id);
        if (file != 0){
            code_index_shift(file, old_range, range_size(new_range));
        }
        code_index_unlock();
    }

    i64 insert_size = range_size(new_range);
    i64 text_shift = replace_range_shift(old_range, insert_size);

    Scratch_Block scratch(app);

    Managed_Scope scope = buffer_get_managed_scope(app, buffer_id);
    Async_Task *lex_task_ptr = scope_attachment(app, scope, buffer_lex_task, Async_Task);

    Base_Allocator *allocator = managed_scope_allocator(app, scope);
    b32 do_full_relex = false;

    if (async_task_is_running_or_pending(&global_async_system, *lex_task_ptr)){
        async_task_cancel(app, &global_async_system, *lex_task_ptr);
        buffer_unmark_as_modified(buffer_id);
        do_full_relex = true;
        *lex_task_ptr = 0;
    }

    Token_Array *ptr = scope_attachment(app, scope, attachment_tokens, Token_Array);
    if (ptr != 0 && ptr->tokens != 0){
        ProfileBlockNamed(app, "attempt resync", profile_attempt_resync);

        i64 token_index_first = token_relex_first(ptr, old_range.first, 1);
        i64 token_index_resync_guess =
            token_relex_resync(ptr, old_range.one_past_last, 16);

        if (token_index_resync_guess - token_index_first >= 4000){
            do_full_relex = true;
        }
        else{
            Token *token_first = ptr->tokens + token_index_first;
            Token *token_resync = ptr->tokens + token_index_resync_guess;

            Range_i64 relex_range = Ii64(token_first->pos, token_resync->pos + token_resync->size + text_shift);
            String_Const_u8 partial_text = push_buffer_range(app, scratch, buffer_id, relex_range);

            Token_List relex_list = lex_full_input_cpp(scratch, partial_text);
            if (relex_range.one_past_last < buffer_get_size(app, buffer_id)){
                token_drop_eof(&relex_list);
            }

            Token_Relex relex = token_relex(relex_list, relex_range.first - text_shift, ptr->tokens, token_index_first, token_index_resync_guess);

            ProfileCloseNow(profile_attempt_resync);

            if (!relex.successful_resync){
                do_full_relex = true;
            }
            else{
                ProfileBlock(app, "apply resync");

                i64 token_index_resync = relex.first_resync_index;

                Range_i64 head = Ii64(0, token_index_first);
                Range_i64 replaced = Ii64(token_index_first, token_index_resync);
                Range_i64 tail = Ii64(token_index_resync, ptr->count);
                i64 resynced_count = (token_index_resync_guess + 1) - token_index_resync;
                i64 relexed_count = relex_list.total_count - resynced_count;
                i64 tail_shift = relexed_count - (token_index_resync - token_index_first);

                i64 new_tokens_count = ptr->count + tail_shift;
                Token *new_tokens = base_array(allocator, Token, new_tokens_count);

                Token *old_tokens = ptr->tokens;
                block_copy_array_shift(new_tokens, old_tokens, head, 0);
                token_fill_memory_from_list(new_tokens + replaced.first, &relex_list, relexed_count);
                for (i64 i = 0, index = replaced.first; i < relexed_count; i += 1, index += 1){
                    new_tokens[index].pos += relex_range.first;
                }
                for (i64 i = tail.first; i < tail.one_past_last; i += 1){
                    old_tokens[i].pos += text_shift;
                }
                block_copy_array_shift(new_tokens, ptr->tokens, tail, tail_shift);

                base_free(allocator, ptr->tokens);

                ptr->tokens = new_tokens;
                ptr->count = new_tokens_count;
                ptr->max = new_tokens_count;

                buffer_mark_as_modified(buffer_id);
            }
        }
    }

    if (do_full_relex){
        *lex_task_ptr = async_task_no_dep(&global_async_system, do_full_lex_async,
                                          make_data_struct(&buffer_id));
    }

    // no meaning for return
    return(0);
}

BUFFER_HOOK_SIG(default_end_buffer){
    Managed_Scope scope = buffer_get_managed_scope(app, buffer_id);
    Async_Task *lex_task_ptr = scope_attachment(app, scope, buffer_lex_task, Async_Task);
    if (lex_task_ptr != 0){
        async_task_cancel(app, &global_async_system, *lex_task_ptr);
    }
    buffer_unmark_as_modified(buffer_id);
    code_index_lock();
    code_index_erase_file(buffer_id);
    code_index_unlock();
    // no meaning for return
    return(0);
}

function void
default_view_change_buffer(App *app, View_ID view_id,
                           Buffer_ID old_buffer_id, Buffer_ID new_buffer_id){
    Managed_Scope scope = view_get_managed_scope(app, view_id);
    Buffer_ID *prev_buffer_id = scope_attachment(app, scope, view_previous_buffer, Buffer_ID);
	if (prev_buffer_id != 0){
		*prev_buffer_id = old_buffer_id;
	}
}

internal void
set_all_default_hooks(App *app){
    set_custom_hook(app, HookID_BufferViewerUpdate, default_view_adjust);

    set_custom_hook(app, HookID_ViewEventHandler, default_view_input_handler);
    set_custom_hook(app, HookID_Tick, default_tick);
    set_custom_hook(app, HookID_RenderCaller, default_render_caller);
    set_custom_hook(app, HookID_WholeScreenRenderCaller, default_whole_screen_render_caller);

    set_custom_hook(app, HookID_DeltaRule, fixed_time_cubic_delta);
    set_custom_hook_memory_size(app, HookID_DeltaRule, delta_ctx_size(fixed_time_cubic_delta_memory_size));

    set_custom_hook(app, HookID_BufferNameResolver, default_buffer_name_resolution);

    set_custom_hook(app, HookID_BeginBuffer, default_begin_buffer);
    set_custom_hook(app, HookID_EndBuffer, end_buffer_close_jump_list);
    set_custom_hook(app, HookID_NewFile, default_new_file);
    set_custom_hook(app, HookID_SaveFile, default_file_save);
    set_custom_hook(app, HookID_BufferEditRange, default_buffer_edit_range);
    set_custom_hook(app, HookID_BufferRegion, default_buffer_region);
    set_custom_hook(app, HookID_ViewChangeBuffer, default_view_change_buffer);

    // if(config_enable_code_wrapping){
    // set_custom_hook(app, HookID_Layout, layout_wrap_whitespace);
    // }
    // else{
    set_custom_hook(app, HookID_Layout, layout_unwrapped);
    // }
    //set_custom_hook(app, HookID_Layout, layout_wrap_anywhere);
    //set_custom_hook(app, HookID_Layout, layout_wrap_whitespace);
    //set_custom_hook(app, HookID_Layout, layout_virt_indent_unwrapped);
    //set_custom_hook(app, HookID_Layout, layout_unwrapped_small_blank_lines);
}

// BOTTOM

