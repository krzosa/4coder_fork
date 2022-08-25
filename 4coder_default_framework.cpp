/*
4coder_default_framework.cpp - Sets up the basics of the framework that is used for default 4coder behaviour.
*/

// TOP

function void
point_stack_push(App *app, Buffer_ID buffer, i64 pos){
    Managed_Object object = alloc_buffer_markers_on_buffer(app, buffer, 1, 0);
    Marker *marker = (Marker*)managed_object_get_pointer(app, object);
    marker->pos = pos;
    marker->lean_right = false;

    i32 next_top = (point_stack.top + 1)%ArrayCount(point_stack.markers);
    if (next_top == point_stack.bot){
        Point_Stack_Slot *slot = &point_stack.markers[point_stack.bot];
        managed_object_free(app, slot->object);
        block_zero_struct(slot);
        point_stack.bot = (point_stack.bot + 1)%ArrayCount(point_stack.markers);
    }

    Point_Stack_Slot *slot = &point_stack.markers[point_stack.top];
    slot->buffer = buffer;
    slot->object = object;
    point_stack.top = next_top;
}

function void
point_stack_push_view_cursor(App *app, View_ID view){
    Buffer_ID buffer = view_get_buffer(app, view, Access_Always);
    i64 pos = view_get_cursor_pos(app, view);
    point_stack_push(app, buffer, pos);
}

function b32
point_stack_pop(App *app){
    b32 result = false;
    if (point_stack.top != point_stack.bot){
        result = true;
        if (point_stack.top > 0){
            point_stack.top -= 1;
        }
        else{
            point_stack.top = ArrayCount(point_stack.markers) - 1;
        }
        Point_Stack_Slot *slot = &point_stack.markers[point_stack.top];
        managed_object_free(app, slot->object);
        block_zero_struct(slot);
    }
    return(result);
}

function b32
point_stack_read_top(App *app, Buffer_ID *buffer_out, i64 *pos_out){
    b32 result = false;
    if (point_stack.top != point_stack.bot){
        result = true;
        i32 prev_top = point_stack.top;
        if (prev_top > 0){
            prev_top -= 1;
        }
        else{
            prev_top = ArrayCount(point_stack.markers) - 1;
        }
        Point_Stack_Slot *slot = &point_stack.markers[prev_top];
        Managed_Object object = slot->object;
        Marker *marker = (Marker*)managed_object_get_pointer(app, object);
        if (marker != 0){
            *buffer_out = slot->buffer;
            *pos_out = marker->pos;
        }
        else{
            *buffer_out = 0;
            *pos_out = 0;
        }
    }
    return(result);
}

////////////////////////////////

function void
unlock_jump_buffer(void){
    locked_buffer.size = 0;
}

function void
lock_jump_buffer(App *app, String_Const_u8 name){
    if (name.size < sizeof(locked_buffer_space)){
        block_copy(locked_buffer_space, name.str, name.size);
        locked_buffer = SCu8(locked_buffer_space, name.size);
    }
}

function void
lock_jump_buffer(App *app, char *name, i32 size){
    lock_jump_buffer(app, SCu8(name, size));
}

function void
lock_jump_buffer(App *app, Buffer_ID buffer_id){
    Scratch_Block scratch(app);
    String_Const_u8 buffer_name = push_buffer_unique_name(app, scratch, buffer_id);
    lock_jump_buffer(app, buffer_name);
}

function Buffer_ID
get_locked_jump_buffer(App *app){
    Buffer_ID result = 0;
    if (locked_buffer.size > 0){
        result = get_buffer_by_name(app, locked_buffer, Access_Always);
    }
    if (result == 0){
        unlock_jump_buffer();
    }
    return(result);
}

function View_ID
get_view_for_locked_jump_buffer(App *app){
    View_ID result = 0;
    Buffer_ID buffer = get_locked_jump_buffer(app);
    if (buffer != 0){
        result = get_first_view_with_buffer(app, buffer);
    }
    return(result);
}

////////////////////////////////

// TODO(allen): re-evaluate the setup of this.
function void
new_view_settings(App *app, View_ID view){
    b32 use_file_bars = config_use_file_bars;
    view_set_setting(app, view, ViewSetting_ShowFileBar, use_file_bars);
}

////////////////////////////////

function void
view_set_passive(App *app, View_ID view_id, b32 value){
    Managed_Scope scope = view_get_managed_scope(app, view_id);
    b32 *is_passive = scope_attachment(app, scope, view_is_passive_loc, b32);
    if (is_passive != 0){
        *is_passive = value;
    }
}

function b32
view_get_is_passive(App *app, View_ID view_id){
    Managed_Scope scope = view_get_managed_scope(app, view_id);
    b32 *is_passive = scope_attachment(app, scope, view_is_passive_loc, b32);
    b32 result = false;
    if (is_passive != 0){
        result = *is_passive;
    }
    return(result);
}

function View_ID
get_next_view_looped_primary_panels(App *app, View_ID start_view_id, Access_Flag access){
    View_ID view_id = start_view_id;
    do{
        view_id = get_next_view_looped_all_panels(app, view_id, access);
        if (!view_get_is_passive(app, view_id)){
            break;
        }
    }while(view_id != start_view_id);
    return(view_id);
}

function View_ID
get_prev_view_looped_primary_panels(App *app, View_ID start_view_id, Access_Flag access){
    View_ID view_id = start_view_id;
    do{
        view_id = get_prev_view_looped_all_panels(app, view_id, access);
        if (!view_get_is_passive(app, view_id)){
            break;
        }
    }while(view_id != start_view_id);
    return(view_id);
}

function View_ID
get_next_view_after_active(App *app, Access_Flag access){
    View_ID view = get_active_view(app, access);
    if (view != 0){
        view = get_next_view_looped_primary_panels(app, view, access);
    }
    return(view);
}

////////////////////////////////

function void
call_after_ctx_shutdown(App *app, View_ID view, Custom_Command_Function *func){
    view_enqueue_command_function(app, view, func);
}

function Fallback_Dispatch_Result
fallback_command_dispatch(App *app, Mapping *mapping, Command_Map *map,
                          User_Input *in){
    Fallback_Dispatch_Result result = {};
    if (mapping != 0 && map != 0){
        Command_Binding binding = map_get_binding_recursive(mapping, map, &in->event);
        if (binding.custom != 0){
            Command_Metadata *metadata = get_command_metadata(binding.custom);
            if (metadata != 0){
                if (metadata->is_ui){
                    result.code = FallbackDispatch_DelayedUICall;
                    result.func = binding.custom;
                }
                else{
                    binding.custom(app);
                    result.code = FallbackDispatch_DidCall;
                }
            }
            else{
                binding.custom(app);
                result.code = FallbackDispatch_DidCall;
            }
        }
    }
    return(result);
}

function b32
ui_fallback_command_dispatch(App *app, View_ID view,
                             Mapping *mapping, Command_Map *map, User_Input *in){
    b32 result = false;
    Fallback_Dispatch_Result disp_result =
        fallback_command_dispatch(app, mapping, map, in);
    if (disp_result.code == FallbackDispatch_DelayedUICall){
        call_after_ctx_shutdown(app, view, disp_result.func);
        result = true;
    }
    if (disp_result.code == FallbackDispatch_Unhandled){
        leave_current_input_unhandled(app);
    }
    return(result);
}

function b32
ui_fallback_command_dispatch(App *app, View_ID view, User_Input *in){
    b32 result = false;
    View_Context ctx = view_current_context(app, view);
    if (ctx.mapping != 0){
        Command_Map *map = mapping_get_map(ctx.mapping, ctx.map_id);
        result = ui_fallback_command_dispatch(app, view, ctx.mapping, map, in);
    }
    else{
        leave_current_input_unhandled(app);
    }
    return(result);
}

////////////////////////////////

function void
view_buffer_set(App *app, Buffer_ID *buffers, i64 *positions, i32 count){
    if (count > 0){
        Scratch_Block scratch(app);

        struct View_Node{
            View_Node *next;
            View_ID view_id;
        };

        View_ID active_view_id = get_active_view(app, Access_Always);
        View_ID first_view_id = active_view_id;
        if (view_get_is_passive(app, active_view_id)){
            first_view_id = get_next_view_looped_primary_panels(app, active_view_id, Access_Always);
        }

        View_ID view_id = first_view_id;

        View_Node *primary_view_first = 0;
        View_Node *primary_view_last = 0;
        i32 available_view_count = 0;

        primary_view_first = primary_view_last = push_array(scratch, View_Node, 1);
        primary_view_last->next = 0;
        primary_view_last->view_id = view_id;
        available_view_count += 1;
        for (;;){
            view_id = get_next_view_looped_primary_panels(app, view_id, Access_Always);
            if (view_id == first_view_id){
                break;
            }
            View_Node *node = push_array(scratch, View_Node, 1);
            primary_view_last->next = node;
            node->next = 0;
            node->view_id = view_id;
            primary_view_last = node;
            available_view_count += 1;
        }

        i32 buffer_set_count = clamp_top(count, available_view_count);
        View_Node *node = primary_view_first;
        for (i32 i = 0; i < buffer_set_count; i += 1, node = node->next){
            if (view_set_buffer(app, node->view_id, buffers[i], 0)){
                view_set_cursor_and_preferred_x(app, node->view_id, seek_pos(positions[i]));
            }
        }
    }
}

////////////////////////////////

function void
change_active_panel_send_command(App *app, Custom_Command_Function *custom_func){
    View_ID view = get_active_view(app, Access_Always);
    view = get_next_view_looped_primary_panels(app, view, Access_Always);
    if (view != 0){
        view_set_active(app, view);
    }
    if (custom_func != 0){
        view_enqueue_command_function(app, view, custom_func);
    }
}

CUSTOM_COMMAND_SIG(change_active_panel)
CUSTOM_DOC("Change the currently active panel, moving to the panel with the next highest view_id.")
{
    change_active_panel_send_command(app, 0);
}

CUSTOM_COMMAND_SIG(change_active_panel_backwards)
CUSTOM_DOC("Change the currently active panel, moving to the panel with the next lowest view_id.")
{
    View_ID view = get_active_view(app, Access_Always);
    view = get_prev_view_looped_primary_panels(app, view, Access_Always);
    if (view != 0){
        view_set_active(app, view);
    }
}

CUSTOM_COMMAND_SIG(open_panel_vsplit)
CUSTOM_DOC("Create a new panel by vertically splitting the active panel.")
{
    View_ID view = get_active_view(app, Access_Always);
    View_ID new_view = open_view(app, view, ViewSplit_Right);
    new_view_settings(app, new_view);
    Buffer_ID buffer = view_get_buffer(app, view, Access_Always);
    view_set_buffer(app, new_view, buffer, 0);
}

CUSTOM_COMMAND_SIG(open_panel_hsplit)
CUSTOM_DOC("Create a new panel by horizontally splitting the active panel.")
{
    View_ID view = get_active_view(app, Access_Always);
    View_ID new_view = open_view(app, view, ViewSplit_Bottom);
    new_view_settings(app, new_view);
    Buffer_ID buffer = view_get_buffer(app, view, Access_Always);
    view_set_buffer(app, new_view, buffer, 0);
}

////////////////////////////////

// NOTE(allen): Credits to nj/FlyingSolomon for authoring the original version of this helper.

function Buffer_ID
create_or_switch_to_buffer_and_clear_by_name(App *app, String_Const_u8 name_string, View_ID default_target_view){
    Buffer_ID search_buffer = get_buffer_by_name(app, name_string, Access_Always);
    if (search_buffer != 0){
        buffer_set_setting(app, search_buffer, BufferSetting_ReadOnly, true);

        View_ID target_view = default_target_view;

        View_ID view_with_buffer_already_open = get_first_view_with_buffer(app, search_buffer);
        if (view_with_buffer_already_open != 0){
            target_view = view_with_buffer_already_open;
            // TODO(allen): there needs to be something like
            // view_exit_to_base_context(app, target_view);
            // view_end_ui_mode(app, target_view);
        }
        else{
            view_set_buffer(app, target_view, search_buffer, 0);
        }
        view_set_active(app, target_view);

        clear_buffer(app, search_buffer);
        buffer_send_end_signal(app, search_buffer);
    }
    else{
        search_buffer = create_buffer(app, name_string, BufferCreate_AlwaysNew);
        buffer_set_setting(app, search_buffer, BufferSetting_Unimportant, true);
        buffer_set_setting(app, search_buffer, BufferSetting_ReadOnly, true);
        view_set_buffer(app, default_target_view, search_buffer, 0);
        view_set_active(app, default_target_view);
    }

    return(search_buffer);
}

////////////////////////////////

function void
save_all_dirty_buffers_with_postfix(App *app, String_Const_u8 postfix){
    // NOTE(Krzosa): Deleted profile code: ProfileScope(app, "save all dirty buffers");
    Scratch_Block scratch(app);
    for (Buffer_ID buffer = get_buffer_next(app, 0, Access_ReadWriteVisible);
         buffer != 0;
         buffer = get_buffer_next(app, buffer, Access_ReadWriteVisible)){
        Dirty_State dirty = buffer_get_dirty_state(app, buffer);
        if (dirty == DirtyState_UnsavedChanges){
            Temp_Memory temp = begin_temp(scratch);
            String_Const_u8 file_name = push_buffer_file_name(app, scratch, buffer);
            if (string_match(string_postfix(file_name, postfix.size), postfix)){
                buffer_save(app, buffer, file_name, 0);
            }
            end_temp(temp);
        }
    }
}

CUSTOM_COMMAND_SIG(save_all_dirty_buffers)
CUSTOM_DOC("Saves all buffers marked dirty (showing the '*' indicator).")
{
    String_Const_u8 empty = {};
    save_all_dirty_buffers_with_postfix(app, empty);
}

////////////////////////////////

function void
set_mouse_suppression(b32 suppress){
    if (suppress){
        suppressing_mouse = true;
        system_show_mouse_cursor(MouseCursorShow_Never);
    }
    else{
        suppressing_mouse = false;
        system_show_mouse_cursor(MouseCursorShow_Always);
    }
}

CUSTOM_COMMAND_SIG(suppress_mouse)
CUSTOM_DOC("Hides the mouse and causes all mosue input (clicks, position, wheel) to be ignored.")
{
    set_mouse_suppression(true);
}

CUSTOM_COMMAND_SIG(allow_mouse)
CUSTOM_DOC("Shows the mouse and causes all mouse input to be processed normally.")
{
    set_mouse_suppression(false);
}

CUSTOM_COMMAND_SIG(toggle_mouse)
CUSTOM_DOC("Toggles the mouse suppression mode, see suppress_mouse and allow_mouse.")
{
    set_mouse_suppression(!suppressing_mouse);
}

CUSTOM_COMMAND_SIG(toggle_highlight_line_at_cursor)
CUSTOM_DOC("Toggles the line highlight at the cursor.")
{
    // TODO(Krzosa)
}

CUSTOM_COMMAND_SIG(toggle_highlight_enclosing_scopes)
CUSTOM_DOC("In code files scopes surrounding the cursor are highlighted with distinguishing colors.")
{
    // TODO(Krzosa)
}

CUSTOM_COMMAND_SIG(toggle_paren_matching_helper)
CUSTOM_DOC("In code files matching parentheses pairs are colored with distinguishing colors.")
{
    // TODO(Krzosa)
}

CUSTOM_COMMAND_SIG(toggle_fullscreen)
CUSTOM_DOC("Toggle fullscreen mode on or off.  The change(s) do not take effect until the next frame.")
{
    system_set_fullscreen(!system_is_fullscreen());
}

////////////////////////////////

function void
setup_essential_mapping(Mapping *mapping, i64 global_id, i64 file_id){
    MappingScope();
    SelectMapping(mapping);

    SelectMap(global_id);
    BindCore(default_startup, CoreCode_Startup);
    BindCore(default_try_exit, CoreCode_TryExit);
    BindCore(clipboard_record_clip, CoreCode_NewClipboardContents);
    BindMouseWheel(mouse_wheel_scroll);
    BindMouseWheel(mouse_wheel_change_face_size, KeyCode_Control);

    SelectMap(file_id);
    ParentMap(global_id);
    BindTextInput(write_text_input);
    BindMouse(click_set_cursor_and_mark, MouseCode_Left);
    BindMouseRelease(click_set_cursor, MouseCode_Left);
    BindCore(click_set_cursor_and_mark, CoreCode_ClickActivateView);
    BindMouseMove(click_set_cursor_if_lbutton);

}

function void
setup_built_in_mapping(App *app, String_Const_u8 name, Mapping *mapping, i64 global_id, i64 file_id){
    Thread_Context *tctx = get_thread_context(app);
    if (string_match(name, string_u8_litexpr("default"))){
        mapping_release(tctx, mapping);
        mapping_init(tctx, mapping);
        setup_default_mapping(mapping, global_id, file_id);
    }
    else if (string_match(name, string_u8_litexpr("mac-default"))){
        mapping_release(tctx, mapping);
        mapping_init(tctx, mapping);
        setup_mac_mapping(mapping, global_id, file_id);
    }
    else if (string_match(name, string_u8_litexpr("choose"))){
        mapping_release(tctx, mapping);
        mapping_init(tctx, mapping);
#if OS_MAC
        setup_mac_mapping(mapping, global_id, file_id);
#else
        setup_default_mapping(mapping, global_id, file_id);
#endif
    }
}

////////////////////////////////

function void
buffer_modified_set_init(void){
    Buffer_Modified_Set *set = &global_buffer_modified_set;

    block_zero_struct(set);
    Base_Allocator *allocator = get_base_allocator_system();
    set->arena = make_arena(allocator);
    set->id_to_node = make_table_u64_u64(allocator, 100);
}

function Buffer_Modified_Node*
buffer_modified_set__alloc_node(Buffer_Modified_Set *set){
    Buffer_Modified_Node *result = set->free;
    if (result == 0){
        result = push_array(&set->arena, Buffer_Modified_Node, 1);
    }
    else{
        sll_stack_pop(set->free);
    }
    return(result);
}

function void
buffer_mark_as_modified(Buffer_ID buffer){
    Buffer_Modified_Set *set = &global_buffer_modified_set;

    Table_Lookup lookup = table_lookup(&set->id_to_node, (u64)buffer);
    if (!lookup.found_match){
        Buffer_Modified_Node *node = buffer_modified_set__alloc_node(set);
        zdll_push_back(set->first, set->last, node);
        node->buffer = buffer;
        table_insert(&set->id_to_node, (u64)buffer, (u64)PtrAsInt(node));
    }
}

function void
buffer_unmark_as_modified(Buffer_ID buffer){
    Buffer_Modified_Set *set = &global_buffer_modified_set;

    Table_Lookup lookup = table_lookup(&set->id_to_node, (u64)buffer);
    if (lookup.found_match){
        u64 val = 0;
        table_read(&set->id_to_node, (u64)buffer, &val);
        Buffer_Modified_Node *node = (Buffer_Modified_Node*)IntAsPtr(val);
        zdll_remove(set->first, set->last, node);
        table_erase(&set->id_to_node, lookup);
        sll_stack_push(set->free, node);
    }
}

function void
buffer_modified_set_clear(void){
    Buffer_Modified_Set *set = &global_buffer_modified_set;

    table_clear(&set->id_to_node);
    if (set->last != 0){
        set->last->next = set->free;
        set->free = set->first;
        set->first = 0;
        set->last = 0;
    }
}

////////////////////////////////

function Fade_Range*
alloc_fade_range(void){
    Fade_Range *result = free_fade_ranges;
    if (result == 0){
        result = push_array(&fade_range_arena, Fade_Range, 1);
    }
    else{
        sll_stack_pop(free_fade_ranges);
    }
    block_zero_struct(result);
    return(result);
}

function void
free_fade_range(Fade_Range *range){
    sll_stack_push(free_fade_ranges, range);
}

function Fade_Range*
buffer_post_fade(App *app, Buffer_ID buffer_id, f32 seconds, Range_i64 range, ARGB_Color color){
    Fade_Range *fade_range = alloc_fade_range();
    sll_queue_push(buffer_fade_ranges.first, buffer_fade_ranges.last, fade_range);
    buffer_fade_ranges.count += 1;
    fade_range->buffer_id = buffer_id;
    fade_range->t = seconds;
    fade_range->full_t = seconds;
    fade_range->range = range;
    fade_range->color = color;
    return(fade_range);
}

function void
buffer_shift_fade_ranges(Buffer_ID buffer_id, i64 shift_after_p, i64 shift_amount){
    for (Fade_Range *node = buffer_fade_ranges.first;
         node != 0;
         node = node->next){
        if (node->buffer_id == buffer_id){
            if (node->range.min >= shift_after_p){
                node->range.min += shift_amount;
                node->range.max += shift_amount;
            }
            else if (node->range.max >= shift_after_p){
                node->range.max += shift_amount;
            }
        }
    }
}

function b32
tick_all_fade_ranges(App *app, f32 t){
    Fade_Range **prev_next = &buffer_fade_ranges.first;
    for (Fade_Range *node = buffer_fade_ranges.first, *next = 0;
         node != 0;
         node = next){
        next = node->next;
        node->t -= t;
        if (node->t <= 0.f){
            if (node->finish_call != 0){
                node->finish_call(app, node);
            }
            *prev_next = next;
            free_fade_range(node);
            buffer_fade_ranges.count -= 1;
        }
        else{
            prev_next = &node->next;
            buffer_fade_ranges.last = node;
        }
    }
    return(buffer_fade_ranges.count > 0);
}

function void
paint_fade_ranges(App *app, Text_Layout_ID layout, Buffer_ID buffer){
    for (Fade_Range *node = buffer_fade_ranges.first;
         node != 0;
         node = node->next){
        if (node->buffer_id == buffer){
            f32 blend = node->t/node->full_t;
            if (node->negate_fade_direction){
                blend = 1.f - blend;
            }
            paint_text_color_blend(app, layout, node->range, node->color, blend);
        }
    }
}

function void
initialize_managed_id_metadata(App *app);
////////////////////////////////


function void
default_input_handler_init(App *app, Arena *arena){
    View_ID view = get_this_ctx_view(app, Access_Always);
    View_Context ctx = view_current_context(app, view);
    ctx.mapping = &framework_mapping;
    ctx.map_id = vars_save_string_lit("keys_global");
    view_alter_context(app, view, &ctx);
}

function Command_Map_ID
default_get_map_id(App *app, View_ID view){
    Command_Map_ID result = 0;
    Buffer_ID buffer = view_get_buffer(app, view, Access_Always);
    Managed_Scope buffer_scope = buffer_get_managed_scope(app, buffer);
    Command_Map_ID *result_ptr = scope_attachment(app, buffer_scope, buffer_map_id, Command_Map_ID);
    if (result_ptr != 0){
        if (*result_ptr == 0){
            *result_ptr = vars_save_string_lit("keys_file");
        }
        result = *result_ptr;
    }
    else{
        result = vars_save_string_lit("keys_global");
    }
    return(result);
}

function void
set_next_rewrite(App *app, View_ID view, Rewrite_Type rewrite){
    Managed_Scope scope = view_get_managed_scope(app, view);
    Rewrite_Type *next_rewrite = scope_attachment(app, scope, view_next_rewrite_loc, Rewrite_Type);
    if (next_rewrite != 0){
        *next_rewrite = rewrite;
    }
}

function void
default_pre_command(App *app, Managed_Scope scope){
    Rewrite_Type *next_rewrite = scope_attachment(app, scope, view_next_rewrite_loc, Rewrite_Type);
    *next_rewrite = Rewrite_None;
}

function void
default_post_command(App *app, Managed_Scope scope){
    Rewrite_Type *next_rewrite = scope_attachment(app, scope, view_next_rewrite_loc, Rewrite_Type);
    if (next_rewrite != 0){
        if (*next_rewrite != Rewrite_NoChange){
            Rewrite_Type *rewrite =
                scope_attachment(app, scope, view_rewrite_loc, Rewrite_Type);
            *rewrite = *next_rewrite;
        }
    }
}

// BOTTOM

