#include <assert.h>
#include <stdio.h>

#include <jd297/list.h>
#include <jd297/sv.h>

#include "ir.h"
#include "gen.h"

#define REGISTER_ACCUMULATOR        0 // RAX (volatile)
#define REGISTER_FORMERLY_BASE      1 // RBX (non-volatile)
#define REGISTER_COUNTER            2 // RCX (volatile)
#define REGISTER_DATA               3 // RDX (volatile)
#define REGISTER_SOURCE_INDEX       4 // RSI (volatile)
#define REGISTER_DESTINATION_INDEX  5 // RDI (volatile)
#define REGISTER_BASE_POINTER       6 // RBP (non-volatile)
#define REGISTER_STACK_POINTER      7 // RSP (non-volatile)
#define REGISTER_ADDITIONAL_1       8 // R8  (volatile)
#define REGISTER_ADDITIONAL_2       9 // R9  (volatile)
#define REGISTER_ADDITIONAL_3      10 // R10 (volatile)
#define REGISTER_ADDITIONAL_4      11 // R11 (volatile)
#define REGISTER_ADDITIONAL_5      12 // R12 (non-volatile)
#define REGISTER_ADDITIONAL_6      13 // R13 (non-volatile)
#define REGISTER_ADDITIONAL_7      14 // R14 (non-volatile)
#define REGISTER_ADDITIONAL_8      15 // R15 (non-volatile)
#define REGISTER_TEMPORARY         11 // R11 (volatile) is the compiler temp register
#define REGISTER_TEMPORARY_2       12 // R12 (volatile) is the compiler temp register 2
#define REGISTER_RETURN             0 // RAX (volatile)

static const char *codegen_x86_64_suffix(IRPrimitiveDataType ptype);
static const char *codegen_x86_64_reg_name(IRPrimitiveDataType ptype, IRSSAEnt *reg);
static int codegen_x86_64_fput_operand(FILE *output, IRPrimitiveDataType ptype, IRSSAEnt *operand);
static IRSSAEnt *codegen_x86_64_param_register(size_t n);

static void codegen_x86_64_mov(FILE *output, IRPrimitiveDataType ptype, IRSSAEnt *src, IRSSAEnt *dst);
static void codegen_x86_64_bin_op(FILE *output, IRCode *code, const char *op);
static void codegen_x86_64_unary_op(FILE *output, IRCode *code, const char *op);
static void codegen_x86_64_logical_cmp(FILE *output, IRCode *code, const char *op);

static void codegen_x86_64_func_begin(FILE *output, IRCode *code);
static void codegen_x86_64_func_end(FILE *output, IRCode *code);
static void codegen_x86_64_imm(FILE *output, IRCode *code);
static void codegen_x86_64_sal(FILE *output, IRCode *code);
static void codegen_x86_64_sar(FILE *output, IRCode *code);
static void codegen_x86_64_add(FILE *output, IRCode *code);
static void codegen_x86_64_sub(FILE *output, IRCode *code);
static void codegen_x86_64_mul(FILE *output, IRCode *code);
static void codegen_x86_64_div(FILE *output, IRCode *code);
// static void codegen_x86_64_mod(FILE *output, IRCode *code);
static void codegen_x86_64_or(FILE *output, IRCode *code);
static void codegen_x86_64_xor(FILE *output, IRCode *code);
static void codegen_x86_64_and(FILE *output, IRCode *code);
static void codegen_x86_64_not(FILE *output, IRCode *code);
static void codegen_x86_64_eq(FILE *output, IRCode *code);
static void codegen_x86_64_neq(FILE *output, IRCode *code);
static void codegen_x86_64_gt(FILE *output, IRCode *code);
static void codegen_x86_64_lt(FILE *output, IRCode *code);
static void codegen_x86_64_gte(FILE *output, IRCode *code);
static void codegen_x86_64_lte(FILE *output, IRCode *code);
static void codegen_x86_64_label(FILE *output, IRCode *code);
static void codegen_x86_64_jmp(FILE *output, IRCode *code);
static void codegen_x86_64_jmp_zero(FILE *output, IRCode *code);
static void codegen_x86_64_jmp_not_zero(FILE *output, IRCode *code);
static void codegen_x86_64_store(FILE *output, IRCode *code);
static void codegen_x86_64_load(FILE *output, IRCode *code);
static void codegen_x86_64_param(FILE *output, IRCode *code);
static void codegen_x86_64_call(FILE *output, IRCode *code);
static void codegen_x86_64_string(FILE *output, IRCode *code);
static void codegen_x86_64_ret(FILE *output, IRCode *code);

static const char *codegen_x86_64_suffix(IRPrimitiveDataType ptype)
{
	switch (ptype) {
		case IR_U8_T:
		case IR_S8_T:
			return "b";
		case IR_U16_T:
		case IR_S16_T:
			return "w";
		case IR_U32_T:
		case IR_S32_T:
			return "l";
		case IR_U64_T:
		case IR_S64_T:
		case IR_PTR_T:
			return "q";
		case IR_F32_T:
			assert(0 && "TODO: not implemented: IR_F32_T");
		case IR_F64_T:
			assert(0 && "TODO: not implemented: IR_F64_T");
		case IR_F128_T:
			assert(0 && "TODO: not implemented: IR_F128_T");
		default:
			assert(0 && "NOT REACHABLE");
	}
}

static const char *codegen_x86_64_reg_name(IRPrimitiveDataType ptype, IRSSAEnt *reg)
{
	switch (ptype) {
		case IR_U8_T:
		case IR_S8_T: {
			switch (reg->as.reg) {
				case REGISTER_ACCUMULATOR:
					return "al";				
				case REGISTER_FORMERLY_BASE:
					return "bl";				
				case REGISTER_COUNTER:
					return "cl";				
				case REGISTER_DATA:
					return "dl";				
				case REGISTER_SOURCE_INDEX:
					return "sil";
				case REGISTER_DESTINATION_INDEX:
					return "dil";
				case REGISTER_BASE_POINTER:
					return "bpl";
				case REGISTER_STACK_POINTER:
					return "spl";
				case REGISTER_ADDITIONAL_1:
					return "r8b";
				case REGISTER_ADDITIONAL_2:
					return "r9b";
				case REGISTER_ADDITIONAL_3:
					return "r10b";
				case REGISTER_ADDITIONAL_4:
					return "r11b";				
				case REGISTER_ADDITIONAL_5:
					return "r12b";				
				case REGISTER_ADDITIONAL_6:
					return "r13b";				
				case REGISTER_ADDITIONAL_7:
					return "r14b";				
				case REGISTER_ADDITIONAL_8:
					return "r15b";
				default:
					assert(0 && "NOT REACHABLE");
			}
		} break;
		case IR_U16_T:
		case IR_S16_T: {
			switch (reg->as.reg) {
				case REGISTER_ACCUMULATOR:
					return "ax";				
				case REGISTER_FORMERLY_BASE:
					return "bx";				
				case REGISTER_COUNTER:
					return "cx";				
				case REGISTER_DATA:
					return "dx";				
				case REGISTER_SOURCE_INDEX:
					return "si";
				case REGISTER_DESTINATION_INDEX:
					return "di";
				case REGISTER_BASE_POINTER:
					return "bp";
				case REGISTER_STACK_POINTER:
					return "sp";
				case REGISTER_ADDITIONAL_1:
					return "r8w";
				case REGISTER_ADDITIONAL_2:
					return "r9w";
				case REGISTER_ADDITIONAL_3:
					return "r10w";
				case REGISTER_ADDITIONAL_4:
					return "r11w";				
				case REGISTER_ADDITIONAL_5:
					return "r12w";				
				case REGISTER_ADDITIONAL_6:
					return "r13w";				
				case REGISTER_ADDITIONAL_7:
					return "r14w";				
				case REGISTER_ADDITIONAL_8:
					return "r15w";
				default:
					assert(0 && "NOT REACHABLE");
			}
		} break;
		case IR_U32_T:
		case IR_S32_T: {
			switch (reg->as.reg) {
				case REGISTER_ACCUMULATOR:
					return "eax";				
				case REGISTER_FORMERLY_BASE:
					return "ebx";				
				case REGISTER_COUNTER:
					return "ecx";				
				case REGISTER_DATA:
					return "edx";				
				case REGISTER_SOURCE_INDEX:
					return "esi";
				case REGISTER_DESTINATION_INDEX:
					return "edi";
				case REGISTER_BASE_POINTER:
					return "ebp";
				case REGISTER_STACK_POINTER:
					return "esp";
				case REGISTER_ADDITIONAL_1:
					return "r8d";
				case REGISTER_ADDITIONAL_2:
					return "r9d";
				case REGISTER_ADDITIONAL_3:
					return "r10d";
				case REGISTER_ADDITIONAL_4:
					return "r11d";				
				case REGISTER_ADDITIONAL_5:
					return "r12d";				
				case REGISTER_ADDITIONAL_6:
					return "r13d";				
				case REGISTER_ADDITIONAL_7:
					return "r14d";				
				case REGISTER_ADDITIONAL_8:
					return "r15d";
				default:
					assert(0 && "NOT REACHABLE");
			}
		} break;
		case IR_U64_T:
		case IR_S64_T:
		case IR_PTR_T: {
			switch (reg->as.reg) {
				case REGISTER_ACCUMULATOR:
					return "rax";				
				case REGISTER_FORMERLY_BASE:
					return "rbx";				
				case REGISTER_COUNTER:
					return "rcx";				
				case REGISTER_DATA:
					return "rdx";				
				case REGISTER_SOURCE_INDEX:
					return "rsi";
				case REGISTER_DESTINATION_INDEX:
					return "rdi";
				case REGISTER_BASE_POINTER:
					return "rbp";
				case REGISTER_STACK_POINTER:
					return "rsp";
				case REGISTER_ADDITIONAL_1:
					return "r8";
				case REGISTER_ADDITIONAL_2:
					return "r9";
				case REGISTER_ADDITIONAL_3:
					return "r10";
				case REGISTER_ADDITIONAL_4:
					return "r11";				
				case REGISTER_ADDITIONAL_5:
					return "r12";				
				case REGISTER_ADDITIONAL_6:
					return "r13";				
				case REGISTER_ADDITIONAL_7:
					return "r14";				
				case REGISTER_ADDITIONAL_8:
					return "r15";
				default:
					assert(0 && "NOT REACHABLE");
			}
		} break;
		case IR_F32_T:
			assert(0 && "TODO: not implemented: IR_F32_T");
		case IR_F64_T:
			assert(0 && "TODO: not implemented: IR_F64_T");
		case IR_F128_T:
			assert(0 && "TODO: not implemented: IR_F128_T");
		default:
			assert(0 && "NOT REACHABLE");
	}
}

static int codegen_x86_64_fput_operand(FILE *output, IRPrimitiveDataType ptype, IRSSAEnt *operand)
{
	if (operand->type == IR_ATYPE_SSA) {
		operand = operand->as.ssa;
	}

	switch (operand->type) {
		case IR_ATYPE_REG:
			fputs("%", output);

			return fputs(codegen_x86_64_reg_name(ptype, operand), output) + 1;
		case IR_ATYPE_LIT: {
			switch (ptype) {
				case IR_U8_T:
				case IR_S8_T:
				case IR_U16_T:
				case IR_S16_T:
				case IR_U32_T:
				case IR_S32_T:
					return fprintf(output, "$%u", operand->as.literal.u);
				case IR_U64_T:
				case IR_S64_T:
				case IR_PTR_T:
					return fprintf(output, "$%lu", operand->as.literal.lu);
				case IR_F32_T:
					assert(0 && "TODO: not implemented: IR_F32_T");
				case IR_F64_T:
					assert(0 && "TODO: not implemented: IR_F64_T");
				case IR_F128_T:
					assert(0 && "TODO: not implemented: IR_F128_T");
				default:
					assert(0 && "NOT REACHABLE");
			}
		} break;
		case IR_ATYPE_STACK:
			return fprintf(output, "-%zu(%%rbp)", *operand->as.stack);
		case IR_ATYPE_ADDR:
			return fprintf(output, "-%zu(%%rbp)", *operand->as.addr);
		case IR_ATYPE_NUM:
			return fprintf(output, "-%zu(%%rbp)", operand->as.num);
		case IR_ATYPE_STR:
			return fprintf(output, ".L.str.%zu(%%rip)", operand->as.str);
		case IR_ATYPE_MEM:
			return fprintf(output, SV_FMT"(%%rip)", SV_PARAMS(operand->as.view));
		case IR_ATYPE_VIEW:
			assert(0 && "TODO: not implemented: IR_ATYPE_VIEW");
		default:
			assert(0 && "NOT REACHABLE");
	}
}

static IRSSAEnt *codegen_x86_64_param_register(size_t n)
{
	size_t reg_num;

	switch (n) {
		case 1:
			reg_num = REGISTER_DESTINATION_INDEX;
			break;
		case 2:
			reg_num = REGISTER_SOURCE_INDEX;
			break;
		case 3:
			reg_num = REGISTER_DATA;
			break;
		case 4:
			reg_num = REGISTER_COUNTER;
			break;
		case 5:
			reg_num = REGISTER_ADDITIONAL_1;
			break;
		case 6:
			reg_num = REGISTER_ADDITIONAL_2;
			break;
		default:
			assert(0 && "NOT REACHABLE");
	}

	return ir_ssa_from_reg(reg_num);
}

static void codegen_x86_64_mov(FILE *output, IRPrimitiveDataType ptype, IRSSAEnt *src, IRSSAEnt *dst)
{
	IRSSAEnt *org_src = src;
	IRSSAEnt *org_dst = dst;

	if (src == NULL || dst == NULL) {
		return;
	}

	if (src == dst) {
		return;
	}

	if (src->type == IR_ATYPE_SSA) {
		IRSSAEnt *tmp = ir_ssa_from_reg(REGISTER_TEMPORARY);

		/*if (src->type != IR_ATYPE_REG) {
			IRSSAEnt *tmp = ir_ssa_from_reg(REGISTER_TEMPORARY);

			codegen_x86_64_mov(output, ptype, src, tmp);

			src = tmp;
		}*/

		codegen_x86_64_mov(output, ptype, src->as.ssa, tmp);

		src = tmp;
	}

	if (dst->type == IR_ATYPE_SSA) {
		IRSSAEnt *tmp = ir_ssa_from_reg(REGISTER_TEMPORARY_2);

		if (src->type != IR_ATYPE_REG) {
			IRSSAEnt *tmp = ir_ssa_from_reg(REGISTER_TEMPORARY);

			codegen_x86_64_mov(output, ptype, src, tmp);

			src = tmp;
		}

		codegen_x86_64_mov(output, ptype, dst->as.ssa, tmp);

		dst = tmp;
	}

	if (
		(
			(src->type == IR_ATYPE_NUM || src->type == IR_ATYPE_STACK || src->type == IR_ATYPE_ADDR) 
				&& 
			(dst->type == IR_ATYPE_NUM || dst->type == IR_ATYPE_STACK || dst->type == IR_ATYPE_ADDR)
		)
			||
		(
			(src->type == IR_ATYPE_LIT && src->as.literal.lu >= (1UL << 32))
				&&
			(dst->type != IR_ATYPE_REG)
		)
			||
		(src->type == IR_ATYPE_STR && dst->type != IR_ATYPE_REG)
	) {
		IRSSAEnt *tmp = ir_ssa_from_reg(REGISTER_TEMPORARY);

		codegen_x86_64_mov(output, ptype, src, tmp);

		src = tmp;
	}

	if (src->type == IR_ATYPE_STR) {
		fputs("\tlea", output);
	} else {
		fputs("\tmov", output);
	}

	fputs(codegen_x86_64_suffix(ptype), output);

	fputs("\t", output);

	if (org_src->type == IR_ATYPE_SSA) {
		fputs("(", output);
	}

	codegen_x86_64_fput_operand(output, ptype, src);
	
	if (org_src->type == IR_ATYPE_SSA) {
		fputs(")", output);
	}

	fputs(", ", output);

	if (org_dst->type == IR_ATYPE_SSA) {
		fputs("(", output);
	}

	codegen_x86_64_fput_operand(output, ptype, dst);
	
	if (org_dst->type == IR_ATYPE_SSA) {
		fputs(")", output);
	}
	
	fputs("\n", output);
}

static void codegen_x86_64_bin_op(FILE *output, IRCode *code, const char *op)
{
	IRSSAEnt *operand;

	fprintf(output, "\t# %s, %d\n", __FUNCTION__, __LINE__);
	
	/* optimization to avoid mutation of arg1 and setting the result correct right away */
	if (code->result->type == IR_ATYPE_REG) {
		if (code->result != code->arg1) {
			codegen_x86_64_mov(output, code->dtype->as.primitive, code->arg1, code->result);
		}
		
		operand = code->result;
	} else {
		operand = ir_ssa_from_reg(REGISTER_TEMPORARY);

		codegen_x86_64_mov(output, code->dtype->as.primitive, code->arg1, operand);
	}

	fprintf(output, "\t%s", op);
	
	fputs(codegen_x86_64_suffix(code->dtype->as.primitive), output);

	fputs("\t", output);

	codegen_x86_64_fput_operand(output, code->dtype->as.primitive, code->arg2);

	fputs(", ", output);

	codegen_x86_64_fput_operand(output, code->dtype->as.primitive, operand);

	fputs("\n", output);

	if (code->result != operand) {
		codegen_x86_64_mov(output, code->dtype->as.primitive, operand, code->result);
	}
}

static void codegen_x86_64_unary_op(FILE *output, IRCode *code, const char *op)
{
	IRSSAEnt *operand;

	fprintf(output, "\t# %s, %d\n", __FUNCTION__, __LINE__);
	
	/* optimization to avoid mutation of arg1 and setting the result correct right away */
	if (code->result->type == IR_ATYPE_REG) {
		if (code->result != code->arg1) {
			codegen_x86_64_mov(output, code->dtype->as.primitive, code->arg1, code->result);
		}
		
		operand = code->result;
	} else {
		operand = ir_ssa_from_reg(REGISTER_TEMPORARY);

		codegen_x86_64_mov(output, code->dtype->as.primitive, code->arg1, operand);
	}

	fprintf(output, "\t%s", op);
	
	fputs(codegen_x86_64_suffix(code->dtype->as.primitive), output);

	fputs("\t", output);

	codegen_x86_64_fput_operand(output, code->dtype->as.primitive, operand);

	fputs("\n", output);

	if (code->result != operand) {
		codegen_x86_64_mov(output, code->dtype->as.primitive, operand, code->result);
	}
}

static void codegen_x86_64_logical_cmp(FILE *output, IRCode *code, const char *op)
{
	IRSSAEnt *result = code->result;

	fprintf(output, "\t# %s, %d\n", __FUNCTION__, __LINE__);

	code->result = ir_ssa_from_reg(REGISTER_TEMPORARY);

	codegen_x86_64_bin_op(output, code, "cmp");
	
	fprintf(output, "\tset%s\t%%%s\n", op, codegen_x86_64_reg_name(IR_U8_T, code->result));

	fputs("\tmovzb", output);

	fputs(codegen_x86_64_suffix(code->dtype->as.primitive), output);

	fputs("\t", output);

	codegen_x86_64_fput_operand(output, IR_U8_T, code->result);

	fputs(", ", output);

	codegen_x86_64_fput_operand(output, code->dtype->as.primitive, code->result);

	fputs("\n", output);

	if (code->result != result) {
		codegen_x86_64_mov(output, code->dtype->as.primitive, code->result, result);
	}
}

void codegen_x86_64_run(FILE *output)
{
	assert(ir_ctx != NULL);
	assert(output != NULL);

	for (list_node_t *it = list_begin(ir_ctx->code); it != list_end(ir_ctx->code); it = list_next(it)) {
		IRCode *code;

		assert(it != NULL);
		assert(it->value != NULL);

		code = it->value;

		switch (code->op) {
			case IR_OC_FUNC_BEGIN:
				codegen_x86_64_func_begin(output, code);
				break;
			case IR_OC_IMM:
				codegen_x86_64_imm(output, code);
				break;
			case IR_OC_POP:
				assert(0 && "TODO: IR_OC_POP");
				// codegen_x86_64_pop(output, code);
				break;
			case IR_OC_SAL:
				codegen_x86_64_sal(output, code);
				break;
			case IR_OC_SAR:
				codegen_x86_64_sar(output, code);
				break;
			case IR_OC_ADD:
				codegen_x86_64_add(output, code);
				break;
			case IR_OC_SUB:
				codegen_x86_64_sub(output, code);
				break;
			case IR_OC_MUL:
				codegen_x86_64_mul(output, code);
				break;
			case IR_OC_DIV:
				codegen_x86_64_div(output, code);
				break;
			case IR_OC_MOD:
				assert(0 && "TODO: IR_OC_MOD");
				// codegen_x86_64_mod(output, code);
				break;
			case IR_OC_OR:
				codegen_x86_64_or(output, code);
				break;
			case IR_OC_XOR:
				codegen_x86_64_xor(output, code);
				break;
			case IR_OC_AND:
				codegen_x86_64_and(output, code);
				break;
			case IR_OC_NOT:
				codegen_x86_64_not(output, code);
				break;
			case IR_OC_EQ:
				codegen_x86_64_eq(output, code);
				break;
			case IR_OC_NEQ:
				codegen_x86_64_neq(output, code);
				break;
			case IR_OC_GT:
				codegen_x86_64_gt(output, code);
				break;
			case IR_OC_LT:
				codegen_x86_64_lt(output, code);
				break;
			case IR_OC_GTE:
				codegen_x86_64_gte(output, code);
				break;
			case IR_OC_LTE:
				codegen_x86_64_lte(output, code);
				break;
			case IR_OC_LABEL:
				codegen_x86_64_label(output, code);
				break;
			case IR_OC_JMP:
				codegen_x86_64_jmp(output, code);
				break;
			case IR_OC_JMP_ZERO:
				codegen_x86_64_jmp_zero(output, code);
				break;
			case IR_OC_JMP_NOT_ZERO:
				codegen_x86_64_jmp_not_zero(output, code);
				break;
			case IR_OC_FUNC_END:
				codegen_x86_64_func_end(output, code);
				break;
			case IR_OC_STORE:
				codegen_x86_64_store(output, code);
				break;
			case IR_OC_LOAD:
				codegen_x86_64_load(output, code);
				break;
			case IR_OC_PARAM:
				codegen_x86_64_param(output, code);
				break;
			case IR_OC_CALL:
				codegen_x86_64_call(output, code);
				break;
			case IR_OC_STRING:
				codegen_x86_64_string(output, code);
				break;
			case IR_OC_RET:
				codegen_x86_64_ret(output, code);
				break;
			case IR_OC_LOCAL:
				assert(0 && "IR_OC_LOCAL should not be in list of code when codegen_x86_64_run is called");
		
			default:
				printf(">>>>>%d\n", code->op);
				assert(0 && "OP Code not implemented yet");
		}
	}
}

static void codegen_x86_64_func_begin(FILE *output, IRCode *code)
{
	size_t stack_size = code->arg1->as.num;

	fputs("\t.text\n", output);

	if ((code->dtype->storage_flags & IR_STORAGE_FLAG_STATIC) == 0) {
		fprintf(output, "\t.globl " SV_FMT "\n", SV_PARAMS(code->result->as.view));
	}

	fprintf(output, SV_FMT ":\n", SV_PARAMS(code->result->as.view));
	fputs("\tendbr64\n", output);
	fputs("\tpushq\t%rbp\n", output);
	fputs("\tmovq\t%rsp, %rbp\n", output);

	// align to 16byte word boundary
	stack_size += stack_size % 16;

	fprintf(output, "\tsubq\t$%zu, %%rsp\n", stack_size);
}

static void codegen_x86_64_func_end(FILE *output, IRCode *code)
{
	codegen_x86_64_label(output, code);

	fputs("\tleave\n", output);
	fputs("\tret\n", output);
}

static void codegen_x86_64_imm(FILE *output, IRCode *code)
{
	fprintf(output, "\t# %s, %d\n", __FUNCTION__, __LINE__);
	codegen_x86_64_mov(output, code->dtype->as.primitive, code->arg1, code->result);
}

static void codegen_x86_64_sal(FILE *output, IRCode *code)
{
	IRSSAEnt *operand;
	IRSSAEnt *reg_counter = ir_ssa_from_reg(REGISTER_COUNTER);

	fprintf(output, "\t# %s, %d\n", __FUNCTION__, __LINE__);

	if (code->arg2 != reg_counter) {
		codegen_x86_64_mov(output, code->dtype->as.primitive, code->arg2, reg_counter);

		code->arg2 = reg_counter;
	}

	/* optimization to avoid mutation of arg1 and setting the result correct right away */
	if (code->result->type == IR_ATYPE_REG) {
		if (code->result != code->arg1) {
			codegen_x86_64_mov(output, code->dtype->as.primitive, code->arg1, code->result);
		}
		
		operand = code->result;
	} else {
		operand = ir_ssa_from_reg(REGISTER_TEMPORARY);

		codegen_x86_64_mov(output, code->dtype->as.primitive, code->arg1, operand);
	}

	fprintf(output, "\t%s", "sal");
	
	fputs(codegen_x86_64_suffix(code->dtype->as.primitive), output);

	fputs("\t", output);

	codegen_x86_64_fput_operand(output, IR_U8_T, code->arg2);

	fputs(", ", output);

	codegen_x86_64_fput_operand(output, code->dtype->as.primitive, operand);

	fputs("\n", output);

	if (code->result != operand) {
		codegen_x86_64_mov(output, code->dtype->as.primitive, operand, code->result);
	}
}

static void codegen_x86_64_sar(FILE *output, IRCode *code)
{
	IRSSAEnt *operand;
	IRSSAEnt *reg_counter = ir_ssa_from_reg(REGISTER_COUNTER);

	fprintf(output, "\t# %s, %d\n", __FUNCTION__, __LINE__);

	if (code->arg2 != reg_counter) {
		codegen_x86_64_mov(output, code->dtype->as.primitive, code->arg2, reg_counter);

		code->arg2 = reg_counter;
	}

	/* optimization to avoid mutation of arg1 and setting the result correct right away */
	if (code->result->type == IR_ATYPE_REG) {
		if (code->result != code->arg1) {
			codegen_x86_64_mov(output, code->dtype->as.primitive, code->arg1, code->result);
		}
		
		operand = code->result;
	} else {
		operand = ir_ssa_from_reg(REGISTER_TEMPORARY);

		codegen_x86_64_mov(output, code->dtype->as.primitive, code->arg1, operand);
	}

	fprintf(output, "\t%s", "sar");
	
	fputs(codegen_x86_64_suffix(code->dtype->as.primitive), output);

	fputs("\t", output);

	codegen_x86_64_fput_operand(output, IR_U8_T, code->arg2);

	fputs(", ", output);

	codegen_x86_64_fput_operand(output, code->dtype->as.primitive, operand);

	fputs("\n", output);

	if (code->result != operand) {
		codegen_x86_64_mov(output, code->dtype->as.primitive, operand, code->result);
	}
}

static void codegen_x86_64_add(FILE *output, IRCode *code)
{
	codegen_x86_64_bin_op(output, code, "add");
}

static void codegen_x86_64_sub(FILE *output, IRCode *code)
{
	codegen_x86_64_bin_op(output, code, "sub");
}

static void codegen_x86_64_mul(FILE *output, IRCode *code)
{
	codegen_x86_64_bin_op(output, code, "imul");
}

static void codegen_x86_64_div(FILE *output, IRCode *code)
{
	(void)output;
	(void)code;

	assert("TODO: not implemented");
}
/*
static void codegen_x86_64_mod(FILE *output, IRCode *code)
{
	const char *arg1_reg_name;

	(void)ctx;
	
	arg1_reg_name = codegen_x86_64_register_name(code->arg1.reg);

	codegen_x86_64_div(output, code);
	
	switch (code->dtype->as.primitive) {
		case IR_U8_T:
		case IR_S8_T:
		case IR_U16_T:
		case IR_S16_T:
		case IR_U32_T:
		case IR_S32_T: {
			fprintf(output, "\tmovl\t%%edx, %%e%sx", arg1_reg_name);
		} break;
		case IR_U64_T:
		case IR_S64_T:
		case IR_PTR_T: {
			fprintf(output, "\tmovq\t%%rdx, %%r%sx", arg1_reg_name);
		} break;
		case IR_F32_T:
		case IR_F64_T:
			assert(0 && "TODO: not implemented: IR_F32_T and IR_F64_T");
		default:
			assert(0 && "NOT REACHABLE");
	}
	
	fprintf(output, "\n");
}
*/

static void codegen_x86_64_or(FILE *output, IRCode *code)
{
	codegen_x86_64_bin_op(output, code, "or");
}

static void codegen_x86_64_xor(FILE *output, IRCode *code)
{
	codegen_x86_64_bin_op(output, code, "xor");
}

static void codegen_x86_64_and(FILE *output, IRCode *code)
{
	codegen_x86_64_bin_op(output, code, "and");
}

static void codegen_x86_64_not(FILE *output, IRCode *code)
{
	codegen_x86_64_unary_op(output, code, "not");
}

static void codegen_x86_64_eq(FILE *output, IRCode *code)
{
	codegen_x86_64_logical_cmp(output, code, "e");
}

static void codegen_x86_64_neq(FILE *output, IRCode *code)
{
	codegen_x86_64_logical_cmp(output, code, "ne");
}

static void codegen_x86_64_gt(FILE *output, IRCode *code)
{
	codegen_x86_64_logical_cmp(output, code, "g");
}

static void codegen_x86_64_lt(FILE *output, IRCode *code)
{
	codegen_x86_64_logical_cmp(output, code, "l");
}

static void codegen_x86_64_gte(FILE *output, IRCode *code)
{
	codegen_x86_64_logical_cmp(output, code, "ge");
}

static void codegen_x86_64_lte(FILE *output, IRCode *code)
{
	codegen_x86_64_logical_cmp(output, code, "le");
}

static void codegen_x86_64_jmp(FILE *output, IRCode *code)
{
	switch (code->result->type) {
		case IR_ATYPE_NUM: {
			fprintf(output, "\tjmp\t.L%zu\n", code->result->as.num);
		} break;
		case IR_ATYPE_ADDR: {
			fprintf(output, "\tjmp\t.L%zu\n", *code->result->as.addr);
		} break;
		default:
			assert(0 && "NOT REACHABLE");
	}
}

static void codegen_x86_64_jmp_zero(FILE *output, IRCode *code)
{
	IRSSAEnt *label = code->result;

	code->result = ir_ssa_from_reg(REGISTER_TEMPORARY);
	code->arg2 = ir_ssa_from_literal(ir_literal_from_lu(0));

	codegen_x86_64_bin_op(output, code, "cmp");
	
	fprintf(output, "\tje\t.L%zu\n", label->as.num);
}

static void codegen_x86_64_jmp_not_zero(FILE *output, IRCode *code)
{
	IRSSAEnt *label = code->result;

	code->result = ir_ssa_from_reg(REGISTER_TEMPORARY);
	code->arg2 = ir_ssa_from_literal(ir_literal_from_lu(0));

	codegen_x86_64_bin_op(output, code, "cmp");
	
	fprintf(output, "\tjne\t.L%zu\n", label->as.num);
}

static void codegen_x86_64_label(FILE *output, IRCode *code)
{
	switch (code->result->type) {
		case IR_ATYPE_NUM: {
			fprintf(output, ".L%zu:\n", code->result->as.num);
		} break;
		case IR_ATYPE_ADDR: {
			fprintf(output, ".L%zu:\n", *code->result->as.addr);
		} break;
		default:
			assert(0 && "NOT REACHABLE");
	}
}

static void codegen_x86_64_store(FILE *output, IRCode *code)
{
	fprintf(output, "\t# %s, %d\n", __FUNCTION__, __LINE__);

	codegen_x86_64_mov(output, code->dtype->as.primitive, code->arg1, code->result);
}

static void codegen_x86_64_load(FILE *output, IRCode *code)
{
	fprintf(output, "\t# %s, %d\n", __FUNCTION__, __LINE__);

	codegen_x86_64_mov(output, code->dtype->as.primitive, code->arg1, code->result);
}

static void codegen_x86_64_param(FILE *output, IRCode *code)
{
	IRSSAEnt *param;

	fprintf(output, "\t# %s, %d\n", __FUNCTION__, __LINE__);
	
	// TODO
	assert(code->result->as.num <= 6 && "Function Call ABI on x86_64 needs to leave the remaining (p7..pn) arguments on the stack");
	
	param = codegen_x86_64_param_register(code->result->as.num);

	codegen_x86_64_mov(output, code->dtype->as.primitive, code->arg1, param);
}

static void codegen_x86_64_call(FILE *output, IRCode *code)
{
	fprintf(output, "\t# %s, %d\n", __FUNCTION__, __LINE__);

	assert(code->arg1->type == IR_ATYPE_VIEW);

	// TODO really??
	// TODO call from a reg value
	// TODO call from a imm value
	// TODO need to check more
	
	// TODO check type if static no @PLT!
	fprintf(output, "\tcall\t"SV_FMT"@PLT\n", SV_PARAMS(code->arg1->as.view));
	
	// TODO check type
	codegen_x86_64_mov(output, code->dtype->as.primitive, ir_ssa_from_reg(REGISTER_RETURN), code->result);
}

static void codegen_x86_64_string(FILE *output, IRCode *code)
{
	fprintf(output, "\t# %s, %d\n", __FUNCTION__, __LINE__);

	//  assert(code->arg2->as.num == 1 && "TODO: not implemented: arg2 with other value then 1");

	fprintf(output, "\t.section\t.rodata\n");
	fprintf(output, ".L.str.%zu:\n", code->result->as.str);
	fprintf(output, "\t.asciz\t\""SV_FMT"\"\n", SV_PARAMS(&code->arg1->as.literal.sv));
	fprintf(output, "\t.size\t.L.str.%zu, %zu\n", code->result->as.str, code->arg1->as.literal.sv.len);
}

static void codegen_x86_64_ret(FILE *output, IRCode *code)
{
	fprintf(output, "\t# %s, %d\n", __FUNCTION__, __LINE__);

	codegen_x86_64_mov(output, code->dtype->as.primitive, code->result, ir_ssa_from_reg(REGISTER_RETURN));
}
