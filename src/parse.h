#ifndef JD297_CC_PARSE
#define JD297_CC_PARSE

#include <stddef.h>

#include "tok.h"
#include "ir.h"
#include "symtbl.h"

extern int yyparse(void);

typedef enum {
	TRANSLATION_UNIT,
	EXTERNAL_DECLARATION,
	FUNCTION_DEFINITION,
	DECLARATION,
	DECLARATION_SPECIFIER,
	DECLARATOR,
	COMPOUND_STATEMENT,
	STORAGE_CLASS_SPECIFIER,
	TYPE_SPECIFIER,
	TYPE_QUALIFIER,
	STRUCT_OR_UNION_SPECIFIER,
	ENUM_SPECIFIER,
	TYPEDEF_NAME,
	STRUCT_OR_UNION,
	IDENTIFIER,
	STRUCT_DECLARATION,
	SPECIFIER_QUALIFIER,
	STRUCT_DECLARATOR_LIST,
	STRUCT_DECLARATOR,
	CONSTANT_EXPRESSION,
	POINTER,
	DIRECT_DECLARATOR,
	PARAMETER_TYPE_LIST,
	CONDITIONAL_EXPRESSION,
	LOGICAL_OR_EXPRESSION,
	EXPRESSION,
	LOGICAL_AND_EXPRESSION,
	INCLUSIVE_OR_EXPRESSION,
	EXCLUSIVE_OR_EXPRESSION,
	AND_EXPRESSION,
	EQUALITY_EXPRESSION,
	RELATIONAL_EXPRESSION,
	SHIFT_EXPRESSION,
	ADDITIVE_EXPRESSION,
	MULTIPLICATIVE_EXPRESSION,
	CAST_EXPRESSION,
	UNARY_EXPRESSION,
	TYPE_NAME,
	POSTFIX_EXPRESSION,
	UNARY_OPERATOR,
	PRIMARY_EXPRESSION,
	ARGUMENT_EXPRESSION_LIST,
	ASSIGNMENT_EXPRESSION,
	CONSTANT,
	STRING,
	INTEGER_CONSTANT,
	CHARACTER_CONSTANT,
	FLOATING_CONSTANT,
	ENUMERATION_CONSTANT,
	ASSIGNMENT_OPERATOR,
	ABSTRACT_DECLARATOR,
	PARAMETER_LIST,
	PARAMETER_DECLARATION,
	DIRECT_ABSTRACT_DECLARATOR,
	ENUMERATOR_LIST,
	ENUMERATOR,
	INIT_DECLARATOR_LIST,
	INIT_DECLARATOR,
	INITIALIZER,
	INITIALIZER_LIST,
	STATEMENT,
	LABELED_STATEMENT,
	EXPRESSION_STATEMENT,
	SELECTION_STATEMENT,
	ITERATION_STATEMENT,
	JUMP_STATEMENT
} ParseTreeType;

typedef struct ParseTreeNode ParseTreeNode;

struct ParseTreeNode {
	ParseTreeType type;
    Tok tok;

    ParseTreeNode **elements;
    size_t len;
    size_t num;

    SymtblEntry *sym;
    IRDataType *dtype;
};

extern int parse_error_count;

extern ParseTreeNode *parse_result;

extern void parse_tree_node_destroy(ParseTreeNode *node);

#define parse_required(root_node, name, empty_label) \
        if ((name = parse_##name()) == NULL) { \
            goto empty_label; \
        } \
        \
        parse_tree_node_add((this_node), (name));

#define parse_opt(root_node, name, next_label)\
        if ((name = parse_##name()) != NULL) {\
            parse_tree_node_add((this_node), (name));\
\
            goto next_label;\
        }

#define parse_list_required(root_node, name, empty_label)\
	{\
        size_t count_##name;\
        for (count_##name = 0; (name = parse_##name()) != NULL; count_##name++) {\
            parse_tree_node_add((root_node), name);\
        }\
        if (count_##name == 0) {\
            goto empty_label;\
        }\
    }

#define parse_list_opt(root_node, name) \
        while ((name = parse_##name()) != NULL) { \
            parse_tree_node_add((root_node), name); \
        }

#endif

#ifdef JD297_CC_PARSE_IMPLEMENTATION
#undef JD297_CC_PARSE_IMPLEMENTATION

static ParseTreeNode *parse_tree_node_create(ParseTreeType type, Tok *tok);

static int parse_tree_node_add(ParseTreeNode *root, ParseTreeNode *child);

static ParseTreeNode *parse_translation_unit(void);
static ParseTreeNode *parse_external_declaration(void);
static ParseTreeNode *parse_function_definition(void);
static ParseTreeNode *parse_declaration(void);
static ParseTreeNode *parse_declaration_specifier(void);
static ParseTreeNode *parse_declarator(void);
static ParseTreeNode *parse_compound_statement(void);
static ParseTreeNode *parse_storage_class_specifier(void);
static ParseTreeNode *parse_type_specifier(void);
static ParseTreeNode *parse_type_qualifier(void);
static ParseTreeNode *parse_struct_or_union_specifier(void);
static ParseTreeNode *parse_enum_specifier(void);
static ParseTreeNode *parse_typedef_name(void);
static ParseTreeNode *parse_struct_or_union(void);
static ParseTreeNode *parse_identifier(void);
static ParseTreeNode *parse_struct_declaration(void);
static ParseTreeNode *parse_specifier_qualifier(void);
static ParseTreeNode *parse_struct_declarator_list(void);
static ParseTreeNode *parse_struct_declarator(void);
static ParseTreeNode *parse_pointer(void);
static ParseTreeNode *parse_constant_expression(void);
static ParseTreeNode *parse_direct_declarator(void);
static ParseTreeNode *parse_parameter_type_list(void);
static ParseTreeNode *parse_conditional_expression(void);
static ParseTreeNode *parse_logical_or_expression(void);
static ParseTreeNode *parse_expression(void);
static ParseTreeNode *parse_logical_and_expression(void);
static ParseTreeNode *parse_inclusive_or_expression(void);
static ParseTreeNode *parse_exclusive_or_expression(void);
static ParseTreeNode *parse_and_expression(void);
static ParseTreeNode *parse_equality_expression(void);
static ParseTreeNode *parse_relational_expression(void);
static ParseTreeNode *parse_shift_expression(void);
static ParseTreeNode *parse_additive_expression(void);
static ParseTreeNode *parse_multiplicative_expression(void);
static ParseTreeNode *parse_cast_expression(void);
static ParseTreeNode *parse_unary_expression(void);
static ParseTreeNode *parse_type_name(void);
static ParseTreeNode *parse_postfix_expression(void);
static ParseTreeNode *parse_unary_operator(void);
static ParseTreeNode *parse_primary_expression(void);
static ParseTreeNode *parse_argument_expression_list(void);
static ParseTreeNode *parse_assignment_expression(void);
static ParseTreeNode *parse_constant(void);
static ParseTreeNode *parse_string(void);
static ParseTreeNode *parse_assignment_operator(void);
static ParseTreeNode *parse_abstract_declarator(void);
static ParseTreeNode *parse_parameter_list(void);
static ParseTreeNode *parse_parameter_declaration(void);
static ParseTreeNode *parse_direct_abstract_declarator(void);
static ParseTreeNode *parse_enumerator_list(void);
static ParseTreeNode *parse_enumerator(void);
static ParseTreeNode *parse_init_declarator_list(void);
static ParseTreeNode *parse_init_declarator(void);
static ParseTreeNode *parse_initializer(void);
static ParseTreeNode *parse_initializer_list(void);
static ParseTreeNode *parse_statement(void);
static ParseTreeNode *parse_labeled_statement(void);
static ParseTreeNode *parse_expression_statement(void);
static ParseTreeNode *parse_selection_statement(void);
static ParseTreeNode *parse_iteration_statement(void);
static ParseTreeNode *parse_jump_statement(void);

#endif
