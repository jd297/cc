#ifndef JD297_CC_DEBUG_UTIL
#define JD297_CC_DEBUG_UTIL

#include <assert.h>

#include "ir.h"
#include "parse.h"
#include "tok.h"

static const char *parse_tree_type_to_str(ParseTreeType type);

static int parse_tree_type_has_token(ParseTreeType type);

static const char *tok_type_to_str(TokType type);

static int tok_type_has_literal(TokType type);

static int tok_type_print_literal(Tok tok);

static const char *parse_tree_type_to_str(ParseTreeType type)
{
	switch (type) {
		case TRANSLATION_UNIT: return "TRANSLATION_UNIT";
		case EXTERNAL_DECLARATION: return "EXTERNAL_DECLARATION";
		case FUNCTION_DEFINITION: return "FUNCTION_DEFINITION";
		case DECLARATION: return "DECLARATION";
		case DECLARATION_SPECIFIER: return "DECLARATION_SPECIFIER";
		case DECLARATOR: return "DECLARATOR";
		case COMPOUND_STATEMENT: return "COMPOUND_STATEMENT";
		case STORAGE_CLASS_SPECIFIER: return "STORAGE_CLASS_SPECIFIER";
		case TYPE_SPECIFIER: return "TYPE_SPECIFIER";
		case TYPE_QUALIFIER: return "TYPE_QUALIFIER";
		case STRUCT_OR_UNION_SPECIFIER: return "STRUCT_OR_UNION_SPECIFIER";
		case ENUM_SPECIFIER: return "ENUM_SPECIFIER";
		case TYPEDEF_NAME: return "TYPEDEF_NAME";
		case STRUCT_OR_UNION: return "STRUCT_OR_UNION";
		case IDENTIFIER: return "IDENTIFIER";
		case STRUCT_DECLARATION: return "STRUCT_DECLARATION";
		case SPECIFIER_QUALIFIER: return "SPECIFIER_QUALIFIER";
		case STRUCT_DECLARATOR_LIST: return "STRUCT_DECLARATOR_LIST";
		case STRUCT_DECLARATOR: return "STRUCT_DECLARATOR";
		case CONSTANT_EXPRESSION: return "CONSTANT_EXPRESSION";
		case POINTER: return "POINTER";
		case DIRECT_DECLARATOR: return "DIRECT_DECLARATOR";
		case PARAMETER_TYPE_LIST: return "PARAMETER_TYPE_LIST";
		case CONDITIONAL_EXPRESSION: return "CONDITIONAL_EXPRESSION";
		case LOGICAL_OR_EXPRESSION: return "LOGICAL_OR_EXPRESSION";
		case EXPRESSION: return "EXPRESSION";
		case LOGICAL_AND_EXPRESSION: return "LOGICAL_AND_EXPRESSION";
		case INCLUSIVE_OR_EXPRESSION: return "INCLUSIVE_OR_EXPRESSION";
		case EXCLUSIVE_OR_EXPRESSION: return "EXCLUSIVE_OR_EXPRESSION";
		case AND_EXPRESSION: return "AND_EXPRESSION";
		case EQUALITY_EXPRESSION: return "EQUALITY_EXPRESSION";
		case RELATIONAL_EXPRESSION: return "RELATIONAL_EXPRESSION";
		case SHIFT_EXPRESSION: return "SHIFT_EXPRESSION";
		case ADDITIVE_EXPRESSION: return "ADDITIVE_EXPRESSION";
		case MULTIPLICATIVE_EXPRESSION: return "MULTIPLICATIVE_EXPRESSION";
		case CAST_EXPRESSION: return "CAST_EXPRESSION";
		case UNARY_EXPRESSION: return "UNARY_EXPRESSION";
		case TYPE_NAME: return "TYPE_NAME";
		case POSTFIX_EXPRESSION: return "POSTFIX_EXPRESSION";
		case UNARY_OPERATOR: return "UNARY_OPERATOR";
		case PRIMARY_EXPRESSION: return "PRIMARY_EXPRESSION";
		case ARGUMENT_EXPRESSION_LIST: return "ARGUMENT_EXPRESSION_LIST";
		case ASSIGNMENT_EXPRESSION: return "ASSIGNMENT_EXPRESSION";
		case CONSTANT: return "CONSTANT";
		case STRING: return "STRING";
		case INTEGER_CONSTANT: return "INTEGER_CONSTANT";
		case CHARACTER_CONSTANT: return "CHARACTER_CONSTANT";
		case FLOATING_CONSTANT: return "FLOATING_CONSTANT";
		case ENUMERATION_CONSTANT: return "ENUMERATION_CONSTANT";
		case ASSIGNMENT_OPERATOR: return "ASSIGNMENT_OPERATOR";
		case ABSTRACT_DECLARATOR: return "ABSTRACT_DECLARATOR";
		case PARAMETER_LIST: return "PARAMETER_LIST";
		case PARAMETER_DECLARATION: return "PARAMETER_DECLARATION";
		case DIRECT_ABSTRACT_DECLARATOR: return "DIRECT_ABSTRACT_DECLARATOR";
		case ENUMERATOR_LIST: return "ENUMERATOR_LIST";
		case ENUMERATOR: return "ENUMERATOR";
		case INIT_DECLARATOR_LIST: return "INIT_DECLARATOR_LIST";
		case INIT_DECLARATOR: return "INIT_DECLARATOR";
		case INITIALIZER: return "INITIALIZER";
		case INITIALIZER_LIST: return "INITIALIZER_LIST";
		case STATEMENT: return "STATEMENT";
		case LABELED_STATEMENT: return "LABELED_STATEMENT";
		case EXPRESSION_STATEMENT: return "EXPRESSION_STATEMENT";
		case SELECTION_STATEMENT: return "SELECTION_STATEMENT";
		case ITERATION_STATEMENT: return "ITERATION_STATEMENT";
		case JUMP_STATEMENT: return "JUMP_STATEMENT";
		default: assert(0 && "NOT REACHABLE");
	}
}

static int parse_tree_type_has_token(ParseTreeType type)
{
	switch (type) {
		case TRANSLATION_UNIT:
		case EXTERNAL_DECLARATION:
		case FUNCTION_DEFINITION:
		case DECLARATION:
		case DECLARATION_SPECIFIER:
		case DECLARATOR:
		case COMPOUND_STATEMENT:
		case STRUCT_DECLARATION:
		case SPECIFIER_QUALIFIER:
		case STRUCT_DECLARATOR_LIST:
		case STRUCT_DECLARATOR:
		case CONSTANT_EXPRESSION:
		case POINTER:
		case PARAMETER_TYPE_LIST:
		case CONDITIONAL_EXPRESSION:
		case LOGICAL_OR_EXPRESSION:
		case EXPRESSION:
		case LOGICAL_AND_EXPRESSION:
		case INCLUSIVE_OR_EXPRESSION:
		case EXCLUSIVE_OR_EXPRESSION:
		case AND_EXPRESSION:
		case EQUALITY_EXPRESSION:
		case RELATIONAL_EXPRESSION:
		case SHIFT_EXPRESSION:
		case ADDITIVE_EXPRESSION:
		case MULTIPLICATIVE_EXPRESSION:
		case CAST_EXPRESSION:
		case POSTFIX_EXPRESSION:
		case TYPE_NAME:
		case PRIMARY_EXPRESSION:
		case ARGUMENT_EXPRESSION_LIST:
		case ASSIGNMENT_EXPRESSION:
		case ABSTRACT_DECLARATOR:
		case PARAMETER_LIST:
		case PARAMETER_DECLARATION:
		case ENUMERATOR_LIST:
		case INIT_DECLARATOR_LIST:
		case INIT_DECLARATOR:
		case INITIALIZER:
		case INITIALIZER_LIST:
		case STATEMENT:
			return 0;

		case UNARY_OPERATOR:
		case DIRECT_DECLARATOR:
		case DIRECT_ABSTRACT_DECLARATOR:
		case UNARY_EXPRESSION:
		case STORAGE_CLASS_SPECIFIER:
		case TYPE_SPECIFIER:
		case TYPE_QUALIFIER:
		case STRUCT_OR_UNION_SPECIFIER:
		case ENUM_SPECIFIER:
		case TYPEDEF_NAME:
		case STRUCT_OR_UNION:
		case IDENTIFIER:
		case CONSTANT:
		case STRING:
		case INTEGER_CONSTANT:
		case CHARACTER_CONSTANT:
		case FLOATING_CONSTANT:
		case ENUMERATION_CONSTANT:
		case ASSIGNMENT_OPERATOR:
		case ENUMERATOR:
		case LABELED_STATEMENT:
		case EXPRESSION_STATEMENT:
		case SELECTION_STATEMENT:
		case ITERATION_STATEMENT:
		case JUMP_STATEMENT:
			return 1;
		default: assert(0 && "NOT REACHABLE");
	}
}

static const char *tok_type_to_str(TokType type)
{
	switch (type) {
		case T_EOF: return "T_EOF";
		case T_PLUS_ASSIGN: return "T_PLUS_ASSIGN";
		case T_MINUS_ASSIGN: return "T_MINUS_ASSIGN";
		case T_MULTIPLY_ASSIGN: return "T_MULTIPLY_ASSIGN";
		case T_DIVIDE_ASSIGN: return "T_DIVIDE_ASSIGN";
		case T_MODULUS_ASSIGN: return "T_MODULUS_ASSIGN";
		case T_BITWISE_AND_ASSIGN: return "T_BITWISE_AND_ASSIGN";
		case T_BITWISE_OR_ASSIGN: return "T_BITWISE_OR_ASSIGN";
		case T_BITWISE_XOR_ASSIGN: return "T_BITWISE_XOR_ASSIGN";
		case T_BITWISE_RIGHTSHIFT_ASSIGN: return "T_BITWISE_RIGHTSHIFT_ASSIGN";
		case T_BITWISE_LEFTSHIFT_ASSIGN: return "T_BITWISE_LEFTSHIFT_ASSIGN";
		case T_LESS_THAN_OR_EQUAL_TO: return "T_LESS_THAN_OR_EQUAL_TO";
		case T_GREATER_THAN_OR_EQUAL_TO: return "T_GREATER_THAN_OR_EQUAL_TO";
		case T_EQUAL_TO: return "T_EQUAL_TO";
		case T_NOT_EQUAL_TO: return "T_NOT_EQUAL_TO";
		case T_LOGICAL_AND: return "T_LOGICAL_AND";
		case T_LOGICAL_OR: return "T_LOGICAL_OR";
		case T_LOGICAL_NOT: return "T_LOGICAL_NOT";
		case T_BITWISE_AND: return "T_BITWISE_AND";
		case T_BITWISE_OR: return "T_BITWISE_OR";
		case T_BITWISE_XOR: return "T_BITWISE_XOR";
		case T_BITWISE_LEFTSHIFT: return "T_BITWISE_LEFTSHIFT";
		case T_BITWISE_RIGHTSHIFT: return "T_BITWISE_RIGHTSHIFT";
		case T_LESS_THAN: return "T_LESS_THAN";
		case T_GREATER_THAN: return "T_GREATER_THAN";
		case T_DOT_DOT_DOT: return "T_DOT_DOT_DOT";
		case T_OPEN_BRACKET: return "T_OPEN_BRACKET";
		case T_CLOSING_BRACKET: return "T_CLOSING_BRACKET";
		case T_OPEN_PARENT: return "T_OPEN_PARENT";
		case T_CLOSING_PARENT: return "T_CLOSING_PARENT";
		case T_OPEN_BRACE: return "T_OPEN_BRACE";
		case T_CLOSING_BRACE: return "T_CLOSING_BRACE";
		case T_DOT: return "T_DOT";
		case T_ARROW: return "T_ARROW";
		case T_COMMA: return "T_COMMA";
		case T_COLON: return "T_COLON";
		case T_SEMICOLON: return "T_SEMICOLON";
		case T_ASSIGNMENT: return "T_ASSIGNMENT";
		case T_TILDE: return "T_TILDE";
		case T_INCREMENT: return "T_INCREMENT";
		case T_DECREMENT: return "T_DECREMENT";
		case T_PLUS: return "T_PLUS";
		case T_MINUS: return "T_MINUS";
		case T_MULTIPLY: return "T_MULTIPLY";
		case T_DIVIDE: return "T_DIVIDE";
		case T_MODULUS: return "T_MODULUS";
		case T_TERNARY: return "T_TERNARY";
		case T_AUTO: return "T_AUTO";
		case T_BREAK: return "T_BREAK";
		case T_CASE: return "T_CASE";
		case T_CHAR: return "T_CHAR";
		case T_CONST: return "T_CONST";
		case T_CONTINUE: return "T_CONTINUE";
		case T_DEFAULT: return "T_DEFAULT";
		case T_DO: return "T_DO";
		case T_DOUBLE: return "T_DOUBLE";
		case T_ELSE: return "T_ELSE";
		case T_ENUM: return "T_ENUM";
		case T_EXTERN: return "T_EXTERN";
		case T_FLOAT: return "T_FLOAT";
		case T_FOR: return "T_FOR";
		case T_GOTO: return "T_GOTO";
		case T_IF: return "T_IF";
		case T_INLINE: return "T_INLINE";
		case T_INT: return "T_INT";
		case T_LONG: return "T_LONG";
		case T_REGISTER: return "T_REGISTER";
		case T_RESTRICT: return "T_RESTRICT";
		case T_RETURN: return "T_RETURN";
		case T_SHORT: return "T_SHORT";
		case T_SIGNED: return "T_SIGNED";
		case T_SIZEOF: return "T_SIZEOF";
		case T_STATIC: return "T_STATIC";
		case T_STRUCT: return "T_STRUCT";
		case T_SWITCH: return "T_SWITCH";
		case T_TYPEDEF: return "T_TYPEDEF";
		case T_UNION: return "T_UNION";
		case T_UNSIGNED: return "T_UNSIGNED";
		case T_VOID: return "T_VOID";
		case T_VOLATILE: return "T_VOLATILE";
		case T_WHILE: return "T_WHILE";
		case T_MACRO_IFDEF: return "T_MACRO_IFDEF";
		case T_MACRO_IFNDEF: return "T_MACRO_IFNDEF";
		case T_MACRO_IF: return "T_MACRO_IF";
		case T_MACRO_ELIF: return "T_MACRO_ELIF";
		case T_MACRO_ELSE: return "T_MACRO_ELSE";
		case T_MACRO_ENDIF: return "T_MACRO_ENDIF";
		case T_MACRO_DEFINE: return "T_MACRO_DEFINE";
		case T_MACRO_UNDEF: return "T_MACRO_UNDEF";
		case T_MACRO_INCLUDE: return "T_MACRO_INCLUDE";
		case T_MACRO_LINE: return "T_MACRO_LINE";
		case T_MACRO_ERROR: return "T_MACRO_ERROR";
		case T_MACRO_PRAGMA: return "T_MACRO_PRAGMA";
		case T_INTEGER_CONSTANT: return "T_INTEGER_CONSTANT";
		case T_FLOATING_CONSTANT: return "T_FLOATING_CONSTANT";
		case T_STRING: return "T_STRING";
		case T_CHARACTER_CONSTANT: return "T_CHARACTER_CONSTANT";
		case T_IDENTIFIER: return "T_IDENTIFIER";
		case T_UNKNOWN: return "T_UNKNOWN";
		default: assert(0 && "NOT REACHABLE");
	}
}

static int tok_type_has_literal(TokType type)
{
	switch (type) {
		case T_EOF:
		case T_PLUS_ASSIGN:
		case T_MINUS_ASSIGN:
		case T_MULTIPLY_ASSIGN:
		case T_DIVIDE_ASSIGN:
		case T_MODULUS_ASSIGN:
		case T_BITWISE_AND_ASSIGN:
		case T_BITWISE_OR_ASSIGN:
		case T_BITWISE_XOR_ASSIGN:
		case T_BITWISE_RIGHTSHIFT_ASSIGN:
		case T_BITWISE_LEFTSHIFT_ASSIGN:
		case T_LESS_THAN_OR_EQUAL_TO:
		case T_GREATER_THAN_OR_EQUAL_TO:
		case T_EQUAL_TO:
		case T_NOT_EQUAL_TO:
		case T_LOGICAL_AND:
		case T_LOGICAL_OR:
		case T_LOGICAL_NOT:
		case T_BITWISE_AND:
		case T_BITWISE_OR:
		case T_BITWISE_XOR:
		case T_BITWISE_LEFTSHIFT:
		case T_BITWISE_RIGHTSHIFT:
		case T_LESS_THAN:
		case T_GREATER_THAN:
		case T_DOT_DOT_DOT:
		case T_OPEN_BRACKET:
		case T_CLOSING_BRACKET:
		case T_OPEN_PARENT:
		case T_CLOSING_PARENT:
		case T_OPEN_BRACE:
		case T_CLOSING_BRACE:
		case T_DOT:
		case T_ARROW:
		case T_COMMA:
		case T_COLON:
		case T_SEMICOLON:
		case T_ASSIGNMENT:
		case T_TILDE:
		case T_INCREMENT:
		case T_DECREMENT:
		case T_PLUS:
		case T_MINUS:
		case T_MULTIPLY:
		case T_DIVIDE:
		case T_MODULUS:
		case T_TERNARY:
		case T_AUTO:
		case T_BREAK:
		case T_CASE:
		case T_CHAR:
		case T_CONST:
		case T_CONTINUE:
		case T_DEFAULT:
		case T_DO:
		case T_DOUBLE:
		case T_ELSE:
		case T_ENUM:
		case T_EXTERN:
		case T_FLOAT:
		case T_FOR:
		case T_GOTO:
		case T_IF:
		case T_INLINE:
		case T_INT:
		case T_LONG:
		case T_REGISTER:
		case T_RESTRICT:
		case T_RETURN:
		case T_SHORT:
		case T_SIGNED:
		case T_SIZEOF:
		case T_STATIC:
		case T_STRUCT:
		case T_SWITCH:
		case T_TYPEDEF:
		case T_UNION:
		case T_UNSIGNED:
		case T_VOID:
		case T_VOLATILE:
		case T_WHILE:
		case T_MACRO_IFDEF:
		case T_MACRO_IFNDEF:
		case T_MACRO_IF:
		case T_MACRO_ELIF:
		case T_MACRO_ELSE:
		case T_MACRO_ENDIF:
		case T_MACRO_DEFINE:
		case T_MACRO_UNDEF:
		case T_MACRO_INCLUDE:
		case T_MACRO_LINE:
		case T_MACRO_ERROR:
		case T_MACRO_PRAGMA:
		case T_UNKNOWN:
			return 0;
		
		case T_INTEGER_CONSTANT:
		case T_FLOATING_CONSTANT:
		case T_STRING:
		case T_CHARACTER_CONSTANT:
		case T_IDENTIFIER:
			return 1;
		default: assert(0 && "NOT REACHABLE");
	}
}

static int tok_type_print_literal(Tok tok)
{
	switch (tok.type) {
		case T_INTEGER_CONSTANT:
			return printf("{ d:%d | u:%u | ld:%ld | lu:%lu }", tok.literal.d, tok.literal.u, tok.literal.ld, tok.literal.lu);
		case T_FLOATING_CONSTANT:
			return printf("{ f:%f | lf:%lf | Lf:%Lf }", tok.literal.f, tok.literal.lf, tok.literal.Lf);
		case T_CHARACTER_CONSTANT:
			return printf("{ c:\'%c\' | d:%d }", tok.literal.d, tok.literal.d);
		case T_STRING:
			return printf("\\\""SV_FMT"\\\" (%zu)", SV_PARAMS(&tok.literal.sv), tok.literal.sv.len);
		case T_IDENTIFIER:
			return printf(SV_FMT" (%zu)", SV_PARAMS(&tok.literal.sv), tok.literal.sv.len);
		default: assert(0 && "NOT REACHABLE");
	}
}

static char *ir_dtype_type_to_str(IRDataArgType type)
{
	switch (type) {
		case IR_TYPE_PRIMITIVE: return "IR_TYPE_PRIMITIVE";
		case IR_TYPE_ENUM: return "IR_TYPE_ENUM";
		case IR_TYPE_STRUCT: return "IR_TYPE_STRUCT";
		case IR_TYPE_ARRAY: return "IR_TYPE_ARRAY";
		case IR_TYPE_POINTER: return "IR_TYPE_POINTER";
		case IR_TYPE_NONE: return "IR_TYPE_NONE";
		default: assert(0 && "NOT REACHABLE");
	}
}

static char *ir_dtype_primitive_to_str(IRPrimitiveDataType primitive)
{
	switch (primitive) {
		case IR_U8_T: return "IR_U8_T";
		case IR_S8_T: return "IR_S8_T";
		case IR_U16_T: return "IR_U16_T";
		case IR_S16_T: return "IR_S16_T";
		case IR_U32_T: return "IR_U32_T";
		case IR_S32_T: return "IR_S32_T";
		case IR_U64_T: return "IR_U64_T";
		case IR_S64_T: return "IR_S64_T";
		case IR_F32_T: return "IR_F32_T";
		case IR_F64_T: return "IR_F64_T";
		case IR_F128_T: return "IR_F128_T";
		case IR_PTR_T: return "IR_PTR_T";
		default: assert(0 && "NOT REACHABLE");
	}
}

static void ir_print_storage_flags(int storage_flags)
{
	printf("{ STORAGE_FLAGS | - ");
	
	if (storage_flags & IR_STORAGE_FLAG_AUTO) {
		printf("AUTO - ");
	}
	
	if (storage_flags & IR_STORAGE_FLAG_REGISTER) {
		printf("REGISTER - ");
	}
	
	if (storage_flags & IR_STORAGE_FLAG_STATIC) {
		printf("STATIC - ");
	}
	
	if (storage_flags & IR_STORAGE_FLAG_EXTERN) {
		printf("EXTERN - ");
	}
	
	if (storage_flags & IR_STORAGE_FLAG_TYPEDEF) {
		printf("TYPEDEF - ");
	}
	
	if (storage_flags == IR_STORAGE_FLAG_NONE) {
		printf("NONE - ");
	}
	
	printf("}");
}

static void ir_print_qualifier_flags(int qualifier_flags)
{
	printf("{ QUALIFIER_FLAGS | - ");
	
	if (qualifier_flags & IR_QUALIFIER_FLAG_CONST) {
		printf("CONST - ");
	}
	
	if (qualifier_flags & IR_QUALIFIER_FLAG_VOLATILE) {
		printf("VOLATILE - ");
	}
	
	if (qualifier_flags == IR_QUALIFIER_FLAG_NONE) {
		printf("NONE - ");
	}
	
	printf("}");
}

static void ir_dtype_print_as(IRDataType *dtype);

static void ir_print_dtype(IRDataType *dtype)
{
	printf("{ TYPE | %s }", ir_dtype_type_to_str(dtype->type));
	
	printf("|");
	ir_print_storage_flags(dtype->storage_flags);
	printf("|");
	ir_print_qualifier_flags(dtype->qualifier_flags);
	printf("|");
	printf("{ AS | ");
	ir_dtype_print_as(dtype);
	printf(" }");
}

static void ir_dtype_print_as(IRDataType *dtype)
{
	switch (dtype->type) {
		case IR_TYPE_PRIMITIVE: {
			printf("%s", ir_dtype_primitive_to_str(dtype->as.primitive));
		} break;
		case IR_TYPE_ENUM: {
			assert(0 && "TODO not implemented with (IR_TYPE_ENUM)");
		} break;
		case IR_TYPE_STRUCT: {
			assert(0 && "TODO not implemented with (IR_TYPE_STRUCT)");
		} break;
		case IR_TYPE_ARRAY: {
			assert(0 && "TODO not implemented with (IR_TYPE_ARRAY)");
		} break;
		case IR_TYPE_POINTER: {
			printf("{");
			printf("POINTER TO | ");
			ir_print_dtype(dtype->as.pointer.to);
			printf("}");
		} break;
		case IR_TYPE_NONE: {
			printf("NONE");
		} break;
		default: assert(0 && "NOT REACHABLE");
	}
}

#endif
