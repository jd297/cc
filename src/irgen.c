#include <assert.h>
#include <stdlib.h>

#define JD297_CC_IRGEN_IMPLEMENTATION
#include "irgen.h"
#include "ir.h"
#include "parse.h"
#include "tok.h"

void irgen_c_run(IR_CTX *ctx, ParseTreeNode *translation_unit)
{
	irgen_c_translation_unit(ctx, translation_unit);
}

static void irgen_c_translation_unit(IR_CTX *ctx, ParseTreeNode *this_node)
{
	for (size_t i = 0; i < this_node->num; ++i) {
		irgen_c_external_declaration(ctx, this_node->elements[i]);
	}
}

static void irgen_c_external_declaration(IR_CTX *ctx, ParseTreeNode *this_node)
{
	ParseTreeNode *node = this_node->elements[0];

	switch (node->type) {
		case FUNCTION_DEFINITION: {
			irgen_c_function_definition(ctx, node);
		} break;
		case DECLARATION: {
			irgen_c_declaration(ctx, node);
		} break;
		default: {
			assert(0 && "NOT REACHABLE");
		}
	}
}

static void irgen_c_function_definition(IR_CTX *ctx, ParseTreeNode *this_node)
{
	ctx->function_return_type = this_node->dtype;

	ir_emit(ctx, IR_OC_FUNC_BEGIN, this_node->dtype, ir_ssa_from_view(ctx, this_node->sym->id), ir_ssa_from_num(ctx, 0), NULL);

	ctx->label_func_end = ctx->label_tmp++;

	for (size_t i = 0; i < this_node->num; ++i) {
		ParseTreeNode *node = this_node->elements[i];

		switch (node->type) {
			case DECLARATION_SPECIFIER: break;

			case DECLARATOR: break; // TODO put in params ?

			case DECLARATION: {
				assert(0 && "TODO not implemented with: (DECLARATION) (old-style)");
			}break;

			case COMPOUND_STATEMENT: {
				irgen_c_compound_statement(ctx, node);
			} break;

			default:
				assert(0 && "NOT REACHABLE");
		}
	}

	ir_emit(ctx, IR_OC_FUNC_END, NULL, ir_ssa_from_num(ctx, ctx->label_func_end), ir_ssa_from_num(ctx, 0), NULL);
}

static void irgen_c_declaration(IR_CTX *ctx, ParseTreeNode *this_node)
{
	if (this_node->num != 2) {
		// TODO: fprintf(stderr, "<file>:<line>:<col>: warning: declaration does not declare anything [-Wmissing-declarations]\n");
		return;
	}
	
	irgen_c_init_declarator_list(ctx, this_node->elements[1]);
}

static void irgen_c_declaration_specifier(IR_CTX *ctx, ParseTreeNode *this_node)
{
        (void) ctx;
        (void) this_node;

        assert(0 && "TODO not implemented");
}

static void irgen_c_declarator(IR_CTX *ctx, ParseTreeNode *this_node)
{
        (void) ctx;
        (void) this_node;

        assert(0 && "TODO not implemented");
}

static void irgen_c_compound_statement(IR_CTX *ctx, ParseTreeNode *this_node)
{
	for (size_t i = 0; i < this_node->num; ++i) {
		ParseTreeNode *node = this_node->elements[i];
		
		switch (node->type) {
			case DECLARATION: {
				irgen_c_declaration(ctx, node);
			} break;
			case STATEMENT: {
				irgen_c_statement(ctx, node);
			} break;
			default: {
				assert(0 && "NOT REACHABLE");
			}
		}
	}
}

static void irgen_c_storage_class_specifier(IR_CTX *ctx, ParseTreeNode *this_node)
{
        (void) ctx;
        (void) this_node;

        assert(0 && "TODO not implemented");
}

static void irgen_c_type_specifier(IR_CTX *ctx, ParseTreeNode *this_node)
{
        (void) ctx;
        (void) this_node;

        assert(0 && "TODO not implemented");
}

static void irgen_c_type_qualifier(IR_CTX *ctx, ParseTreeNode *this_node)
{
        (void) ctx;
        (void) this_node;

        assert(0 && "TODO not implemented");
}

static void irgen_c_struct_or_union_specifier(IR_CTX *ctx, ParseTreeNode *this_node)
{
        (void) ctx;
        (void) this_node;

        assert(0 && "TODO not implemented");
}

static void irgen_c_enum_specifier(IR_CTX *ctx, ParseTreeNode *this_node)
{
        (void) ctx;
        (void) this_node;

        assert(0 && "TODO not implemented");
}

static void irgen_c_typedef_name(IR_CTX *ctx, ParseTreeNode *this_node)
{
        (void) ctx;
        (void) this_node;

        assert(0 && "TODO not implemented");
}

static void irgen_c_struct_or_union(IR_CTX *ctx, ParseTreeNode *this_node)
{
        (void) ctx;
        (void) this_node;

        assert(0 && "TODO not implemented");
}

static void irgen_c_identifier(IR_CTX *ctx, ParseTreeNode *this_node)
{
        (void) ctx;
        (void) this_node;

        assert(0 && "TODO not implemented");
}

static void irgen_c_struct_declaration(IR_CTX *ctx, ParseTreeNode *this_node)
{
        (void) ctx;
        (void) this_node;

        assert(0 && "TODO not implemented");
}

static void irgen_c_specifier_qualifier(IR_CTX *ctx, ParseTreeNode *this_node)
{
        (void) ctx;
        (void) this_node;

        assert(0 && "TODO not implemented");
}

static void irgen_c_struct_declarator_list(IR_CTX *ctx, ParseTreeNode *this_node)
{
        (void) ctx;
        (void) this_node;

        assert(0 && "TODO not implemented");
}

static void irgen_c_struct_declarator(IR_CTX *ctx, ParseTreeNode *this_node)
{
        (void) ctx;
        (void) this_node;

        assert(0 && "TODO not implemented");
}

static void irgen_c_constant_expression(IR_CTX *ctx, ParseTreeNode *this_node)
{
        irgen_c_conditional_expression(ctx, this_node->elements[0]);
}

static void irgen_c_pointer(IR_CTX *ctx, ParseTreeNode *this_node)
{
        (void) ctx;
        (void) this_node;

        assert(0 && "TODO not implemented");
}

static void irgen_c_direct_declarator(IR_CTX *ctx, ParseTreeNode *this_node)
{
        (void) ctx;
        (void) this_node;

        assert(0 && "TODO not implemented");
}

static void irgen_c_parameter_type_list(IR_CTX *ctx, ParseTreeNode *this_node)
{
        (void) ctx;
        (void) this_node;

        assert(0 && "TODO not implemented");
}

static void irgen_c_conditional_expression(IR_CTX *ctx, ParseTreeNode *this_node)
{
        IRSSAEnt *dssa;
        
        assert(this_node->num == 1 || this_node->num == 3);

		ParseTreeNode *logical_or_expression = this_node->elements[0];

        irgen_c_logical_or_expression(ctx, logical_or_expression);
        
        if (this_node->num == 1) {
        	return;
        }
        
        const size_t logical_label_end = ctx->label_tmp++;
        const size_t logical_label_false = ctx->label_tmp++;

        dssa = ir_ssa_default(ctx);

        ir_emit(ctx, IR_OC_JMP_ZERO, logical_or_expression->dtype, ir_ssa_from_num(ctx, logical_label_false), ir_ssa_latest(ctx), NULL);

        irgen_c_expression(ctx, this_node->elements[1]);

		ir_emit(ctx, IR_OC_STORE, this_node->dtype, dssa, ir_ssa_latest(ctx), NULL);

        ir_emit(ctx, IR_OC_JMP, NULL, ir_ssa_from_num(ctx, logical_label_end), NULL, NULL);

		/* label false */
        ir_emit(ctx, IR_OC_LABEL, NULL, ir_ssa_from_num(ctx, logical_label_false), NULL, NULL);

        irgen_c_conditional_expression(ctx, this_node->elements[2]);

        ir_emit(ctx, IR_OC_STORE, this_node->dtype, dssa, ir_ssa_latest(ctx), NULL);

        /* label end */
        ir_emit(ctx, IR_OC_LABEL, NULL, ir_ssa_from_num(ctx, logical_label_end), NULL, NULL);

        ctx->ssa_latest = dssa;
}

static void irgen_c_logical_or_expression(IR_CTX *ctx, ParseTreeNode *this_node)
{
		ParseTreeNode *lnode;
		ParseTreeNode *rnode;
        IRSSAEnt *dssa;

        assert(this_node->num > 0);

        lnode = this_node->elements[0];

        if (this_node->num == 1) {
			irgen_c_logical_and_expression(ctx, lnode);

			return;
		}

		const size_t logical_label_end = ctx->label_tmp++;
        const size_t logical_label_true = ctx->label_tmp++;

        if (lnode->type == LOGICAL_OR_EXPRESSION) {
			irgen_c_logical_or_expression(ctx, lnode);
		} else {
			irgen_c_logical_and_expression(ctx, lnode);
		}

		ir_emit(ctx, IR_OC_JMP_NOT_ZERO, lnode->dtype, ir_ssa_from_num(ctx, logical_label_true), ir_ssa_latest(ctx), NULL);
		
		rnode = this_node->elements[1];

		irgen_c_logical_and_expression(ctx, rnode);

		ir_emit(ctx, IR_OC_JMP_NOT_ZERO, rnode->dtype, ir_ssa_from_num(ctx, logical_label_true), ir_ssa_latest(ctx), NULL);

		dssa = ir_ssa_default(ctx);

        ir_emit(ctx, IR_OC_IMM, this_node->dtype, dssa, ir_ssa_from_literal(ctx, ir_literal_from_lu(0)), NULL);

        ir_emit(ctx, IR_OC_JMP, NULL, ir_ssa_from_num(ctx, logical_label_end), NULL, NULL);
        
        /* label true */
        ir_emit(ctx, IR_OC_LABEL, NULL, ir_ssa_from_num(ctx, logical_label_true), NULL, NULL);

        ir_emit(ctx, IR_OC_IMM, this_node->dtype, dssa, ir_ssa_from_literal(ctx, ir_literal_from_lu(1)), NULL);
        
        /* label end */
        ir_emit(ctx, IR_OC_LABEL, NULL, ir_ssa_from_num(ctx, logical_label_end), NULL, NULL);

        ctx->ssa_latest = dssa;
}

static void irgen_c_expression(IR_CTX *ctx, ParseTreeNode *this_node)
{
		assert(this_node->num >= 1);

		for (size_t i = 0; i < this_node->num; ++i) {
			/*
			 * NOTE:
			 * the last assignment_expression is the result of an expression
			 */
			irgen_c_assignment_expression(ctx, this_node->elements[i]);
		}
}

static void irgen_c_logical_and_expression(IR_CTX *ctx, ParseTreeNode *this_node)
{
		ParseTreeNode *lnode;
		ParseTreeNode *rnode;
        IRSSAEnt *dssa;

        assert(this_node->num > 0);

        lnode = this_node->elements[0];

        if (this_node->num == 1) {
			irgen_c_inclusive_or_expression(ctx, lnode);

			return;
		}

		const size_t logical_label_end = ctx->label_tmp++;
        const size_t logical_label_false = ctx->label_tmp++;

        if (lnode->type == LOGICAL_AND_EXPRESSION) {
			irgen_c_logical_and_expression(ctx, lnode);
		} else {
			irgen_c_inclusive_or_expression(ctx, lnode);
		}

		ir_emit(ctx, IR_OC_JMP_ZERO, lnode->dtype, ir_ssa_from_num(ctx, logical_label_false), ir_ssa_latest(ctx), NULL);
		
		rnode = this_node->elements[1];

		irgen_c_inclusive_or_expression(ctx, rnode);

		ir_emit(ctx, IR_OC_JMP_ZERO, rnode->dtype, ir_ssa_from_num(ctx, logical_label_false), ir_ssa_latest(ctx), NULL);

		dssa = ir_ssa_default(ctx);

        ir_emit(ctx, IR_OC_IMM, this_node->dtype, dssa, ir_ssa_from_literal(ctx, ir_literal_from_lu(1)), NULL);

        ir_emit(ctx, IR_OC_JMP, NULL, ir_ssa_from_num(ctx, logical_label_end), NULL, NULL);
        
        /* label false */
        ir_emit(ctx, IR_OC_LABEL, NULL, ir_ssa_from_num(ctx, logical_label_false), NULL, NULL);

        ir_emit(ctx, IR_OC_IMM, this_node->dtype, dssa, ir_ssa_from_literal(ctx, ir_literal_from_lu(0)), NULL);
        
        /* label end */
        ir_emit(ctx, IR_OC_LABEL, NULL, ir_ssa_from_num(ctx, logical_label_end), NULL, NULL);

        ctx->ssa_latest = dssa;
}

static void irgen_c_inclusive_or_expression(IR_CTX *ctx, ParseTreeNode *this_node)
{
	ParseTreeNode *lnode;
	ParseTreeNode *rnode;
	IRSSAEnt *lssa, *rssa, *dssa;

	assert(this_node->num > 0);
	
	lnode = this_node->elements[0];

	if (this_node->num == 1) {
		irgen_c_exclusive_or_expression(ctx, lnode);

		return;
	}

	if (lnode->type == INCLUSIVE_OR_EXPRESSION) {
		irgen_c_inclusive_or_expression(ctx, lnode);
	} else {
		irgen_c_exclusive_or_expression(ctx, lnode);
    }

    lssa = ir_ssa_latest(ctx);
    
    rnode = this_node->elements[1];

    irgen_c_exclusive_or_expression(ctx, rnode);

	rssa = ir_ssa_latest(ctx);

    dssa = ir_ssa_default(ctx);

	ir_emit(ctx, IR_OC_OR, this_node->dtype, dssa, lssa, rssa);
}

static void irgen_c_exclusive_or_expression(IR_CTX *ctx, ParseTreeNode *this_node)
{
    ParseTreeNode *lnode;
	ParseTreeNode *rnode;
	IRSSAEnt *lssa, *rssa, *dssa;

	assert(this_node->num > 0);

	lnode = this_node->elements[0];

	if (this_node->num == 1) {
		irgen_c_and_expression(ctx, lnode);

		return;
	}

	if (lnode->type == EXCLUSIVE_OR_EXPRESSION) {
		irgen_c_exclusive_or_expression(ctx, lnode);
	} else {
		irgen_c_and_expression(ctx, lnode);
    }

    lssa = ir_ssa_latest(ctx);

	rnode = this_node->elements[1];

    irgen_c_and_expression(ctx, rnode);

	rssa = ir_ssa_latest(ctx);

    dssa = ir_ssa_default(ctx);

	ir_emit(ctx, IR_OC_XOR, this_node->dtype, dssa, lssa, rssa);
}

static void irgen_c_and_expression(IR_CTX *ctx, ParseTreeNode *this_node)
{
    ParseTreeNode *lnode;
	ParseTreeNode *rnode;
	IRSSAEnt *lssa, *rssa, *dssa;

	assert(this_node->num > 0);

	lnode = this_node->elements[0];

	if (this_node->num == 1) {
		irgen_c_equality_expression(ctx, lnode);

		return;
	}

	if (lnode->type == AND_EXPRESSION) {
		irgen_c_and_expression(ctx, lnode);
	} else {
		irgen_c_equality_expression(ctx, lnode);
    }

    lssa = ir_ssa_latest(ctx);

	rnode = this_node->elements[1];

    irgen_c_equality_expression(ctx, rnode);

	rssa = ir_ssa_latest(ctx);

    dssa = ir_ssa_default(ctx);

	ir_emit(ctx, IR_OC_AND, this_node->dtype, dssa, lssa, rssa);
}

static void irgen_c_equality_expression(IR_CTX *ctx, ParseTreeNode *this_node)
{
	ParseTreeNode *lnode;
	ParseTreeNode *rnode;
	IRSSAEnt *lssa, *rssa, *dssa;
	IROpCode op;

	assert(this_node->num > 0);
	
	lnode = this_node->elements[0];

	if (this_node->num == 1) {
		irgen_c_relational_expression(ctx, lnode);

		return;
	}

	if (lnode->type == EQUALITY_EXPRESSION) {
		irgen_c_equality_expression(ctx, lnode);
	} else {
		irgen_c_relational_expression(ctx, lnode);
    }

    lssa = ir_ssa_latest(ctx);
    
    rnode = this_node->elements[1];

    irgen_c_relational_expression(ctx, rnode);

	rssa = ir_ssa_latest(ctx);

    dssa = ir_ssa_default(ctx);

	switch (this_node->tok.type) {
		case T_EQUAL_TO:
			op = IR_OC_EQ;
			break;
		case T_NOT_EQUAL_TO:
			op = IR_OC_NEQ;
			break;
		default:
			assert(0 && "NOT REACHABLE");
    }

	ir_emit(ctx, op, this_node->dtype, dssa, lssa, rssa);
}

static void irgen_c_relational_expression(IR_CTX *ctx, ParseTreeNode *this_node)
{
	ParseTreeNode *lnode;
	ParseTreeNode *rnode;
	IRSSAEnt *lssa, *rssa, *dssa;
	IROpCode op;

	assert(this_node->num > 0);
	
	lnode = this_node->elements[0];

	if (this_node->num == 1) {
		irgen_c_shift_expression(ctx, lnode);

		return;
	}

	if (lnode->type == RELATIONAL_EXPRESSION) {
		irgen_c_relational_expression(ctx, lnode);
	} else {
		irgen_c_shift_expression(ctx, lnode);
    }

    lssa = ir_ssa_latest(ctx);
    
    rnode = this_node->elements[1];

    irgen_c_shift_expression(ctx, rnode);

	rssa = ir_ssa_latest(ctx);

    dssa = ir_ssa_default(ctx);

	switch (this_node->tok.type) {
		case T_GREATER_THAN:
			op = IR_OC_GT;
			break;
		case T_LESS_THAN:
			op = IR_OC_LT;
			break;
		case T_GREATER_THAN_OR_EQUAL_TO:
			op = IR_OC_GTE;
			break;
		case T_LESS_THAN_OR_EQUAL_TO:
			op = IR_OC_LTE;
			break;
		default:
			assert(0 && "NOT REACHABLE");
    }

	ir_emit(ctx, op, this_node->dtype, dssa, lssa, rssa);
}

static void irgen_c_shift_expression(IR_CTX *ctx, ParseTreeNode *this_node)
{
	ParseTreeNode *lnode;
	ParseTreeNode *rnode;
	IRSSAEnt *lssa, *rssa, *dssa;
	IROpCode op;

	assert(this_node->num > 0);
	
	lnode = this_node->elements[0];

	if (this_node->num == 1) {
		irgen_c_additive_expression(ctx, lnode);

		return;
	}

	if (lnode->type == SHIFT_EXPRESSION) {
		irgen_c_shift_expression(ctx, lnode);
	} else {
		irgen_c_additive_expression(ctx, lnode);
    }

    lssa = ir_ssa_latest(ctx);
    
    rnode = this_node->elements[1];

    irgen_c_additive_expression(ctx, rnode);

	rssa = ir_ssa_latest(ctx);

    dssa = ir_ssa_default(ctx);

	switch (this_node->tok.type) {
		case T_BITWISE_LEFTSHIFT:
			op = IR_OC_SAL;
			break;
		case T_BITWISE_RIGHTSHIFT:
			op = IR_OC_SAR;
			break;
		default:
			assert(0 && "NOT REACHABLE");
    }

	ir_emit(ctx, op, this_node->dtype, dssa, lssa, rssa);
}

static void irgen_c_additive_expression(IR_CTX *ctx, ParseTreeNode *this_node)
{
	ParseTreeNode *lnode;
	ParseTreeNode *rnode;
	IRSSAEnt *lssa, *rssa, *dssa;
	IROpCode op;

	assert(this_node->num > 0);
	
	lnode = this_node->elements[0];

	if (this_node->num == 1) {
		irgen_c_multiplicative_expression(ctx, lnode);

		return;
	}

	if (lnode->type == ADDITIVE_EXPRESSION) {
		irgen_c_additive_expression(ctx, lnode);
	} else {
		irgen_c_multiplicative_expression(ctx, lnode);
    }

    lssa = ir_ssa_latest(ctx);
    
    rnode = this_node->elements[1];

    irgen_c_multiplicative_expression(ctx, rnode);

	rssa = ir_ssa_latest(ctx);

    dssa = ir_ssa_default(ctx);

	switch (this_node->tok.type) {
		case T_MINUS:
			op = IR_OC_SUB;
			break;
		case T_PLUS:
			op = IR_OC_ADD;
			break;
		default:
			assert(0 && "NOT REACHABLE");
    }

	ir_emit(ctx, op, this_node->dtype, dssa, lssa, rssa);
}

static void irgen_c_multiplicative_expression(IR_CTX *ctx, ParseTreeNode *this_node)
{       
	ParseTreeNode *lnode;
	ParseTreeNode *rnode;
	IRSSAEnt *lssa, *rssa, *dssa;
	IROpCode op;

	assert(this_node->num > 0);
	
	lnode = this_node->elements[0];

	if (this_node->num == 1) {
		irgen_c_cast_expression(ctx, lnode);

		return;
	}

	if (lnode->type == MULTIPLICATIVE_EXPRESSION) {
		irgen_c_multiplicative_expression(ctx, lnode);
	} else {
		irgen_c_cast_expression(ctx, lnode);
    }

    lssa = ir_ssa_latest(ctx);
    
    rnode = this_node->elements[1];

    irgen_c_cast_expression(ctx, rnode);

	rssa = ir_ssa_latest(ctx);

    dssa = ir_ssa_default(ctx);

	switch (this_node->tok.type) {
		case T_MULTIPLY:
			op = IR_OC_MUL;
			break;
		case T_DIVIDE:
			op = IR_OC_DIV;
			break;
		case T_MODULUS:
			op = IR_OC_MOD;
			break;
		default:
			assert(0 && "NOT REACHABLE");
    }

	ir_emit(ctx, op, this_node->dtype, dssa, lssa, rssa);
}

static void irgen_c_cast_expression(IR_CTX *ctx, ParseTreeNode *this_node)
{
        ParseTreeNode *node = this_node->elements[0];

		// TODO will happen because parser is buggy ??
		// TODO this_node ??
		if (node->tok.type == T_OPEN_PARENT) {
        	assert(0 && "TODO not implemented: with T_OPEN_PARENT");
        }

        irgen_c_unary_expression(ctx, node);
}

static void irgen_c_unary_expression(IR_CTX *ctx, ParseTreeNode *this_node)
{
        ParseTreeNode *node = this_node->elements[0];

		switch (node->type) {
			case POSTFIX_EXPRESSION: {
				irgen_c_postfix_expression(ctx, node);
			} break;
			case UNARY_EXPRESSION: {
				switch (this_node->tok.type) {
					case T_INCREMENT: {
						assert(0 && "TODO not implemented with (T_INCREMENT)");
					} break;
					case T_DECREMENT: {
						assert(0 && "TODO not implemented with (T_DECREMENT)");
					} break;
					case T_SIZEOF: {
						assert(0 && "TODO not implemented with (T_SIZEOF)");
					} break;
					default:
						assert(0 && "NOT REACHABLE");
				}
			} break;
			case UNARY_OPERATOR: {
				ParseTreeNode *cast_expression = this_node->elements[1];

				switch (node->tok.type) {
					case T_BITWISE_AND: {
						assert(0 && "TODO not implemented with (T_BITWISE_AND)");
					} break;
					case T_MULTIPLY: {
						irgen_c_cast_expression(ctx, cast_expression);
						
						ctx->ssa_latest = ir_ssa_from_ssa(ctx, ir_ssa_latest(ctx));
					} break;
					case T_TILDE: {
						irgen_c_cast_expression(ctx, cast_expression);
						
						ir_emit(ctx, IR_OC_NOT, this_node->dtype, ir_ssa_default(ctx), ir_ssa_latest(ctx), NULL);
					} break;
					case T_LOGICAL_NOT: {
						irgen_c_cast_expression(ctx, cast_expression);
						
						ir_emit(ctx, IR_OC_EQ, this_node->dtype, ir_ssa_default(ctx), ir_ssa_from_literal(ctx, ir_literal_from_lu(0)), ir_ssa_latest(ctx));
					} break;
					case T_MINUS: {
						irgen_c_cast_expression(ctx, cast_expression);
						
						ir_emit(ctx, IR_OC_SUB, this_node->dtype, ir_ssa_default(ctx), ir_ssa_from_literal(ctx, ir_literal_from_lu(0)), ir_ssa_latest(ctx));
					} break;
					case T_PLUS: {
						irgen_c_cast_expression(ctx, cast_expression);
					} break;
					default:
						assert(0 && "NOT REACHABLE");
				}
			} break;
			default:
				assert(0 && "NOT REACHABLE");
		}
}

static void irgen_c_type_name(IR_CTX *ctx, ParseTreeNode *this_node)
{
        (void) ctx;
        (void) this_node;

        assert(0 && "TODO not implemented");
}

static void irgen_c_postfix_expression(IR_CTX *ctx, ParseTreeNode *this_node)
{
        switch (this_node->tok.type) {
			case T_UNKNOWN:
				irgen_c_primary_expression(ctx, this_node->elements[0]);
				break;
			case T_OPEN_BRACKET:
				assert(0 && "TODO not implemented: T_OPEN_BRACKET");
			case T_OPEN_PARENT: {
				IRSSAEnt **argument_ssas = NULL;
				IRSSAEnt *function;

				if (this_node->num == 2) {
					ParseTreeNode *argument_expression_list = this_node->elements[1];

					argument_ssas = malloc(sizeof(IRSSAEnt *) * argument_expression_list->num);

					for (size_t i = 0; i < argument_expression_list->num; ++i) {
						irgen_c_assignment_expression(ctx, argument_expression_list->elements[i]);

						argument_ssas[i] = ir_ssa_latest(ctx);
					}
					
					for (size_t i = 0; i < argument_expression_list->num; ++i) {
						irgen_c_assignment_expression(ctx, argument_expression_list->elements[i]);

						argument_ssas[i] = ir_ssa_latest(ctx);
					}
					
					for (size_t i = 0; i < argument_expression_list->num; ++i) {
						ir_emit(ctx, IR_OC_PARAM, argument_expression_list->elements[i]->dtype, ir_ssa_from_num(ctx, i + 1), argument_ssas[i], NULL);
					}

					free(argument_ssas);
				}

				irgen_c_postfix_expression(ctx, this_node->elements[0]);

				function = ir_ssa_latest(ctx);

				ir_emit(ctx, IR_OC_CALL, this_node->dtype, ir_ssa_default(ctx), function, ir_ssa_from_num(ctx, this_node->elements[1]->num));
			} break;
			case T_DOT:
				assert(0 && "TODO not implemented: T_DOT");
			case T_ARROW:
				assert(0 && "TODO not implemented: T_ARROW");
			case T_INCREMENT:
				assert(0 && "TODO not implemented: T_INCREMENT");
			case T_DECREMENT:
				assert(0 && "TODO not implemented: T_DECREMENT");
			default:
				assert(0 && "NOT REACHABLE");
		}
}

static void irgen_c_unary_operator(IR_CTX *ctx, ParseTreeNode *this_node)
{
        (void) ctx;
        (void) this_node;

        assert(0 && "FUNCTION IS DEPRECATED!");
}

static void irgen_c_primary_expression(IR_CTX *ctx, ParseTreeNode *this_node)
{
        ParseTreeNode *node = this_node->elements[0];

		switch (node->type) {
			case IDENTIFIER: {
				// TODO probably incomplete but fine for now...
				// TODO need globals etc.
				SymtblEntry *entry = node->sym;
				
				if(entry == NULL) {
					ctx->ssa_latest = ir_ssa_from_view(ctx, &node->tok.literal.sv);
					
					return;
				}

				assert(entry->etype == SYMTBL_ENTRY_LOCAL);

				ctx->ssa_latest = ir_ssa_from_stack(ctx, &entry->as.local.addr);
			} break;
			case CONSTANT:
				irgen_c_constant(ctx, node);
				break;
			case STRING:
				irgen_c_string(ctx, node);
				break;
			case EXPRESSION:
				irgen_c_expression(ctx, node);
				break;
			default:
				assert(0 && "NOT REACHABLE");
		}
}

static void irgen_c_assignment_expression(IR_CTX *ctx, ParseTreeNode *this_node)
{
    switch (this_node->elements[0]->type) {
		case CONDITIONAL_EXPRESSION: {
			irgen_c_conditional_expression(ctx, this_node->elements[0]);
		} break;
		case UNARY_EXPRESSION: {
			IROpCode op;
			IRSSAEnt *lval, *rval, *arg1 = NULL, *arg2 = NULL;

			irgen_c_unary_expression(ctx, this_node->elements[0]);
			
			lval = ir_ssa_latest(ctx);

			irgen_c_assignment_expression(ctx, this_node->elements[2]);

			rval = ir_ssa_latest(ctx);

			if (this_node->elements[1]->tok.type != T_ASSIGNMENT) {
				arg1 = lval;
				arg2 = rval;
			} else {
				arg1 = rval;
			}

			switch (this_node->elements[1]->tok.type) {
				case T_MULTIPLY_ASSIGN:
					op = IR_OC_MUL;
					break;
				case T_DIVIDE_ASSIGN:
					op = IR_OC_DIV;
					break;
				case T_MODULUS_ASSIGN:
					op = IR_OC_MOD;
					break;
				case T_PLUS_ASSIGN:
					op = IR_OC_ADD;
					break;
				case T_MINUS_ASSIGN:
					op = IR_OC_SUB;
					break;
				case T_BITWISE_LEFTSHIFT_ASSIGN:
					op = IR_OC_SAL;
					break;
				case T_BITWISE_RIGHTSHIFT_ASSIGN:
					op = IR_OC_SAR;
					break;
				case T_BITWISE_AND_ASSIGN:
					op = IR_OC_AND;
					break;
				case T_BITWISE_XOR_ASSIGN:
					op = IR_OC_XOR;
					break;
				case T_BITWISE_OR_ASSIGN:
					op = IR_OC_OR;
					break;
				case T_ASSIGNMENT:
					op = IR_OC_STORE;
					break;
				default:
					assert(0 && "NOT REACHABLE");
			}
			
			ir_emit(ctx, op, this_node->dtype, lval, arg1, arg2);
		} break;
		default:
			assert(0 && "NOT REACHABLE");
	}
}

static void irgen_c_constant(IR_CTX *ctx, ParseTreeNode *this_node)
{
    switch (this_node->tok.type) {
		case T_INTEGER_CONSTANT:
			ir_emit(ctx, IR_OC_IMM, this_node->dtype, ir_ssa_default(ctx), ir_ssa_from_literal(ctx, this_node->tok.literal), NULL);
			break;
		case T_FLOATING_CONSTANT:
			ir_emit(ctx, IR_OC_IMM, this_node->dtype, ir_ssa_default(ctx), ir_ssa_from_literal(ctx, this_node->tok.literal), NULL);
			break;
		case T_CHARACTER_CONSTANT:
			ir_emit(ctx, IR_OC_IMM, this_node->dtype, ir_ssa_default(ctx), ir_ssa_from_literal(ctx, this_node->tok.literal), NULL);
			break;
		case T_IDENTIFIER:
			assert(0 && "TODO not implemented: ENUMERATION_CONSTANT");
			break;
		default:
			assert(0 && "NOT REACHABLE");
	}
}

static void irgen_c_string(IR_CTX *ctx, ParseTreeNode *this_node)
{
	const size_t label_str = ctx->label_str++;

    ir_emit(ctx, IR_OC_STRING, this_node->dtype, ir_ssa_from_str(ctx, label_str), ir_ssa_from_literal(ctx, this_node->tok.literal) , NULL);

	ir_emit(ctx, IR_OC_LOAD, this_node->dtype, ir_ssa_default(ctx), ir_ssa_from_str(ctx, label_str), NULL);
}

static void irgen_c_assignment_operator(IR_CTX *ctx, ParseTreeNode *this_node)
{
	(void) ctx;
    (void) this_node;

	assert(0 && "FUNCTION IS DEPRECATED!");
}

static void irgen_c_abstract_declarator(IR_CTX *ctx, ParseTreeNode *this_node)
{
    (void) ctx;
    (void) this_node;

    assert(0 && "TODO not implemented");
}

static void irgen_c_parameter_list(IR_CTX *ctx, ParseTreeNode *this_node)
{
    (void) ctx;
    (void) this_node;

    assert(0 && "TODO not implemented");
}

static void irgen_c_parameter_declaration(IR_CTX *ctx, ParseTreeNode *this_node)
{
    (void) ctx;
    (void) this_node;

    assert(0 && "TODO not implemented");
}

static void irgen_c_direct_abstract_declarator(IR_CTX *ctx, ParseTreeNode *this_node)
{
    (void) ctx;
    (void) this_node;

    assert(0 && "TODO not implemented");
}

static void irgen_c_enumerator_list(IR_CTX *ctx, ParseTreeNode *this_node)
{
    (void) ctx;
    (void) this_node;

    assert(0 && "TODO not implemented");
}

static void irgen_c_enumerator(IR_CTX *ctx, ParseTreeNode *this_node)
{
    (void) ctx;
    (void) this_node;

    assert(0 && "TODO not implemented");
}

static void irgen_c_init_declarator_list(IR_CTX *ctx, ParseTreeNode *this_node)
{
	for (size_t i = 0; i < this_node->num; ++i) {
		irgen_c_init_declarator(ctx, this_node->elements[i]);
	}
}

static void irgen_c_init_declarator(IR_CTX *ctx, ParseTreeNode *this_node)
{
	IRSSAEnt *ssa_local = ir_ssa_from_stack(ctx, &this_node->sym->as.local.addr);

    ir_emit(ctx, IR_OC_LOCAL, this_node->sym->dtype, ssa_local, NULL, NULL);
    
    if (this_node->num == 1) {	
    	return;
    }

	irgen_c_initializer(ctx, this_node->elements[1]);
	
	IRSSAEnt *ssa_init = ir_ssa_latest(ctx);

	ir_emit(ctx, IR_OC_STORE, this_node->sym->dtype, ssa_local, ssa_init, NULL);
}

static void irgen_c_initializer(IR_CTX *ctx, ParseTreeNode *this_node)
{
    ParseTreeNode *node = this_node->elements[0];

	switch (node->type) {
		case ASSIGNMENT_EXPRESSION:
			irgen_c_assignment_expression(ctx, node);
			break;
		case INITIALIZER_LIST:
			assert(0 && "TODO not implemented ({ initializer_list })");
			break;
		default:
			assert(0 && "NOT REACHABLE");
	}
}

static void irgen_c_initializer_list(IR_CTX *ctx, ParseTreeNode *this_node)
{
    (void) ctx;
    (void) this_node;

    assert(0 && "TODO not implemented");
}

static void irgen_c_statement(IR_CTX *ctx, ParseTreeNode *this_node)
{
	ParseTreeNode *node = this_node->elements[0];
	
	switch (node->type) {
		case LABELED_STATEMENT: {
			irgen_c_labeled_statement(ctx, node);
		} break;
		case EXPRESSION_STATEMENT: {
			irgen_c_expression_statement(ctx, node);
		} break;
		case COMPOUND_STATEMENT: {
			irgen_c_compound_statement(ctx, node);
		} break;
		case SELECTION_STATEMENT: {
			irgen_c_selection_statement(ctx, node);
		} break;
		case ITERATION_STATEMENT: {
			irgen_c_iteration_statement(ctx, node);
		} break;
		case JUMP_STATEMENT: {
			irgen_c_jump_statement(ctx, node);
		} break;
		default: {
			assert(0 && "NOT REACHABLE");
		}
	}
}

static void irgen_c_labeled_statement(IR_CTX *ctx, ParseTreeNode *this_node)
{
	switch (this_node->tok.type) {
		case T_IDENTIFIER: {
			SymtblEntry *label_entry;
			ParseTreeNode *statement = this_node->elements[0];

			label_entry = this_node->sym;

			assert(label_entry != NULL);

			label_entry->as.label.lid = ctx->label_tmp++;

			ir_emit(ctx, IR_OC_LABEL, NULL, ir_ssa_from_num(ctx, label_entry->as.label.lid), NULL, NULL);

			irgen_c_statement(ctx, statement);
		} break;
		case T_DEFAULT: {
			ParseTreeNode *statement = this_node->elements[0];
			const size_t label = ctx->label_tmp++;
			
			ir_emit(ctx, IR_OC_LABEL, NULL, ir_ssa_from_num(ctx, label), NULL, NULL);
			
			irgen_c_statement(ctx, statement);

			ctx->switch_stmt_section = ctx->code_current;
			
			ctx->code_current = ctx->switch_test_section;
			
			ir_emit(ctx, IR_OC_JMP, NULL, ir_ssa_from_num(ctx, label), NULL, NULL);
			
			ctx->switch_test_section = list_prev(ctx->code_current);
			
			ctx->code_current = ctx->switch_stmt_section;
		} break;
		case T_CASE: {
			ParseTreeNode *constant_expression = this_node->elements[0];
			ParseTreeNode *statement = this_node->elements[1];
			const size_t label = ctx->label_tmp++;
			
			ir_emit(ctx, IR_OC_LABEL, NULL, ir_ssa_from_num(ctx, label), NULL, NULL);
			
			irgen_c_statement(ctx, statement);

			ctx->switch_stmt_section = ctx->code_current;
			
			ctx->code_current = ctx->switch_test_section;
			
			irgen_c_constant_expression(ctx, constant_expression);
			
			ir_emit(ctx, IR_OC_EQ, constant_expression->dtype, ir_ssa_default(ctx), ir_ssa_latest(ctx), ctx->switch_expression);

			ir_emit(ctx, IR_OC_JMP_NOT_ZERO, constant_expression->dtype, ir_ssa_from_num(ctx, label), ir_ssa_latest(ctx), NULL);
			
			ctx->switch_test_section = ctx->code_current;
			
			ctx->code_current = ctx->switch_stmt_section;
		} break;
		default: {
			assert(0 && "NOT REACHABLE");
		}
	}
}

static void irgen_c_expression_statement(IR_CTX *ctx, ParseTreeNode *this_node)
{
    if (this_node->num == 0) {
		return;
	}

    irgen_c_expression(ctx, this_node->elements[0]);
}

static void irgen_c_selection_statement(IR_CTX *ctx, ParseTreeNode *this_node)
{
	switch(this_node->tok.type) {
    	case T_IF: {
    		const size_t before_label_select_begin = ctx->label_select_begin;
			const size_t before_label_select_end = ctx->label_select_end;

			ctx->label_select_begin = ctx->label_tmp++;
			ctx->label_select_end = ctx->label_tmp++;

			ParseTreeNode *expression = this_node->elements[0];

    		irgen_c_expression(ctx, expression);
    		
    		ir_emit(ctx, IR_OC_JMP_ZERO, expression->dtype, ir_ssa_from_num(ctx, ctx->label_select_begin), ir_ssa_latest(ctx), NULL);

    		irgen_c_statement(ctx, this_node->elements[1]);

    		/* jmp_if_end */
			ir_emit(ctx, IR_OC_JMP, NULL, ir_ssa_from_num(ctx, ctx->label_select_end), NULL, NULL);

    		/* label_else */
			ir_emit(ctx, IR_OC_LABEL, NULL, ir_ssa_from_num(ctx, ctx->label_select_begin), NULL, NULL);

    		if (this_node->num == 3) {
    			irgen_c_statement(ctx, this_node->elements[2]);

    			/* jmp_if_end */
				ir_emit(ctx, IR_OC_JMP, NULL, ir_ssa_from_num(ctx, ctx->label_select_end), NULL, NULL);
			}
			
			/* label_end */
			ir_emit(ctx, IR_OC_LABEL, NULL, ir_ssa_from_num(ctx, ctx->label_select_end), NULL, NULL);

			ctx->label_select_begin = before_label_select_begin;
			ctx->label_select_end = before_label_select_end;
    	} break;
    	case T_SWITCH: {
    		IRSSAEnt *before_switch_expression = ctx->switch_expression;
    		const size_t label_switch_test = ctx->label_tmp++;
    		const size_t before_label_iter_end = ctx->label_iter_end;
    		list_node_t *before_switch_stmt_section = ctx->switch_stmt_section;
    		list_node_t *before_switch_test_section = ctx->switch_test_section;
    		list_node_t *switch_end;
    		
    		ctx->label_iter_end = ctx->label_tmp++;
    		
    		irgen_c_expression(ctx, this_node->elements[0]);
    		
    		ctx->switch_expression = ir_ssa_latest(ctx);
    		
    		/* jmp switch_test */
			ir_emit(ctx, IR_OC_JMP, NULL, ir_ssa_from_num(ctx, label_switch_test), NULL, NULL);
    		
    		/* ...[switch_stmt_section] */
    		ctx->switch_stmt_section = ctx->code_current;
    		
    		/* jmp switch_end */
			ir_emit(ctx, IR_OC_JMP, NULL, ir_ssa_from_num(ctx, ctx->label_iter_end), NULL, NULL);
			
			/* label switch_test */
			ir_emit(ctx, IR_OC_LABEL, NULL, ir_ssa_from_num(ctx, label_switch_test), NULL, NULL);
    		
    		/* ...[switch_test_section] */
    		ctx->switch_test_section = ctx->code_current;
    		
    		/* label switch_end */
			ir_emit(ctx, IR_OC_LABEL, NULL, ir_ssa_from_num(ctx, ctx->label_iter_end), NULL, NULL);
    		
    		switch_end = ctx->code_current;
    		
    		ctx->code_current = ctx->switch_stmt_section;
    		
    		irgen_c_statement(ctx, this_node->elements[1]);

    		ctx->code_current = switch_end;
    		ctx->switch_expression = before_switch_expression;
    		ctx->label_iter_end = before_label_iter_end;
    		ctx->switch_stmt_section = before_switch_stmt_section;
    		ctx->switch_test_section = before_switch_test_section;
    	} break;
    	default: {
			assert(0 && "NOT REACHABLE");
		}
    }
}

static void irgen_c_iteration_statement(IR_CTX *ctx, ParseTreeNode *this_node)
{
	const size_t before_label_iter_begin = ctx->label_iter_begin;
	const size_t before_label_iter_end = ctx->label_iter_end;

	ctx->label_iter_begin = ctx->label_tmp++;
	ctx->label_iter_end = ctx->label_tmp++;

    switch(this_node->tok.type) {
		case T_WHILE: {
			/* label_begin */
			ir_emit(ctx, IR_OC_LABEL, NULL, ir_ssa_from_num(ctx, ctx->label_iter_begin), NULL, NULL);

			ParseTreeNode *expression = this_node->elements[0];

			irgen_c_expression(ctx, expression);

			ir_emit(ctx, IR_OC_JMP_ZERO, expression->dtype, ir_ssa_from_num(ctx, ctx->label_iter_end), ir_ssa_latest(ctx), NULL);
			
			ParseTreeNode *statement = this_node->elements[1];

			irgen_c_statement(ctx, statement);
			
			/* jmp_begin */
			ir_emit(ctx, IR_OC_JMP, NULL, ir_ssa_from_num(ctx, ctx->label_iter_begin), NULL, NULL);
		} break;
		case T_FOR: {
			ParseTreeNode *expression_init = NULL;
			ParseTreeNode *expression_cond = NULL;
			ParseTreeNode *expression_last = NULL;
			ParseTreeNode *statement = this_node->elements[this_node->num - 1];
			
			for (size_t i = 0; i < this_node->num - 1; ++i) {
				ParseTreeNode *expression = this_node->elements[i];
				
				switch (expression->tok.type) {
					case T_OPEN_PARENT:
						expression_init = expression;
						break;
					case T_SEMICOLON:
						expression_cond = expression;
						break;
					case T_CLOSING_PARENT:
						expression_last = expression;
						break;
					default:
						assert(0 && "NOT REACHABLE");
				}
			}
			
			if (expression_init != NULL) {
				irgen_c_expression(ctx, expression_init);
			}

			const size_t for_label_begin = ctx->label_iter_begin;

			if (expression_last != NULL) {
				ctx->label_iter_begin = ctx->label_tmp++;
			}

			/* label_begin */
			ir_emit(ctx, IR_OC_LABEL, NULL, ir_ssa_from_num(ctx, for_label_begin), NULL, NULL);
			
			if (expression_cond != NULL) {
				irgen_c_expression(ctx, expression_cond);

				ir_emit(ctx, IR_OC_JMP_ZERO, expression_cond->dtype, ir_ssa_from_num(ctx, ctx->label_iter_end), ir_ssa_latest(ctx), NULL);
			}
			
			irgen_c_statement(ctx, statement);
			
			if (expression_last != NULL) {
				/* label_expression_last */
				ir_emit(ctx, IR_OC_LABEL, NULL, ir_ssa_from_num(ctx, ctx->label_iter_begin), NULL, NULL);

				irgen_c_expression(ctx, expression_last);
			}

			/* jmp_begin */
			ir_emit(ctx, IR_OC_JMP, NULL, ir_ssa_from_num(ctx, for_label_begin), NULL, NULL);
		} break;
		case T_DO: {
			/* label_begin */
			ir_emit(ctx, IR_OC_LABEL, NULL, ir_ssa_from_num(ctx, ctx->label_iter_begin), NULL, NULL);

			ParseTreeNode *statement = this_node->elements[0];

			irgen_c_statement(ctx, statement);
			
			ParseTreeNode *expression = this_node->elements[1];

			irgen_c_expression(ctx, expression);
			
			ir_emit(ctx, IR_OC_JMP_NOT_ZERO, expression->dtype, ir_ssa_from_num(ctx, ctx->label_iter_begin), ir_ssa_latest(ctx), NULL);
		} break;
		default: {
			assert(0 && "NOT REACHABLE");
		}
    }
    
    /* label_end */
	ir_emit(ctx, IR_OC_LABEL, NULL, ir_ssa_from_num(ctx, ctx->label_iter_end), NULL, NULL);
	
	ctx->label_iter_begin = before_label_iter_begin;
	ctx->label_iter_end = before_label_iter_end;
}

static void irgen_c_jump_statement(IR_CTX *ctx, ParseTreeNode *this_node)
{
	switch(this_node->tok.type) {
		case T_GOTO: {
			ParseTreeNode *identifier;
			SymtblEntry *label_entry;

			identifier = this_node->elements[0];

			assert(identifier != NULL);

			label_entry = identifier->sym;

			assert(label_entry != NULL);

			ir_emit(ctx, IR_OC_JMP, NULL, ir_ssa_from_addr(ctx, &label_entry->as.label.lid), NULL, NULL);
		} break;
		case T_CONTINUE: {
			ir_emit(ctx, IR_OC_JMP, NULL, ir_ssa_from_num(ctx, ctx->label_iter_begin), NULL, NULL);
		} break;
		case T_BREAK: {
			ir_emit(ctx, IR_OC_JMP, NULL, ir_ssa_from_num(ctx, ctx->label_iter_end), NULL, NULL);
		} break;
		case T_RETURN: {
			if (this_node->num > 0) {
				irgen_c_expression(ctx, this_node->elements[0]);
				
				ir_emit(ctx, IR_OC_RET, ctx->function_return_type, ir_ssa_latest(ctx), NULL, NULL);
			}

			ir_emit(ctx, IR_OC_JMP, NULL, ir_ssa_from_num(ctx, ctx->label_func_end), NULL, NULL);
		} break;
		default:
			assert(0 && "NOT REACHABLE");
	}
}
