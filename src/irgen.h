#ifndef JD297_CC_IRGEN
#define JD297_CC_IRGEN

#include "ir.h"
#include "parse.h"

extern void irgen_c_run(IR_CTX *ctx, ParseTreeNode *translation_unit);

#endif

#ifdef JD297_CC_IRGEN_IMPLEMENTATION
#undef JD297_CC_IRGEN_IMPLEMENTATION

static void irgen_c_translation_unit(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_external_declaration(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_function_definition(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_declaration(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_declaration_specifier(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_declarator(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_compound_statement(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_storage_class_specifier(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_type_specifier(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_type_qualifier(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_struct_or_union_specifier(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_enum_specifier(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_typedef_name(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_struct_or_union(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_identifier(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_struct_declaration(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_specifier_qualifier(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_struct_declarator_list(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_struct_declarator(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_constant_expression(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_pointer(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_direct_declarator(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_parameter_type_list(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_conditional_expression(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_logical_or_expression(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_expression(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_logical_and_expression(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_inclusive_or_expression(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_exclusive_or_expression(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_and_expression(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_equality_expression(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_relational_expression(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_shift_expression(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_additive_expression(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_multiplicative_expression(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_cast_expression(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_unary_expression(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_type_name(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_postfix_expression(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_unary_operator(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_primary_expression(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_assignment_expression(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_constant(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_string(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_assignment_operator(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_abstract_declarator(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_parameter_list(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_parameter_declaration(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_direct_abstract_declarator(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_enumerator_list(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_enumerator(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_init_declarator_list(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_init_declarator(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_initializer(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_initializer_list(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_statement(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_labeled_statement(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_expression_statement(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_selection_statement(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_iteration_statement(IR_CTX *ctx, ParseTreeNode *this_node);
static void irgen_c_jump_statement(IR_CTX *ctx, ParseTreeNode *this_node);

#endif
