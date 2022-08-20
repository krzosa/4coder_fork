global char *remedybg_server_name;

#if 0
#include "remedybg_driver.cpp"


global b32   remedybg_connected;
static uint8_t remedybg_command_buf[COMMAND_BUF_SIZE];
static uint8_t remedybg_reply_buf[REPLY_BUF_SIZE];
struct ClientContext remedybg_ctx = {
    0,
    {remedybg_command_buf, 0, sizeof(remedybg_command_buf)},
    {remedybg_reply_buf, 0, sizeof(remedybg_reply_buf)},
    RDBG_IF_DEBUGGING_TARGET_STOP_DEBUGGING,
    RDBG_IF_SESSION_IS_MODIFIED_CONTINUE_WITHOUT_SAVING,
};

function b32
remedybg_is_connected(App *app){
    if(!remedybg_connected){
        // TODO(Krzosa): Error
        print_message(app, string_u8_litexpr("We are not connected! Try opening up a new remedy session."));
    }
    return remedybg_connected;
}

CUSTOM_COMMAND_SIG(remedy_maximize)
CUSTOM_DOC("Maximizes remedy"){
    if(!remedybg_is_connected(app)) return;

    rdbg_CommandResult res;
    BringDebuggerToForeground(&remedybg_ctx, &res);

    if(res != RDBG_COMMAND_RESULT_OK){
        // TODO(Error)
        print_message(app, string_u8_litexpr("Failed to maximize remedy"));
    }
}


CUSTOM_COMMAND_SIG(remedy_run_program)
CUSTOM_DOC("Runs debugged program"){
    if(!remedybg_is_connected(app)) return;
    rdbg_CommandResult res;
    StartDebugging(&remedybg_ctx, true, &res);
    if(res != RDBG_COMMAND_RESULT_OK){
        print_message(app, string_u8_litexpr("Failed to run the program"));
    }
}

CUSTOM_COMMAND_SIG(remedy_run_to_cursor)
CUSTOM_DOC("Run to cursor"){
    if(!remedybg_is_connected(app)) return;

    Scratch_Block scratch(app);
    Active_View_Info a = get_active_view_info(app, AccessFlag_Read);

    String8 filename = push_buffer_file_name(app, scratch, a.buffer);
    filename = push_stringf(scratch, "%.*s\0", string_expand(filename)); // null terminate
    i64 line = a.cursor.line;

    if (InitConnection(remedybg_server_name, DebugControlPipe, sizeof(remedybg_ctx.last_error), remedybg_ctx.last_error, &remedybg_ctx.command_pipe_handle)){
        rdbg_CommandResult res;
        BringDebuggerToForeground(&remedybg_ctx, &res);
        if(res != RDBG_COMMAND_RESULT_OK) print_message(app, string_u8_litexpr("Failed to maximize"));
        StartDebugging(&remedybg_ctx, true, &res);
        if(res == RDBG_COMMAND_RESULT_OK){
            RunToFileAtLine(&remedybg_ctx, (char *)filename.str, line, &res);
            if(res != RDBG_COMMAND_RESULT_OK){
                // TODO(Error)
                print_message(app, string_u8_litexpr("Failed to run to cursor"));
            }
        }
        else{
            print_message(app, string_u8_litexpr("Failed to run to cursor, start debugging"));
        }
    }
}
#endif

CUSTOM_COMMAND_SIG(remedy_start_debug)
CUSTOM_DOC("Open the app in remedybg")
{
    exec_system_commandf(app, global_compilation_view, standard_build_buffer_identifier, EXEC_COMMANDF_DEFAULT_DIR, "remedybg.exe start-debugging");
}

CUSTOM_COMMAND_SIG(remedy_stop_debug)
CUSTOM_DOC("Open the app in remedybg")
{
    exec_system_commandf(app, global_compilation_view, standard_build_buffer_identifier, EXEC_COMMANDF_DEFAULT_DIR, "remedybg.exe stop-debugging");
}

CUSTOM_COMMAND_SIG(debugger_continue)
CUSTOM_DOC("Continue execution")
{
    exec_system_commandf(app, global_compilation_view, standard_build_buffer_identifier, EXEC_COMMANDF_DEFAULT_DIR, "remedybg.exe continue-execution");
}

CUSTOM_COMMAND_SIG(remedy_open_file_at_cursor)
CUSTOM_DOC("Open current file in debugger")
{
    exec_system_commandf(app, global_compilation_view, standard_build_buffer_identifier, EXEC_COMMANDF_DEFAULT_DIR, "remedybg.exe open-file {file} {line}");
}

CUSTOM_COMMAND_SIG(remedy_run_to_cursor)
CUSTOM_DOC("Run to cursor")
{
    exec_system_commandf(app, global_compilation_view, standard_build_buffer_identifier, EXEC_COMMANDF_DEFAULT_DIR, "remedybg.exe run-to-cursor {file} {line}");
}

CUSTOM_COMMAND_SIG(remedy_breakpoint_at_cursor)
CUSTOM_DOC("Set remedybg breakpoint at cursor")
{
    exec_system_commandf(app, global_compilation_view, standard_build_buffer_identifier, EXEC_COMMANDF_DEFAULT_DIR, "remedybg.exe add-breakpoint-at-file {file} {line}");
}

CUSTOM_COMMAND_SIG(debugger_remove_breakpoint_at_cursor)
CUSTOM_DOC("Remove remedybg breakpoint at cursor")
{
    exec_system_commandf(app, global_compilation_view, standard_build_buffer_identifier, EXEC_COMMANDF_DEFAULT_DIR, "remedybg.exe remove-breakpoint-at-file {file} {line}");
}

CUSTOM_COMMAND_SIG(remedy_open)
CUSTOM_DOC("Opens a lister that allows you to choose a remedy session or executable file"){
    for (;;){
        Scratch_Block scratch(app);
        View_ID view = get_this_ctx_view(app, Access_Always);
        File_Name_Result result = get_file_name_from_user(app, scratch, "Open:", view);
        if (result.canceled) break;

        String8 file_name = result.file_name_activated;
        if (file_name.size == 0) break;

        String8 path = result.path_in_text_field;
        String8 full_file_name = push_u8_stringf(scratch, "%.*s/%.*s", string_expand(path), string_expand(file_name));

        if (result.is_folder){
            set_hot_directory(app, full_file_name);
            continue;
        }

        if (character_is_slash(file_name.str[file_name.size - 1])){
            File_Attributes attribs = system_quick_file_attributes(scratch, full_file_name);
            if (HasFlag(attribs.flags, FileAttribute_IsDirectory)){
                set_hot_directory(app, full_file_name);
                continue;
            }
            if (query_create_folder(app, file_name)){
                set_hot_directory(app, full_file_name);
                continue;
            }
            break;
        }

        remedybg_server_name = "4Coder";
        String8 cmd = push_stringf(scratch, "start remedybg.exe --servername %s %.*s", remedybg_server_name, string_expand(full_file_name));
        print_message(app, cmd);
        print_message(app, string_u8_litexpr("\n"));


        String8 hot_dir = push_hot_directory(app, scratch);
        Child_Process_ID child_process_id = create_child_process(app, hot_dir, cmd);
        Unused(child_process_id);
#if 0
        if (child_process_id != 0){
            for(int i = 0; i < 100; i++){
                if (InitConnection(remedybg_server_name, DebugControlPipe, sizeof(remedybg_ctx.last_error), remedybg_ctx.last_error, &remedybg_ctx.command_pipe_handle)){
                    remedybg_connected = true;
                    print_message(app, string_u8_litexpr("Connected to remedybg\n"));

                    break;
                }
                Sleep(10);
            }


            if(!remedybg_connected){
                print_message(app, string_u8_litexpr("Failed to connect to remedybg\n"));
            }
        }
        break;
#endif
    }
}