#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#define JD297_CC_PARSE_IMPLEMENTATION
#include "parse.h"
#include "lex.h"
#include "tok.h"
#include "symtbl.h"
#include "ir.h"

int parse_error_count;
ParseTreeNode *parse_result;

static Symtbl *parse_scope_global;
static Symtbl *parse_scope_function;
static Symtbl *parse_scope_current;

extern int yyparse(void)
{
	if ((parse_scope_global = symtbl_create(NULL)) == NULL) {
		return -1;
	}

	/*parse_scope_current = parse_scope_global;
	parse_scope_function = NULL;*/
	parse_error_count = 0;

	parse_result = parse_translation_unit();

	symtbl_free(parse_scope_global);

    if (parse_result == NULL || parse_error_count > 0) {
    	return -1;
    }

    return 0;
}

static ParseTreeNode *parse_translation_unit(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(TRANSLATION_UNIT, NULL);
    ParseTreeNode *external_declaration;

    parse_list_opt(this_node, external_declaration);

    return this_node;
}

static ParseTreeNode *parse_external_declaration(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(EXTERNAL_DECLARATION, NULL);
    ParseTreeNode *function_definition;
    ParseTreeNode *declaration;

	parse_scope_current = parse_scope_global;

    parse_opt(this_node, function_definition, OK);

    parse_opt(this_node, declaration, OK);

    parse_tree_node_destroy(this_node);

    return NULL;

OK:
    return this_node;
}

static ParseTreeNode *parse_function_definition(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(FUNCTION_DEFINITION, NULL);
    ParseTreeNode *declaration_specifier;
    ParseTreeNode *declarator;
    ParseTreeNode *declaration;
    ParseTreeNode *compound_statement;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    parse_list_opt(this_node, declaration_specifier);
    
    parse_required(this_node, declarator, ERROR);

    parse_list_opt(this_node, declaration);

    parse_required(this_node, compound_statement, ERROR);

    return this_node;

ERROR:
	lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_declaration(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(DECLARATION, NULL);
    ParseTreeNode *declaration_specifier;
    ParseTreeNode *init_declarator_list;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    parse_list_required(this_node, declaration_specifier, ERROR);

    parse_opt(this_node, init_declarator_list, NEXT);

NEXT:
	if (yylex() != ';') {
        goto ERROR;
    }

    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_declaration_specifier(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(DECLARATION_SPECIFIER, NULL);
    ParseTreeNode *storage_class_specifier;
    ParseTreeNode *type_specifier;
    ParseTreeNode *type_qualifier;

	/* TODO should go to a OK state no list */
NEXT:
    parse_opt(this_node, storage_class_specifier, NEXT);

    parse_opt(this_node, type_specifier, NEXT);

    parse_opt(this_node, type_qualifier, NEXT);


    if (this_node->num == 0) {
        goto ERROR;
    }

    return this_node;

ERROR:
	parse_tree_node_destroy(this_node);

	return NULL;
}

static ParseTreeNode *parse_declarator(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(DECLARATOR, NULL);
    ParseTreeNode *pointer;
    ParseTreeNode *direct_declarator;

    parse_opt(this_node, pointer, NEXT_DIRECT_DECLARATOR);

NEXT_DIRECT_DECLARATOR:
	parse_required(this_node, direct_declarator, ERROR);

    return this_node;

ERROR:
	parse_tree_node_destroy(this_node);

	return NULL;
}

static ParseTreeNode *parse_compound_statement(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(COMPOUND_STATEMENT, NULL);
    ParseTreeNode *declaration;
    ParseTreeNode *statement;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    if (yylex() != '{') {
        goto ERROR;
    }

    parse_list_opt(this_node, declaration);

    parse_list_opt(this_node, statement);

    if (yylex() != '}') {
        goto ERROR;
    }

    return this_node;

ERROR:
	lex_setpos(lex_pos_saved);

	parse_tree_node_destroy(this_node);

	return NULL;
}

static ParseTreeNode *parse_storage_class_specifier(void)
{
    switch (yylex()) {
        case T_AUTO:
        case T_REGISTER:
        case T_STATIC:
        case T_EXTERN:
        case T_TYPEDEF: {
            return parse_tree_node_create(STORAGE_CLASS_SPECIFIER, &lex_tok);
        }
        default: {
		    lex_setpos(yytext);

		    return NULL;
		}
    }
}

static ParseTreeNode *parse_type_specifier(void)
{
    ParseTreeNode *this_node;
    ParseTreeNode *struct_or_union_specifier;
    ParseTreeNode *enum_specifier;
    ParseTreeNode *typedef_name;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    switch (yylex()) {
        case T_VOID:
        case T_CHAR:
        case T_SHORT:
        case T_INT:
        case T_LONG:
        case T_FLOAT:
        case T_DOUBLE:
        case T_SIGNED:
        case T_UNSIGNED: {
            return parse_tree_node_create(TYPE_SPECIFIER, &lex_tok);
        }
        default: {
        	lex_setpos(yytext);
        } break;
    }

	this_node = parse_tree_node_create(TYPE_SPECIFIER, NULL);

    parse_opt(this_node, struct_or_union_specifier, OK);

    parse_opt(this_node, enum_specifier, OK);

    parse_opt(this_node, typedef_name, OK);

    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;

OK:
	return this_node;
}

static ParseTreeNode *parse_type_qualifier(void)
{
    switch (yylex()) {
        case T_CONST:
        case T_VOLATILE: {
            return parse_tree_node_create(TYPE_QUALIFIER, &lex_tok);
        } break;
        default: {
            lex_setpos(yytext);

            return NULL;
        }       
    }
}

static ParseTreeNode *parse_struct_or_union_specifier(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(STRUCT_OR_UNION_SPECIFIER, NULL);
    ParseTreeNode *struct_or_union;
    ParseTreeNode *identifier;
    ParseTreeNode *struct_declaration;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    parse_required(this_node, struct_or_union, ERROR);

    parse_opt(this_node, identifier, HAS_IDENTIFIER);
    
    if (yylex() != '{') {
        goto ERROR;
    }

    parse_list_required(this_node, struct_declaration, ERROR);
    
    if (yylex() != '}') {
        goto ERROR;
    }

HAS_IDENTIFIER:
	if (yylex() != '{') {
        lex_setpos(yytext);

        goto OK;
    }
    
    parse_list_required(this_node, struct_declaration, ERROR);

    if (yylex() != '}') {
        goto ERROR;
    }

OK:
    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_enum_specifier(void)
{
	ParseTreeNode *this_node = NULL;
    ParseTreeNode *identifier;
    ParseTreeNode *enumerator;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    if (yylex() != T_ENUM) {
        goto ERROR;
    }

    this_node = parse_tree_node_create(ENUM_SPECIFIER, &lex_tok);

    parse_opt(this_node, identifier, HAS_IDENTIFIER);

    if (yylex() != '{') {
        goto ERROR;
    }

    parse_list_required(this_node, enumerator, ERROR);
    
    if (yylex() != '}') {
        goto ERROR;
    }
    
    goto OK;

HAS_IDENTIFIER:
    if (yylex() != '{') {
        lex_setpos(yytext);

        goto OK;
    }
    
    parse_list_required(this_node, enumerator, ERROR);

    if (yylex() != '}') {
        goto ERROR;
    }

OK:
	return this_node;

ERROR:
	lex_setpos(lex_pos_saved);

	parse_tree_node_destroy(this_node);

	return NULL;
}

static ParseTreeNode *parse_typedef_name(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(TYPEDEF_NAME, NULL);
    ParseTreeNode *identifier;

    goto ERROR; /* TODO check if identifier is registered as a typedef use symtbl */

    parse_required(this_node, identifier, ERROR);

    return this_node;

ERROR:
    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_struct_or_union(void)
{
    switch (yylex()) {
        case T_STRUCT:
        case T_UNION: {
            return parse_tree_node_create(STRUCT_OR_UNION, &lex_tok);
        } break;
        default: {
            lex_setpos(yytext);

            return NULL;   
        }
    }
}

static ParseTreeNode *parse_identifier(void)
{
    if (yylex() == T_IDENTIFIER) {
        return parse_tree_node_create(IDENTIFIER, &lex_tok);
    }

    lex_setpos(yytext);

    return NULL;
}

static ParseTreeNode *parse_struct_declaration(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(STRUCT_DECLARATION, NULL);
    ParseTreeNode *specifier_qualifier;
    ParseTreeNode *struct_declarator;
	char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    parse_list_required(this_node, specifier_qualifier, ERROR);

    parse_list_required(this_node, struct_declarator, ERROR);

    if (yylex() == ';') {
        return this_node;
    }

ERROR:
	lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_specifier_qualifier(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(SPECIFIER_QUALIFIER, NULL);
    ParseTreeNode *type_specifier;
    ParseTreeNode *type_qualifier;

    parse_opt(this_node, type_specifier, OK);

    parse_opt(this_node, type_qualifier, OK);

    parse_tree_node_destroy(this_node);

    return NULL;

OK:
    return this_node;
}

static ParseTreeNode *parse_struct_declarator_list(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(STRUCT_DECLARATOR_LIST, NULL);
    ParseTreeNode *struct_declarator;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

NEXT_STRUCT_DECLARATOR:
    parse_required(this_node, struct_declarator, CHECK_ERROR);

    if (yylex() == ',') {
        goto NEXT_STRUCT_DECLARATOR;
    }

    lex_setpos(yytext);
    
CHECK_ERROR:
    if (this_node->num == 0) {
        goto ERROR;
    }

    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_struct_declarator(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(STRUCT_DECLARATOR, NULL);
	ParseTreeNode *declarator;
    ParseTreeNode *constant_expression;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();
    
    parse_opt(this_node, declarator, HAS_DECLARATOR);

    if (yylex() != ':') {
        goto ERROR;
    }
    
    parse_required(this_node, constant_expression, ERROR);
    
    goto OK;

HAS_DECLARATOR:
    if (yylex() != ':') {
        lex_setpos(yytext);

        goto OK;
    }
    
    parse_required(this_node, constant_expression, ERROR);

OK:
    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_constant_expression(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(CONSTANT_EXPRESSION, NULL);
    ParseTreeNode *conditional_expression;

    parse_required(this_node, conditional_expression, ERROR);

    return this_node;

ERROR:
    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_pointer(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(POINTER, NULL);
	ParseTreeNode *type_qualifier;
    ParseTreeNode *pointer;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();
    
    if (yylex() != '*') {
        goto ERROR;
    }
    
    parse_list_opt(this_node, type_qualifier);
    
    parse_opt(this_node, pointer, OK);

OK:
    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_direct_declarator(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(DIRECT_DECLARATOR, NULL);
	ParseTreeNode *identifier;
    ParseTreeNode *declarator;
    ParseTreeNode *constant_expression;
    ParseTreeNode *parameter_type_list;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();
    
    parse_opt(this_node, identifier, AFTER_DIRECT_DECLARATOR);

    if (yylex() != '(') {
        goto ERROR;
    }
    
    parse_required(this_node, declarator, ERROR);
    
    if (yylex() != ')') {
        goto ERROR;
    }

AFTER_DIRECT_DECLARATOR:
    switch(yylex()) {
        case '(': {
        	this_node->tok = lex_tok;

            parse_required(this_node, parameter_type_list, NEXT_AFTER_DIRECT_DECLARATOR_CHECK_IDENTIFIER_LIST);
            
            goto NEXT_AFTER_DIRECT_DECLARATOR_PARENT;
            
NEXT_AFTER_DIRECT_DECLARATOR_CHECK_IDENTIFIER_LIST:
            parse_list_opt(this_node, identifier);

NEXT_AFTER_DIRECT_DECLARATOR_PARENT:
            if (yylex() != T_CLOSING_PARENT) {
                goto ERROR;
            }

            break;
        }
        case '{': {
        	this_node->tok = lex_tok;

            parse_opt(this_node, constant_expression, NEXT_AFTER_DIRECT_DECLARATOR_BRACKET);

NEXT_AFTER_DIRECT_DECLARATOR_BRACKET:

            if (yylex() != '}') {
                goto ERROR;
            }
            
            break;
        }
        default: {
            if (this_node->num == 1) {
                lex_setpos(yytext);

                break;
            }

            goto ERROR;
        }
    }

    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_parameter_type_list(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(PARAMETER_TYPE_LIST, NULL);
    ParseTreeNode *parameter_list;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();
    
    parse_required(this_node, parameter_list, ERROR);
    
    if (yylex() == ',') {
        if (yylex() != T_DOT_DOT_DOT) {
            goto ERROR;
        }
        
        this_node->tok = lex_tok;
    } else {
        lex_setpos(yytext);
    }

    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_conditional_expression(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(CONDITIONAL_EXPRESSION, NULL);
    ParseTreeNode *logical_or_expression;
    ParseTreeNode *expression;
    ParseTreeNode *conditional_expression;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    parse_required(this_node, logical_or_expression, ERROR);

    if (yylex() != '?') {
        lex_setpos(yytext);

        goto OK;
    }

    parse_required(this_node, expression, ERROR);
    
    if (yylex() != ':') {
        goto ERROR;
    }
    
    parse_required(this_node, conditional_expression, ERROR);

OK:
    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_logical_or_expression(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(LOGICAL_OR_EXPRESSION, NULL);
    ParseTreeNode *left_node;
    ParseTreeNode *logical_and_expression;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    parse_required(this_node, logical_and_expression, ERROR);

	switch (yylex()) {
        case T_LOGICAL_OR: {
            parse_required(this_node, logical_and_expression, ERROR);

            this_node->tok = lex_tok;
        } break;
        default: {
            lex_setpos(yytext);

            goto OK;
        }
    }

    while (1) {
       switch (yylex()) {
            case T_LOGICAL_OR: {
                left_node = this_node;

                this_node = parse_tree_node_create(LOGICAL_OR_EXPRESSION, &lex_tok);

                parse_tree_node_add(this_node, left_node);

                parse_required(this_node, logical_and_expression, ERROR);
            } break;
            default: {
                lex_setpos(yytext);

                goto OK;
            }
        }
    }

OK:
    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_expression(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(EXPRESSION, NULL);
    ParseTreeNode *assignment_expression;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();
    
NEXT_ASSIGNMENT_EXPRESSION:
    parse_required(this_node, assignment_expression, ERROR);

    if (yylex() == ',') {
        goto NEXT_ASSIGNMENT_EXPRESSION;
    }

    lex_setpos(yytext);

    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_logical_and_expression(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(LOGICAL_AND_EXPRESSION, NULL);
	ParseTreeNode *left_node;
    ParseTreeNode *inclusive_or_expression;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    parse_required(this_node, inclusive_or_expression, ERROR);

    switch (yylex()) {
        case T_LOGICAL_AND: {
            parse_required(this_node, inclusive_or_expression, ERROR);

            this_node->tok = lex_tok;
        } break;
        default: {
            lex_setpos(yytext);

            goto OK;
        }
    }

    while (1) {
       switch (yylex()) {
            case T_LOGICAL_AND: {
                left_node = this_node;

                this_node = parse_tree_node_create(LOGICAL_AND_EXPRESSION, &lex_tok);

                parse_tree_node_add(this_node, left_node);

                parse_required(this_node, inclusive_or_expression, ERROR);
            } break;
            default: {
                lex_setpos(yytext);

                goto OK;
            }
        }
    }

OK:
    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_inclusive_or_expression(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(INCLUSIVE_OR_EXPRESSION, NULL);
    ParseTreeNode *left_node;
    ParseTreeNode *exclusive_or_expression;
    char *lex_pos_saved;
    
    lex_pos_saved = lex_tell();

    parse_required(this_node, exclusive_or_expression, ERROR);

    switch (yylex()) {
        case '|': {
            parse_required(this_node, exclusive_or_expression, ERROR);

            this_node->tok = lex_tok;
        } break;
        default: {
            lex_setpos(yytext);

            goto OK;
        }
    }

    while (1) {
       switch (yylex()) {
            case '|': {
                left_node = this_node;

                this_node = parse_tree_node_create(INCLUSIVE_OR_EXPRESSION, &lex_tok);

                parse_tree_node_add(this_node, left_node);

                parse_required(this_node, exclusive_or_expression, ERROR);
            } break;
            default: {
                lex_setpos(yytext);

                goto OK;
            }
        }
    }

OK:
    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_exclusive_or_expression(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(EXCLUSIVE_OR_EXPRESSION, NULL);
	ParseTreeNode *left_node;
    ParseTreeNode *and_expression;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    parse_required(this_node, and_expression, ERROR);

    switch (yylex()) {
        case T_BITWISE_XOR: {
            parse_required(this_node, and_expression, ERROR);

            this_node->tok = lex_tok;
        } break;
        default: {
            lex_setpos(yytext);

            goto OK;
        }
    }

    while (1) {
       switch (yylex()) {
            case T_BITWISE_XOR: {
                left_node = this_node;

                this_node = parse_tree_node_create(EXCLUSIVE_OR_EXPRESSION, &lex_tok);

                parse_tree_node_add(this_node, left_node);

                parse_required(this_node, and_expression, ERROR);
            } break;
            default: {
                lex_setpos(yytext);

                goto OK;
            }
        }
    }

OK:
    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_and_expression(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(AND_EXPRESSION, NULL);
	ParseTreeNode *left_node;
    ParseTreeNode *equality_expression;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    parse_required(this_node, equality_expression, ERROR);

	switch (yylex()) {
        case T_BITWISE_AND: {
            parse_required(this_node, equality_expression, ERROR);

            this_node->tok = lex_tok;
        } break;
        default: {
            lex_setpos(yytext);

            goto OK;
        }
    }

    while (1) {
       switch (yylex()) {
            case T_BITWISE_AND: {
                left_node = this_node;

                this_node = parse_tree_node_create(AND_EXPRESSION, &lex_tok);

                parse_tree_node_add(this_node, left_node);

                parse_required(this_node, equality_expression, ERROR);
            } break;
            default: {
                lex_setpos(yytext);

                goto OK;
            }
        }
    }

OK:
    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_equality_expression(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(EQUALITY_EXPRESSION, NULL);
	ParseTreeNode *left_node;
    ParseTreeNode *relational_expression;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();
    
    parse_required(this_node, relational_expression, ERROR);

    switch (yylex()) {
        case T_EQUAL_TO:
        case T_NOT_EQUAL_TO: {
            parse_required(this_node, relational_expression, ERROR);

            this_node->tok = lex_tok;
        } break;
        default: {
            lex_setpos(yytext);

            goto OK;
        }
    }

    while (1) {
        switch (yylex()) {
            case T_EQUAL_TO:
            case T_NOT_EQUAL_TO: {
                left_node = this_node;

                this_node = parse_tree_node_create(EQUALITY_EXPRESSION, &lex_tok);

                parse_tree_node_add(this_node, left_node);

                parse_required(this_node, relational_expression, ERROR);
            } break;
            default: {
                lex_setpos(yytext);

                goto OK;
            }
        }
    }

OK:
    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);
    
    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_relational_expression(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(RELATIONAL_EXPRESSION, NULL);
	ParseTreeNode *left_node;
    ParseTreeNode *shift_expression;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    parse_required(this_node, shift_expression, ERROR);

	switch (yylex()) {
        case T_LESS_THAN:
        case T_GREATER_THAN:
        case T_LESS_THAN_OR_EQUAL_TO:
        case T_GREATER_THAN_OR_EQUAL_TO: {
            parse_required(this_node, shift_expression, ERROR);

            this_node->tok = lex_tok;
        } break;
        default: {
            lex_setpos(yytext);

            goto OK;
        }
    }

    while (1) {
       switch (yylex()) {
            case T_LESS_THAN:
            case T_GREATER_THAN:
            case T_LESS_THAN_OR_EQUAL_TO:
            case T_GREATER_THAN_OR_EQUAL_TO: {
                left_node = this_node;

                this_node = parse_tree_node_create(RELATIONAL_EXPRESSION, &lex_tok);

                parse_tree_node_add(this_node, left_node);

                parse_required(this_node, shift_expression, ERROR);
            } break;
            default: {
                lex_setpos(yytext);

                goto OK;
            }
        }
    }

OK:
    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);
    
    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_shift_expression(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(SHIFT_EXPRESSION, NULL);
	ParseTreeNode *left_node;
    ParseTreeNode *additive_expression;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();
    
    parse_required(this_node, additive_expression, ERROR);

    switch (yylex()) {
        case T_BITWISE_LEFTSHIFT:
        case T_BITWISE_RIGHTSHIFT: {
            parse_required(this_node, additive_expression, ERROR);

            this_node->tok = lex_tok;
        } break;
        default: {
            lex_setpos(yytext);

            goto OK;
        }
    }

    while (1) {
       switch (yylex()) {
            case T_BITWISE_LEFTSHIFT:
            case T_BITWISE_RIGHTSHIFT: {
                left_node = this_node;

                this_node = parse_tree_node_create(SHIFT_EXPRESSION, &lex_tok);

                parse_tree_node_add(this_node, left_node);

                parse_required(this_node, additive_expression, ERROR);
            } break;
            default: {
                lex_setpos(yytext);

                goto OK;
            }
        }
    }

OK:
    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);
    
    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_additive_expression(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(ADDITIVE_EXPRESSION, NULL);
	ParseTreeNode *left_node;
    ParseTreeNode *multiplicative_expression;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();
    
    parse_required(this_node, multiplicative_expression, ERROR);

   	switch (yylex()) {
        case T_PLUS:
        case T_MINUS: {
            parse_required(this_node, multiplicative_expression, ERROR);

            this_node->tok = lex_tok;
        } break;
        default: {
            lex_setpos(yytext);

            goto OK;
        }
    }

    while (1) {
       switch (yylex()) {
            case T_PLUS:
            case T_MINUS: {
                left_node = this_node;

                this_node = parse_tree_node_create(ADDITIVE_EXPRESSION, &lex_tok);

                parse_tree_node_add(this_node, left_node);

                parse_required(this_node, multiplicative_expression, ERROR);
            } break;
            default: {
                lex_setpos(yytext);

                goto OK;
            }
        }
    }

OK:
    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);
    
    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_multiplicative_expression(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(MULTIPLICATIVE_EXPRESSION, NULL);
	ParseTreeNode *left_node;
    ParseTreeNode *cast_expression;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();
    
    parse_required(this_node, cast_expression, ERROR);

   	switch (yylex()) {
        case '*':
        case '/':
        case '%': {
            parse_required(this_node, cast_expression, ERROR);

            this_node->tok = lex_tok;
        } break;
        default: {
            lex_setpos(yytext);

            goto OK;
        }
    }

    while (1) {
       switch (yylex()) {
            case '*':
            case '/':
            case '%': {
                left_node = this_node;

                this_node = parse_tree_node_create(MULTIPLICATIVE_EXPRESSION, &lex_tok);

                parse_tree_node_add(this_node, left_node);

                parse_required(this_node, cast_expression, ERROR);
            } break;
            default: {
                lex_setpos(yytext);

                goto OK;
            }
        }
    }

OK:
    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);
    
    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_cast_expression(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(CAST_EXPRESSION, NULL);
    ParseTreeNode *unary_expression;
    ParseTreeNode *type_name;
    ParseTreeNode *cast_expression;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    parse_opt(this_node, unary_expression, OK);

    if (yylex() != '(') {
        goto ERROR;
    }

    parse_required(this_node, type_name, ERROR);

    if (yylex() != ')') {
        goto ERROR;
    }
    
    parse_required(this_node, cast_expression, ERROR);

OK:
    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_unary_expression(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(UNARY_EXPRESSION, NULL);

    ParseTreeNode *postfix_expression;
    ParseTreeNode *unary_expression;
    ParseTreeNode *unary_operator;
    ParseTreeNode *cast_expression;
    ParseTreeNode *type_name;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    parse_opt(this_node, postfix_expression, OK);

    parse_required(this_node, unary_operator, MORE_TOKENS);
    
    parse_required(this_node, cast_expression, ERROR);
    
    return this_node;

MORE_TOKENS:
    switch(yylex()) {
        case T_INCREMENT:
        case T_DECREMENT: {
            parse_required(this_node, unary_expression, ERROR);
        } break;
        case T_SIZEOF: {
        	this_node->tok = lex_tok;

            parse_opt(this_node, unary_expression, OK);
            
            parse_required(this_node, type_name, ERROR);
        } break;
        default: goto ERROR;
    }

OK:
    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_type_name(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(TYPE_NAME, NULL);
    ParseTreeNode *specifier_qualifier;
    ParseTreeNode *abstract_declarator;

NEXT_SPECIFIER_QUALIFIER:
    parse_opt(this_node, specifier_qualifier, NEXT_SPECIFIER_QUALIFIER);

    if (this_node->num == 0) {
        goto ERROR;
    }

    parse_opt(this_node, abstract_declarator, OK);

OK:
    return this_node;

ERROR:
    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_postfix_expression(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(POSTFIX_EXPRESSION, NULL);
	ParseTreeNode *left_node;
    ParseTreeNode *primary_expression;
    ParseTreeNode *expression;
    ParseTreeNode *argument_expression_list;
    ParseTreeNode *identifier;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    parse_required(this_node, primary_expression, ERROR);

    while (1) {
       switch(yylex()) {
            case '{': {
                left_node = this_node;
                
                this_node = parse_tree_node_create(POSTFIX_EXPRESSION, &lex_tok);

                parse_tree_node_add(this_node, left_node);
                
                parse_required(this_node, expression, ERROR);
                
                if (yylex() != '}') {
                    goto ERROR;
                }
            } break;
            case '(': {
                left_node = this_node;
                
                this_node = parse_tree_node_create(POSTFIX_EXPRESSION, &lex_tok);

                parse_tree_node_add(this_node, left_node);
                
                parse_list_opt(this_node, argument_expression_list);
                
                if (yylex() != ')') {
                    goto ERROR;
                }
            } break;
            case '.':
            case T_ARROW: {
                left_node = this_node;
                
                this_node = parse_tree_node_create(POSTFIX_EXPRESSION, &lex_tok);

                parse_tree_node_add(this_node, left_node);
                
                parse_required(this_node, identifier, ERROR);
            } break;
            case T_INCREMENT:
            case T_DECREMENT: {
                left_node = this_node;
                
                this_node = parse_tree_node_create(POSTFIX_EXPRESSION, &lex_tok);

                parse_tree_node_add(this_node, left_node);
            } break;
            default: {
                lex_setpos(yytext);
                
                goto OK;
            }
        }
    }

OK:
    return this_node;

ERROR:
    lex_setpos(lex_pos_saved); 

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_unary_operator(void)
{
    switch (yylex()) {
        case '&':
        case '*':
        case '+':
        case '-':
        case '~':
        case '!': {
            return parse_tree_node_create(UNARY_OPERATOR, &lex_tok);
        }
        default: goto ERROR;
    }

ERROR:
    lex_setpos(yytext);

	return NULL;
}

static ParseTreeNode *parse_primary_expression(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(PRIMARY_EXPRESSION, NULL);

    ParseTreeNode *identifier;
    ParseTreeNode *constant;
    ParseTreeNode *string;
    ParseTreeNode *expression;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    parse_opt(this_node, identifier, OK);

    parse_opt(this_node, constant, OK);

    parse_opt(this_node, string, OK);

    if (yylex() != '(') {
        goto ERROR;
    }

    parse_required(this_node, expression, ERROR);

    if (yylex() != ')') {
        goto ERROR;
    }

OK:
    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_argument_expression_list(void)
{
	ParseTreeNode *this_node = parse_tree_node_create(ARGUMENT_EXPRESSION_LIST, NULL);
    ParseTreeNode *assignment_expression;
    char *lex_pos_saved;
    char *lex_pos_saved_comma;

NEXT_ASSIGNMENT_EXPRESSION_LIST:
	lex_pos_saved = lex_tell();

    parse_required(this_node, assignment_expression, NEXT_ASSIGNMENT_EXPRESSION_LIST_AFTER);

	lex_pos_saved_comma = lex_tell();

    if (yylex() == ',') {
        goto NEXT_ASSIGNMENT_EXPRESSION_LIST;
    }
    
    lex_pos_saved = lex_pos_saved_comma;
    
NEXT_ASSIGNMENT_EXPRESSION_LIST_AFTER:
    if (this_node->num == 0) {
        lex_setpos(lex_pos_saved);

        goto ERROR;
    }

    lex_setpos(lex_pos_saved_comma);

    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_assignment_expression(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(ASSIGNMENT_EXPRESSION, NULL);
    ParseTreeNode *conditional_expression;
    ParseTreeNode *unary_expression;
    ParseTreeNode *assignment_operator;
    ParseTreeNode *assignment_expression;
    char *lex_pos_saved;

	lex_pos_saved = lex_tell();

    parse_required(this_node, unary_expression, NEXT_CONDITIONAL_EXPRESSION);

    parse_required(this_node, assignment_operator, NEXT_CONDITIONAL_EXPRESSION_REMOVE_PREV);

    parse_required(this_node, assignment_expression, ERROR);

	goto OK;

NEXT_CONDITIONAL_EXPRESSION_REMOVE_PREV:
	parse_tree_node_destroy(this_node);

	this_node = parse_tree_node_create(ASSIGNMENT_EXPRESSION, NULL);

NEXT_CONDITIONAL_EXPRESSION:
	lex_setpos(lex_pos_saved);
	
	parse_required(this_node, conditional_expression, ERROR);

OK:
    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_constant(void)
{
    switch(yylex()) {
    	// TODO: case enumeration-constant
    	case T_INTEGER_CONSTANT:
    	case T_FLOATING_CONSTANT:
        case T_CHARACTER_CONSTANT: {
            return parse_tree_node_create(CONSTANT, &lex_tok);
        } break;
        default: goto ERROR;
    }
    
ERROR:
	lex_setpos(yytext);
	
	return NULL;
}

static ParseTreeNode *parse_string(void)
{    
    if (yylex() != T_STRING) {
    	goto ERROR;
    }

    return parse_tree_node_create(STRING, &lex_tok);

ERROR:
    lex_setpos(yytext);

    return NULL;
}

static ParseTreeNode *parse_assignment_operator(void)
{
    switch(yylex()) {
        case T_ASSIGNMENT:
        case T_MULTIPLY_ASSIGN:
        case T_DIVIDE_ASSIGN:
        case T_MODULUS_ASSIGN:
        case T_PLUS_ASSIGN:
        case T_MINUS_ASSIGN:
        case T_BITWISE_LEFTSHIFT_ASSIGN:
        case T_BITWISE_RIGHTSHIFT_ASSIGN:
        case T_BITWISE_AND_ASSIGN:
        case T_BITWISE_XOR_ASSIGN:
        case T_BITWISE_OR_ASSIGN: {
            return parse_tree_node_create(ASSIGNMENT_OPERATOR, &lex_tok);
        }
        default: {
        	lex_setpos(yytext);

        	return NULL;
        }
    }
}

static ParseTreeNode *parse_abstract_declarator(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(ABSTRACT_DECLARATOR, NULL);
    ParseTreeNode *pointer;
    ParseTreeNode *direct_abstract_declarator;

    parse_opt(this_node, pointer, NEXT_AFTER_POINTER);

    parse_required(this_node, direct_abstract_declarator, ERROR);

    goto OK;

NEXT_AFTER_POINTER:
    parse_opt(this_node, direct_abstract_declarator, OK);

OK:
    return this_node;

ERROR:
    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_parameter_list(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(PARAMETER_LIST, NULL);
    ParseTreeNode *parameter_declaration;
    char *lex_pos_saved;
    char *lex_pos_saved_comma;

NEXT_PARAMETER_DECLARATION_LIST:
	lex_pos_saved = lex_tell();

    parse_required(this_node, parameter_declaration, NEXT_PARAMETER_DECLARATION_LIST_AFTER);

    lex_pos_saved_comma = lex_tell();
    
    if (yylex() == ',') {
        goto NEXT_PARAMETER_DECLARATION_LIST;
    }
    
    lex_pos_saved = lex_pos_saved_comma;

NEXT_PARAMETER_DECLARATION_LIST_AFTER:
    if (this_node->num == 0) {
        lex_setpos(lex_pos_saved);

        goto ERROR;
    }

    lex_setpos(lex_pos_saved_comma);

    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_parameter_declaration(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(PARAMETER_DECLARATION, NULL);
    ParseTreeNode *declaration_specifier;
    ParseTreeNode *declarator;
    ParseTreeNode *abstract_declarator;

    parse_list_required(this_node, declaration_specifier, ERROR);
    
    parse_opt(this_node, declarator, OK);
    
    parse_opt(this_node, abstract_declarator, OK);

OK:
    return this_node;

ERROR:
    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_direct_abstract_declarator(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(DIRECT_ABSTRACT_DECLARATOR, NULL);
    ParseTreeNode *left_node = NULL;
    ParseTreeNode *abstract_declarator;
    ParseTreeNode *constant_expression;
    ParseTreeNode *parameter_type_list;
    char *lex_pos_saved;

WHILE_BEGIN:
	lex_pos_saved = lex_tell();

    switch (yylex()) {
    	case '(': {
			this_node->tok = lex_tok;

		    if (this_node->num == 0) {
		        parse_opt(this_node, abstract_declarator, WHILE_TAIL);
		    }
		    
		    parse_opt(this_node, parameter_type_list, NEXT_AFTER_PARAMETER_TYPE_LIST);

NEXT_AFTER_PARAMETER_TYPE_LIST:
		    if (yylex() != ')') {
		        goto WHILE_BREAK; /* TODO ERROR ?? */
		    }

		    goto WHILE_TAIL;
		}
    	case '{': {
			this_node->tok = lex_tok;

		    parse_opt(this_node, constant_expression, NEXT_AFTER_CONSTANT_EXPRESSION);

NEXT_AFTER_CONSTANT_EXPRESSION:
		    if (yylex() != '}') {
		        goto WHILE_BREAK; /* TODO ERROR ?? */
		    }

		    goto WHILE_TAIL;
		}
    	default: goto WHILE_BREAK;
    }

WHILE_TAIL:
    left_node = this_node;

    this_node = parse_tree_node_create(DIRECT_ABSTRACT_DECLARATOR, NULL);

    parse_tree_node_add(this_node, left_node);

    goto WHILE_BEGIN;

WHILE_BREAK:
    lex_setpos(lex_pos_saved);
    
    if (this_node->num != 0) {
        this_node->elements[0] = NULL; /* TODO WHERE IS ERROR STATE ?? */
    }
    
    parse_tree_node_destroy(this_node);
    
    return left_node;
}

static ParseTreeNode *parse_enumerator_list(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(ENUMERATOR_LIST, NULL);
    ParseTreeNode *enumerator;
    char *lex_pos_saved;

	lex_pos_saved = lex_tell();

NEXT_ENUMERATOR_LIST:
    parse_required(this_node, enumerator, ERROR);

    if (yylex() == ',') {
        goto NEXT_ENUMERATOR_LIST;
    }

    lex_setpos(yytext);

    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_enumerator(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(ENUMERATOR, NULL);
    ParseTreeNode *identifier;
    ParseTreeNode *constant_expression;
    char *lex_pos_saved;

	lex_pos_saved = lex_tell();

    parse_required(this_node, identifier, ERROR);

    if (yylex() != '=') {
        lex_setpos(yytext);

        goto OK;
    }

    parse_required(this_node, constant_expression, ERROR);

OK:
    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_init_declarator_list(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(INIT_DECLARATOR_LIST, NULL);
    ParseTreeNode *init_declarator;
    char *lex_pos_saved;

	lex_pos_saved = lex_tell();

NEXT_INIT_DECLARATOR:
    parse_required(this_node, init_declarator, CHECK_ERROR);
    
    if (yylex() == ',') {
        goto NEXT_INIT_DECLARATOR;
    }

    lex_setpos(yytext);

CHECK_ERROR:
    if (this_node->num == 0) {
        goto ERROR;
    }

    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_init_declarator(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(INIT_DECLARATOR, NULL);
    ParseTreeNode *declarator;
    ParseTreeNode *initializer;
    char *lex_pos_saved;

	lex_pos_saved = lex_tell();

    parse_required(this_node, declarator, ERROR);

    if (yylex() != '=') {
        lex_setpos(yytext);

        goto OK;
    }

    parse_required(this_node, initializer, ERROR);

OK:
    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_initializer(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(INITIALIZER, NULL);
    ParseTreeNode *assignment_expression;
    ParseTreeNode *initializer_list;
    char *lex_pos_saved;

	lex_pos_saved = lex_tell();

    parse_opt(this_node, assignment_expression, OK);

    if (yylex() != '{') {
        goto ERROR;
    }

    parse_required(this_node, initializer_list, ERROR);

    if (yylex() != ',') {
        lex_setpos(yytext);
    }

    if (yylex() != '}') {
        goto ERROR;
    }

OK:
    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_initializer_list(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(INITIALIZER_LIST, NULL);
    ParseTreeNode *initializer;
    char *lex_pos_saved;

	lex_pos_saved = lex_tell();

NEXT_INITIALIZER_LIST:
    parse_required(this_node, initializer, ERROR);

    if (yylex() == ',') {
        goto NEXT_INITIALIZER_LIST;
    }

    lex_setpos(yytext);

    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_statement(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(STATEMENT, NULL);
    ParseTreeNode *labeled_statement;
    ParseTreeNode *expression_statement;
    ParseTreeNode *compound_statement;
    ParseTreeNode *selection_statement;
    ParseTreeNode *iteration_statement;
    ParseTreeNode *jump_statement;

    parse_opt(this_node, labeled_statement, OK);

    parse_opt(this_node, expression_statement, OK);

    parse_opt(this_node, compound_statement, OK);

    parse_opt(this_node, selection_statement, OK);

    parse_opt(this_node, iteration_statement, OK);

    parse_opt(this_node, jump_statement, OK);

    parse_tree_node_destroy(this_node);

    return NULL;

    OK:
        return this_node;
}

static ParseTreeNode *parse_labeled_statement(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(LABELED_STATEMENT, NULL);
    ParseTreeNode *statement;
    ParseTreeNode *constant_expression;
    char *lex_pos_saved;

	lex_pos_saved = lex_tell();

    switch (yylex()) {
    	case T_IDENTIFIER: {
    		this_node->tok = lex_tok;
		} break;
		case T_DEFAULT: {
			this_node->tok = lex_tok;
		} break;
		case T_CASE: {
			this_node->tok = lex_tok;
		    
		    parse_required(this_node, constant_expression, ERROR);
		} break;
		default: goto ERROR;
	}

    if (yylex() != ':') {
        goto ERROR;
    }

    parse_required(this_node, statement, ERROR);

    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_expression_statement(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(EXPRESSION_STATEMENT, NULL);
    ParseTreeNode *expression;
    char *lex_pos_saved;

	lex_pos_saved = lex_tell();

    parse_opt(this_node, expression, next);

    next: {
        if (yylex() != ';') {
            goto ERROR;
        }
    }

    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);    

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_selection_statement(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(SELECTION_STATEMENT, NULL);
    ParseTreeNode *expression;
    ParseTreeNode *statement;
    char *lex_pos_saved;

	lex_pos_saved = lex_tell();

    switch (yylex()) {
        case T_IF:
        case T_SWITCH: {
            this_node->tok = lex_tok;
        } break;
        default: goto ERROR;
    }

    if (yylex() != '(') {
        goto ERROR;
    }

    parse_required(this_node, expression, ERROR);
    
    if (yylex() != ')') {
        goto ERROR;
    }

    parse_required(this_node, statement, ERROR);

    if (this_node->tok.type == T_SWITCH) {
        goto OK;
    }

    if (yylex() != T_ELSE) {
        lex_setpos(yytext);
    
        goto OK;
    }

    parse_required(this_node, statement, ERROR);

OK:
    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_iteration_statement(void)
{
	ParseTreeNode *this_node = parse_tree_node_create(ITERATION_STATEMENT, NULL);
    ParseTreeNode *expression;
    ParseTreeNode *statement;
    char *lex_pos_saved;

	lex_pos_saved = lex_tell();

    switch(yylex()) {
        case T_WHILE: {
			this_node->tok = lex_tok;

            if (yylex() != '(') {
                goto ERROR;
            }

            parse_required(this_node, expression, ERROR);

            if (yylex() != ')') {
                goto ERROR;
            }

            parse_required(this_node, statement, ERROR);
        } break;
        case T_DO: {
            this_node->tok = lex_tok;

            parse_required(this_node, statement, ERROR);

            if (yylex() != T_WHILE) {
                goto ERROR;
            }

            if (yylex() != '(') {
                goto ERROR;
            }

            parse_required(this_node, expression, ERROR);

            if (yylex() != ')') {
                goto ERROR;
            }

            if (yylex() != ';') {
                goto ERROR;
            }
        } break;
        case T_FOR: {
            this_node->tok = lex_tok;

            if (yylex() != '(') {
                goto ERROR;
            }

            parse_required(this_node, expression, NEXT_EXPRESSION_INIT);
            
            expression->tok.type = T_OPEN_PARENT; /* TODO HACK FOR IDENTIFICATION IN irgen */

NEXT_EXPRESSION_INIT:
            if (yylex() != ';') {
                goto ERROR;
            }

            parse_required(this_node, expression, NEXT_EXPRESSION_CONDITION);

			expression->tok.type = T_SEMICOLON; /* TODO HACK FOR IDENTIFICATION IN irgen */

NEXT_EXPRESSION_CONDITION:
            if (yylex() != ';') {
                goto ERROR;
            }

            parse_required(this_node, expression, NEXT_EXPRESSION_POST);

			expression->tok.type = T_CLOSING_PARENT; /* TODO HACK FOR IDENTIFICATION IN irgen */

NEXT_EXPRESSION_POST:
            if (yylex() != ')') {
                goto ERROR;
            }

            parse_required(this_node, statement, ERROR);
        } break;
        default: goto ERROR;
    }

    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_jump_statement(void)
{
    ParseTreeNode *this_node = parse_tree_node_create(JUMP_STATEMENT, NULL);
    ParseTreeNode *identifier;
    ParseTreeNode *expression;
    char *lex_pos_saved;

	lex_pos_saved = lex_tell();

    switch(yylex()) {
        case T_GOTO: {
    		this_node->tok = lex_tok;

            parse_required(this_node, identifier, NEXT_TOKEN_SEMICOLON);
        } break;
        case T_RETURN: {
        	this_node->tok = lex_tok;

            parse_opt(this_node, expression, NEXT_TOKEN_SEMICOLON);
        } break;
        case T_CONTINUE:
        case T_BREAK: {
        	this_node->tok = lex_tok;
    	} break;
        default: goto ERROR;
    }

NEXT_TOKEN_SEMICOLON:
    if (yylex() != ';') {
    	goto ERROR;
    }

    return this_node;

ERROR:
    lex_setpos(lex_pos_saved);

    parse_tree_node_destroy(this_node);

    return NULL;
}

static ParseTreeNode *parse_tree_node_create(ParseTreeType type, Tok *tok)
{
    ParseTreeNode *node = (ParseTreeNode *)calloc(1, sizeof(ParseTreeNode));

    assert(node != NULL);

    node->type = type;

    if (tok != NULL) {
        node->tok = *tok;
    }

    return node;
}

static int parse_tree_node_add(ParseTreeNode *root, ParseTreeNode *child)
{
    if (root->elements == NULL) {
        root->elements = (ParseTreeNode **)calloc(1, sizeof(ParseTreeNode *));

        assert(root->elements != NULL);

        root->elements[root->num] = child;

        root->len = 1;
        root->num++;

        return 0;
    }

    if (root->len == root->num) {
        root->elements = (ParseTreeNode **)realloc(root->elements, sizeof(ParseTreeNode *) * root->len * 2);

     	assert(root->elements != NULL);

        root->elements[root->num] = child;

        root->len *= 2;
        root->num++;

        return 0;
    }

    root->elements[root->num] = child;
    root->num++;

    return 0;
}

extern void parse_tree_node_destroy(ParseTreeNode *node)
{
    if (node == NULL) {
        return;
    }

    if (node->elements != NULL) {
    	size_t i;
        for (i = 0; i < node->num; ++i) {
            parse_tree_node_destroy(node->elements[i]);
        }
    }

    free(node);
}
