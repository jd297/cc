#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include <jd297/sv.h>
#include <jd297/list.h>
#include <jd297/lmap_sv.h>

#include "ir.h"

IRDataType *ir_dtype_assign(IRDataType *src)
{
	IRDataType *dtype = malloc(sizeof(IRDataType));

	assert(dtype != NULL);

	*dtype = *src;

	return dtype;
}

IRDataType *ir_dtype_from_primitive(IRPrimitiveDataType primitive, int qualifier_flags, int storage_flags)
{
	IRDataType *dtype = malloc(sizeof(IRDataType));

	assert(dtype != NULL);

	dtype->type = IR_TYPE_PRIMITIVE;
	dtype->qualifier_flags = qualifier_flags;
	dtype->storage_flags = storage_flags;
	dtype->as.primitive = primitive;

	return dtype;
}

void ir_dtype_wrap_pointer(IRDataType *ptr, IRDataType *to)
{
	ptr->storage_flags = to->storage_flags;
	ptr->as.pointer.to = to;
}

IRLiteral ir_literal_from_d(int d)
{
	IRLiteral lit;
	
	lit.d = d;
	
	return lit;
}

IRLiteral ir_literal_from_u(unsigned int u)
{
	IRLiteral lit;
	
	lit.u = u;
	
	return lit;
}

IRLiteral ir_literal_from_ld(long int ld)
{
	IRLiteral lit;
	
	lit.ld = ld;
	
	return lit;
}

IRLiteral ir_literal_from_lu(unsigned long int lu)
{
	IRLiteral lit;
	
	lit.lu = lu;
	
	return lit;
}

IRLiteral ir_literal_from_f(float f)
{
	IRLiteral lit;
	
	lit.f = f;
	
	return lit;
}

IRLiteral ir_literal_from_lf(double lf)
{
	IRLiteral lit;
	
	lit.lf = lf;
	
	return lit;
}

IRLiteral ir_literal_from_Lf(long double Lf)
{
	IRLiteral lit;
	
	lit.Lf = Lf;
	
	return lit;
}

IRLiteral ir_literal_from_sv(sv_t sv)
{
	IRLiteral lit;
	
	lit.sv = sv;
	
	return lit;
}

IR_CTX *ir_ctx_create(void)
{
	IR_CTX *ctx;
	list_t *code;
	list_t *ssa;

	ctx = calloc(1, sizeof(IR_CTX));

	assert(ctx != NULL);

	code = malloc(sizeof(list_t));

	assert(code != NULL);
	
	assert(list_create(code) != -1);
	
	ctx->code_current = list_begin(code);

	ssa = malloc(sizeof(list_t));

	assert(ssa != NULL);
	
	assert(list_create(ssa) != -1);

	ctx->code = code;
	ctx->ssa = ssa;
	
	return ctx;
}

void ir_ctx_destroy(IR_CTX *ctx)
{
	assert(ctx != NULL);
	assert(ctx->code != NULL);
	assert(ctx->ssa != NULL);

	/* TODO free entries in code and ssa */

	list_free(ctx->code);
	list_free(ctx->ssa);

	free(ctx->code);
	free(ctx->ssa);
	free(ctx);
}

IRSSAEnt *ir_ssa_latest(IR_CTX *ctx)
{
	return ctx->ssa_latest;
}

IRSSAEnt *ir_ssa_default(IR_CTX *ctx)
{
	IRSSAEnt *ssa = malloc(sizeof(IRSSAEnt));

	ssa->type = IR_ATYPE_NUM;
	ssa->as.num = 0;

	list_insert(ctx->ssa, list_end(ctx->ssa), ssa);

	return ssa;
}

IRSSAEnt *ir_ssa_from_view(IR_CTX *ctx, sv_t *view)
{
	IRSSAEnt *ssa = malloc(sizeof(IRSSAEnt));

	ssa->type = IR_ATYPE_VIEW;
	ssa->as.view = view;

	list_insert(ctx->ssa, list_end(ctx->ssa), ssa);

	return ssa;
}

IRSSAEnt *ir_ssa_from_num(IR_CTX *ctx, size_t num)
{
	IRSSAEnt *ssa = malloc(sizeof(IRSSAEnt));

	ssa->type = IR_ATYPE_NUM;
	ssa->as.num = num;

	list_insert(ctx->ssa, list_end(ctx->ssa), ssa);

	return ssa;
}

IRSSAEnt *ir_ssa_from_str(IR_CTX *ctx, size_t str)
{
	IRSSAEnt *ssa = malloc(sizeof(IRSSAEnt));

	ssa->type = IR_ATYPE_STR;
	ssa->as.str = str;

	list_insert(ctx->ssa, list_end(ctx->ssa), ssa);

	return ssa;
}

IRSSAEnt *ir_ssa_from_stack(IR_CTX *ctx, size_t *stack)
{
	IRSSAEnt *ssa = malloc(sizeof(IRSSAEnt));

	ssa->type = IR_ATYPE_STACK;
	ssa->as.stack = stack;

	list_insert(ctx->ssa, list_end(ctx->ssa), ssa);

	return ssa;
}

IRSSAEnt *ir_ssa_from_addr(IR_CTX *ctx, size_t *addr)
{
	IRSSAEnt *ssa = malloc(sizeof(IRSSAEnt));

	ssa->type = IR_ATYPE_ADDR;
	ssa->as.addr = addr;

	list_insert(ctx->ssa, list_end(ctx->ssa), ssa);

	return ssa;
}

IRSSAEnt *ir_ssa_from_literal(IR_CTX *ctx, IRLiteral literal)
{
	IRSSAEnt *ssa = malloc(sizeof(IRSSAEnt));

	ssa->type = IR_ATYPE_LIT;
	ssa->as.literal = literal;

	list_insert(ctx->ssa, list_end(ctx->ssa), ssa);

	return ssa;
}

IRSSAEnt *ir_ssa_from_reg(IR_CTX *ctx, size_t reg)
{
	IRSSAEnt *ssa = malloc(sizeof(IRSSAEnt));

	ssa->type = IR_ATYPE_REG;
	ssa->as.reg = reg;

	list_insert(ctx->ssa, list_end(ctx->ssa), ssa);

	return ssa;
}

IRSSAEnt *ir_ssa_from_ssa(IR_CTX *ctx, IRSSAEnt *ssa)
{
	IRSSAEnt *ent = malloc(sizeof(IRSSAEnt));

	ent->type = IR_ATYPE_SSA;
	ent->as.ssa = ssa;

	list_insert(ctx->ssa, list_end(ctx->ssa), ent);

	return ent;
}

void ir_emit(IR_CTX *ctx, IROpCode op, IRDataType *dtype, IRSSAEnt *result, IRSSAEnt *arg1, IRSSAEnt *arg2)
{
	IRCode *code;

	assert(ctx != NULL);

	code = malloc(sizeof(IRCode));

	assert(code != NULL);

	code->op = op;
	code->dtype = dtype;
	code->arg1 = arg1;
	code->arg2 = arg2;
	code->result = result;

	ctx->code_current = list_insert(ctx->code, ctx->code_current == list_end(ctx->code) ? ctx->code_current : list_next(ctx->code_current), code);
	
	if (result != NULL) {
		ctx->ssa_latest = result;
	}
}
/* -------------------------------------------------------------------------- */
#include <stdio.h>

static const char *ir_opcode_to_str(IROpCode op)
{
	switch (op) {
		case IR_OC_FUNC_BEGIN: return "IR_OC_FUNC_BEGIN";
		case IR_OC_FUNC_END: return "IR_OC_FUNC_END";
		case IR_OC_IMM: return "IR_OC_IMM";
		case IR_OC_PUSH: return "IR_OC_PUSH";
		case IR_OC_POP: return "IR_OC_POP";
		case IR_OC_SAL: return "IR_OC_SAL";
		case IR_OC_SAR: return "IR_OC_SAR";
		case IR_OC_ADD: return "IR_OC_ADD";
		case IR_OC_SUB: return "IR_OC_SUB";
		case IR_OC_MUL: return "IR_OC_MUL";
		case IR_OC_DIV: return "IR_OC_DIV";
		case IR_OC_MOD: return "IR_OC_MOD";
		case IR_OC_OR: return "IR_OC_OR";
		case IR_OC_XOR: return "IR_OC_XOR";
		case IR_OC_NOT: return "IR_OC_NOT";
		case IR_OC_AND: return "IR_OC_AND";
		case IR_OC_EQ: return "IR_OC_EQ";
		case IR_OC_NEQ: return "IR_OC_NEQ";
		case IR_OC_GT: return "IR_OC_GT";
		case IR_OC_LT: return "IR_OC_LT";
		case IR_OC_GTE: return "IR_OC_GTE";
		case IR_OC_LTE: return "IR_OC_LTE";
		case IR_OC_LABEL: return "IR_OC_LABEL";
		case IR_OC_JMP: return "IR_OC_JMP";
		case IR_OC_JMP_ZERO: return "IR_OC_JMP_ZERO";
		case IR_OC_JMP_NOT_ZERO: return "IR_OC_JMP_NOT_ZERO";
		case IR_OC_RET: return "IR_OC_RET";
		case IR_OC_LOCAL: return "IR_OC_LOCAL";
		case IR_OC_LOAD: return "IR_OC_LOAD";
		case IR_OC_STORE: return "IR_OC_STORE";
		case IR_OC_STACK_ALLOC: return "IR_OC_STACK_ALLOC";
		case IR_OC_STACK_DEALLOC: return "IR_OC_STACK_DEALLOC";
		case IR_OC_PARAM: return "IR_OC_PARAM";
		case IR_OC_CALL: return "IR_OC_CALL";
		case IR_OC_STRING: return "IR_OC_STRING";
		case IR_OC_LOAD_STRING: return "IR_OC_LOAD_STRING";
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

static void ir_dump_dtype(IRDataType *dtype)
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
			ir_dump_dtype(dtype->as.pointer.to);
			printf("}");
		} break;
		case IR_TYPE_NONE: {
			printf("NONE");
		} break;
		default: assert(0 && "NOT REACHABLE");
	}
}

static void ir_dump_ssa_ent(IRSSAEnt *ssa)
{
	printf("HIER");
}

static void ir_dump_code(IRCode *code)
{
	printf("\t_%p [shape=record]\n", (void *)code);
	printf("\t_%p [", (void *)code);
	
	printf("label=\"%s", ir_opcode_to_str(code->op));
	
	if (code->dtype != NULL) {
		printf(" | { DTYPE | ");
		ir_dump_dtype(code->dtype);
		printf("}");
	}
	
	if (code->result != NULL) {
		printf(" | { RESULT | ");
		ir_dump_ssa_ent(code->result);
		printf("}");	
	}
	
	if (code->arg1 != NULL) {
		printf(" | { ARG1 | ");
		ir_dump_ssa_ent(code->arg1);
		printf("}");	
	}
	
	if (code->arg2 != NULL) {
		printf(" | { ARG2 | ");
		ir_dump_ssa_ent(code->arg2);
		printf("}");	
	}
	
	printf("\"]\n");
}

void ir_dump(IR_CTX *ctx)
{
	printf("digraph graphname {\n");

	for (list_node_t *it = list_begin(ctx->code); it != list_end(ctx->code); it = list_next(it)) {
		list_node_t *prev;
		
		ir_dump_code(it->value);
		
		prev = list_prev(it);

		if (prev == NULL) {
			continue;
		}

		printf("\t_%p -> _%p\n", (void *)prev->value, (void *)it->value);
	}
	
	printf("}\n");
}
