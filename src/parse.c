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

static Symtbl *parse_scope_global;
static Symtbl *parse_scope_function;
static Symtbl *parse_scope_current;
static SymtblEntry *parse_function_entry;

extern int yyparse(void)
{
	ParseReturn result;

	if ((parse_scope_global = symtbl_create(NULL)) == NULL) {
		return -1;
	}

	if (ir_ctx_create() != 0) {
		return -1;
	}

	parse_error_count = 0;

	result = parse_translation_unit();

	symtbl_free(parse_scope_global);

    if (result == PARSE_ERROR || parse_error_count > 0) {
    	return -1;
    }

    return 0;
}

static ParseReturn parse_translation_unit(void)
{
    parse_list_opt(parse_external_declaration);

    return PARSE_OK;
}

static ParseReturn parse_external_declaration(void)
{
    parse_scope_current = parse_scope_global;
	parse_function_entry = NULL;

    parse_opt(parse_function_definition, OK);

    parse_opt(parse_declaration, OK);

/* ERROR: */
    return PARSE_ERROR;

OK:
    return PARSE_OK;
}

static ParseReturn parse_function_definition(void)
{
    char *lex_pos_saved;

	parse_decl_state_reset(SYM_CLASS_FUNCTION);

    lex_pos_saved = lex_tell();

    parse_list_opt(parse_declaration_specifier);
    
    parse_required(parse_declarator, ERROR);

    parse_list_opt(parse_declaration);

    ir_emit(IR_OC_FUNC_BEGIN, NULL, ir_ssa_from_view(&parse_function_entry->id), NULL, NULL);

    parse_required(parse_compound_statement, ERROR);

	ir_emit(IR_OC_FUNC_END, NULL, NULL, NULL, NULL);

	/* TODO check all elements in namespace2 (LABEL) have flag is_labeled == 1 */

    return PARSE_OK;

ERROR:
	lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_declaration(void)
{
    char *lex_pos_saved;

	parse_decl_state_reset(SYM_CLASS_OBJECT);

    lex_pos_saved = lex_tell();

    parse_list_required(parse_declaration_specifier, ERROR);

    parse_opt(parse_init_declarator_list, NEXT);

NEXT:
	if (yylex() != ';') {
        goto ERROR;
    }

    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_declaration_specifier(void)
{
    parse_opt(parse_storage_class_specifier, OK);

    parse_opt(parse_type_specifier, OK);

    parse_opt(parse_type_qualifier, OK);

/* ERROR: */
	return PARSE_ERROR;

OK:
    return PARSE_OK;
}

static ParseReturn parse_declarator(void)
{
    parse_opt(parse_pointer, NEXT_DIRECT_DECLARATOR);

NEXT_DIRECT_DECLARATOR:
	parse_required(parse_direct_declarator, ERROR);

    return PARSE_OK;

ERROR:
	return PARSE_ERROR;
}

static ParseReturn parse_compound_statement(void)
{
    char *lex_pos_saved;
    Symtbl *scope_saved;

	scope_saved = parse_scope_current;

    lex_pos_saved = lex_tell();

    if (yylex() != '{') {
        goto ERROR;
    }

	if (parse_scope_current != parse_function_entry->as.function.scope) {
		parse_scope_current = symtbl_create(parse_scope_current);

		assert(parse_scope_current != NULL);
	}

    parse_list_opt(parse_declaration);

    parse_list_opt(parse_statement);

	/* restore scope on success or failure (error recovery purpose) */
	if (parse_scope_current != parse_function_entry->as.function.scope) {
		symtbl_free(parse_scope_current);
	}

	parse_scope_current = scope_saved;

    if (yylex() != '}') {
        goto ERROR;
    }

/* OK: */
    return PARSE_OK;

ERROR:
	lex_setpos(lex_pos_saved);

	return PARSE_ERROR;
}

static ParseReturn parse_storage_class_specifier(void)
{
    switch (yylex()) {
        case T_AUTO:
        case T_REGISTER:
        case T_STATIC:
        case T_EXTERN:
        case T_TYPEDEF: {
			parse_decl_state_add_decl_spec(lex_tok.type);

            return PARSE_OK;
        }
        default: {
		    lex_setpos(yytext);

		    return PARSE_ERROR;
		}
    }
}

static ParseReturn parse_type_specifier(void)
{
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
			parse_decl_state_add_decl_spec(lex_tok.type);

            return PARSE_OK;
        }
        default: {
        	lex_setpos(yytext);
        } break;
    }

    parse_opt(parse_struct_or_union_specifier, OK);

    parse_opt(parse_enum_specifier, OK);

    parse_opt(parse_typedef_name, OK);

/* ERROR: */
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;

OK:
	return PARSE_OK;
}

static ParseReturn parse_type_qualifier(void)
{
    switch (yylex()) {
        case T_CONST:
        case T_VOLATILE: {
			parse_decl_state_add_decl_spec(lex_tok.type);

            return PARSE_OK;
        } break;
        default: {
            lex_setpos(yytext);

            return PARSE_ERROR;
        }       
    }
}

static ParseReturn parse_struct_or_union_specifier(void)
{
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    parse_required(parse_struct_or_union, ERROR);

    parse_opt(parse_identifier, HAS_IDENTIFIER);
    
    if (yylex() != '{') {
        goto ERROR;
    }

    parse_list_required(parse_struct_declaration, ERROR);
    
    if (yylex() != '}') {
        goto ERROR;
    }

HAS_IDENTIFIER:
	if (yylex() != '{') {
        lex_setpos(yytext);

        goto OK;
    }
    
    parse_list_required(parse_struct_declaration, ERROR);

    if (yylex() != '}') {
        goto ERROR;
    }

OK:
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_enum_specifier(void)
{
	char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    if (yylex() != T_ENUM) {
        goto ERROR;
    }

	parse_decl_state_add_decl_spec(T_ENUM);

    parse_opt(parse_identifier, HAS_IDENTIFIER);

    if (yylex() != '{') {
        goto ERROR;
    }

    parse_list_required(parse_enumerator, ERROR);
    
    if (yylex() != '}') {
        goto ERROR;
    }
    
    goto OK;

HAS_IDENTIFIER:
    if (yylex() != '{') {
        lex_setpos(yytext);

        goto OK;
    }
    
    parse_list_required(parse_enumerator, ERROR);

    if (yylex() != '}') {
        goto ERROR;
    }

OK:
	return PARSE_OK;

ERROR:
	lex_setpos(lex_pos_saved);

	return PARSE_ERROR;
}

static ParseReturn parse_typedef_name(void)
{
	Tok identifier_tok;
    char *lex_pos_saved;
    SymtblEntry *typedef_name_entry;

    lex_pos_saved = lex_tell();

	if (yylex() != T_IDENTIFIER) {
		goto ERROR;
	}
	
	identifier_tok = lex_tok;

	typedef_name_entry = symtbl_get_typedef_name_entry(parse_scope_current, &identifier_tok.literal.sv);

	if (typedef_name_entry == NULL) {
		goto ERROR;
	}

    if (typedef_name_entry->eclass != SYM_CLASS_TYPEDEF_NAME) {
    	goto ERROR;
    }

	/* TODO add semantical check somehow...
	if (type_specifier != 0) {
		goto ERROR;
	}
	*/

	decl_state.decl_spec.is_from_typedef = 1;

	decl_state.dtype = *typedef_name_entry->dtype;
	decl_state.dtype.storage_flags = decl_state.decl_spec.storage_flags;
	decl_state.dtype.qualifier_flags |= typedef_name_entry->dtype->qualifier_flags;
    
/* OK: */
    return PARSE_OK;

ERROR:
	lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_struct_or_union(void)
{
    switch (yylex()) {
        case T_STRUCT:
        case T_UNION: {
			parse_decl_state_add_decl_spec(lex_tok.type);

            return PARSE_OK;
        } break;
        default: {
            lex_setpos(yytext);

            return PARSE_ERROR;   
        }
    }
}

static ParseReturn parse_identifier(void)
{
	/* TODO REMOVE THIS FUNCTION, THE LOOKAHEAD IS TRIVIAL NOW */
    if (yylex() == T_IDENTIFIER) {
        return PARSE_OK;
    }

/* ERROR: */
    lex_setpos(yytext);

    return PARSE_ERROR;
}

static ParseReturn parse_struct_declaration(void)
{
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    parse_list_required(parse_specifier_qualifier, ERROR);

    parse_list_required(parse_struct_declarator, ERROR);

    if (yylex() == ';') {
        return PARSE_OK;
    }

ERROR:
	lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_specifier_qualifier(void)
{
    parse_opt(parse_type_specifier, OK);

    parse_opt(parse_type_qualifier, OK);

/* ERROR */
    return PARSE_ERROR;

OK:
    return PARSE_OK;
}

static ParseReturn parse_struct_declarator_list(void)
{
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

	while (1) {
		parse_required(parse_struct_declarator, ERROR);

		if (yylex() != ',') {
		    lex_setpos(yytext);
		    
		    goto OK;
		}
    }

OK:
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_struct_declarator(void)
{
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();
    
    parse_opt(parse_declarator, HAS_DECLARATOR);

    if (yylex() != ':') {
        goto ERROR;
    }
    
    parse_required(parse_constant_expression, ERROR);
    
    goto OK;

HAS_DECLARATOR:
    if (yylex() != ':') {
        lex_setpos(yytext);

        goto OK;
    }
    
    parse_required(parse_constant_expression, ERROR);

OK:
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_constant_expression(void)
{
    parse_required(parse_conditional_expression, ERROR);

    return PARSE_OK;

ERROR:
    return PARSE_ERROR;
}

static ParseReturn parse_pointer(void)
{
    char *lex_pos_saved;
    IRDataType *dtype_saved;

	dtype_saved = ir_dtype_assign(&decl_state.dtype);

	assert(dtype_saved != NULL);

    lex_pos_saved = lex_tell();
    
    if (yylex() != '*') {
        goto ERROR;
    }

    parse_decl_state_reset(decl_state.eclass);
    
    parse_decl_state_add_decl_spec('*');

    parse_list_opt(parse_type_qualifier);

	assert(dtype_saved != NULL);

	ir_dtype_wrap_pointer(&decl_state.dtype, dtype_saved);

    parse_opt(parse_pointer, OK);

OK:
    return PARSE_OK;

ERROR:
	free(dtype_saved);

    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_direct_declarator(void)
{
	char *lex_pos_saved = lex_tell();

	if (yylex() == T_IDENTIFIER) {
		parse_direct_declarator_id = lex_tok.literal.sv;
		
		goto NEXT;
    }

	lex_setpos(lex_pos_saved);

	if (yylex() != '(') {
		lex_setpos(lex_pos_saved);
		
		goto ERROR;
	}

	if (parse_direct_declarator() != PARSE_OK) {
		lex_setpos(lex_pos_saved);
		
		goto ERROR;
	}

	if (yylex() != ')') {
		lex_setpos(lex_pos_saved);
		
		goto ERROR;
	}

NEXT:
	while (1) {
		switch (yylex()) {
			case '(': {
				/* TODO CHECK REDECLARATION */
				Symtbl *function_scope = NULL;
				Symtbl *scope_saved = parse_scope_current;
				SymtblEntryClass eclass = decl_state.eclass;
				SymtblEntry *sym_entry = symtbl_add_function_entry(
							parse_scope_current, parse_direct_declarator_id, 
							ir_dtype_assign(&decl_state.dtype));

				assert(sym_entry != NULL);

				sym_entry->scope_self = parse_scope_current;

				function_scope = symtbl_create(parse_scope_global);

				assert(function_scope != NULL);

				parse_scope_current = function_scope;

				sym_entry->as.function.scope = function_scope;

				if (parse_parameter_type_list() == PARSE_ERROR) {
					assert(0 && "TODO not implemented without: ( parameter-type-list )");
				}

				if (yylex() != ')') {
					lex_setpos(lex_pos_saved);
					
					goto ERROR;
				}

				if (eclass != SYM_CLASS_FUNCTION) {
					parse_scope_current = scope_saved;
				} else {
					/* ?? TODO QUICK FIX, BUT DOES NOT WORK FOR FORWARDDECELS */
					parse_function_entry = sym_entry;
					ir_ctx->function_return_type = sym_entry->dtype;
				}
			} goto OK;
			case '[': {
				/* TODO CHECK REDECLARATION */
				if (parse_constant_expression() == PARSE_OK) {
					assert(0 && "TODO not implemented with: [ constant-expression ]");
				}

				if (yylex() != ']') {
					lex_setpos(lex_pos_saved);
					
					goto ERROR;
				}
			} break;
			default: {
				/* TODO CHECK REDECLARATION */
				lex_setpos(yytext);

				switch(decl_state.eclass) {
					case SYM_CLASS_OBJECT: {
						SymtblEntry *sym_entry = symtbl_add_object_entry(
							parse_scope_current, parse_direct_declarator_id, 
							ir_dtype_assign(&decl_state.dtype));

						assert(sym_entry != NULL);

						sym_entry->scope_self = parse_scope_current;
					} break;
					case SYM_CLASS_TYPEDEF_NAME: {
						SymtblEntry *sym_entry = symtbl_add_typedef_name_entry(
							parse_scope_current, parse_direct_declarator_id, 
							ir_dtype_assign(&decl_state.dtype));

						assert(sym_entry != NULL);

						sym_entry->scope_self = parse_scope_current;
					} break;
					case SYM_CLASS_TAG_OF_STRUCT: {
						// TODO push SymtblEntry on Stack for use of SYM_CLASS_STRUCT_UNION_MEMBER
						assert(0 && "TODO NOT IMPLEMENTED with (SYM_CLASS_TAG_OF_STRUCT)");
					} break;
					case SYM_CLASS_UNION: {
						// TODO push SymtblEntry on Stack for use of SYM_CLASS_STRUCT_UNION_MEMBER
						assert(0 && "TODO NOT IMPLEMENTED with (SYM_CLASS_UNION)");
					} break;
					case SYM_CLASS_ENUMERATION: {
						// TODO maybe push SymtblEntry on Stack for use of SYM_CLASS_ENUM_CONSTANT
						assert(0 && "TODO NOT IMPLEMENTED with (SYM_CLASS_ENUMERATION)");
					} break;
					default:
						goto ERROR;
				}
			} goto OK;
		}
	}

OK:
	return PARSE_OK;
	
ERROR:
	lex_setpos(lex_pos_saved);

	return PARSE_ERROR;
}

static ParseReturn parse_parameter_type_list(void)
{
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    parse_required(parse_parameter_list, ERROR);
    
    if (yylex() == ',') {
        if (yylex() != T_DOT_DOT_DOT) {
            goto ERROR;
        }
        
        assert(0 && "TODO NOT IMPLEMENTED: add ... as IR_TYPE_NONE maybe so argc count grows");
    } else {
        lex_setpos(yytext);
    }

/* OK: */
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_expression(void)
{
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

	while (1) {
		parse_required(parse_assignment_expression, ERROR);

		if (yylex() != ',') {
		    lex_setpos(yytext);

		    goto OK;
		}
	}

OK:
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_conditional_expression(void)
{
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    parse_required(parse_logical_or_expression, ERROR);

    if (yylex() != '?') {
        lex_setpos(yytext);

        goto OK;
    }

    parse_required(parse_expression, ERROR);
    
    if (yylex() != ':') {
        goto ERROR;
    }
    
    parse_required(parse_conditional_expression, ERROR);

OK:
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_logical_or_expression(void)
{
	const IRDataType *logical_or_dtype = ir_dtype_from_primitive(
    	codegen_get_primitive_data_type(IR_GENERIC_INT),
    	IR_QUALIFIER_FLAG_NONE, IR_STORAGE_FLAG_NONE);
	IRSSAEnt *dssa;
	size_t logical_label_end;
    size_t logical_label_true;
    int has_operator = 0;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

	while (1) {
		parse_required(parse_logical_and_expression, ERROR);

		if (yylex() == T_LOGICAL_OR) {
			if (has_operator == 0) {
				logical_label_end = ir_ctx->label_tmp++;
				logical_label_true = ir_ctx->label_tmp++;
			}

			has_operator = 1;
		}
		
		if (has_operator == 1) {
			ir_emit(IR_OC_JMP_NOT_ZERO, ir_ssa_latest()->dtype, ir_ssa_from_num(logical_label_true), ir_ssa_latest(), NULL);
		}
		
		if (lex_tok.type != T_LOGICAL_OR) {
			lex_setpos(yytext);

			goto OK;
		}
	}

OK:
	if (has_operator == 0) {
		return PARSE_OK;
	}

	/* TAIL CODE */
	dssa = ir_ssa_default(logical_or_dtype);

	/* DEFAULT IS FALSE */
	ir_emit(IR_OC_IMM, logical_or_dtype, dssa, ir_ssa_from_literal(ir_literal_from_lu(0),logical_or_dtype), NULL);
    ir_emit(IR_OC_JMP, NULL, ir_ssa_from_num(logical_label_end), NULL, NULL);

    /* LABEL TRUE */
    ir_emit(IR_OC_LABEL, NULL, ir_ssa_from_num(logical_label_true), NULL, NULL);
    ir_emit(IR_OC_IMM, logical_or_dtype, dssa, ir_ssa_from_literal(ir_literal_from_lu(1), logical_or_dtype), NULL);

    /* LABEL END */
    ir_emit(IR_OC_LABEL, NULL, ir_ssa_from_num(logical_label_end), NULL, NULL);

    ir_ctx->ssa_latest = dssa;

    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_logical_and_expression(void)
{
    const IRDataType *logical_and_dtype = ir_dtype_from_primitive(
		codegen_get_primitive_data_type(IR_GENERIC_INT),
		IR_QUALIFIER_FLAG_NONE, IR_STORAGE_FLAG_NONE);
	IRSSAEnt *dssa;
	size_t logical_label_end;
    size_t logical_label_false;
    int has_operator = 0;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

	while (1) {
		parse_required(parse_inclusive_or_expression, ERROR);

		if (yylex() == T_LOGICAL_AND) {
			if (has_operator == 0) {
				logical_label_end = ir_ctx->label_tmp++;
				logical_label_false = ir_ctx->label_tmp++;
			}

			has_operator = 1;
		}
		
		if (has_operator == 1) {
			ir_emit(IR_OC_JMP_ZERO, ir_ssa_latest()->dtype, ir_ssa_from_num(logical_label_false), ir_ssa_latest(), NULL);
		}
		
		if (lex_tok.type != T_LOGICAL_AND) {
			lex_setpos(yytext);

			goto OK;
		}
	}

OK:
	if (has_operator == 0) {
		return PARSE_OK;
	}

	/* TAIL CODE */
	dssa = ir_ssa_default(logical_and_dtype);

	/* DEFAULT IS TRUE */
	ir_emit(IR_OC_IMM, logical_and_dtype, dssa, ir_ssa_from_literal(ir_literal_from_lu(1),logical_and_dtype), NULL);
    ir_emit(IR_OC_JMP, NULL, ir_ssa_from_num(logical_label_end), NULL, NULL);

    /* LABEL FALSE */
    ir_emit(IR_OC_LABEL, NULL, ir_ssa_from_num(logical_label_false), NULL, NULL);
    ir_emit(IR_OC_IMM, logical_and_dtype, dssa, ir_ssa_from_literal(ir_literal_from_lu(0), logical_and_dtype), NULL);

    /* LABEL END */
    ir_emit(IR_OC_LABEL, NULL, ir_ssa_from_num(logical_label_end), NULL, NULL);

    ir_ctx->ssa_latest = dssa;

    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_inclusive_or_expression(void)
{
	IROpCode op;
	IRSSAEnt *lhs;
    char *lex_pos_saved;
    
    lex_pos_saved = lex_tell();

    parse_required(parse_exclusive_or_expression, ERROR);

	while (1) {
		lhs = ir_ssa_latest();
		
       switch (yylex()) {
            case '|': op = IR_OC_OR; break;
            default: {
                lex_setpos(yytext);

                goto OK;
            }
        }
        
        parse_required(parse_exclusive_or_expression, ERROR);

    	ir_emit(op, NULL, NULL, lhs, ir_ssa_latest());
    }

OK:
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_exclusive_or_expression(void)
{
	IROpCode op;
	IRSSAEnt *lhs;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    parse_required(parse_and_expression, ERROR);

	while (1) {
    	lhs = ir_ssa_latest();

       switch (yylex()) {
            case '^': op = IR_OC_XOR; break;
            default: {
                lex_setpos(yytext);

                goto OK;
            }
        }

        parse_required(parse_and_expression, ERROR);

		ir_emit(op, NULL, NULL, lhs, ir_ssa_latest());
    }

OK:
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_and_expression(void)
{
	IROpCode op;
	IRSSAEnt *lhs;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    parse_required(parse_equality_expression, ERROR);

	while (1) {
    	lhs = ir_ssa_latest();

       switch (yylex()) {
            case '&': op = IR_OC_AND; break;
            default: {
                lex_setpos(yytext);

                goto OK;
            }
        }

        parse_required(parse_equality_expression, ERROR);
        
        ir_emit(op, NULL, NULL, lhs, ir_ssa_latest());
    }

OK:
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_equality_expression(void)
{
	IROpCode op;
	IRSSAEnt *lhs;
    char *lex_pos_saved;
    const IRDataType *equality_dtype = ir_dtype_from_primitive(
    	codegen_get_primitive_data_type(IR_GENERIC_INT),
    	IR_QUALIFIER_FLAG_NONE, IR_STORAGE_FLAG_NONE);

    lex_pos_saved = lex_tell();
    
    parse_required(parse_relational_expression, ERROR);

	while (1) {
    	lhs = ir_ssa_latest();

        switch (yylex()) {
            case T_EQUAL_TO: op = IR_OC_EQ; break;
        	case T_NOT_EQUAL_TO: op = IR_OC_NEQ; break;
            default: {
                lex_setpos(yytext);

                goto OK;
            }
        }
        
        parse_required(parse_relational_expression, ERROR);

		ir_emit(op, equality_dtype, NULL, lhs, ir_ssa_latest());
    }

OK:
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_relational_expression(void)
{
	IROpCode op;
	IRSSAEnt *lhs;
    char *lex_pos_saved;
    const IRDataType *relational_dtype = ir_dtype_from_primitive(
    	codegen_get_primitive_data_type(IR_GENERIC_INT),
    	IR_QUALIFIER_FLAG_NONE, IR_STORAGE_FLAG_NONE);

    lex_pos_saved = lex_tell();

    parse_required(parse_shift_expression, ERROR);

	while (1) {
    	lhs = ir_ssa_latest();

       switch (yylex()) {
            case T_LESS_THAN: op = IR_OC_LT; break;
		    case T_GREATER_THAN: op = IR_OC_GT; break;
		    case T_LESS_THAN_OR_EQUAL_TO: op = IR_OC_LTE; break;
		    case T_GREATER_THAN_OR_EQUAL_TO: op = IR_OC_GTE; break;
            default: {
                lex_setpos(yytext);

                goto OK;
            }
        }
        
        parse_required(parse_shift_expression, ERROR);

		ir_emit(op, relational_dtype, NULL, lhs, ir_ssa_latest());
    }

OK:
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_shift_expression(void)
{
	IROpCode op;
	IRSSAEnt *lhs;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();
    
    parse_required(parse_additive_expression, ERROR);

	while (1) {
    	lhs = ir_ssa_latest();

       switch (yylex()) {
            case T_BITWISE_LEFTSHIFT: op = IR_OC_SAL; break;
        	case T_BITWISE_RIGHTSHIFT: op = IR_OC_SAR; break;
            default: {
                lex_setpos(yytext);

                goto OK;
            }
        }
        
        parse_required(parse_additive_expression, ERROR);

		ir_emit(op, NULL, NULL, lhs, ir_ssa_latest());
    }

OK:
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_additive_expression(void)
{
	IROpCode op;
	IRSSAEnt *lhs;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();
    
    parse_required(parse_multiplicative_expression, ERROR);
    
    while (1) {
		lhs = ir_ssa_latest();
    	
		switch (yylex()) {
			case T_PLUS: op = IR_OC_ADD; break;
        	case T_MINUS: op = IR_OC_SUB; break;
			default: {
				lex_setpos(yytext);

				goto OK;
			}
		}

		parse_required(parse_multiplicative_expression, ERROR);

		ir_emit(op, NULL, NULL, lhs, ir_ssa_latest());
    }

OK:
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_multiplicative_expression(void)
{
	IROpCode op;
	IRSSAEnt *lhs;
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();
    
    parse_required(parse_cast_expression, ERROR);

	while (1) {
    	lhs = ir_ssa_latest();

       switch (yylex()) {
            case '*': op = IR_OC_MUL; break;
		    case '/': op = IR_OC_DIV; break;
		    case '%':  op = IR_OC_MOD; break;
            default: {
                lex_setpos(yytext);

                goto OK;
            }
        }
        
        parse_required(parse_cast_expression, ERROR);
        
        ir_emit(op, NULL, NULL, lhs, ir_ssa_latest());
    }

OK:
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_cast_expression(void)
{
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    parse_opt(parse_unary_expression, OK);

    if (yylex() != '(') {
        goto ERROR;
    }

    parse_required(parse_type_name, ERROR); /* TODO dtype unused */

    if (yylex() != ')') {
        goto ERROR;
    }
    
    parse_required(parse_cast_expression, ERROR);

OK:
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_unary_expression(void)
{
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    parse_opt(parse_postfix_expression, OK);

    parse_required(parse_unary_operator, MORE_TOKENS);
    
    parse_required(parse_cast_expression, ERROR);
    
    return PARSE_OK;

MORE_TOKENS:
    switch(yylex()) {
        case T_INCREMENT:
        case T_DECREMENT: {
            parse_required(parse_unary_expression, ERROR);
        } break;
        case T_SIZEOF: {
            parse_opt(parse_unary_expression, OK);
            
            parse_required(parse_type_name, ERROR);
        } break;
        default: goto ERROR;
    }

OK:
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_type_name(void)
{
    parse_decl_state_reset(SYM_CLASS_UNKNOWN);

	parse_list_required(parse_specifier_qualifier, ERROR);

    parse_opt(parse_abstract_declarator, OK);

OK:
    return PARSE_OK;

ERROR:
    return PARSE_ERROR;
}

static ParseReturn parse_postfix_expression(void)
{
    char *lex_pos_saved;

    lex_pos_saved = lex_tell();

    parse_required(parse_primary_expression, ERROR);

    while (1) {
       switch(yylex()) {
            case '[': {
                parse_required(parse_expression, ERROR);
                
                if (yylex() != ']') {
                    goto ERROR;
                }
            } break;
            case '(': {
                parse_list_opt(parse_argument_expression_list);
                
                if (yylex() != ')') {
                    goto ERROR;
                }
            } break;
            case '.':
            case T_ARROW: {
                parse_required(parse_identifier, ERROR);
            } break;
            case T_INCREMENT:
            case T_DECREMENT: {
                
            } break;
            default: {
                lex_setpos(yytext);
                
                goto OK;
            }
        }
    }

OK:
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_unary_operator(void)
{
    switch (yylex()) {
        case '&':
        case '*':
        case '+':
        case '-':
        case '~':
        case '!': {
            goto OK;
        }
        default: goto ERROR;
    }

OK:
	return PARSE_OK;

ERROR:
    lex_setpos(yytext);

	return PARSE_ERROR;
}

static ParseReturn parse_primary_expression(void)
{
	char *lex_pos_saved;

    lex_pos_saved = lex_tell();

	/* HAS TO BE BEFORE IDENTIFIER PRODUCTION BECAUSE
	SEMANTICAL ERROR FOR UNDECLARED IDENTIFIER */
	parse_opt(parse_constant, OK);

    if (yylex() == T_IDENTIFIER) {
    	/* TODO */
		/* TODO MAYBE JUST SET ssa_latest */
		/* TODO SYM ENTRY SHOULD BE AN SSA_ENT */
		SymtblEntry *object_entry;

		object_entry = symtbl_get_object_entry(parse_scope_current, &lex_tok.literal.sv);

		assert(object_entry != NULL && "semantical error undeclared identifier");

		goto OK;
    } else {
    	lex_setpos(yytext);
    }

    parse_opt(parse_string, OK);

    if (yylex() != '(') {
        goto ERROR;
    }

    parse_required(parse_expression, ERROR);

    if (yylex() != ')') {
        goto ERROR;
    }

OK:
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_argument_expression_list(void)
{
	char *lex_pos_saved;

	lex_pos_saved = lex_tell();

	while (1) {
		parse_required(parse_assignment_expression, ERROR);

		if (yylex() != ',') {
		    lex_setpos(yytext);
		    
		    goto OK;
		}
	}

OK:
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_assignment_expression(void)
{
    char *lex_pos_saved;

	lex_pos_saved = lex_tell();

    parse_required(parse_unary_expression, NEXT_CONDITIONAL_EXPRESSION);

    parse_required(parse_assignment_operator, NEXT_CONDITIONAL_EXPRESSION_REMOVE_PREV);

    parse_required(parse_assignment_expression, ERROR);

	goto OK;

NEXT_CONDITIONAL_EXPRESSION_REMOVE_PREV:

NEXT_CONDITIONAL_EXPRESSION:
	lex_setpos(lex_pos_saved);
	
	parse_required(parse_conditional_expression, ERROR);

OK:
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_constant(void)
{
	IRPrimitiveDataType p;
	IRDataType *dtype;

    switch(yylex()) {
    	// TODO: case enumeration-constant
    	case T_INTEGER_CONSTANT: {
    		p = codegen_get_primitive_data_type(IR_GENERIC_INT); /* TODO */
    		
		} break;
    	case T_FLOATING_CONSTANT: {
    		p = codegen_get_primitive_data_type(IR_GENERIC_FLOAT); /* TODO */
		} break;
        case T_CHARACTER_CONSTANT: {
        	p = codegen_get_primitive_data_type(IR_GENERIC_CHAR);
        } break;
        default: goto ERROR;
    }
    
/* OK: */
	dtype = ir_dtype_from_primitive(p, IR_QUALIFIER_FLAG_NONE, IR_STORAGE_FLAG_NONE);

	ir_emit(IR_OC_IMM, dtype, NULL, ir_ssa_from_literal(lex_tok.literal, dtype), NULL);

    return PARSE_OK;
    
ERROR:
	lex_setpos(yytext);
	
	return PARSE_ERROR;
}

static ParseReturn parse_string(void)
{ 
	/* TODO REMOVE THIS FUNCTION, THE LOOKAHEAD IS TRIVIAL NOW */
    if (yylex() == T_STRING) {
    	return PARSE_OK;
    }

/* ERROR: */
    lex_setpos(yytext);

    return PARSE_ERROR;
}

static ParseReturn parse_assignment_operator(void)
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
            return PARSE_OK;
        }
        default: {
        	lex_setpos(yytext);

        	return PARSE_ERROR;
        }
    }
}

static ParseReturn parse_abstract_declarator(void)
{
    parse_opt(parse_pointer, NEXT_AFTER_POINTER);

    parse_required(parse_direct_abstract_declarator, ERROR);

    goto OK;

NEXT_AFTER_POINTER:
    parse_opt(parse_direct_abstract_declarator, OK);

OK:
    return PARSE_OK;

ERROR:
    return PARSE_ERROR;
}

static ParseReturn parse_parameter_list(void)
{
	size_t node_count = 0;

	while (1) {
		parse_required(parse_parameter_declaration, CHECK_ERROR);

		++node_count;

		if (yylex() != ',') {
		    lex_setpos(yytext);
		    
		    goto OK;
		}
    }

CHECK_ERROR:
	if (node_count == 0) {
		goto ERROR;
	}

	lex_setpos(yytext); /* RESTORES LAST ',' */

OK:
    return PARSE_OK;

ERROR:
    return PARSE_ERROR;
}

static ParseReturn parse_parameter_declaration(void)
{
    parse_decl_state_reset(SYM_CLASS_OBJECT);

    parse_list_required(parse_declaration_specifier, ERROR);
    
    parse_opt(parse_declarator, OK);
    
    parse_opt(parse_abstract_declarator, OK);

OK:
    return PARSE_OK;

ERROR:
    return PARSE_ERROR;
}

static ParseReturn parse_direct_abstract_declarator(void)
{
	size_t node_count = 0;
	char *lex_pos_saved;

	lex_pos_saved = lex_tell();

	if (yylex() != '(') {
		lex_setpos(lex_pos_saved);
		
		goto NEXT;
	}
	
	if (parse_abstract_declarator() == PARSE_ERROR) {
		lex_setpos(lex_pos_saved);
		
		goto NEXT;
	}
	
	if (yylex() != ')') {
		lex_setpos(lex_pos_saved);
		
		goto NEXT;
	}

	++node_count;
	
NEXT:
	while (1) {
		lex_pos_saved = lex_tell();

		switch (yylex()) {
			case '(': {
				if (parse_parameter_type_list() == PARSE_OK) {
					assert(0 && "TODO not implemented with: ( parameter-type-list )");
				}

				if (yylex() != ')') {
					goto ERROR;
				}

				++node_count;
			} break;
			case '[': {
				if (parse_constant_expression() == PARSE_OK) {
					assert(0 && "TODO not implemented with: [ constant-expression ]");
				}
				
				if (yylex() != ']') {
					goto ERROR;
				}

				++node_count;
			} break;
			default: goto CHECK_ERROR;
		}
	}

CHECK_ERROR:
	if (node_count == 0) {
		goto ERROR;
	}

/* OK: */
	return PARSE_OK;

ERROR:
	lex_setpos(lex_pos_saved);

	return PARSE_ERROR;
}

static ParseReturn parse_enumerator_list(void)
{
    char *lex_pos_saved;

	lex_pos_saved = lex_tell();

	while (1) {
		parse_required(parse_enumerator, ERROR);

		if (yylex() != ',') {
		    lex_setpos(yytext);

		    goto OK;
		}
		
	}

OK:
	return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_enumerator(void)
{
    char *lex_pos_saved;

	lex_pos_saved = lex_tell();

    parse_required(parse_identifier, ERROR);

    if (yylex() != '=') {
        lex_setpos(yytext);

        goto OK;
    }

    parse_required(parse_constant_expression, ERROR);

OK:
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_init_declarator_list(void)
{
	char *lex_pos_saved;

	lex_pos_saved = lex_tell();

	while (1) {
		parse_required(parse_init_declarator, ERROR);
		
		if (yylex() != ',') {
			lex_setpos(yytext);

			goto OK;
		}
    }

OK:
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_init_declarator(void)
{
    char *lex_pos_saved;

	lex_pos_saved = lex_tell();

    parse_required(parse_declarator, ERROR);

    if (yylex() != '=') {
        lex_setpos(yytext);

        goto OK;
    }

    parse_required(parse_initializer, ERROR);

OK:
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_initializer(void)
{
    char *lex_pos_saved;

	lex_pos_saved = lex_tell();

    parse_opt(parse_assignment_expression, OK);

    if (yylex() != '{') {
        goto ERROR;
    }

    parse_required(parse_initializer_list, ERROR);

    if (yylex() != ',') {
        lex_setpos(yytext);
    }

    if (yylex() != '}') {
        goto ERROR;
    }

OK:
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_initializer_list(void)
{
	size_t node_count = 0;

	while (1) {
		parse_required(parse_initializer, CHECK_ERROR);

		++node_count;

		if (yylex() != ',') {
		    lex_setpos(yytext);
		    
		    goto OK;
		}
    }

CHECK_ERROR:
	if (node_count == 0) {
		goto ERROR;
	}

	lex_setpos(yytext); /* RESTORES LAST ',' */

OK:
    return PARSE_OK;

ERROR:
    return PARSE_ERROR;
}

static ParseReturn parse_statement(void)
{
	parse_opt(parse_labeled_statement, OK);

    parse_opt(parse_expression_statement, OK);

    parse_opt(parse_compound_statement, OK);

    parse_opt(parse_selection_statement, OK);

    parse_opt(parse_iteration_statement, OK);

    parse_opt(parse_jump_statement, OK);

/* ERROR: */
    return PARSE_ERROR;

OK:
	return PARSE_OK;
}

static ParseReturn parse_labeled_statement(void)
{
    char *lex_pos_saved;

	lex_pos_saved = lex_tell();

    switch (yylex()) {
    	case T_IDENTIFIER: {
    		SymtblEntry *label_entry;

    		if (yylex() != ':') {
				goto ERROR;
			}
			
			label_entry = symtbl_get_label_entry(parse_function_entry->as.function.scope, &lex_tok.literal.sv);
		
			if (label_entry == NULL) {
				label_entry = symtbl_add_label_entry(parse_function_entry->as.function.scope, lex_tok.literal.sv);
				
				assert(label_entry != NULL);
				
				label_entry->state = SYM_STATE_DEFINITION;
				label_entry->as.label.id = ir_ctx->label_tmp++;
			} else if (label_entry->state == SYM_STATE_DEFINITION) {
				assert(0 && "TODO error: redefiniton of label \'...\'");
			} else {
				label_entry->state = SYM_STATE_DEFINITION;
			}

			ir_emit(IR_OC_LABEL, NULL, ir_ssa_from_num(label_entry->as.label.id), NULL, NULL);

			parse_required(parse_statement, ERROR);
		} break;
		case T_DEFAULT: {
			if (yylex() != ':') {
				goto ERROR;
			}
			
			parse_required(parse_statement, ERROR);
		} break;
		case T_CASE: {
		    parse_required(parse_constant_expression, ERROR);
		    
		    if (yylex() != ':') {
				goto ERROR;
			}
			
			parse_required(parse_statement, ERROR);
		} break;
		default: goto ERROR;
	}

    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_expression_statement(void)
{
    char *lex_pos_saved;

	lex_pos_saved = lex_tell();

    parse_opt(parse_expression, next);

    next: {
        if (yylex() != ';') {
            goto ERROR;
        }
    }

    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_selection_statement(void)
{
	char *lex_pos_saved;

	lex_pos_saved = lex_tell();

    switch (yylex()) {
        case T_IF: {
        	const size_t before_label_select_begin = ir_ctx->label_select_begin;
			const size_t before_label_select_end = ir_ctx->label_select_end;

			ir_ctx->label_select_begin = ir_ctx->label_tmp++;
			ir_ctx->label_select_end = ir_ctx->label_tmp++;
			
			if (yylex() != '(') {
				goto ERROR;
			}

			parse_required(parse_expression, ERROR);

			if (yylex() != ')') {
				goto ERROR;
			}

			/* TODO SEMANTICAL CHECK ON ir_ssa_latest()->dtype MUST BE ARITHMETIC OR POINTER TYPE */
			ir_emit(IR_OC_JMP_ZERO, ir_ssa_latest()->dtype, ir_ssa_from_num(ir_ctx->label_select_begin), ir_ssa_latest(), NULL);

    		parse_required(parse_statement, ERROR);

    		/* JMP END */
			ir_emit(IR_OC_JMP, NULL, ir_ssa_from_num(ir_ctx->label_select_end), NULL, NULL);

    		/* LABEL ELSE */
			ir_emit(IR_OC_LABEL, NULL, ir_ssa_from_num(ir_ctx->label_select_begin), NULL, NULL);

			if (yylex() == T_ELSE) {
				parse_required(parse_statement, ERROR);
				
				ir_emit(IR_OC_JMP, NULL, ir_ssa_from_num(ir_ctx->label_select_end), NULL, NULL);
			} else {
				lex_setpos(yytext);
			}

			/* LABEL END */
			ir_emit(IR_OC_LABEL, NULL, ir_ssa_from_num(ir_ctx->label_select_end), NULL, NULL);

			ir_ctx->label_select_begin = before_label_select_begin;
			ir_ctx->label_select_end = before_label_select_end;
        } break;
        case T_SWITCH: {
            assert(0 && "TODO not implemented with: T_SWITCH");
        } break;
        default: goto ERROR;
    }

/* OK: */
    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}

static ParseReturn parse_iteration_statement(void)
{
	const size_t before_label_iter_begin = ir_ctx->label_iter_begin;
	const size_t before_label_iter_end = ir_ctx->label_iter_end;
	const size_t label_tmp_saved = ir_ctx->label_tmp;
	char *lex_pos_saved;

	ir_ctx->label_iter_begin = ir_ctx->label_tmp++;
	ir_ctx->label_iter_end = ir_ctx->label_tmp++;

	lex_pos_saved = lex_tell();

    switch(yylex()) {
        case T_WHILE: {
        	if (yylex() != '(') {
                goto ERROR;
            }
            
            /* LABEL BEGIN */
			ir_emit(IR_OC_LABEL, NULL, ir_ssa_from_num(ir_ctx->label_iter_begin), NULL, NULL);

			/* TODO SEMANTICAL CHECK ON ir_ssa_latest()->dtype MUST BE ARITHMETIC OR POINTER TYPE */
            parse_required(parse_expression, ERROR);

			/* JMP END */
			ir_emit(IR_OC_JMP_ZERO, ir_ssa_latest()->dtype, ir_ssa_from_num(ir_ctx->label_iter_end), ir_ssa_latest(), NULL);

            if (yylex() != ')') {
                goto ERROR;
            }

            parse_required(parse_statement, ERROR);

			/* JMP BEGIN */
            ir_emit(IR_OC_JMP, NULL, ir_ssa_from_num(ir_ctx->label_iter_begin), NULL, NULL);
        } break;
        case T_DO: {
        	/* LABEL BEGIN */
			ir_emit(IR_OC_LABEL, NULL, ir_ssa_from_num(ir_ctx->label_iter_begin), NULL, NULL);

            parse_required(parse_statement, ERROR);

            if (yylex() != T_WHILE) {
                goto ERROR;
            }

            if (yylex() != '(') {
                goto ERROR;
            }

			/* TODO SEMANTICAL CHECK ON ir_ssa_latest()->dtype MUST BE ARITHMETIC OR POINTER TYPE */
            parse_required(parse_expression, ERROR);

			/* JMP BEGIN */
			ir_emit(IR_OC_JMP_NOT_ZERO, ir_ssa_latest()->dtype, ir_ssa_from_num(ir_ctx->label_iter_begin), ir_ssa_latest(), NULL);

            if (yylex() != ')') {
                goto ERROR;
            }

            if (yylex() != ';') {
                goto ERROR;
            }
        } break;
        case T_FOR: {
        	/* NOTE:
        	"FOR NOW" (haha) I keep it simple and generate the IR in order. With
        	this approach I have unnecessarily more labels and jmps. But you could
        	argue that an optimizer will fix the order anyway.
        	*/
        	const size_t for_label_begin = ir_ctx->label_tmp++;
        	const size_t for_label_stmt = ir_ctx->label_tmp++;

            if (yylex() != '(') {
                goto ERROR;
            }

            (void)parse_expression();

            if (yylex() != ';') {
                goto ERROR;
            }

			/* LABEL FOR BEGIN */
			ir_emit(IR_OC_LABEL, NULL, ir_ssa_from_num(for_label_begin), NULL, NULL);

			if (parse_expression() == PARSE_OK) {
				/* TODO SEMANTICAL CHECK ON ir_ssa_latest()->dtype MUST BE ARITHMETIC OR POINTER TYPE */
				ir_emit(IR_OC_JMP_ZERO, ir_ssa_latest()->dtype, ir_ssa_from_num(ir_ctx->label_iter_end), ir_ssa_latest(), NULL);
			}

			/* JMP STMT */
			ir_emit(IR_OC_LABEL, NULL, ir_ssa_from_num(for_label_stmt), NULL, NULL);

            if (yylex() != ';') {
                goto ERROR;
            }

			/* LABEL EXPR_LAST */
			ir_emit(IR_OC_LABEL, NULL, ir_ssa_from_num(ir_ctx->label_iter_begin), NULL, NULL);

            (void)parse_expression();

            /* JMP FOR BEGIN */
            ir_emit(IR_OC_LABEL, NULL, ir_ssa_from_num(for_label_begin), NULL, NULL);

            if (yylex() != ')') {
                goto ERROR;
            }

			/* LABEL STMT */
			ir_emit(IR_OC_LABEL, NULL, ir_ssa_from_num(for_label_stmt), NULL, NULL);

            parse_required(parse_statement, ERROR);

            /* JMP EXPR_LAST */
			ir_emit(IR_OC_JMP, NULL, ir_ssa_from_num(ir_ctx->label_iter_begin), NULL, NULL);
        } break;
        default: goto ERROR;
    }

/* OK: */
	/* LABEL END */
	ir_emit(IR_OC_LABEL, NULL, ir_ssa_from_num(ir_ctx->label_iter_end), NULL, NULL);
	
	ir_ctx->label_iter_begin = before_label_iter_begin;
	ir_ctx->label_iter_end = before_label_iter_end;

    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

	ir_ctx->label_iter_begin = before_label_iter_begin;
	ir_ctx->label_iter_end = before_label_iter_end;
	ir_ctx->label_tmp = label_tmp_saved;

    return PARSE_ERROR;
}

static ParseReturn parse_jump_statement(void)
{
    char *lex_pos_saved;

	lex_pos_saved = lex_tell();

    switch(yylex()) {
        case T_GOTO: {
			SymtblEntry *label_entry;

            if (yylex() != T_IDENTIFIER) {
            	goto ERROR;
            }
			
			label_entry = symtbl_get_label_entry(parse_function_entry->as.function.scope, &lex_tok.literal.sv);
		
			if (label_entry == NULL) {
				label_entry = symtbl_add_label_entry(parse_function_entry->as.function.scope, lex_tok.literal.sv);
				
				assert(label_entry != NULL);
				
				label_entry->as.label.id = ir_ctx->label_tmp++;
				label_entry->state = SYM_STATE_DECLARATION;
			}

			ir_emit(IR_OC_JMP, NULL, ir_ssa_from_num(label_entry->as.label.id), NULL, NULL);

        } break;
        case T_RETURN: {
        	if (parse_expression() == PARSE_OK) {
            	ir_emit(IR_OC_RET, NULL, NULL, NULL, NULL);
            }

			ir_emit(IR_OC_JMP, NULL, ir_ssa_from_num(ir_ctx->label_func_end), NULL, NULL);
        } break;
        case T_CONTINUE: {
			ir_emit(IR_OC_JMP, NULL, ir_ssa_from_num(ir_ctx->label_iter_begin), NULL, NULL);
		} break;
		case T_BREAK: {
			ir_emit(IR_OC_JMP, NULL, ir_ssa_from_num(ir_ctx->label_iter_end), NULL, NULL);
		} break;
        default: goto ERROR;
    }

    if (yylex() != ';') {
    	goto ERROR;
    }

    return PARSE_OK;

ERROR:
    lex_setpos(lex_pos_saved);

    return PARSE_ERROR;
}
