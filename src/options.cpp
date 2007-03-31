/**
 * @file options.cpp
 * Parses the options from the config file.
 *
 * @author  Ben Gardner
 * @license GPL v2+
 *
 * $Id$
 */
#include "uncrustify_types.h"
#include "args.h"
#include "prototypes.h"
#include <cstring>
#ifdef HAVE_STRINGS_H
#include <strings.h> /* strcasecmp() */
#endif
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cctype>


static std::map<std::string, option_map_value>      option_name_map;
static std::map<uncrustify_groups, group_map_value> group_map;
static uncrustify_groups                            current_group;


static void unc_add_option(const char *name,
                           uncrustify_options id,
                           argtype_e type,
                           const char *short_desc = NULL,
                           const char *long_desc = NULL,
                           int min_val = 0,
                           int max_val = 16);


void unc_begin_group(uncrustify_groups id, const char *short_desc,
                     const char *long_desc)
{
   current_group = id;

   group_map_value value;

   value.id         = id;
   value.short_desc = short_desc;
   value.long_desc  = long_desc;

   group_map[id] = value;
}

void unc_add_option(const char *name, uncrustify_options id, argtype_e type,
                    const char *short_desc, const char *long_desc,
                    int min_val, int max_val)
{
   group_map[current_group].options.push_back(id);

   option_map_value value;

   value.id         = id;
   value.group_id   = current_group;
   value.type       = type;
   value.name       = name;
   value.short_desc = short_desc;
   value.long_desc  = long_desc;
   value.min_val    = 0;

   /* Calculate the max/min values */
   switch (type)
   {
   case AT_BOOL:
      value.max_val = 1;
      break;

   case AT_IARF:
      value.max_val = 3;
      break;

   case AT_NUM:
      value.min_val = min_val;
      value.max_val = max_val;
      break;

   case AT_LINE:
      value.max_val = 3;
      break;

   case AT_POS:
      value.max_val = 2;
      break;

   default:
      fprintf(stderr, "FATAL: Illegal option type %d for '%s'\n", type, name);
      exit(EXIT_FAILURE);
   }

   option_name_map[name] = value;

   int name_len = strlen(name);
   if (name_len > cpd.max_option_name_len)
   {
      cpd.max_option_name_len = name_len;
   }
}

const option_map_value *unc_find_option(const char *name)
{
   if (option_name_map.find(name) == option_name_map.end())
   {
      return(NULL);
   }
   return(&option_name_map[name]);
}

void register_options(void)
{
   unc_begin_group(UG_general, "General options");
   unc_add_option("newlines", UO_newlines, AT_LINE, "The type of line endings");
   unc_add_option("input_tab_size", UO_input_tab_size, AT_NUM, "The original size of tabs in the input", "", 1, 32);
   unc_add_option("output_tab_size", UO_output_tab_size, AT_NUM, "The size of tabs in the output (only used if align_with_tabs=true)", "", 1, 32);
   unc_add_option("string_escape_char", UO_string_escape_char, AT_NUM, "The ascii value of the string escape char, usually 92 (\\). (Pawn)", "", 0, 255);

   unc_begin_group(UG_space, "Spacing options");
   unc_add_option("sp_arith", UO_sp_arith, AT_IARF, "Add or remove space around arithmetic operator '+', '-', '/', '*', etc");
   unc_add_option("sp_assign", UO_sp_assign, AT_IARF, "Add or remove space around assignment operator '=', '+=', etc");
   unc_add_option("sp_bool", UO_sp_bool, AT_IARF, "Add or remove space around boolean operators '&&' and '||'");
   unc_add_option("sp_compare", UO_sp_compare, AT_IARF, "Add or remove space around compare operator '<', '>', '==', etc");
   unc_add_option("sp_inside_paren", UO_sp_inside_paren, AT_IARF, "Add or remove space inside '(' and ')'");
   unc_add_option("sp_paren_paren", UO_sp_paren_paren, AT_IARF, "Add or remove space between nested parens");
   unc_add_option("sp_balance_nested_parens", UO_sp_balance_nested_parens, AT_BOOL, "Whether to balance spaces inside nested parens");
   unc_add_option("sp_paren_brace", UO_sp_paren_brace, AT_IARF, "Add or remove space between ')' and '{'");
   unc_add_option("sp_before_ptr_star", UO_sp_before_ptr_star, AT_IARF, "Add or remove space before pointer star '*'");
   unc_add_option("sp_between_ptr_star", UO_sp_between_ptr_star, AT_IARF, "Add or remove space between pointer stars '*'");
   unc_add_option("sp_after_ptr_star", UO_sp_after_ptr_star, AT_IARF, "Add or remove space after pointer star '*'");
   unc_add_option("sp_before_byref", UO_sp_before_byref, AT_IARF, "Add or remove space before reference sign '&'");
   unc_add_option("sp_after_byref", UO_sp_after_byref, AT_IARF, "Add or remove space after reference sign '&'");
   unc_add_option("sp_before_angle", UO_sp_before_angle, AT_IARF, "Add or remove space before '<>'");
   unc_add_option("sp_after_angle", UO_sp_after_angle, AT_IARF, "Add or remove space after '<>'");
   unc_add_option("sp_angle_paren", UO_sp_angle_paren, AT_IARF, "Add or remove space between '<>' and '(' as found in 'new List<byte>();'");
   unc_add_option("sp_angle_word", UO_sp_angle_word, AT_IARF, "Add or remove space between '<>' and a word as in 'List<byte> m;'");
   unc_add_option("sp_before_sparen", UO_sp_before_sparen, AT_IARF, "Add or remove space before '(' of 'if', 'for', 'switch', and 'while'");
   unc_add_option("sp_inside_sparen", UO_sp_inside_sparen, AT_IARF, "Add or remove space inside if-condition '(' and ')'");
   unc_add_option("sp_after_sparen", UO_sp_after_sparen, AT_IARF, "Add or remove space after ')' of 'if', 'for', 'switch', and 'while'");
   unc_add_option("sp_sparen_brace", UO_sp_sparen_brace, AT_IARF, "Add or remove space between ')' and '{' of 'if', 'for', 'switch', and 'while'");
   unc_add_option("sp_special_semi", UO_sp_special_semi, AT_IARF, "Add or remove space before empty statement ';' on 'if', 'for' and 'while'");
   unc_add_option("sp_before_semi", UO_sp_before_semi, AT_IARF, "Add or remove space before ';'");
   unc_add_option("sp_before_semi_for", UO_sp_before_semi_for, AT_IARF, "Add or remove space before ';' in non-empty 'for' statements");
   unc_add_option("sp_before_semi_for_empty", UO_sp_before_semi_for_empty, AT_IARF, "Add or remove space before a semicolon of an empty part of a for statment.");
   unc_add_option("sp_before_square", UO_sp_before_square, AT_IARF, "Add or remove space before '[' (except '[]')");
   unc_add_option("sp_before_squares", UO_sp_before_squares, AT_IARF, "Add or remove space before '[]'");
   unc_add_option("sp_inside_square", UO_sp_inside_square, AT_IARF, "Add or remove space inside '[' and ']'");
   unc_add_option("sp_after_comma", UO_sp_after_comma, AT_IARF, "Add or remove space after ','");
   unc_add_option("sp_before_comma", UO_sp_before_comma, AT_IARF, "Add or remove space before ','");
   unc_add_option("sp_after_operator", UO_sp_after_operator, AT_IARF, "Add or remove space between 'operator' and operator sign");
   unc_add_option("sp_after_cast", UO_sp_after_cast, AT_IARF, "Add or remove space after cast");
   unc_add_option("sp_inside_paren_cast", UO_sp_inside_paren_cast, AT_IARF, "Add or remove spaces inside cast parens");
   unc_add_option("sp_sizeof_paren", UO_sp_sizeof_paren, AT_IARF, "Add or remove space between 'sizeof' and '('");
   unc_add_option("sp_after_tag", UO_sp_after_tag, AT_IARF, "Add or remove space after the tag keyword (Pawn)");
   unc_add_option("sp_inside_braces_enum", UO_sp_inside_braces_enum, AT_IARF, "Add or remove space inside enum '{' and '}'");
   unc_add_option("sp_inside_braces_struct", UO_sp_inside_braces_struct, AT_IARF, "Add or remove space inside struct/union '{' and '}'");
   unc_add_option("sp_inside_braces", UO_sp_inside_braces, AT_IARF, "Add or remove space inside '{' and '}'");
   unc_add_option("sp_inside_angle", UO_sp_inside_angle, AT_IARF, "Add or remove space inside '<' and '>'");
   unc_add_option("sp_type_func", UO_sp_type_func, AT_IARF, "Add or remove space between return type and function name\nA minimum of 1 is forced except for pointer return types.");
   unc_add_option("sp_func_proto_paren", UO_sp_func_proto_paren, AT_IARF, "Add or remove space between function name and '(' on function declaration");
   unc_add_option("sp_func_def_paren", UO_sp_func_def_paren, AT_IARF, "Add or remove space between function name and '(' on function definition");
   unc_add_option("sp_inside_fparens", UO_sp_inside_fparens, AT_IARF, "Add or remove space inside empty function '()'");
   unc_add_option("sp_inside_fparen", UO_sp_inside_fparen, AT_IARF, "Add or remove space inside function '(' and ')'");
   unc_add_option("sp_square_fparen", UO_sp_square_fparen, AT_IARF, "Add or remove space between ']' and '(' when part of a function call.");
   unc_add_option("sp_fparen_brace", UO_sp_fparen_brace, AT_IARF, "Add or remove space between ')' and '{' of function");
   unc_add_option("sp_func_call_paren", UO_sp_func_call_paren, AT_IARF, "Add or remove space between function name and '(' on function calls");
   unc_add_option("sp_func_class_paren", UO_sp_func_class_paren, AT_IARF, "Add or remove space between a constructor/destructor and the open paren");
   unc_add_option("sp_return_paren", UO_sp_return_paren, AT_IARF, "Add or remove space between 'return' and '('");
   unc_add_option("sp_macro", UO_sp_macro, AT_IARF, "Add or remove space between macro and value");
   unc_add_option("sp_macro_func", UO_sp_macro_func, AT_IARF, "Add or remove space between macro function ')' and value");
   unc_add_option("sp_else_brace", UO_sp_else_brace, AT_IARF, "Add or remove space between 'else' and '{' if on the same line");
   unc_add_option("sp_brace_else", UO_sp_brace_else, AT_IARF, "Add or remove space between '}' and 'else' if on the same line");
   unc_add_option("sp_catch_brace", UO_sp_catch_brace, AT_IARF, "Add or remove space between 'catch' and '{' if on the same line");
   unc_add_option("sp_brace_catch", UO_sp_brace_catch, AT_IARF, "Add or remove space between '}' and 'catch' if on the same line");
   unc_add_option("sp_finally_brace", UO_sp_finally_brace, AT_IARF, "Add or remove space between 'finally' and '{' if on the same line");
   unc_add_option("sp_brace_finally", UO_sp_brace_finally, AT_IARF, "Add or remove space between '}' and 'finally' if on the same line");
   unc_add_option("sp_try_brace", UO_sp_try_brace, AT_IARF, "Add or remove space between 'try' and '{' if on the same line");
   unc_add_option("sp_getset_brace", UO_sp_getset_brace, AT_IARF, "Add or remove space between get/set and '{' if on the same line");

   unc_begin_group(UG_indent, "Indenting");
   unc_add_option("indent_columns", UO_indent_columns, AT_NUM, "The number of columns to indent per level.\nUsually 2, 3, 4, or 8.");
   unc_add_option("indent_with_tabs", UO_indent_with_tabs, AT_NUM, "How to use tabs when indenting code\n0=spaces only\n1=indent with tabs, align with spaces\n2=indent and align with tabs", "", 0, 2);
   unc_add_option("indent_align_string", UO_indent_align_string, AT_BOOL, "Whether to indent strings broken by '\\' so that they line up");
   unc_add_option("indent_xml_string", UO_indent_xml_string, AT_NUM, "The number of spaces to indent multi-line XML strings.\nRequires indent_align_string=True");
   unc_add_option("indent_brace", UO_indent_brace, AT_NUM, "Spaces to indent '{' from level");
   unc_add_option("indent_braces", UO_indent_braces, AT_BOOL, "Whether braces are indented to the body level");
   unc_add_option("indent_braces_no_func", UO_indent_braces_no_func, AT_BOOL, "Disabled indenting function braces if indent_braces is true");
   unc_add_option("indent_brace_parent", UO_indent_brace_parent, AT_BOOL, "Indent based on the size of the brace parent, ie 'if' => 3 spaces, 'for' => 4 spaces, etc.");
   unc_add_option("indent_namespace", UO_indent_namespace, AT_BOOL, "Whether the 'namespace' body is indented");
   unc_add_option("indent_class", UO_indent_class, AT_BOOL, "Whether the 'class' body is indented");
   unc_add_option("indent_class_colon", UO_indent_class_colon, AT_BOOL, "Whether to indent the stuff after a leading class colon");
   unc_add_option("indent_func_call_param", UO_indent_func_call_param, AT_BOOL, "Whether to indent continued function call parameters one indent level (true) or aligns instead of indent (false)");
   unc_add_option("indent_member", UO_indent_member, AT_NUM, "The number of spaces to indent a continued '->' or '.'\nUsually set to 0, 1, or indent_columns.");
   unc_add_option("indent_sing_line_comments", UO_indent_sing_line_comments, AT_NUM, "Spaces to indent single line ('//') comments on lines before code");
   unc_add_option("indent_switch_case", UO_indent_switch_case, AT_NUM, "Spaces to indent 'case' from 'switch'\nUsually 0 or indent_columns.");
   unc_add_option("indent_case_brace", UO_indent_case_brace, AT_NUM, "Spaces to indent '{' from 'case'.\nBy default, the brace will appear under the 'c' in case.\nUsually set to 0 or indent_columns.");
   unc_add_option("indent_col1_comment", UO_indent_col1_comment, AT_BOOL, "Whether to indent comments found in first column");
   unc_add_option("indent_label", UO_indent_label, AT_NUM, "How to indent goto labels\n >0 : absolute column where 1 is the leftmost column\n <=0 : subtract from brace indent", "", -16, 16);
   unc_add_option("indent_access_spec", UO_indent_access_spec, AT_NUM, "Same as indent_label, but for access specifiers that are followed by a colon", "", -16, 16);
   unc_add_option("indent_paren_nl", UO_indent_paren_nl, AT_BOOL, "If an open paren is followed by a newline, indent the next line so that it lines up after the open paren (not recommended)");
   unc_add_option("indent_square_nl", UO_indent_square_nl, AT_BOOL, "If an open square is followed by a newline, indent the next line so that it lines up after the open square (not recommended)");

   unc_begin_group(UG_newline, "Newline adding and removing options");
   unc_add_option("code_width", UO_code_width, AT_NUM, "Try to limit code width to N number of columns", "", 16, 256);
   unc_add_option("nl_collapse_empty_body", UO_nl_collapse_empty_body, AT_BOOL, "Whether to collapse empty blocks between '{' and '}'");
   unc_add_option("nl_assign_leave_one_liners", UO_nl_assign_leave_one_liners, AT_BOOL, "Don't touch one-line braced assignments - 'foo_t f = { 1, 2 };'");
   unc_add_option("nl_class_leave_one_liners", UO_nl_class_leave_one_liners, AT_BOOL, "Don't touch one-line function bodies inside a class xx { } body");
   unc_add_option("nl_start_of_file", UO_nl_start_of_file, AT_IARF, "Add or remove newlines at the start of the file");
   unc_add_option("nl_start_of_file_min", UO_nl_start_of_file_min, AT_NUM, "The number of newlines at the start of the file (only used if nl_start_of_file is 'add' or 'force'");
   unc_add_option("nl_end_of_file", UO_nl_end_of_file, AT_IARF, "Add or remove newline at the end of the file");
   unc_add_option("nl_end_of_file_min", UO_nl_end_of_file_min, AT_NUM, "The number of newlines at the end of the file (only used if nl_end_of_file is 'add' or 'force')");
   unc_add_option("nl_assign_brace", UO_nl_assign_brace, AT_IARF, "Add or remove newline between '=' and '{'");
   unc_add_option("nl_func_var_def_blk", UO_nl_func_var_def_blk, AT_NUM, "The number of newlines after a block of variable definitions");
   unc_add_option("nl_fcall_brace", UO_nl_fcall_brace, AT_IARF, "Add or remove newline between function call and '('");
   unc_add_option("nl_enum_brace", UO_nl_enum_brace, AT_IARF, "Add or remove newline between 'enum' and '{'");
   unc_add_option("nl_struct_brace", UO_nl_struct_brace, AT_IARF, "Add or remove newline between 'struct and '{'");
   unc_add_option("nl_union_brace", UO_nl_union_brace, AT_IARF, "Add or remove newline between 'union' and '{'");
   unc_add_option("nl_if_brace", UO_nl_if_brace, AT_IARF, "Add or remove newline between 'if' and '{'");
   unc_add_option("nl_brace_else", UO_nl_brace_else, AT_IARF, "Add or remove newline between '}' and 'else'");
   unc_add_option("nl_elseif_brace", UO_nl_elseif_brace, AT_IARF, "Add or remove newline between 'else if' and '{'\nIf set to ignore, nl_if_brace is used instead");
   unc_add_option("nl_else_brace", UO_nl_else_brace, AT_IARF, "Add or remove newline between 'else' and '{'");
   unc_add_option("nl_brace_finally", UO_nl_brace_finally, AT_IARF, "Add or remove newline between '}' and 'finally'");
   unc_add_option("nl_finally_brace", UO_nl_finally_brace, AT_IARF, "Add or remove newline between 'finally' and '{'");
   unc_add_option("nl_try_brace", UO_nl_try_brace, AT_IARF, "Add or remove newline between 'try' and '{'");
   unc_add_option("nl_getset_brace", UO_nl_getset_brace, AT_IARF, "Add or remove newline between get/set and '{'");
   unc_add_option("nl_for_brace", UO_nl_for_brace, AT_IARF, "Add or remove newline between 'for' and '{'");
   unc_add_option("nl_catch_brace", UO_nl_catch_brace, AT_IARF, "Add or remove newline between 'catch' and '{'");
   unc_add_option("nl_brace_catch", UO_nl_brace_catch, AT_IARF, "Add or remove newline between '}' and 'catch'");
   unc_add_option("nl_while_brace", UO_nl_while_brace, AT_IARF, "Add or remove newline between 'while' and '{'");
   unc_add_option("nl_do_brace", UO_nl_do_brace, AT_IARF, "Add or remove newline between 'do' and '{'");
   unc_add_option("nl_brace_while", UO_nl_brace_while, AT_IARF, "Add or remove newline between '}' and 'while' of 'do' statement");
   unc_add_option("nl_switch_brace", UO_nl_switch_brace, AT_IARF, "Add or remove newline between 'switch' and '{'");
   unc_add_option("nl_before_case", UO_nl_before_case, AT_BOOL, "Whether to put a newline before 'case' statement");
   unc_add_option("nl_after_case", UO_nl_after_case, AT_BOOL, "Whether to put a newline after 'case' statement");
   unc_add_option("nl_namespace_brace", UO_nl_namespace_brace, AT_IARF, "Newline between namespace and {");
   unc_add_option("nl_template_class", UO_nl_template_class, AT_IARF, "Add or remove newline between 'template<>' and 'class'");
   unc_add_option("nl_class_brace", UO_nl_class_brace, AT_IARF, "Add or remove newline between 'class' and '{'");
   unc_add_option("nl_class_init_args", UO_nl_class_init_args, AT_IARF, "Add or remove newline after each ',' in the constructor member initialization");
   unc_add_option("nl_func_type_name", UO_nl_func_type_name, AT_IARF, "Add or remove newline between return type and function name in definition");
   unc_add_option("nl_func_decl_start", UO_nl_func_decl_start, AT_IARF, "Add or remove newline after '(' in a function declaration");
   unc_add_option("nl_func_decl_args", UO_nl_func_decl_args, AT_IARF, "Add or remove newline after each ',' in a function declaration");
   unc_add_option("nl_func_decl_end", UO_nl_func_decl_end, AT_IARF, "Add or remove newline before the ')' in a function declaration");
   unc_add_option("nl_fdef_brace", UO_nl_fdef_brace, AT_IARF, "Add or remove newline between function signature and '{'");
   unc_add_option("nl_after_return", UO_nl_after_return, AT_BOOL, "Whether to put a newline after 'return' statement");
   unc_add_option("nl_after_semicolon", UO_nl_after_semicolon, AT_BOOL, "Whether to put a newline after semicolons, except in 'for' statements");
   unc_add_option("nl_after_brace_open", UO_nl_after_brace_open, AT_BOOL, "Whether to put a newline after brace open");
   unc_add_option("nl_define_macro", UO_nl_define_macro, AT_BOOL, "Whether to alter newlines in '#define' macros");
   unc_add_option("nl_squeeze_ifdef", UO_nl_squeeze_ifdef, AT_BOOL, "Whether to not put blanks after '#ifxx', '#elxx', or before '#endif'");
   unc_add_option("nl_before_if", UO_nl_before_if, AT_IARF, "Add or remove newline before 'if'");
   unc_add_option("nl_after_if", UO_nl_after_if, AT_IARF, "Add or remove newline after 'if'");
   unc_add_option("nl_before_for", UO_nl_before_for, AT_IARF, "Add or remove newline before 'for'");
   unc_add_option("nl_after_for", UO_nl_after_for, AT_IARF, "Add or remove newline after 'for'");
   unc_add_option("nl_before_while", UO_nl_before_while, AT_IARF, "Add or remove newline before 'while'");
   unc_add_option("nl_after_while", UO_nl_after_while, AT_IARF, "Add or remove newline after 'while'");
   unc_add_option("nl_before_switch", UO_nl_before_switch, AT_IARF, "Add or remove newline before 'switch'");
   unc_add_option("nl_after_switch", UO_nl_after_switch, AT_IARF, "Add or remove newline after 'switch'");
   unc_add_option("nl_before_do", UO_nl_before_do, AT_IARF, "Add or remove newline before 'do'");
   unc_add_option("nl_after_do", UO_nl_after_do, AT_IARF, "Add or remove newline after 'do'");
   unc_add_option("nl_ds_struct_enum_cmt", UO_nl_ds_struct_enum_cmt, AT_BOOL, "Whether to double-space commented-entries in struct/enum");

   unc_begin_group(UG_blankline, "Blank line options", "Note that it takes 2 newlines to get a blank line");
   unc_add_option("nl_max", UO_nl_max, AT_NUM, "The maximum consecutive newlines");
   unc_add_option("nl_after_func_proto", UO_nl_after_func_proto, AT_NUM, "The number of newlines after a function prototype, if followed by another function prototype");
   unc_add_option("nl_after_func_proto_group", UO_nl_after_func_proto_group, AT_NUM, "The number of newlines after a function prototype, if not followed by another function prototype");
   unc_add_option("nl_after_func_body", UO_nl_after_func_body, AT_NUM, "The number of newlines after '}' of a multi-line function body");
   unc_add_option("nl_after_func_body_one_liner", UO_nl_after_func_body_one_liner, AT_NUM, "The number of newlines after '}' of a single line function body");
   unc_add_option("nl_before_block_comment", UO_nl_before_block_comment, AT_NUM, "The minimum number of newlines before a multi-line comment.\nDoesn't apply if after a brace open or another multi-line comment.");
   unc_add_option("nl_before_c_comment", UO_nl_before_c_comment, AT_NUM, "The minimum number of newlines before a single-line C comment.\nDoesn't apply if after a brace open or other single-line C comments.");
   unc_add_option("nl_before_cpp_comment", UO_nl_before_cpp_comment, AT_NUM, "The minimum number of newlines before a CPP comment.\nDoesn't apply if after a brace open or other CPP comments.");
   unc_add_option("eat_blanks_after_open_brace", UO_eat_blanks_after_open_brace, AT_BOOL, "Whether to remove blank lines after '{'");
   unc_add_option("eat_blanks_before_close_brace", UO_eat_blanks_before_close_brace, AT_BOOL, "Whether to remove blank lines before '}'");

   unc_begin_group(UG_position, "Positioning options");
   unc_add_option("pos_bool", UO_pos_bool, AT_POS, "The position of boolean operators in wrapped expressions");
   unc_add_option("pos_class_colon", UO_pos_class_colon, AT_POS, "The position of colons between constructor and member initialization");

   unc_add_option("ls_for_split_full", UO_ls_for_split_full, AT_BOOL, "Whether to fully split long 'for' statements at semi-colons");
   unc_add_option("ls_func_split_full", UO_ls_func_split_full, AT_BOOL, "Whether to fully split long function protos/calls at commas");

   unc_begin_group(UG_align, "Code alignment (not left column spaces/tabs)");
   unc_add_option("align_keep_tabs", UO_align_keep_tabs, AT_BOOL, "Whether to keep non-indenting tabs");
   unc_add_option("align_with_tabs", UO_align_with_tabs, AT_BOOL, "Whether to use tabs for alinging");
   unc_add_option("align_on_tabstop", UO_align_on_tabstop, AT_BOOL, "Whether to bump out to the next tab when aligning");
   unc_add_option("align_number_left", UO_align_number_left, AT_BOOL, "Whether to left-align numbers");
   unc_add_option("align_func_params", UO_align_func_params, AT_BOOL, "Align variable definitions in prototypes and functions");
   unc_add_option("align_var_def_span", UO_align_var_def_span, AT_NUM, "The span for aligning variable definitions (0=don't align)", "", 0, 5000);
   unc_add_option("align_var_def_star_style", UO_align_var_def_star_style, AT_NUM, "How to align the star in variable definitions.\n 0=Part of the type\n 1=Part of the variable\n 2=Dangling", "", 0, 2);
   unc_add_option("align_var_def_thresh", UO_align_var_def_thresh, AT_NUM, "The threshold for aligning variable definitions (0=no limit)", "", 0, 5000);
   unc_add_option("align_var_def_colon", UO_align_var_def_colon, AT_BOOL, "Whether to align the colon in struct bit fields");
   unc_add_option("align_var_def_inline", UO_align_var_def_inline, AT_BOOL, "Whether to align inline struct/enum/union variable definitions");
   unc_add_option("align_assign_span", UO_align_assign_span, AT_NUM, "The span for aligning on '=' in assignments (0=don't align)", "", 0, 5000);
   unc_add_option("align_assign_thresh", UO_align_assign_thresh, AT_NUM, "The threshold for aligning on '=' in assignments (0=no limit)", "", 0, 5000);
   unc_add_option("align_enum_equ_span", UO_align_enum_equ_span, AT_NUM, "The span for aligning on '=' in enums (0=don't align)", "", 0, 5000);
   unc_add_option("align_enum_equ_thresh", UO_align_enum_equ_thresh, AT_NUM, "The threshold for aligning on '=' in enums (0=no limit)", "", 0, 5000);
   unc_add_option("align_var_struct_span", UO_align_var_struct_span, AT_NUM, "The span for aligning struct/union (0=don't align)", "", 0, 5000);
   unc_add_option("align_struct_init_span", UO_align_struct_init_span, AT_NUM, "The span for aligning struct initializer values (0=don't align)", "", 0, 5000);
   unc_add_option("align_typedef_gap", UO_align_typedef_gap, AT_NUM, "The minimum space between the type and the synonym of a typedef");
   unc_add_option("align_typedef_span", UO_align_typedef_span, AT_NUM, "The span for aligning single-line typedefs (0=don't align)");
   unc_add_option("align_typedef_star_style", UO_align_typedef_star_style, AT_NUM, "Controls the positioning of the '*' in typedefs. Just try it.\n0: Align on typdef type, ignore '*'\n1: The '*' is part of type name: typedef int  *pint;\n2: The '*' is part of the type, but dangling: typedef int *pint;", "", 0, 2);
   unc_add_option("align_right_cmt_span", UO_align_right_cmt_span, AT_NUM, "The span for aligning comments that end lines (0=don't align)", "", 0, 5000);
   unc_add_option("align_func_proto_span", UO_align_func_proto_span, AT_NUM, "The span for aligning function prototypes (0=don't align)", "", 0, 5000);
   unc_add_option("align_nl_cont", UO_align_nl_cont, AT_BOOL, "Whether to align macros wrapped with a backslash and a newline");
   unc_add_option("align_pp_define_gap", UO_align_pp_define_gap, AT_NUM, "The minimum space between label and value of a preprocessor define");
   unc_add_option("align_pp_define_span", UO_align_pp_define_span, AT_NUM, "The span for aligning on '#define' bodies (0=don't align)", "", 0, 5000);

   unc_begin_group(UG_comment, "Comment modifications");
   unc_add_option("cmt_cpp_group", UO_cmt_cpp_group, AT_BOOL, "Whether to group cpp-comments that look like they are in a block");
   unc_add_option("cmt_cpp_nl_start", UO_cmt_cpp_nl_start, AT_BOOL, "Whether to put an empty '/*' on the first line of the combined cpp-comment");
   unc_add_option("cmt_cpp_nl_end", UO_cmt_cpp_nl_end, AT_BOOL, "Whether to put a newline before the closing '*/' of the combined cpp-comment");
   unc_add_option("cmt_cpp_to_c", UO_cmt_cpp_to_c, AT_BOOL, "Whether to change cpp-comments into c-comments");
   unc_add_option("cmt_star_cont", UO_cmt_star_cont, AT_BOOL, "Whether to put a star on subsequent comment lines");
   unc_add_option("cmt_sp_before_star_cont", UO_cmt_sp_before_star_cont, AT_NUM, "The number of spaces to insert at the start of subsequent comment lines");
   unc_add_option("cmt_sp_after_star_cont", UO_cmt_sp_after_star_cont, AT_NUM, "The number of spaces to insert after the star on subsequent comment lines");

   unc_begin_group(UG_codemodify, "Code modifying options (non-whitespace)");
   unc_add_option("mod_full_brace_do", UO_mod_full_brace_do, AT_IARF, "Add or remove braces on single-line 'do' statement");
   unc_add_option("mod_full_brace_for", UO_mod_full_brace_for, AT_IARF, "Add or remove braces on single-line 'for' statement");
   unc_add_option("mod_full_brace_function", UO_mod_full_brace_function, AT_IARF, "Add or remove braces on single-line function defintions. (Pawn)");
   unc_add_option("mod_full_brace_if", UO_mod_full_brace_if, AT_IARF, "Add or remove braces on single-line 'if' statement");
   unc_add_option("mod_full_brace_nl", UO_mod_full_brace_nl, AT_NUM, "Don't remove braces around statements that span N newlines", "", 0, 5000);
   unc_add_option("mod_full_brace_while", UO_mod_full_brace_while, AT_IARF, "Add or remove braces on single-line 'while' statement");
   unc_add_option("mod_paren_on_return", UO_mod_paren_on_return, AT_IARF, "Add or remove unnecessary paren on 'return' statement");
   unc_add_option("mod_pawn_semicolon", UO_mod_pawn_semicolon, AT_BOOL, "Whether to change optional semicolons to real semicolons");
   unc_add_option("mod_full_paren_if_bool", UO_mod_full_paren_if_bool, AT_BOOL, "Add parens on 'while' and 'if' statement around bools");

   unc_begin_group(UG_preprocessor, "Preprocessor options");
   unc_add_option("pp_indent", UO_pp_indent, AT_IARF, "Add or remove indent of preprocessor directives");
   unc_add_option("pp_space", UO_pp_space, AT_IARF, "Add or remove space between # and, say, define");
}

const group_map_value *get_group_name(int ug)
{
   for (group_map_it it = group_map.begin();
        it != group_map.end();
        it++)
   {
      if (it->second.id == ug)
      {
         return(&it->second);
      }
   }
   return(NULL);
}

const option_map_value *get_option_name(int uo)
{
   for (option_name_map_it it = option_name_map.begin();
        it != option_name_map.end();
        it++)
   {
      if (it->second.id == uo)
      {
         return(&it->second);
      }
   }
   return(NULL);
}


/**
 * Convert the value string to a number.
 */
static int convert_value(const option_map_value *entry, const char *val)
{
   const option_map_value *tmp;
   bool btrue;
   int  mult;

   if (entry->type == AT_LINE)
   {
      if (strcasecmp(val, "CRLF") == 0)
      {
         return(LE_CRLF);
      }
      if (strcasecmp(val, "LF") == 0)
      {
         return(LE_LF);
      }
      if (strcasecmp(val, "CR") == 0)
      {
         return(LE_CR);
      }
      if (strcasecmp(val, "AUTO") != 0)
      {
         LOG_FMT(LWARN, "%s:%d Expected AUTO, LF, CRLF, or CR for %s, got %s\n",
                 cpd.filename, cpd.line_number, entry->name, val);
         cpd.error_count++;
      }
      return(LE_AUTO);
   }

   if (entry->type == AT_POS)
   {
      if ((strcasecmp(val, "LEAD") == 0) ||
          (strcasecmp(val, "START") == 0))
      {
         return(TP_LEAD);
      }
      if ((strcasecmp(val, "TRAIL") == 0) ||
          (strcasecmp(val, "END") == 0))
      {
         return(TP_TRAIL);
      }
      if (strcasecmp(val, "IGNORE") != 0)
      {
         LOG_FMT(LWARN, "%s:%d Expected IGNORE, LEAD/START, or TRAIL/END for %s, got %s\n",
                 cpd.filename, cpd.line_number, entry->name, val);
         cpd.error_count++;
      }
      return(TP_IGNORE);
   }

   if (entry->type == AT_NUM)
   {
      if (isdigit(*val) ||
          (isdigit(val[1]) && ((*val == '-') || (*val == '+'))))
      {
         return(strtol(val, NULL, 0));
      }
      else
      {
         /* Try to see if it is a variable */
         mult = 1;
         if (*val == '-')
         {
            mult = -1;
            val++;
         }

         if (((tmp = unc_find_option(val)) != NULL) && (tmp->type == entry->type))
         {
            return(cpd.settings[tmp->id].n * mult);
         }
      }
      LOG_FMT(LWARN, "%s:%d Expected a number for %s, got %s\n",
              cpd.filename, cpd.line_number, entry->name, val);
      cpd.error_count++;
      return(0);
   }

   if (entry->type == AT_BOOL)
   {
      if ((strcasecmp(val, "true") == 0) ||
          (strcasecmp(val, "t") == 0) ||
          (strcmp(val, "1") == 0))
      {
         return(1);
      }

      if ((strcasecmp(val, "false") == 0) ||
          (strcasecmp(val, "f") == 0) ||
          (strcmp(val, "0") == 0))
      {
         return(0);
      }

      btrue = true;
      if ((*val == '-') || (*val == '~'))
      {
         btrue = false;
         val++;
      }

      if (((tmp = unc_find_option(val)) != NULL) && (tmp->type == entry->type))
      {
         return(cpd.settings[tmp->id].b ? btrue : !btrue);
      }
      LOG_FMT(LWARN, "%s:%d Expected 'True' or 'False' for %s, got %s\n",
              cpd.filename, cpd.line_number, entry->name, val);
      cpd.error_count++;
      return(0);
   }

   /* Must be AT_IARF */

   if ((strcasecmp(val, "add") == 0) || (strcasecmp(val, "a") == 0))
   {
      return(AV_ADD);
   }
   if ((strcasecmp(val, "remove") == 0) || (strcasecmp(val, "r") == 0))
   {
      return(AV_REMOVE);
   }
   if ((strcasecmp(val, "force") == 0) || (strcasecmp(val, "f") == 0))
   {
      return(AV_FORCE);
   }
   if ((strcasecmp(val, "ignore") == 0) || (strcasecmp(val, "i") == 0))
   {
      return(AV_IGNORE);
   }
   if (((tmp = unc_find_option(val)) != NULL) && (tmp->type == entry->type))
   {
      return(cpd.settings[tmp->id].a);
   }
   LOG_FMT(LWARN, "%s:%d Expected 'Add', 'Remove', 'Force', or 'Ignore' for %s, got %s\n",
           cpd.filename, cpd.line_number, entry->name, val);
   cpd.error_count++;
   return(0);
}

int set_option_value(const char *name, const char *value)
{
   const option_map_value *entry;

   if ((entry = unc_find_option(name)) != NULL)
   {
      cpd.settings[entry->id].n = convert_value(entry, value);
      return(entry->id);
   }
   return(-1);
}

int load_option_file(const char *filename)
{
   FILE *pfile;
   char buffer[256];
   char *ptr;
   int  id;
   char *args[32];
   int  argc;
   int  idx;

   cpd.line_number = 0;

   pfile = fopen(filename, "r");
   if (pfile == NULL)
   {
      LOG_FMT(LERR, "%s: fopen(%s) failed: %s (%d)\n",
              __func__, filename, strerror(errno), errno);
      cpd.error_count++;
      return(-1);
   }

   /* Read in the file line by line */
   while (fgets(buffer, sizeof(buffer), pfile) != NULL)
   {
      cpd.line_number++;

      /* Chop off trailing comments */
      if ((ptr = strchr(buffer, '#')) != NULL)
      {
         *ptr = 0;
      }

      /* Blow away the '=' to make things simple */
      if ((ptr = strchr(buffer, '=')) != NULL)
      {
         *ptr = ' ';
      }

      /* Blow away all commas */
      ptr = buffer;
      while ((ptr = strchr(ptr, ',')) != NULL)
      {
         *ptr = ' ';
      }

      /* Split the line */
      argc = Args::SplitLine(buffer, args, ARRAY_SIZE(args) - 1);
      if (argc < 2)
      {
         if (argc > 0)
         {
            LOG_FMT(LWARN, "%s:%d Wrong number of arguments: %s...\n",
                    filename, cpd.line_number, buffer);
            cpd.error_count++;
         }
         continue;
      }
      args[argc] = NULL;

      if (strcasecmp(args[0], "type") == 0)
      {
         for (idx = 1; idx < argc; idx++)
         {
            add_keyword(args[idx], CT_TYPE, LANG_ALL);
         }
      }
      else if (strcasecmp(args[0], "define") == 0)
      {
         add_define(args[1], args[2]);
      }
      else if (strcasecmp(args[0], "macro-open") == 0)
      {
         add_keyword(args[1], CT_MACRO_OPEN, LANG_ALL);
      }
      else if (strcasecmp(args[0], "macro-close") == 0)
      {
         add_keyword(args[1], CT_MACRO_CLOSE, LANG_ALL);
      }
      else if (strcasecmp(args[0], "macro-else") == 0)
      {
         add_keyword(args[1], CT_MACRO_ELSE, LANG_ALL);
      }
      else
      {
         /* must be a regular option = value */
         if ((id = set_option_value(args[0], args[1])) < 0)
         {
            LOG_FMT(LWARN, "%s:%d Unknown symbol '%s'\n",
                    filename, cpd.line_number, args[0]);
            cpd.error_count++;
         }
      }
   }

   fclose(pfile);
   return(0);
}


int save_option_file(FILE *pfile, bool withDoc)
{
   std::string val_string;
   const char  *val_str;
   int         val_len;
   int         name_len;
   int         idx;

   /* Print the options by group */
   for (group_map_it jt = group_map.begin(); jt != group_map.end(); jt++)
   {
      if (withDoc)
      {
         fputs("\n#\n", pfile);
         fprintf(pfile, "# %s\n", jt->second.short_desc);
         fputs("#\n\n", pfile);
      }

      bool first = true;

      for (option_list_it it = jt->second.options.begin(); it != jt->second.options.end(); it++)
      {
         const option_map_value *option = get_option_name(*it);

         if (withDoc && (option->short_desc != NULL) && (*option->short_desc != 0))
         {
            fprintf(pfile, "%s# ", first ? "" : "\n");
            for (idx = 0; option->short_desc[idx] != 0; idx++)
            {
               fputc(option->short_desc[idx], pfile);
               if ((option->short_desc[idx] == '\n') &&
                   (option->short_desc[idx + 1] != 0))
               {
                  fputs("# ", pfile);
               }
            }
            if (option->short_desc[idx - 1] != '\n')
            {
               fputc('\n', pfile);
            }
         }
         first      = false;
         val_string = op_val_to_string(option->type, cpd.settings[option->id]);
         val_str    = val_string.c_str();
         val_len    = strlen(val_str);
         name_len   = strlen(option->name);

         fprintf(pfile, "%s %*.s= %s",
                 option->name, cpd.max_option_name_len - name_len, " ",
                 val_str);
         if (withDoc)
         {
            fprintf(pfile, "%*.s # %s",
                    8 - val_len, " ",
                    argtype_to_string(option->type).c_str());
         }
         fputs("\n", pfile);
      }
   }

   if (withDoc)
   {
      fprintf(pfile,
              "\n"
              "# You can force a token to be a type with the 'type' option.\n"
              "# Example:\n"
              "# type myfoo1 myfoo2\n"
              "#\n"
              "# You can create custom macro-based indentation using macro-open, \n"
              "# macro-else and macro-close.\n"
              "# Example:\n"
              "# macro-open  BEGIN_TEMPLATE_MESSAGE_MAP\n"
              "# macro-open  BEGIN_MESSAGE_MAP\n"
              "# macro-close END_MESSAGE_MAP\n"
              );
   }

   /* Print custom keywords */
   const chunk_tag_t *ct;
   idx = 0;
   while ((ct = get_custom_keyword_idx(idx)) != NULL)
   {
      if (ct->type == CT_TYPE)
      {
         fprintf(pfile, "type %*.s%s\n",
                 cpd.max_option_name_len - 4, " ", ct->tag);
      }
      else if (ct->type == CT_MACRO_OPEN)
      {
         fprintf(pfile, "macro-open %*.s%s\n",
                 cpd.max_option_name_len - 11, " ", ct->tag);
      }
      else if (ct->type == CT_MACRO_CLOSE)
      {
         fprintf(pfile, "macro-close %*.s%s\n",
                 cpd.max_option_name_len - 12, " ", ct->tag);
      }
      else if (ct->type == CT_MACRO_ELSE)
      {
         fprintf(pfile, "macro-else %*.s%s\n",
                 cpd.max_option_name_len - 11, " ", ct->tag);
      }
      else
      {
         LOG_FMT(LWARN, "%s: unable to save '%s'\n", __func__, ct->tag);
      }
   }

   /* Print custom defines */
   const define_tag_t *dt;
   idx = 0;
   while ((dt = get_define_idx(idx)) != NULL)
   {
      fprintf(pfile, "define %*.s%s \"%s\"\n",
              cpd.max_option_name_len - 6, " ", dt->tag, dt->value);
   }

   fclose(pfile);
   return(0);
}

void print_options(FILE *pfile, bool verbose)
{
   int        max_width = 0;
   int        cur_width;
   const char *text;

   const char *names[] =
   {
      "{ False, True }",
      "{ Ignore, Add, Remove, Force }",
      "Number",
      "{ Auto, LF, CR, CRLF }",
      "{ Ignore, Lead, Trail }",
   };

   option_name_map_it it;

   /* Find the max width of the names */
   for (it = option_name_map.begin(); it != option_name_map.end(); it++)
   {
      cur_width = strlen(it->second.name);
      if (cur_width > max_width)
      {
         max_width = cur_width;
      }
   }
   max_width++;

   /* Print the all out */
   for (group_map_it jt = group_map.begin(); jt != group_map.end(); jt++)
   {
      fprintf(pfile, "#\n# %s\n#\n\n", jt->second.short_desc);

      for (option_list_it it = jt->second.options.begin(); it != jt->second.options.end(); it++)
      {
         const option_map_value *option = get_option_name(*it);
         cur_width = strlen(option->name);
         fprintf(pfile, "%s%*c%s\n",
                 option->name,
                 max_width - cur_width, ' ',
                 names[option->type]);

         text = option->short_desc;

         if (text != NULL)
         {
            fputs("  ", pfile);
            while (*text != 0)
            {
               fputc(*text, pfile);
               if (*text == '\n')
               {
                  fputs("  ", pfile);
               }
               text++;
            }
         }
         fputs("\n\n", pfile);
      }
   }
}

/**
 * Sets non-zero settings defaults
 *
 * TODO: select from various sets? - ie, K&R, GNU, Linux, Ben
 */
void set_option_defaults(void)
{
   cpd.settings[UO_newlines].le          = LE_AUTO;
   cpd.settings[UO_input_tab_size].n     = 8;
   cpd.settings[UO_output_tab_size].n    = 8;
   cpd.settings[UO_indent_columns].n     = 8;
   cpd.settings[UO_indent_with_tabs].n   = 1;
   cpd.settings[UO_indent_label].n       = 1;
   cpd.settings[UO_indent_access_spec].n = 1;
   cpd.settings[UO_sp_before_comma].a    = AV_REMOVE;
   cpd.settings[UO_string_escape_char].n = '\\';
}

std::string argtype_to_string(argtype_e argtype)
{
   switch (argtype)
   {
   case AT_BOOL:
      return("false/true");

   case AT_IARF:
      return("ignore/add/remove/force");

   case AT_NUM:
      return("number");

   case AT_LINE:
      return("auto/lf/crlf/cr");

   case AT_POS:
      return("ignore/lead/trail");

   default:
      LOG_FMT(LWARN, "Unknown argtype '%d'\n", argtype);
      return("");
   }
}

std::string bool_to_string(bool val)
{
   if (val)
   {
      return("true");
   }
   else
   {
      return("false");
   }
}

std::string argval_to_string(argval_t argval)
{
   switch (argval)
   {
   case AV_IGNORE:
      return("ignore");

   case AV_ADD:
      return("add");

   case AV_REMOVE:
      return("remove");

   case AV_FORCE:
      return("force");

   default:
      LOG_FMT(LWARN, "Unknown argval '%d'\n", argval);
      return("");
   }
}

std::string number_to_string(int number)
{
   char buffer[12]; // 11 + 1

   sprintf(buffer, "%d", number);
   return(buffer);
}

std::string lineends_to_string(lineends_e linends)
{
   switch (linends)
   {
   case LE_LF:
      return("lf");

   case LE_CRLF:
      return("crlf");

   case LE_CR:
      return("cr");

   case LE_AUTO:
      return("auto");

   default:
      LOG_FMT(LWARN, "Unknown lineends '%d'\n", linends);
      return("");
   }
}

std::string tokenpos_to_string(tokenpos_e tokenpos)
{
   switch (tokenpos)
   {
   case TP_IGNORE:
      return("ignore");

   case TP_LEAD:
      return("lead");

   case TP_TRAIL:
      return("trail");

   default:
      LOG_FMT(LWARN, "Unknown tokenpos '%d'\n", tokenpos);
      return("");
   }
}

std::string op_val_to_string(argtype_e argtype, op_val_t op_val)
{
   switch (argtype)
   {
   case AT_BOOL:
      return(bool_to_string(op_val.b));

   case AT_IARF:
      return(argval_to_string(op_val.a));

   case AT_NUM:
      return(number_to_string(op_val.n));

   case AT_LINE:
      return(lineends_to_string(op_val.le));

   case AT_POS:
      return(tokenpos_to_string(op_val.tp));

   default:
      LOG_FMT(LWARN, "Unknown argtype '%d'\n", argtype);
      return("");
   }
}
