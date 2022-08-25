
// All configs are reloaded at the same time because that simplifies memory
// managment.
// This arena gets reset on every config reload
Arena config_string_arena;
String8 project_path;

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

function File_Name_Data dump_file(Arena *arena, String_Const_u8 file_name);

function bool
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
        return false;
    }
    return true;
}

CUSTOM_COMMAND_SIG(load_config_files)
CUSTOM_DOC("Loads config files: one near executable, one in current directory")
{
    Scratch_Block scratch(app);
    linalloc_clear(&config_string_arena);

    String8 hot_path = push_hot_directory(app, scratch);
    String8 binary_path = system_get_path(scratch, SystemPath_Binary);

    String8 path_to_config = push_stringf(scratch, "%.*s/config.txt", string_expand(binary_path));
    String8 path_to_project = push_stringf(scratch, "%.*s/project.txt", string_expand(hot_path));

    load_config(app, path_to_config);
    if(load_config(app, path_to_project)){
        project_path = hot_path;
    }
}

#if 0
struct NEW_Binding{
    NEW_Binding *next;

    Command_Binding binding;

    int mod_count;
    Input_Event_Kind kind;
    Key_Code sub_code;
    Key_Code mods[3];
};

struct NEW_Mapping{
    NEW_Mapping *next;
    String_ID mapping_id;

    NEW_Binding *first;
    NEW_Binding *last;
};

NEW_Mapping *NEW_global_first_mapping;
NEW_Mapping *NEW_global_last_mapping;

function NEW_Mapping *
NEW_create_mapping(Arena *arena, String8 name){
    NEW_Mapping *mapping = push_array_zero(arena, NEW_Mapping, 1);
    mapping->mapping_id = vars_save_string(name);
    sll_queue_push(NEW_global_first_mapping, NEW_global_last_mapping, mapping);
    return mapping;
}

function NEW_Mapping *
NEW_get_mapping(String_ID id){
    for(NEW_Mapping *m = NEW_global_first_mapping; m; m=m->next){
        if(m->mapping_id == id) return m;
    }
    // TODO(Krzosa): Return default mapping
    return 0;
}

function NEW_Binding *
NEW_get_binding(NEW_Mapping *map, Input_Event_Kind kind, Key_Code key, int mod_count, Key_Code *mods){
    for(NEW_Binding *b = map->first; b; b=b->next){
        b32 mods_match = mod_count == b->mod_count;
        if(mods_match){
            for(int i = 0; i < mod_count; i++){
                if(mods[i] != b->mods[i]) mods_match = false;
            }
        }


        if(b->kind == kind && b->sub_code == key && mods_match){
            return b;
        }
    }
    return 0;
}

function NEW_Binding *
NEW_get_binding(NEW_Mapping *map, Command_Binding binding){
    for(NEW_Binding *b = map->first; b; b=b->next){
        if(BindingGetPtr(b->binding) == BindingGetPtr(binding)){
            return b;
        }
    }
    return 0;
}

function NEW_Binding *
NEW_add_binding(Arena *arena, NEW_Mapping *map, Command_Binding binding, Input_Event_Kind kind, Key_Code key, int mod_count, Key_Code *mods){
    NEW_Binding *bind = NEW_get_binding(map, kind, key, mod_count, mods);
    if(!bind){
        bind = push_array_zero(arena, NEW_Binding, 1);
        sll_queue_push(map->first, map->last, bind);
    }

    bind->binding = binding;
    bind->kind = kind;
    bind->sub_code = key;
    bind->mod_count = mod_count;
    for(int i = 0; i < mod_count; i++) bind->mods[i] = mods[i];
    return bind;
}

function void
NEW_setup_bindings(Arena *arena){
    NEW_Mapping *map = NEW_create_mapping(arena, string_u8_litexpr("normal"));

    Key_Code mods[] = {KeyCode_Alt};
    NEW_add_binding(arena, map, exit_4coder, InputEventKind_KeyStroke, KeyCode_F4, ArrayCount(mods), mods);
}

#endif