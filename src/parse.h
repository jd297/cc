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

extern int parse_error_count;

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
	int is_ptr;
	int is_from_typedef;
} ParseDeclarationSpecifier;

typedef struct {
	ParseDeclarationSpecifier decl_spec;
	IRDataType dtype;
	SymtblEntryClass eclass;
} ParseDeclarationState;

#endif

#ifdef JD297_CC_PARSE_IMPLEMENTATION
#undef JD297_CC_PARSE_IMPLEMENTATION

static ParseDeclarationState decl_state;

static void parse_decl_state_build_dtype(void)
{
	switch (decl_state.decl_spec.data_structure_type) {
    	case STRUCT:
    		assert(0 && "TODO not implemented: with (STRUCT)");
    	case UNION:
    		assert(0 && "TODO not implemented: with (UNION)");
    	case ENUM:
    		assert(0 && "TODO not implemented: with (ENUM)");
    	case PRIMITIVE: {
    		if (decl_state.decl_spec.basic_data_type == VOID) {
    			decl_state.dtype.type = IR_TYPE_NONE;

    			break;
    		}

    		decl_state.dtype.type = IR_TYPE_PRIMITIVE;
    		
    		switch (decl_state.decl_spec.basic_data_type) {
    			case CHAR: {
    				switch (decl_state.decl_spec.value_sign) {
    					case VALUE_SIGN_NOT_SET: {
    						decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_CHAR);
    					} break;
    					case SIGNED: {
    						decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_SIGNED_CHAR);
    					} break;
    					case UNSIGNED: {
    						decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_UNSIGNED_CHAR);
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
    								decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_INT);
    							} break;
								case SHORT: {
    								decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_SHORT_INT);
    							} break;
								case LONG: {
    								decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_LONG_INT);
    							} break;
    							default:
									assert(0 && "NOT REACHABLE");
    						}
    					} break;
						case SIGNED: {
    						switch (decl_state.decl_spec.length_type) {
    							case LENGTH_TYPE_NOT_SET: {
    								decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_SIGNED_INT);
    							} break;
								case SHORT: {
    								decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_SIGNED_SHORT_INT);
    							} break;
								case LONG: {
    								decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_SIGNED_LONG_INT);
    							} break;
    							default:
									assert(0 && "NOT REACHABLE");
    						}
    					} break;
						case UNSIGNED: {
    						switch (decl_state.decl_spec.length_type) {
    							case LENGTH_TYPE_NOT_SET: {
    								decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_UNSIGNED_INT);
    							} break;
								case SHORT: {
    								decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_UNSIGNED_SHORT_INT);
    							} break;
								case LONG: {
    								decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_UNSIGNED_LONG_INT);
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
    								decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_SHORT);
    							} break;
								case LONG: {
    								decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_LONG);
    							} break;
    							default:
									assert(0 && "NOT REACHABLE");
    						}
    					} break;
						case SIGNED: {
    						switch (decl_state.decl_spec.length_type) {
    							case LENGTH_TYPE_NOT_SET: {
    								decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_SIGNED);
    							} break;
								case SHORT: {
    								decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_SIGNED_SHORT);
    							} break;
								case LONG: {
    								decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_SIGNED_LONG);
    							} break;
    							default:
									assert(0 && "NOT REACHABLE");
    						}
    					} break;
						case UNSIGNED: {
    						switch (decl_state.decl_spec.length_type) {
    							case LENGTH_TYPE_NOT_SET: {
    								decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_UNSIGNED);
    							} break;
								case SHORT: {
    								decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_UNSIGNED_SHORT);
    							} break;
								case LONG: {
    								decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_UNSIGNED_LONG);
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
    					decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_LONG_DOUBLE);
    					break;
    				}
    				
    				decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_DOUBLE);
    			} break;
    			case FLOAT: {
    				decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_FLOAT);
				} break;
    			case VOID:
    			default:
    				assert(0 && "NOT REACHABLE");
    		}
		} break;
    	case DATA_STRUCTURE_TYPE_NOT_SET: {
		    if (decl_state.decl_spec.is_ptr == 1) {
		    	decl_state.dtype.type = IR_TYPE_POINTER;

		    	break;
		    }

			decl_state.dtype.type = IR_TYPE_PRIMITIVE;
    		decl_state.dtype.as.primitive = codegen_get_primitive_data_type(IR_GENERIC_INT);
    	} break;
    	default:
    		assert(0 && "NOT REACHABLE");
    }

	decl_state.dtype.qualifier_flags = decl_state.decl_spec.qualifier_flags;
	decl_state.dtype.storage_flags = decl_state.decl_spec.storage_flags;
}

static void parse_decl_state_reset(SymtblEntryClass eclass)
{
	switch (eclass) {
		case SYM_CLASS_OBJECT:
		case SYM_CLASS_FUNCTION:
		case SYM_CLASS_TYPEDEF_NAME:
		case SYM_CLASS_TAG_OF_STRUCT:
		case SYM_CLASS_UNION:
		case SYM_CLASS_ENUMERATION:
		case SYM_CLASS_STRUCT_UNION_MEMBER:
		case SYM_CLASS_UNKNOWN:
			break;
		case SYM_CLASS_ENUM_CONSTANT:
		case SYM_CLASS_LABEL:
		default:
			assert(0 && "NOT REACHABLE");
	}

	(void)memset(&decl_state, 0, sizeof(ParseDeclarationState));

	decl_state.eclass = eclass;
}

static void parse_decl_state_add_decl_spec(TokType type)
{
	switch (type) {
		case '*': {
			decl_state.decl_spec.is_ptr = 1;
		} break;
		/* STORAGE_CLASS_SPECIFIER */
		/* TODO validate that only one of them exist */
		case T_AUTO: {
			decl_state.decl_spec.storage_flags |= IR_STORAGE_FLAG_AUTO;
		} break;
		case T_REGISTER: {
			decl_state.decl_spec.storage_flags |= IR_STORAGE_FLAG_REGISTER;
		} break;
		case T_STATIC: {
			decl_state.decl_spec.storage_flags |= IR_STORAGE_FLAG_STATIC;
		} break;
		case T_EXTERN: {
			decl_state.decl_spec.storage_flags |= IR_STORAGE_FLAG_EXTERN;
		} break;
		case T_TYPEDEF: {
			decl_state.decl_spec.storage_flags |= IR_STORAGE_FLAG_TYPEDEF;
			
			decl_state.eclass = SYM_CLASS_TYPEDEF_NAME;
		} break;
		/* TYPE_QUALIFIER */
		case T_CONST: {
			decl_state.decl_spec.qualifier_flags |= IR_QUALIFIER_FLAG_CONST;
		} break;
		case T_VOLATILE: {
			decl_state.decl_spec.qualifier_flags |= IR_QUALIFIER_FLAG_VOLATILE;
		} break;
		/* TYPE_SPECIFIER */
		// TODO struct-or-union, enum
		case T_VOID: {
			assert(decl_state.decl_spec.is_from_typedef == 0);

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
			assert(decl_state.decl_spec.is_from_typedef == 0);

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
			assert(decl_state.decl_spec.is_from_typedef == 0);

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
			assert(decl_state.decl_spec.is_from_typedef == 0);

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
			assert(decl_state.decl_spec.is_from_typedef == 0);

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
			assert(decl_state.decl_spec.is_from_typedef == 0);

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
			assert(decl_state.decl_spec.is_from_typedef == 0);

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
			assert(decl_state.decl_spec.is_from_typedef == 0);

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
			assert(decl_state.decl_spec.is_from_typedef == 0);

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

	if (decl_state.decl_spec.is_from_typedef != 1) {
		parse_decl_state_build_dtype();
	}
}

typedef void *ParseReturn;

#define PARSE_OK	((ParseReturn)0)
#define PARSE_ERROR ((ParseReturn)-1)

#define parse_required(name, empty_label)\
        if ((name()) == PARSE_ERROR) {\
            goto empty_label;\
        }

#define parse_opt(name, next_label)\
        if ((name()) != PARSE_ERROR) {\
            goto next_label;\
        }

#define parse_list_required(name, empty_label)\
	{\
        size_t count_##name;\
        for (count_##name = 0; (name()) != PARSE_ERROR; count_##name++);\
        if (count_##name == 0) {\
            goto empty_label;\
        }\
    }

#define parse_list_opt(name)\
        while ((name()) != PARSE_ERROR);

static ParseReturn parse_translation_unit(void);
static ParseReturn parse_external_declaration(void);
static ParseReturn parse_function_definition(void);
static ParseReturn parse_declaration(void);
static ParseReturn parse_declaration_specifier(void);
static ParseReturn parse_declarator(void);
static ParseReturn parse_compound_statement(void);
static ParseReturn parse_storage_class_specifier(void);
static ParseReturn parse_type_specifier(void);
static ParseReturn parse_type_qualifier(void);
static ParseReturn parse_struct_or_union_specifier(void);
static ParseReturn parse_enum_specifier(void);
static ParseReturn parse_typedef_name(void);
static ParseReturn parse_struct_or_union(void);
static ParseReturn parse_identifier(void);
static ParseReturn parse_struct_declaration(void);
static ParseReturn parse_specifier_qualifier(void);
static ParseReturn parse_struct_declarator_list(void);
static ParseReturn parse_struct_declarator(void);
static ParseReturn parse_pointer(void);
static ParseReturn parse_constant_expression(void);
static ParseReturn parse_direct_declarator(void);
static ParseReturn parse_parameter_type_list(void);
static ParseReturn parse_conditional_expression(void);
static ParseReturn parse_logical_or_expression(void);
static ParseReturn parse_expression(void);
static ParseReturn parse_logical_and_expression(void);
static ParseReturn parse_inclusive_or_expression(void);
static ParseReturn parse_exclusive_or_expression(void);
static ParseReturn parse_and_expression(void);
static ParseReturn parse_equality_expression(void);
static ParseReturn parse_relational_expression(void);
static ParseReturn parse_shift_expression(void);
static ParseReturn parse_additive_expression(void);
static ParseReturn parse_multiplicative_expression(void);
static ParseReturn parse_cast_expression(void);
static ParseReturn parse_unary_expression(void);
static ParseReturn parse_type_name(void);
static ParseReturn parse_postfix_expression(void);
static ParseReturn parse_unary_operator(void);
static ParseReturn parse_primary_expression(void);
static ParseReturn parse_argument_expression_list(void);
static ParseReturn parse_assignment_expression(void);
static ParseReturn parse_constant(void);
static ParseReturn parse_string(void);
static ParseReturn parse_abstract_declarator(void);
static ParseReturn parse_parameter_list(void);
static ParseReturn parse_parameter_declaration(void);
static ParseReturn parse_direct_abstract_declarator(void);
static ParseReturn parse_enumerator_list(void);
static ParseReturn parse_enumerator(void);
static ParseReturn parse_init_declarator_list(void);
static ParseReturn parse_init_declarator(void);
static ParseReturn parse_initializer(void);
static ParseReturn parse_initializer_list(void);
static ParseReturn parse_statement(void);
static ParseReturn parse_labeled_statement(void);
static ParseReturn parse_expression_statement(void);
static ParseReturn parse_selection_statement(void);
static ParseReturn parse_iteration_statement(void);
static ParseReturn parse_jump_statement(void);

#endif
