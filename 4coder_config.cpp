
Arena config_arena;

function void
load_config(App *app, Arena *arena){
    Scratch_Block scratch(app);

    String8 binary_path = system_get_path(scratch, SystemPath_Binary);
    String8 config_path = push_stringf(scratch, "%.*s/config.txt", string_expand(binary_path));
    File_Name_Data fdata = dump_file(arena, config_path);
    String8 data = fdata.data;
    if(data.size){
        Loaded_Config values = parse_config(arena, scratch, data);
        for(int i = 0; i < values.count; i++){
            set_config_value(values.values + i);
        }
    }
    else{
        print_message(app, string_u8_litexpr("Config not found. Try putting it near the binary."));
    }

}

CUSTOM_COMMAND_SIG(reload_config)
CUSTOM_DOC("Reload config variables")
{
    load_config(app, &config_arena);
}