#ifndef JD297_CC_PARSE
#define JD297_CC_PARSE

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "tok.h"
#include "ir.h"
#include "symtbl.h"
#include "gen.h"

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

typedef struct {
	enum {
		VALUE_SIGN_NOT_SET = 0,
		SIGNED,
		UNSIGNED
	} value_sign;
	
	enum {
    	LENGTH_TYPE_NOT_SET = 0,
    	SHORT,
    	LONG
    } length_type;
        
    enum {
    	BASIC_DATA_TYPE_NOT_SET = 0,
    	INT,
    	CHAR,
    	DOUBLE,
    	FLOAT,
    	VOID
    } basic_data_type;
        
    enum {
    	DATA_STRUCTURE_TYPE_NOT_SET = 0,
    	STRUCT,
    	UNION,
    	ENUM,
    	PRIMITIVE
    } data_structure_type;

    int qualifier_flags;
	int storage_flags;
} ParseDeclarationSpecifier;

typedef struct {
	ParseDeclarationSpecifier decl_spec;
	IRDataType *dtype;	
} ParseDeclarationState;

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

static ParseDeclarationState decl_state;

static void parse_decl_state_reset(void)
{
	(void)memset(&decl_state, 0, sizeof(ParseDeclarationState));
}

static void parse_decl_state_add_decl_spec(TokType type)
{
	switch (type) {
		/* STORAGE_CLASS_SPECIFIER */
		case T_AUTO: {
			if (decl_state.decl_spec.storage_flags & IR_STORAGE_FLAG_AUTO) {
				assert(0 && "TODO warning message");
			}
			
			decl_state.decl_spec.storage_flags |= IR_STORAGE_FLAG_AUTO;
		} break;
		case T_REGISTER: {
			if (decl_state.decl_spec.storage_flags & IR_STORAGE_FLAG_REGISTER) {
				assert(0 && "TODO warning message");
			}
			
			decl_state.decl_spec.storage_flags |= IR_STORAGE_FLAG_REGISTER;
		} break;
		case T_STATIC: {
			if (decl_state.decl_spec.storage_flags & IR_STORAGE_FLAG_STATIC) {
				assert(0 && "TODO warning message");
			}
			
			decl_state.decl_spec.storage_flags |= IR_STORAGE_FLAG_STATIC;
		} break;
		case T_EXTERN: {
			if (decl_state.decl_spec.storage_flags & IR_STORAGE_FLAG_EXTERN) {
				assert(0 && "TODO warning message");
			}
			
			decl_state.decl_spec.storage_flags |= IR_STORAGE_FLAG_EXTERN;
		} break;
		case T_TYPEDEF: {
			assert(0 && "TODO not implemented: with (T_TYPEDEF)");
			
			if (decl_state.decl_spec.storage_flags & IR_STORAGE_FLAG_TYPEDEF) {
				assert(0 && "TODO warning message");
			}
			
			decl_state.decl_spec.storage_flags |= IR_STORAGE_FLAG_TYPEDEF;
		} break;
		/* TYPE_QUALIFIER */
		case T_CONST: {
			if (decl_state.decl_spec.qualifier_flags & IR_QUALIFIER_FLAG_CONST) {
				assert(0 && "TODO warning message");
			}
			
			decl_state.decl_spec.qualifier_flags |= IR_QUALIFIER_FLAG_CONST;
		} break;
		case T_VOLATILE: {
			if (decl_state.decl_spec.qualifier_flags & IR_QUALIFIER_FLAG_VOLATILE) {
				assert(0 && "TODO warning message");
			}
			
			decl_state.decl_spec.qualifier_flags |= IR_QUALIFIER_FLAG_VOLATILE;
		} break;
		/* TYPE_SPECIFIER */
		// TODO struct-or-union, enum, typedef
		case T_VOID: {
			if (decl_state.decl_spec.length_type != LENGTH_TYPE_NOT_SET) {
				assert(0 && "TODO error message");
			}

			if (decl_state.decl_spec.value_sign != VALUE_SIGN_NOT_SET) {
				assert(0 && "TODO error message");
			}

			if (decl_state.decl_spec.data_structure_type != DATA_STRUCTURE_TYPE_NOT_SET) {
				assert(0 && "TODO error message");
			}
			
			if (decl_state.decl_spec.basic_data_type != BASIC_DATA_TYPE_NOT_SET) {
				assert(0 && "TODO error message");
			}
			
			decl_state.decl_spec.basic_data_type = VOID;
			decl_state.decl_spec.data_structure_type = PRIMITIVE;
		} break;
		case T_CHAR: {
			if (decl_state.decl_spec.length_type != LENGTH_TYPE_NOT_SET) {
				assert(0 && "TODO error message");
			}

			if (decl_state.decl_spec.data_structure_type != DATA_STRUCTURE_TYPE_NOT_SET && decl_state.decl_spec.data_structure_type != PRIMITIVE) {
				assert(0 && "TODO error message");
			}
			
			if (decl_state.decl_spec.basic_data_type != BASIC_DATA_TYPE_NOT_SET) {
				assert(0 && "TODO error message");
			}
			
			decl_state.decl_spec.basic_data_type = CHAR;
			decl_state.decl_spec.data_structure_type = PRIMITIVE;
		} break;
		case T_SHORT: {
			if (decl_state.decl_spec.length_type != LENGTH_TYPE_NOT_SET) {
				assert(0 && "TODO error message");
			}
			
			if (decl_state.decl_spec.data_structure_type != DATA_STRUCTURE_TYPE_NOT_SET && decl_state.decl_spec.data_structure_type != PRIMITIVE) {
				assert(0 && "TODO error message");
			}
			
			if (decl_state.decl_spec.basic_data_type != BASIC_DATA_TYPE_NOT_SET && (decl_state.decl_spec.basic_data_type == VOID || decl_state.decl_spec.basic_data_type == FLOAT || decl_state.decl_spec.basic_data_type == CHAR || decl_state.decl_spec.basic_data_type == DOUBLE)) {
				assert(0 && "TODO error message");
			}
			
			decl_state.decl_spec.length_type = SHORT;
			decl_state.decl_spec.data_structure_type = PRIMITIVE;
		} break;
		case T_INT: {
			if (decl_state.decl_spec.data_structure_type != DATA_STRUCTURE_TYPE_NOT_SET && decl_state.decl_spec.data_structure_type != PRIMITIVE) {
				assert(0 && "TODO error message");
			}
			
			if (decl_state.decl_spec.basic_data_type != BASIC_DATA_TYPE_NOT_SET) {
				assert(0 && "TODO error message");
			}
			
			decl_state.decl_spec.basic_data_type = INT;
			decl_state.decl_spec.data_structure_type = PRIMITIVE;
		} break;
		case T_LONG: {
			if (decl_state.decl_spec.length_type != LENGTH_TYPE_NOT_SET) {
				assert(0 && "TODO error message");
			}
			
			if (decl_state.decl_spec.data_structure_type != DATA_STRUCTURE_TYPE_NOT_SET && decl_state.decl_spec.data_structure_type != PRIMITIVE) {
				assert(0 && "TODO error message");
			}
			
			if (decl_state.decl_spec.basic_data_type != BASIC_DATA_TYPE_NOT_SET && (decl_state.decl_spec.basic_data_type == VOID || decl_state.decl_spec.basic_data_type == FLOAT || decl_state.decl_spec.basic_data_type == CHAR)) {
				assert(0 && "TODO error message");
			}
			
			decl_state.decl_spec.length_type = LONG;
			decl_state.decl_spec.data_structure_type = PRIMITIVE;
		} break;
		case T_FLOAT: {
			if (decl_state.decl_spec.length_type != LENGTH_TYPE_NOT_SET) {
				assert(0 && "TODO error message");
			}

			if (decl_state.decl_spec.data_structure_type != DATA_STRUCTURE_TYPE_NOT_SET) {
				assert(0 && "TODO error message");
			}
			
			if (decl_state.decl_spec.basic_data_type != BASIC_DATA_TYPE_NOT_SET) {
				assert(0 && "TODO error message");
			}
			
			if (decl_state.decl_spec.value_sign != VALUE_SIGN_NOT_SET) {
				assert(0 && "TODO error message");
			}
			
			decl_state.decl_spec.basic_data_type = FLOAT;
			decl_state.decl_spec.data_structure_type = PRIMITIVE;
		} break;
		case T_DOUBLE: {
			if (decl_state.decl_spec.length_type == SHORT) {
				assert(0 && "TODO error message");
			}

			if (decl_state.decl_spec.data_structure_type != DATA_STRUCTURE_TYPE_NOT_SET && decl_state.decl_spec.data_structure_type != PRIMITIVE) {
				assert(0 && "TODO error message");
			}
			
			if (decl_state.decl_spec.basic_data_type != BASIC_DATA_TYPE_NOT_SET) {
				assert(0 && "TODO error message");
			}
			
			if (decl_state.decl_spec.value_sign != VALUE_SIGN_NOT_SET) {
				assert(0 && "TODO error message");
			}
			
			decl_state.decl_spec.basic_data_type = DOUBLE;
			decl_state.decl_spec.data_structure_type = PRIMITIVE;
		} break;
		case T_SIGNED: {
			if (decl_state.decl_spec.value_sign != VALUE_SIGN_NOT_SET) {
				assert(0 && "TODO error message");
			}
			
			if (decl_state.decl_spec.data_structure_type != DATA_STRUCTURE_TYPE_NOT_SET && decl_state.decl_spec.data_structure_type != PRIMITIVE) {
				assert(0 && "TODO error message");
			}
			
			if (decl_state.decl_spec.basic_data_type != BASIC_DATA_TYPE_NOT_SET && (decl_state.decl_spec.basic_data_type == VOID || decl_state.decl_spec.basic_data_type == FLOAT || decl_state.decl_spec.basic_data_type == DOUBLE)) {
				assert(0 && "TODO error message");
			}

			decl_state.decl_spec.value_sign = SIGNED;
			decl_state.decl_spec.data_structure_type = PRIMITIVE;
		} break;
		case T_UNSIGNED: {
			if (decl_state.decl_spec.value_sign != VALUE_SIGN_NOT_SET) {
				assert(0 && "TODO error message");
			}
			
			if (decl_state.decl_spec.data_structure_type != DATA_STRUCTURE_TYPE_NOT_SET && decl_state.decl_spec.data_structure_type != PRIMITIVE) {
				assert(0 && "TODO error message");
			}
			
			if (decl_state.decl_spec.basic_data_type != BASIC_DATA_TYPE_NOT_SET && (decl_state.decl_spec.basic_data_type == VOID || decl_state.decl_spec.basic_data_type == FLOAT || decl_state.decl_spec.basic_data_type == DOUBLE)) {
				assert(0 && "TODO error message");
			}

			decl_state.decl_spec.value_sign = UNSIGNED;
			decl_state.decl_spec.data_structure_type = PRIMITIVE;
		} break;
		default:
			assert(0 && "NOT REACHABLE");
	}
}

static void parse_decl_state_build_dtype(void)
{
	decl_state.dtype = malloc(sizeof(IRDataType));

    assert(decl_state.dtype != NULL);

	switch (decl_state.decl_spec.data_structure_type) {
    	case STRUCT:
    		assert(0 && "TODO not implemented: with (STRUCT)");
    	case UNION:
    		assert(0 && "TODO not implemented: with (UNION)");
    	case ENUM:
    		assert(0 && "TODO not implemented: with (ENUM)");
    	case PRIMITIVE: {
    		if (decl_state.decl_spec.basic_data_type == VOID) {
    			decl_state.dtype->type = IR_TYPE_NONE;

    			break;
    		}

    		decl_state.dtype->type = IR_TYPE_PRIMITIVE;
    		
    		switch (decl_state.decl_spec.basic_data_type) {
    			case CHAR: {
    				switch (decl_state.decl_spec.value_sign) {
    					case VALUE_SIGN_NOT_SET: {
    						decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_CHAR);
    					} break;
    					case SIGNED: {
    						decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_SIGNED_CHAR);
    					} break;
    					case UNSIGNED: {
    						decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_UNSIGNED_CHAR);
    					} break;
    					default:
							assert(0 && "NOT REACHABLE");
    				}
    			} break;
    			case INT: {
    				switch (decl_state.decl_spec.value_sign) {
    					case VALUE_SIGN_NOT_SET: {
    						switch (decl_state.decl_spec.length_type) {
    							case LENGTH_TYPE_NOT_SET: {
    								decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_INT);
    							} break;
								case SHORT: {
    								decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_SHORT_INT);
    							} break;
								case LONG: {
    								decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_LONG_INT);
    							} break;
    							default:
									assert(0 && "NOT REACHABLE");
    						}
    					} break;
						case SIGNED: {
    						switch (decl_state.decl_spec.length_type) {
    							case LENGTH_TYPE_NOT_SET: {
    								decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_SIGNED_INT);
    							} break;
								case SHORT: {
    								decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_SIGNED_SHORT_INT);
    							} break;
								case LONG: {
    								decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_SIGNED_LONG_INT);
    							} break;
    							default:
									assert(0 && "NOT REACHABLE");
    						}
    					} break;
						case UNSIGNED: {
    						switch (decl_state.decl_spec.length_type) {
    							case LENGTH_TYPE_NOT_SET: {
    								decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_UNSIGNED_INT);
    							} break;
								case SHORT: {
    								decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_UNSIGNED_SHORT_INT);
    							} break;
								case LONG: {
    								decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_UNSIGNED_LONG_INT);
    							} break;
    							default:
									assert(0 && "NOT REACHABLE");
    						}
    					} break;
    					default:
							assert(0 && "NOT REACHABLE");
    				}
    			} break;
    			case BASIC_DATA_TYPE_NOT_SET: {
    				switch (decl_state.decl_spec.value_sign) {
    					case VALUE_SIGN_NOT_SET: {
    						switch (decl_state.decl_spec.length_type) {
    							case LENGTH_TYPE_NOT_SET: {
    								assert(0 && "NOT REACHABLE");
    							} break;
								case SHORT: {
    								decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_SHORT);
    							} break;
								case LONG: {
    								decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_LONG);
    							} break;
    							default:
									assert(0 && "NOT REACHABLE");
    						}
    					} break;
						case SIGNED: {
    						switch (decl_state.decl_spec.length_type) {
    							case LENGTH_TYPE_NOT_SET: {
    								decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_SIGNED);
    							} break;
								case SHORT: {
    								decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_SIGNED_SHORT);
    							} break;
								case LONG: {
    								decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_SIGNED_LONG);
    							} break;
    							default:
									assert(0 && "NOT REACHABLE");
    						}
    					} break;
						case UNSIGNED: {
    						switch (decl_state.decl_spec.length_type) {
    							case LENGTH_TYPE_NOT_SET: {
    								decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_UNSIGNED);
    							} break;
								case SHORT: {
    								decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_UNSIGNED_SHORT);
    							} break;
								case LONG: {
    								decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_UNSIGNED_LONG);
    							} break;
    							default:
									assert(0 && "NOT REACHABLE");
    						}
    					} break;
    					default:
							assert(0 && "NOT REACHABLE");
    				}
    			} break;
    			case DOUBLE: {
    				if (decl_state.decl_spec.length_type == LONG) {
    					decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_LONG_DOUBLE);
    					break;
    				}
    				
    				decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_DOUBLE);
    			} break;
    			case FLOAT: {
    				decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_FLOAT);
				} break;
    			case VOID:
    			default:
    				assert(0 && "NOT REACHABLE");
    		}
		} break;
    	case DATA_STRUCTURE_TYPE_NOT_SET: {
    		decl_state.dtype->type = IR_TYPE_PRIMITIVE;
    		decl_state.dtype->as.primitive = codegen_get_primitive_data_type(IR_GENERIC_INT);
    	} break;
    	default:
    		assert(0 && "NOT REACHABLE");
    }

	decl_state.dtype->qualifier_flags = decl_state.decl_spec.qualifier_flags;
	decl_state.dtype->storage_flags = decl_state.decl_spec.storage_flags;
}

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
