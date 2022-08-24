
// NOTE(Krzosa): We need to save all the strings in config.
// We don't want a situtation when you loaded a config and old strings get invalidated and stuff.
// This seems like it requires a complicated dynamic memory allocation algorithm to handle.
// So lets just say we don't care. To simplify, we save strings when config changes and we never free them.
// Shouldn't be much of a problem. Small amounts of memory on action that happens rarely.
Arena config_string_arena;

function void
load_config(App *app, String8 path){
    Scratch_Block scratch(app);

    File_Name_Data fdata = dump_file(scratch, path);
    String8 data = fdata.data;
    if(data.size){
        Loaded_Config values = parse_config(scratch, &config_string_arena, data);
        for(int i = 0; i < values.count; i++){
            Config_Value *v = values.values + i;


            // Skip value if a value is specified to be for a different OS

            for(int sindex = 0; sindex < 2; sindex++){
                if(v->subsection[sindex].size &&
                   ((OS_WINDOWS == 0 && string_match(v->subsection[sindex], string_u8_litexpr("windows"))) ||
                    (OS_MAC     == 0 && string_match(v->subsection[sindex], string_u8_litexpr("mac")))     ||
                    (OS_LINUX   == 0 && string_match(v->subsection[sindex], string_u8_litexpr("linux"))))){
                    goto skip_this_value;
                }
            }

            set_config_value(v);
            skip_this_value:;
        }
    }
    else{
        print_messagef(app, "Looked for: %.*s but couldn't find", string_expand(path));
    }
}

CUSTOM_COMMAND_SIG(reload_config)
CUSTOM_DOC("Reload config variables")
{
    Scratch_Block scratch(app);
    String8 binary_path = system_get_path(scratch, SystemPath_Binary);
    String8 path = push_stringf(scratch, "%.*s/config.txt", string_expand(binary_path));
    load_config(app, path);
}

CUSTOM_COMMAND_SIG(load_project_file)
CUSTOM_DOC("Loads or reloads a project file in current working directory")
{
    Scratch_Block scratch(app);
    String8 hot_path = push_hot_directory(app, scratch);
    String8 path = push_stringf(scratch, "%.*s/project.txt", string_expand(hot_path));
    load_config(app, path);
}

function Key_Code
dynamic_binding_key_code_from_string(String_Const_u8 key_string){
    Key_Code result = 0;
    for (i32 i = 1; i < KeyCode_COUNT; i += 1){
        String_Const_u8 str = SCu8(key_code_name[i]);
        if (string_match(str, key_string)){
            result = i;
            break;
        }
    }
    return result;
}

#if 0
function void
load_keybindings(){
    Thread_Context* tctx = get_thread_context(app);
    mapping_release(tctx, mapping);
    mapping_init(tctx, mapping);
    MappingScope();
    SelectMapping(mapping);

    String_ID map_name_id = vars_save_string(map_name);
    SelectMap(map_name_id);

    String_Const_u8 cmd_string = {0};
    String_Const_u8 key_string = {0};
    String_Const_u8 mod_string[9] = {0};

    Command_Metadata *command = get_command_metadata_from_name(cmd_string);
    Key_Code keycode = dynamic_binding_key_code_from_string(key_string);

    i32 mod_count = 0;
    Key_Code mods[ArrayCount(mod_string)] = {0};
    for (i32 i = 0; i < ArrayCount(mod_string); i += 1){
        if (mod_string[i].str){
            mods[mod_count] = dynamic_binding_key_code_from_string(mod_string[i]);
            mod_count += 1;
        }
    }

    if (keycode != 0 && command != 0){
        Input_Modifier_Set mods_set = { mods, mod_count, };
        map_set_binding(mapping, map, command->proc, InputEventKind_KeyStroke, keycode, &mods_set);
    }
    else{
        // Error
    }
}
#endif



