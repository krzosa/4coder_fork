
struct Config_Value{
    String8 section;
    String8 subsection[2];
    String8 type;
    String8 name;
    String8 value_specifier;

    String8 value_str[4];
    i32 value_str_count;

    bool value_bool;
    f32  value_float;
    i64  value_int;
    u32  value_u32;
};

struct Loaded_Config{
    Config_Value *values;
    i32 count;
};

function Loaded_Config
parse_config(Arena *scratch, Arena *string_arena, String8 text, String8 default_section = string_u8_litexpr("config")){
    Token_List token_list = lex_full_input_cpp(scratch, text);
    Config_Value *values = push_array(scratch, Config_Value, 256);
    i32 values_count = 0;

    String8 subsections[2] = {};
    String8 section = default_section;
    Token_Iterator_List it = token_iterator(0, &token_list);
    for(;;){
        Token *token = next_token(&it);
        if(token->kind == TokenBaseKind_EOF){
            break;
        }

        //
        // Parse section
        //
        else if(token->sub_kind == TokenCppKind_BrackOp){
            token = match_token(&it, TokenBaseKind_Identifier);
            if(token){
                section = get_token_string(text, token);

                // Subsections
                if(match_token(&it, TokenCppKind_Colon)){
                    for(int i = 0; i < 2; i++){
                        // TODO(Krzosa): Errors, require token
                        Token *subsection = match_token(&it, TokenBaseKind_Identifier);
                        if(subsection){
                            subsections[i] = get_token_string(text, subsection);
                            if(!match_token(&it, TokenCppKind_Comma)){
                                break;
                            }
                        }
                        else break;
                    }
                }

                match_token(&it, TokenCppKind_BrackCl);
            }
            else{
                // TODO(Krzosa): Notify but don't error
            }
        }

        //
        // Parse value
        //
        else if(token->kind == TokenBaseKind_Identifier){
            String8 string = get_token_string(text, token);
            if(match_token(&it, TokenCppKind_Eq)){
                Token *value    = next_token(&it);
                Config_Value *v = values + values_count++;
                v->value_str[0] = get_token_string(text, value);
                v->name         = string;
                v->section      = section;
                v->subsection[0] = subsections[0];
                v->subsection[1] = subsections[1];

                switch(value->sub_kind){
                    case TokenCppKind_LiteralFalse:
                    Case(TokenCppKind_LiteralTrue){
                        v->type = string_u8_litexpr("b32");
                        v->value_specifier = string_u8_litexpr("value_bool");
                        v->value_bool = value->sub_kind == TokenCppKind_LiteralTrue ? true : false;
                    }Break;

                    Case(TokenCppKind_LiteralString){
                        v->value_str_count = 1;
                        v->type = string_u8_litexpr("String8");
                        v->value_specifier = string_u8_litexpr("value_str[0]");

                        //
                        // Can have more then one value
                        //
                        for(int i = 0; i < 4; i++){
                            v->value_str[i].str += 1; v->value_str[i].size -= 2;
                            v->value_str[i] = push_string_copy(string_arena, v->value_str[i]);
                            if(match_token(&it, TokenCppKind_Comma)){
                                Token *next_value = next_token(&it);
                                if(next_value->kind == TokenBaseKind_LiteralString){
                                    value = next_value;
                                    v->value_specifier = string_u8_litexpr("value_str");
                                    v->value_str[v->value_str_count] = get_token_string(text, value);
                                    v->value_str_count += 1;
                                } else break;
                            } else break;
                        }
                    }Break;

                    Case(TokenCppKind_LiteralInteger){
                        v->type = string_u8_litexpr("i64");
                        v->value_int = string_to_integer(v->value_str[0], 10);
                        v->value_specifier = string_u8_litexpr("value_int");
                    }Break;

                    Case(TokenCppKind_LiteralIntegerHex){
                        v->type      = string_u8_litexpr("u32");
                        String8 hex  = string_skip(v->value_str[0], 2);
                        v->value_u32 = string_to_integer(hex, 16);
                        v->value_specifier = string_u8_litexpr("value_u32");
                    }Break;

                    Case(TokenCppKind_Identifier){
                        v->type = string_u8_litexpr("i64");
                        // TODO(Krzosa)
                        v->value_specifier = string_u8_litexpr("value_int");
                    }Break;

                    case TokenCppKind_LiteralFloat64:
                    Case(TokenCppKind_LiteralFloat32){
                        v->type = string_u8_litexpr("f32");
                        v->value_float = atof((char *)v->value_str[0].str);
                        v->value_specifier = string_u8_litexpr("value_float");
                    }Break;

                    default:{
                        // TODO(Krzosa): Notify but don't error
                    }
                }
            }
        }
        else{
            // TODO(Krzosa): Notify but don't error
        }
    }

    Loaded_Config result = {values, values_count};
    return result;
}

