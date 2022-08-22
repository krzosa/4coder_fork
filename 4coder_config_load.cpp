
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
            set_config_value(values.values + i);
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
