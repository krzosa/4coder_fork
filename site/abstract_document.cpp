/*
 * Mr. 4th Dimention - Allen Webster
 *
 * 25.02.2016
 *
 * File editing view for 4coder
 *
 */

// TOP

#if !defined(ABSTRACT_DOCUMENT_H)
#define ABSTRACT_DOCUMENT_H

#define NotImplemented Assert(!"Not Implemented!")

// Enriched Text

struct Enriched_Text{
     String source;
};

static Enriched_Text
load_enriched_text(Partition *part, char *directory, char *filename){
    Enriched_Text result = {0};
    
    char space[256];
    String fname = make_fixed_width_string(space);
    append_sc(&fname, directory);
    append_sc(&fname, "\\");
    append_sc(&fname, filename);
    terminate_with_null(&fname);
    
     result.source = file_dump(fname.str);
    return(result);
    }

// Document Declaration

enum{
    Doc_Root,
    Doc_Section,
    Doc_Todo,
    Doc_Enriched_Text,
    Doc_Element_List,
    Doc_Full_Elements,
    Doc_Table_Of_Contents
};

typedef struct Alternate_Name{
    String macro;
    String public_name;
} Alternate_Name;

typedef struct Alternate_Names_Array{
    Alternate_Name *names;
} Alternate_Names_Array;

enum{
    AltName_Standard,
    AltName_Macro,
    AltName_Public_Name,
};

struct Document_Item{
    Document_Item *next;
    Document_Item *parent;
    int32_t type;
    union{
        struct{
            Document_Item *first_child;
            Document_Item *last_child;
    String name;
            String id;
    } section;
    
    struct{
        Meta_Unit *unit;
        Alternate_Names_Array *alt_names;
        int32_t alt_name_type;
    } unit_elements;
    
    struct{
        Enriched_Text *text;
    } text;
};
};
static Document_Item null_document_item = {0};

struct Abstract_Document{
    // Document value members
    Document_Item *root_item;
    
    // Document building members
    Partition *part;
    Document_Item *section_stack[16];
     int32_t section_top;
};
static Abstract_Document null_abstract_document = {0};

static void
set_section_name(Partition *part, Document_Item *item, char *name){
    int32_t name_len = str_size(name);
    item->section.name = make_string_cap(push_array(part, char, name_len+1), 0, name_len+1);
    partition_align(part, 8);
    append_sc(&item->section.name, name);
}

static void
set_section_id(Partition *part, Document_Item *item, char *id){
    int32_t id_len = str_size(id);
    item->section.id = make_string_cap(push_array(part, char, id_len+1), 0, id_len+1);
    partition_align(part, 8);
    append_sc(&item->section.id, id);
}

static void
begin_document_description(Abstract_Document *doc, Partition *part, char *title){
    *doc = null_abstract_document;
    doc->part = part;
    
    doc->root_item = push_struct(doc->part, Document_Item);
    *doc->root_item = null_document_item;
    doc->section_stack[doc->section_top] = doc->root_item;
    doc->root_item->type = Doc_Root;
    
    set_section_name(doc->part, doc->root_item, title);
}

static void
end_document_description(Abstract_Document *doc){
    Assert(doc->section_top == 0);
}

static void
append_child(Document_Item *parent, Document_Item *item){
    Assert(parent->type == Doc_Root || parent->type == Doc_Section);
    if (parent->section.last_child == 0){
        parent->section.first_child = item;
    }
    else{
        parent->section.last_child->next = item;
    }
    parent->section.last_child = item;
    item->parent = parent;
}

static void
begin_section(Abstract_Document *doc, char *title, char *id){
    Assert(doc->section_top+1 < ArrayCount(doc->section_stack));
    
    Document_Item *parent = doc->section_stack[doc->section_top];
    Document_Item *section = push_struct(doc->part, Document_Item);
    *section = null_document_item;
    doc->section_stack[++doc->section_top] = section;
    
    section->type = Doc_Section;
    
    set_section_name(doc->part, section, title);
    if (id){
    set_section_id(doc->part, section, id);
    }
    
    append_child(parent, section);
    }

static void
end_section(Abstract_Document *doc){
    Assert(doc->section_top > 0);
    --doc->section_top;
}

static void
add_todo(Abstract_Document *doc){
    Document_Item *parent = doc->section_stack[doc->section_top];
    Document_Item *item = push_struct(doc->part, Document_Item);
    *item = null_document_item;
    item->type = Doc_Todo;
    
    append_child(parent, item);
}

static void
add_element_list(Abstract_Document *doc, Meta_Unit *unit){
    Document_Item *parent = doc->section_stack[doc->section_top];
    Document_Item *item = push_struct(doc->part, Document_Item);
    *item = null_document_item;
    item->type = Doc_Element_List;
    item->unit_elements.unit = unit;
    
    append_child(parent, item);
}

static void
add_element_list(Abstract_Document *doc, Meta_Unit *unit, Alternate_Names_Array *alt_names, int32_t alt_name_type){
    Document_Item *parent = doc->section_stack[doc->section_top];
    Document_Item *item = push_struct(doc->part, Document_Item);
    *item = null_document_item;
    item->type = Doc_Element_List;
    item->unit_elements.unit = unit;
    item->unit_elements.alt_names = alt_names;
    item->unit_elements.alt_name_type = alt_name_type;
    
    append_child(parent, item);
}

static void
add_full_elements(Abstract_Document *doc, Meta_Unit *unit){
    Document_Item *parent = doc->section_stack[doc->section_top];
    Document_Item *item = push_struct(doc->part, Document_Item);
    *item = null_document_item;
    item->type = Doc_Full_Elements;
    item->unit_elements.unit = unit;
    
    append_child(parent, item);
}

static void
add_full_elements(Abstract_Document *doc, Meta_Unit *unit, Alternate_Names_Array *alt_names, int32_t alt_name_type){
    Document_Item *parent = doc->section_stack[doc->section_top];
    Document_Item *item = push_struct(doc->part, Document_Item);
    *item = null_document_item;
    item->type = Doc_Full_Elements;
    item->unit_elements.unit = unit;
    item->unit_elements.alt_names = alt_names;
    item->unit_elements.alt_name_type = alt_name_type;
    
    append_child(parent, item);
}

static void
add_table_of_contents(Abstract_Document *doc){
    Document_Item *parent = doc->section_stack[doc->section_top];
    Document_Item *item = push_struct(doc->part, Document_Item);
    *item = null_document_item;
    item->type = Doc_Table_Of_Contents;
    
    append_child(parent, item);
}

static void
add_enriched_text(Abstract_Document *doc, Enriched_Text *text){
    Assert(doc->section_top+1 < ArrayCount(doc->section_stack));
    
    Document_Item *parent = doc->section_stack[doc->section_top];
    Document_Item *item = push_struct(doc->part, Document_Item);
    *item = null_document_item;
    item->type = Doc_Enriched_Text;
    item->text.text = text;
    
    append_child(parent, item);
}

// Document Generation

#define HTML_BACK_COLOR   "#FAFAFA"
#define HTML_TEXT_COLOR   "#0D0D0D"
#define HTML_CODE_BACK    "#DFDFDF"
#define HTML_EXAMPLE_BACK "#EFEFDF"

#define HTML_POP_COLOR_1  "#309030"
#define HTML_POP_BACK_1   "#E0FFD0"
#define HTML_VISITED_LINK "#A0C050"

#define HTML_POP_COLOR_2  "#005000"

#define HTML_CODE_STYLE "font-family: \"Courier New\", Courier, monospace; text-align: left;"

#define HTML_CODE_BLOCK_STYLE(back)                             \
"margin-top: 3mm; margin-bottom: 3mm; font-size: .95em; "  \
"background: "back"; padding: 0.25em;"

#define HTML_DESCRIPT_SECTION_STYLE HTML_CODE_BLOCK_STYLE(HTML_CODE_BACK)
#define HTML_EXAMPLE_CODE_STYLE HTML_CODE_BLOCK_STYLE(HTML_EXAMPLE_BACK)

#define HTML_DOC_HEAD_OPEN  "<div style='margin-top: 3mm; margin-bottom: 3mm; color: "HTML_POP_COLOR_1";'><b><i>"
#define HTML_DOC_HEAD_CLOSE "</i></b></div>"

#define HTML_DOC_ITEM_HEAD_STYLE "font-weight: 600;"

#define HTML_DOC_ITEM_HEAD_INL_OPEN  "<span style='"HTML_DOC_ITEM_HEAD_STYLE"'>"
#define HTML_DOC_ITEM_HEAD_INL_CLOSE "</span>"

#define HTML_DOC_ITEM_HEAD_OPEN  "<div style='"HTML_DOC_ITEM_HEAD_STYLE"'>"
#define HTML_DOC_ITEM_HEAD_CLOSE "</div>"

#define HTML_DOC_ITEM_OPEN  "<div style='margin-left: 5mm; margin-right: 5mm;'>"
#define HTML_DOC_ITEM_CLOSE "</div>"

#define HTML_EXAMPLE_CODE_OPEN  "<div style='"HTML_CODE_STYLE HTML_EXAMPLE_CODE_STYLE"'>"
#define HTML_EXAMPLE_CODE_CLOSE "</div>"

struct Section_Counter{
    int32_t counter[16];
    int32_t nest_level;
};

static void
append_section_number_reduced(String *out, Section_Counter section_counter, int32_t reduce){
    int32_t level = section_counter.nest_level-reduce;
    for (int32_t i = 1; i <= level; ++i){
        append_int_to_str(out, section_counter.counter[i]);
        if (i != level){
            append_sc(out, ".");
        }
    }
}

static void
append_section_number(String *out, Section_Counter section_counter){
    append_section_number_reduced(out, section_counter, 0);
}

static int32_t
extract_command_body(String *out, String l, int32_t *i_in_out, int32_t *body_start_out, int32_t *body_end_out, String command_name){
    int32_t result = 0;
    
    int32_t i = *i_in_out;
    
        for (; i < l.size; ++i){
            if (!char_is_whitespace(l.str[i])){
                break;
            }
        }
        
        int32_t found_command_body = 0;
        int32_t body_start = 0, body_end = 0;
        if (l.str[i] == '{'){
            body_start = i+1;
            
            for (++i; i < l.size; ++i){
                if (l.str[i] == '}'){
                    found_command_body = 1;
                    body_end = i;
                    ++i;
                    break;
                }
            }
        }
    
        if (found_command_body){
            result = 1;
        }
        else{
#define STR_START "<span style='color:#F00'>! Doc generator error: missing body for "
            #define STR_SLOW " !</span>"
            append_sc(out, STR_START);
            append_ss(out, command_name);
            append_sc(out, STR_SLOW);
#undef STR
            
            fprintf(stderr, "error: missing body for %.*s\n", command_name.size, command_name.str);
        }
        
        *i_in_out = i;
        *body_start_out = body_start;
        *body_end_out = body_end;
        
    return(result);
}

static void
write_enriched_text_html(String *out, Enriched_Text *text){
    String source = text->source;
    
    append_sc(out, "<div>");
    
    for (String line = get_first_double_line(source);
         line.str;
         line = get_next_double_line(source, line)){
        String l  = skip_chop_whitespace(line);
        append_sc(out, "<p>");
        
        //append_ss(out, l);
        int32_t start = 0, i = 0;
        for (; i < l.size; ++i){
            if (l.str[i] == '\\'){
                append_ss(out, substr(l, start, i-start));
                
                int32_t command_start = i+1;
                int32_t command_end = command_start;
                for (; command_end < l.size; ++command_end){
                    if (!char_is_alpha_numeric(l.str[command_end])){
                        break;
                    }
                }
                
                if (command_end == command_start){
                    if (command_end < l.size && l.str[command_end] == '\\'){
                        ++command_end;
                    }
                }
                
                String command_string = substr(l, command_start, command_end - command_start);
                
                static String enriched_commands[] = {
                    make_lit_string("\\"),
                    make_lit_string("VERSION"),
                    make_lit_string("CODE_STYLE"),
                    make_lit_string("DOC_LINK"),
                };
                
                i = command_end;
                
                int32_t match_index = 0;
                if (string_set_match(enriched_commands, ArrayCount(enriched_commands), command_string, &match_index)){
                    switch (match_index){
                        case 0: append_sc(out, "\\"); break;
                        case 1: append_sc(out, VERSION); break;
                        case 2:
                        {
                            int32_t body_start = 0, body_end = 0;
                             int32_t has_body = extract_command_body(out, l, &i, &body_start, &body_end, command_string);
                            if (has_body){
                                String body_text = substr(l, body_start, body_end - body_start);
                            append_sc(out, "<span style='"HTML_CODE_STYLE"'>");
                            append_ss(out, body_text);
                            append_sc(out, "</span>");
                        }
                        }break;
                        
                        case 3:
                        {
                            int32_t body_start = 0, body_end = 0;
                            int32_t has_body = extract_command_body(out, l, &i, &body_start, &body_end, command_string);
                            if (has_body){
                                String body_text = substr(l, body_start, body_end - body_start);
                                append_sc(out, "<a href='#");
                                append_ss(out, body_text);
                                append_sc(out, "_doc'>");
                                append_ss(out, body_text);
                                append_sc(out, "</a>");
                            }
                        }break;
                    }
                }
                else{
                    append_sc(out, "<span style='color:#F00'>! Doc generator error: unrecognized command !</span>");
                    fprintf(stderr, "error: unrecognized command %.*s\n", command_string.size, command_string.str);
                }
                
                start = i;
            }
        }
        
        if (start != i){
            append_ss(out, substr(l, start, i-start));
        }
        
        append_sc(out, "</p>");
    }
    
    append_sc(out, "</div>");
    }
    
    static void
        print_item_in_list(String *out, String name, char *id_postfix){
        append_sc(out, "<li><a href='#");
        append_ss(out, name);
        append_sc(out, id_postfix);
        append_sc(out, "'>");
        append_ss(out, name);
        append_sc(out, "</a></li>");
    }
    
    static void
        init_used_links(Partition *part, Used_Links *used, int32_t count){
        used->strs = push_array(part, String, count);
        used->count = 0;
        used->max = count;
    }
    
    static int32_t
        try_to_use(Used_Links *used, String str){
        int32_t result = 1;
        int32_t index = 0;
        
        if (string_set_match(used->strs, used->count, str, &index)){
            result = 0;
        }
        else{
            used->strs[used->count++] = str;
        }
        
        return(result);
    }
    
    static void
        print_struct_html(String *out, Item_Node *member, int32_t hide_children){
        String name = member->name;
        String type = member->type;
        String type_postfix = member->type_postfix;
        
        append_ss     (out, type);
        append_s_char (out, ' ');
        append_ss     (out, name);
        append_ss     (out, type_postfix);
        
        if (match_ss(type, make_lit_string("struct")) ||
            match_ss(type, make_lit_string("union"))){
            
            if (hide_children){
                append_sc(out, " { /* non-public internals */ } ;");
            }
            else{
                append_sc(out, " {<br><div style='margin-left: 8mm;'>");
                
                for (Item_Node *member_iter = member->first_child;
                     member_iter != 0;
                     member_iter = member_iter->next_sibling){
                    print_struct_html(out, member_iter, hide_children);
                }
                
                append_sc(out, "</div>};<br>");
            }
        }
        else{
            append_sc(out, ";<br>");
        }
    }
    
    static void
        print_function_html(String *out, Used_Links *used, String cpp_name, String ret, char *function_call_head, String name, Argument_Breakdown breakdown){
        
        append_ss     (out, ret);
        append_s_char (out, ' ');
        append_sc     (out, function_call_head);
        append_ss     (out, name);
        
        if (breakdown.count == 0){
            append_sc(out, "()");
        }
        else if (breakdown.count == 1){
            append_sc(out, "(");
            append_ss(out, breakdown.args[0].param_string);
            append_sc(out, ")");
        }
        else{
            append_sc(out, "(<div style='margin-left: 4mm;'>");
            
            for (int32_t j = 0; j < breakdown.count; ++j){
                append_ss(out, breakdown.args[j].param_string);
                if (j < breakdown.count - 1){
                    append_s_char(out, ',');
                }
                append_sc(out, "<br>");
            }
            
            append_sc(out, "</div>)");
        }
    }
    
    static void
        print_macro_html(String *out, String name, Argument_Breakdown breakdown){
        
        append_sc (out, "#define ");
        append_ss (out, name);
        
        if (breakdown.count == 0){
            append_sc(out, "()");
        }
        else if (breakdown.count == 1){
            append_s_char  (out, '(');
            append_ss      (out, breakdown.args[0].param_string);
            append_s_char  (out, ')');
        }
        else{
            append_sc (out, "(<div style='margin-left: 4mm;'>");
            
            for (int32_t j = 0; j < breakdown.count; ++j){
                append_ss(out, breakdown.args[j].param_string);
                if (j < breakdown.count - 1){
                    append_s_char(out, ',');
                }
                append_sc(out, "<br>");
            }
            
            append_sc(out, ")</div>)");
        }
    }
    
    enum Doc_Chunk_Type{
        DocChunk_PlainText,
        DocChunk_CodeExample,
        
        DocChunk_Count
    };
    
    static String doc_chunk_headers[] = {
        make_lit_string(""),
        make_lit_string("CODE_EXAMPLE"),
    };
    
    static String
        get_next_doc_chunk(String source, String prev_chunk, Doc_Chunk_Type *type){
        String chunk = {0};
        String word = {0};
        int32_t pos = source.size;
        int32_t word_index = 0;
        Doc_Chunk_Type t = DocChunk_PlainText;
        
        int32_t start_pos = (int32_t)(prev_chunk.str - source.str) + prev_chunk.size;
        String source_tail = substr_tail(source, start_pos);
        
        Assert(DocChunk_Count == ArrayCount(doc_chunk_headers));
        
        for (word = get_first_word(source_tail);
             word.str;
             word = get_next_word(source_tail, word), ++word_index){
            
            for (int32_t i = 1; i < DocChunk_Count; ++i){
                if (match_ss(word, doc_chunk_headers[i])){
                    pos = (int32_t)(word.str - source.str);
                    t = (Doc_Chunk_Type)i;
                    goto doublebreak;
                }
            }
        }
        doublebreak:;
        
        *type = DocChunk_PlainText;
        if (word_index == 0){
            *type = t;
            
            int32_t nest_level = 1;
            int32_t i = find_s_char(source, pos, '(');
            for (++i; i < source.size; ++i){
                if (source.str[i] == '('){
                    ++nest_level;
                }
                else if (source.str[i] == ')'){
                    --nest_level;
                    if (nest_level == 0){
                        break;
                    }
                }
            }
            
            pos = i+1;
        }
        
        chunk = substr(source, start_pos, pos - start_pos);
        
        int32_t is_all_white = 1;
        for (int32_t i = 0; i < chunk.size; ++i){
            if (!char_is_whitespace(chunk.str[i])){
                is_all_white = 0;
                break;
            }
        }
        
        if (is_all_white){
            chunk = null_string;
        }
        
        return(chunk);
    }
    
    static String
        get_first_doc_chunk(String source, Doc_Chunk_Type *type){
        String start_str = make_string(source.str, 0);
        String chunk = get_next_doc_chunk(source, start_str, type);
        return(chunk);
    }
    
    static void
        print_doc_description(String *out, Partition *part, String src){
        Doc_Chunk_Type type;
        
        for (String chunk = get_first_doc_chunk(src, &type);
             chunk.str;
             chunk = get_next_doc_chunk(src, chunk, &type)){
            
            switch (type){
                case DocChunk_PlainText:
                {
                    for (String line = get_first_double_line(chunk);
                         line.str;
                         line = get_next_double_line(chunk, line)){
                        append_ss(out, line);
                        append_sc(out, "<br><br>");
                    }
                }break;
                
                case DocChunk_CodeExample:
                {
                    int32_t start = 0;
                    int32_t end = chunk.size-1;
                    while (start < end && chunk.str[start] != '(') ++start;
                    start += 1;
                    while (end > start && chunk.str[end] != ')') --end;
                    
                    
                    append_sc(out, HTML_EXAMPLE_CODE_OPEN);
                    
                    if (start < end){
                        String code_example = substr(chunk, start, end - start);
                        int32_t first_line = 1;
                        
                        for (String line = get_first_line(code_example);
                             line.str;
                             line = get_next_line(code_example, line)){
                            
                            if (!(first_line && line.size == 0)){
                                int32_t space_i = 0;
                                for (; space_i < line.size; ++space_i){
                                    if (line.str[space_i] == ' '){
                                        append_sc(out, "&nbsp;");
                                    }
                                    else{
                                        break;
                                    }
                                }
                                
                                String line_tail = substr_tail(line, space_i);
                                append_ss(out, line_tail);
                                append_sc(out, "<br>");
                            }
                            first_line = 0;
                        }
                    }
                    
                    append_sc(out, HTML_EXAMPLE_CODE_CLOSE);
                }break;
            }
        }
    }
    
    static void
        print_struct_docs(String *out, Partition *part, Item_Node *member){
        for (Item_Node *member_iter = member->first_child;
             member_iter != 0;
             member_iter = member_iter->next_sibling){
            String type = member_iter->type;
            if (match_ss(type, make_lit_string("struct")) ||
                match_ss(type, make_lit_string("union"))){
                print_struct_docs(out, part, member_iter);
            }
            else{
                Documentation doc = {0};
                perform_doc_parse(part, member_iter->doc_string, &doc);
                
                append_sc(out, "<div>");
                
                append_sc(out, "<div style='"HTML_CODE_STYLE"'>"HTML_DOC_ITEM_HEAD_INL_OPEN);
                append_ss(out, member_iter->name);
                append_sc(out, HTML_DOC_ITEM_HEAD_INL_CLOSE"</div>");
                
                append_sc(out, "<div style='margin-bottom: 6mm;'>"HTML_DOC_ITEM_OPEN);
                print_doc_description(out, part, doc.main_doc);
                append_sc(out, HTML_DOC_ITEM_CLOSE"</div>");
                
                append_sc(out, "</div>");
            }
        }
    }
    
    static void
        print_see_also(String *out, Documentation *doc){
        int32_t doc_see_count = doc->see_also_count;
        if (doc_see_count > 0){
            append_sc(out, HTML_DOC_HEAD_OPEN"See Also"HTML_DOC_HEAD_CLOSE);
            
            for (int32_t j = 0; j < doc_see_count; ++j){
                String see_also = doc->see_also[j];
                append_sc(out, HTML_DOC_ITEM_OPEN"<a href='#");
                append_ss(out, see_also);
                append_sc(out, "_doc'>");
                append_ss(out, see_also);
                append_sc(out, "</a>"HTML_DOC_ITEM_CLOSE);
            }
        }
    }
    
    static void
        print_function_docs(String *out, Partition *part, String name, String doc_string){
        if (doc_string.size == 0){
            append_sc(out, "No documentation generated for this function.");
            fprintf(stderr, "warning: no documentation string for %.*s\n", name.size, name.str);
        }
        
        Temp_Memory temp = begin_temp_memory(part);
        
        Documentation doc = {0};
        
        perform_doc_parse(part, doc_string, &doc);
        
        int32_t doc_param_count = doc.param_count;
        if (doc_param_count > 0){
            append_sc(out, HTML_DOC_HEAD_OPEN"Parameters"HTML_DOC_HEAD_CLOSE);
            
            for (int32_t j = 0; j < doc_param_count; ++j){
                String param_name = doc.param_name[j];
                String param_docs = doc.param_docs[j];
                
                // TODO(allen): check that param_name is actually
                // a parameter to this function!
                
                append_sc(out, "<div>"HTML_DOC_ITEM_HEAD_OPEN);
                append_ss(out, param_name);
                append_sc(out, HTML_DOC_ITEM_HEAD_CLOSE"<div style='margin-bottom: 6mm;'>"HTML_DOC_ITEM_OPEN);
                append_ss(out, param_docs);
                append_sc(out, HTML_DOC_ITEM_CLOSE"</div></div>");
            }
        }
        
        String ret_doc = doc.return_doc;
        if (ret_doc.size != 0){
            append_sc(out, HTML_DOC_HEAD_OPEN"Return"HTML_DOC_HEAD_CLOSE HTML_DOC_ITEM_OPEN);
            append_ss(out, ret_doc);
            append_sc(out, HTML_DOC_ITEM_CLOSE);
        }
        
        String main_doc = doc.main_doc;
        if (main_doc.size != 0){
            append_sc(out, HTML_DOC_HEAD_OPEN"Description"HTML_DOC_HEAD_CLOSE HTML_DOC_ITEM_OPEN);
            print_doc_description(out, part, main_doc);
            append_sc(out, HTML_DOC_ITEM_CLOSE);
        }
        
        print_see_also(out, &doc);
        
        end_temp_memory(temp);
    }
    
    static void
        print_item_html(String *out, Partition *part, Used_Links *used, Item_Node *item, char *id_postfix, char *section, int32_t I, Alternate_Name *alt_name, int32_t alt_name_type){
        Temp_Memory temp = begin_temp_memory(part);
        
        String name = item->name;
            
            switch (alt_name_type){
                case AltName_Macro:
                {
                    name = alt_name->macro;
                }break;
                
                case AltName_Public_Name:
                {
                    name = alt_name->public_name;
                }break;
            }
            
        /* NOTE(allen):
        Open a div for the whole item.
        Put a heading in it with the name and section.
        Open a "descriptive" box for the display of the code interface.
        */
        append_sc(out, "<div id='");
        append_ss(out, name);
        append_sc(out, id_postfix);
        append_sc(out, "' style='margin-bottom: 1cm;'>");
        
        int32_t has_cpp_name = 0;
        if (item->cpp_name.str != 0){
            if (try_to_use(used, item->cpp_name)){
                append_sc(out, "<div id='");
                append_ss(out, item->cpp_name);
                append_sc(out, id_postfix);
                append_sc(out, "'>");
                has_cpp_name = 1;
            }
        }
        
        append_sc         (out, "<h4>&sect;");
        append_sc         (out, section);
        append_s_char     (out, '.');
        append_int_to_str (out, I);
        append_sc         (out, ": ");
        append_ss         (out, name);
        append_sc         (out, "</h4>");
        
        append_sc(out, "<div style='"HTML_CODE_STYLE" "HTML_DESCRIPT_SECTION_STYLE"'>");
        
        switch (item->t){
            case Item_Function:
            {
                // NOTE(allen): Code box
                print_function_html(out, used, item->cpp_name, item->ret, "", name, item->breakdown);
                
                // NOTE(allen): Close the code box
                append_sc(out, "</div>");
                
                // NOTE(allen): Descriptive section
                print_function_docs(out, part, name, item->doc_string);
            }break;
            
            case Item_Macro:
            {
                // NOTE(allen): Code box
                print_macro_html(out, name, item->breakdown);
                
                // NOTE(allen): Close the code box
                append_sc(out, "</div>");
                
                // NOTE(allen): Descriptive section
                print_function_docs(out, part, name, item->doc_string);
            }break;
            
            case Item_Typedef:
            {
                String type = item->type;
                
                // NOTE(allen): Code box
                append_sc     (out, "typedef ");
                append_ss     (out, type);
                append_s_char (out, ' ');
                append_ss     (out, name);
                append_s_char (out, ';');
                
                // NOTE(allen): Close the code box
                append_sc(out, "</div>");
                
                // NOTE(allen): Descriptive section
                String doc_string = item->doc_string;
                Documentation doc = {0};
                perform_doc_parse(part, doc_string, &doc);
                
                String main_doc = doc.main_doc;
                if (main_doc.size != 0){
                    append_sc(out, HTML_DOC_HEAD_OPEN"Description"HTML_DOC_HEAD_CLOSE);
                    
                    append_sc(out, HTML_DOC_ITEM_OPEN);
                    print_doc_description(out, part, main_doc);
                    append_sc(out, HTML_DOC_ITEM_CLOSE);
                }
                else{
                    fprintf(stderr, "warning: no documentation string for %.*s\n", name.size, name.str);
                }
                
                print_see_also(out, &doc);
                
            }break;
            
            case Item_Enum:
            {
                // NOTE(allen): Code box
                append_sc     (out, "enum ");
                append_ss     (out, name);
                append_s_char (out, ';');
                
                // NOTE(allen): Close the code box
                append_sc(out, "</div>");
                
                // NOTE(allen): Descriptive section
                String doc_string = item->doc_string;
                Documentation doc = {0};
                perform_doc_parse(part, doc_string, &doc);
                
                String main_doc = doc.main_doc;
                if (main_doc.size != 0){
                    append_sc(out, HTML_DOC_HEAD_OPEN"Description"HTML_DOC_HEAD_CLOSE);
                    
                    append_sc(out, HTML_DOC_ITEM_OPEN);
                    print_doc_description(out, part, main_doc);
                    append_sc(out, HTML_DOC_ITEM_CLOSE);
                }
                else{
                    fprintf(stderr, "warning: no documentation string for %.*s\n", name.size, name.str);
                }
                
                if (item->first_child){
                    append_sc(out, HTML_DOC_HEAD_OPEN"Values"HTML_DOC_HEAD_CLOSE);
                    
                    for (Item_Node *member = item->first_child;
                         member;
                         member = member->next_sibling){
                        Documentation doc = {0};
                        perform_doc_parse(part, member->doc_string, &doc);
                        
                        append_sc(out, "<div>");
                        
                        // NOTE(allen): Dafuq is this all?
                        append_sc(out, "<div><span style='"HTML_CODE_STYLE"'>"HTML_DOC_ITEM_HEAD_INL_OPEN);
                        append_ss(out, member->name);
                        append_sc(out, HTML_DOC_ITEM_HEAD_INL_CLOSE);
                        
                        if (member->value.str){
                            append_sc(out, " = ");
                            append_ss(out, member->value);
                        }
                        
                        append_sc(out, "</span></div>");
                        
                        append_sc(out, "<div style='margin-bottom: 6mm;'>"HTML_DOC_ITEM_OPEN);
                        print_doc_description(out, part, doc.main_doc);
                        append_sc(out, HTML_DOC_ITEM_CLOSE"</div>");
                        
                        append_sc(out, "</div>");
                    }
                }
                
                print_see_also(out, &doc);
                
            }break;
            
            case Item_Struct: case Item_Union:
            {
                String doc_string = item->doc_string;
                
                int32_t hide_members = 0;
                
                if (doc_string.size == 0){
                    hide_members = 1;
                }
                else{
                    for (String word = get_first_word(doc_string);
                         word.str;
                         word = get_next_word(doc_string, word)){
                        if (match_ss(word, make_lit_string("HIDE_MEMBERS"))){
                            hide_members = 1;
                            break;
                        }
                    }
                }
                
                // NOTE(allen): Code box
                print_struct_html(out, item, hide_members);
                
                // NOTE(allen): Close the code box
                append_sc(out, "</div>");
                
                // NOTE(allen): Descriptive section
                {
                    Documentation doc = {0};
                    perform_doc_parse(part, doc_string, &doc);
                    
                    String main_doc = doc.main_doc;
                    if (main_doc.size != 0){
                        append_sc(out, HTML_DOC_HEAD_OPEN"Description"HTML_DOC_HEAD_CLOSE);
                        
                        append_sc(out, HTML_DOC_ITEM_OPEN);
                        print_doc_description(out, part, main_doc);
                        append_sc(out, HTML_DOC_ITEM_CLOSE);
                    }
                    else{
                        fprintf(stderr, "warning: no documentation string for %.*s\n", name.size, name.str);
                    }
                    
                    if (!hide_members){
                        if (item->first_child){
                            append_sc(out, HTML_DOC_HEAD_OPEN"Fields"HTML_DOC_HEAD_CLOSE);
                            print_struct_docs(out, part, item);
                        }
                    }
                    
                    print_see_also(out, &doc);
                }
            }break;
        }
        
        if (has_cpp_name){
            append_sc(out, "</div>");
        }
        
        // NOTE(allen): Close the item box
        append_sc(out, "</div><hr>");
        
        end_temp_memory(temp);
    }
    
static void
        doc_item_head_html(String *out, Partition *part, Used_Links *used_links, Document_Item *item, Section_Counter section_counter){
    switch (item->type){
        case Doc_Root:
        {
            append_sc(out,
                      "<html lang=\"en-US\">"
                      "<head>"
                      "<title>");
            
            append_ss(out, item->section.name);
            
            append_sc(out,
                      "</title>"
                      "<style>"
                      
                      "body { "
                      "background: " HTML_BACK_COLOR "; "
                      "color: " HTML_TEXT_COLOR "; "
                      "}"
                      
                      // H things
                      "h1,h2,h3,h4 { "
                      "color: " HTML_POP_COLOR_1 "; "
                      "margin: 0; "
                      "}"
                      
                      "h2 { "
                      "margin-top: 6mm; "
                      "}"
                      
                      "h3 { "
                      "margin-top: 5mm; margin-bottom: 5mm; "
                      "}"
                      
                      "h4 { "
                      "font-size: 1.1em; "
                      "}"
                      
                      // ANCHORS
                      "a { "
                      "color: " HTML_POP_COLOR_1 "; "
                      "text-decoration: none; "
                      "}"
                      "a:visited { "
                      "color: " HTML_VISITED_LINK "; "
                      "}"
                      "a:hover { "
                      "background: " HTML_POP_BACK_1 "; "
                      "}"
                      
                      // LIST
                      "ul { "
                      "list-style: none; "
                      "padding: 0; "
                      "margin: 0; "
                      "}"
                      
                      "</style>"
                      "</head>\n"
                      "<body>"
                      "<div style='font-family:Arial; margin: 0 auto; "
                      "width: 800px; text-align: justify; line-height: 1.25;'>");
            
            
            append_sc(out, "<h1 style='margin-top: 5mm; margin-bottom: 5mm;'>");
            append_ss(out, item->section.name);
            append_sc(out, "</h1>");
        }break;
        
        case Doc_Section:
        {
            if (section_counter.nest_level <= 1){
            if (item->section.id.size > 0){
            append_sc(out, "\n<h2 id='section_");
            append_ss(out, item->section.id);
            append_sc(out, "'>&sect;");
            }
            else{
                append_sc(out, "\n<h2>&sect;");
            }
            append_section_number(out, section_counter);
                append_sc(out, " ");
            append_ss(out, item->section.name);
            append_sc(out, "</h2>");
        }
        else{
            if (item->section.id.size > 0){
                append_sc(out, "<h3 id='section_");
                append_ss(out, item->section.id);
                append_sc(out, "'>&sect;");
            }
            else{
                append_sc(out, "<h3>&sect;");
            }
            append_section_number(out, section_counter);
            append_sc(out, " ");
            append_ss(out, item->section.name);
            append_sc(out, "</h3>");
        }
        }break;
        
        case Doc_Todo:
        {
            append_sc(out, "<div><i>Coming Soon</i><div>");
        }break;
        
        case Doc_Enriched_Text:
        {
            write_enriched_text_html(out, item->text.text);
        }break;
        
        case Doc_Element_List:
        {
            append_sc(out, "<ul>");
            
            Meta_Unit *unit = item->unit_elements.unit;
            Alternate_Names_Array *alt_names = item->unit_elements.alt_names;
            int32_t count = unit->set.count;
            
             switch (item->unit_elements.alt_name_type){
                 case AltName_Standard:
                 {
            for (int32_t i = 0; i < count; ++i){
                print_item_in_list(out, unit->set.items[i].name, "_doc");
            }
        }break;
        
        case AltName_Macro:
        {
            for (int32_t i = 0; i < count; ++i){
                print_item_in_list(out, alt_names->names[i].macro, "_doc");
            }
        }break;
        
        case AltName_Public_Name:
        {
            for (int32_t i = 0; i < count; ++i){
                print_item_in_list(out, alt_names->names[i].public_name, "_doc");
            }
        }break;
        }
            
            append_sc(out, "</ul>");
        }break;
        
        case Doc_Full_Elements:
        {
            Meta_Unit *unit = item->unit_elements.unit;
            Alternate_Names_Array *alt_names = item->unit_elements.alt_names;
            int32_t count = unit->set.count;
            
            char section_space[32];
            String section_str = make_fixed_width_string(section_space);
            append_section_number_reduced(&section_str, section_counter, 1);
            terminate_with_null(&section_str);
            
            if (alt_names){
            int32_t I = 1;
            for (int32_t i = 0; i < count; ++i, ++I){
                print_item_html(out, part, used_links, &unit->set.items[i], "_doc", section_str.str, I, &alt_names->names[i], item->unit_elements.alt_name_type);
            }
        }
        else{
            int32_t I = 1;
            for (int32_t i = 0; i < count; ++i, ++I){
                print_item_html(out, part, used_links, &unit->set.items[i], "_doc", section_str.str, I, 0, 0);
            }
        }
        }break;
        
        case Doc_Table_Of_Contents:
        {
            append_sc(out, "<h3 style='margin:0;'>Table of Contents</h3><ul>");
            
            int32_t i = 1;
            for (Document_Item *toc_item = item->parent->section.first_child;
                 toc_item != 0;
                 toc_item = toc_item->next){
                if (toc_item->type == Doc_Section){
                    if (toc_item->section.id.size > 0){
                append_sc(out, "<li><a href='#section_");
                append_ss(out, toc_item->section.id);
                append_sc(out, "'>&sect;");
                    }
                    else{
                        append_sc(out, "<li>&sect;");
                    }
                append_int_to_str (out, i);
                append_s_char     (out, ' ');
                    append_ss         (out, toc_item->section.name);
                    append_sc         (out, "</a></li>");
                    ++i;
                }
            }
            
            append_sc(out, "</ul>");
        }break;
    }
}

static void
doc_item_foot_html(String *out, Partition *part, Used_Links *used_links, Document_Item *item, Section_Counter section_counter){
    switch (item->type){
        case Doc_Root:
        {
            append_sc(out, "</div></body></html>");
        }break;
        
        case Doc_Section: break;
        
        case Doc_Table_Of_Contents: break;
    }
}

static void
generate_item_html(String *out, Partition *part, Used_Links *used_links, Document_Item *item, Section_Counter *section_counter){
    Section_Counter sc = *section_counter;
    doc_item_head_html(out, part, used_links, item, sc);
    
    if (item->type == Doc_Root || item->type == Doc_Section){
        int32_t level = ++section_counter->nest_level;
        section_counter->counter[level] = 1;
        for (Document_Item *m = item->section.first_child;
             m != 0;
             m = m->next){
            generate_item_html(out, part, used_links, m, section_counter);
        }
        --section_counter->nest_level;
        ++section_counter->counter[section_counter->nest_level];
    }
    
    doc_item_foot_html(out, part, used_links, item, sc);
}

static void
generate_document_html(String *out, Partition *part, Abstract_Document *doc){
    Used_Links used_links = {0};
    init_used_links(part, &used_links, 4000);
    
    Section_Counter section_counter = {0};
    section_counter.counter[section_counter.nest_level] = 1;
    generate_item_html(out, part, &used_links, doc->root_item, &section_counter);
}

#endif

// BOTTOM
