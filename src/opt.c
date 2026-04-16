#include <assert.h>
#include <stdlib.h>

#include "ir.h"
#include "opt.h"
#include "gen.h"

extern int optimizer_func_begin(list_node_t *it);

extern int optimizer_func_end(list_node_t *it);

extern int optimizer_stack_allocation(list_node_t *it);

extern int optimizer_string(list_node_t *it);

int optimizer_run(void)
{
	for (list_node_t *it = list_begin(ir_ctx->code); it != list_end(ir_ctx->code); it = list_next(it)) {
		IRCode *code = it->value;
		
		switch (code->op) {
			case IR_OC_FUNC_BEGIN: {
				if (optimizer_func_begin(it) != 0) {
					return -1;
				}
			} break;
			case IR_OC_FUNC_END: {
				if (optimizer_func_end(it) != 0) {
					return -1;
				}
			} break;
			default: break;
		}
	}

	return 0;
}

int optimizer_func_begin(list_node_t *it)
{
	if (optimizer_stack_allocation(it) == -1) {
		return -1;	
	}

	return 0;
}

int optimizer_func_end(list_node_t *it)
{
	if (optimizer_string(it) == -1) {
		return -1;	
	}

	return 0;
}

int optimizer_stack_allocation(list_node_t *begin)
{
	IRCode *func_begin_code = begin->value;

	for (list_node_t *it = begin; it != list_end(ir_ctx->code); ) {
		IRCode *code = it->value;
		
		switch (code->op) {
			case IR_OC_LOCAL: {
				// TODO align to 8byte is there to make it simple
				ir_ctx->stack_offset += 8; // codegen_get_type_size(code->result.rtype);
			
				*code->result->as.stack = ir_ctx->stack_offset;

				it = list_erase(ir_ctx->code, it);
			} break;
			case IR_OC_FUNC_END: {
				func_begin_code->arg1->as.num = ir_ctx->stack_offset;
				code->arg1->as.num = ir_ctx->stack_offset;

				ir_ctx->stack_offset = 0;

				return 0;
			} break;
			case IR_OC_IMM:
			case IR_OC_SAL:
			case IR_OC_SAR:
			case IR_OC_ADD:
			case IR_OC_SUB:
			case IR_OC_MUL:
			case IR_OC_DIV:
			case IR_OC_MOD:
			case IR_OC_OR:
			case IR_OC_XOR:
			case IR_OC_AND:
			case IR_OC_EQ:
			case IR_OC_NEQ:
			case IR_OC_GT:
			case IR_OC_LT:
			case IR_OC_GTE:
			case IR_OC_LTE:
			case IR_OC_LOAD:
			case IR_OC_STORE:
			case IR_OC_CALL: {
				if (code->result != NULL && code->result->type == IR_ATYPE_NUM) {
					ir_ctx->stack_offset += 8; // codegen_get_type_size(code->result.rtype);
			
					code->result->as.num = ir_ctx->stack_offset;
				}
			} // NO BREAK
			default: {
				it = list_next(it);
			} break;
		}
	}

	return -1;
}

int optimizer_string(list_node_t *end)
{
	for (list_node_t *it = end; it != list_begin(ir_ctx->code); ) {
		IRCode *code = it->value;
		
		switch (code->op) {
			case IR_OC_STRING: {
				list_insert(ir_ctx->code, list_next(end), code);
				it = list_erase(ir_ctx->code, it);
			} break;
			case IR_OC_FUNC_BEGIN: return 0;
			default: it = list_prev(it); break;
		}
	}
	
	return 0;
}
