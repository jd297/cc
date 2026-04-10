#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include <jd297/sv.h>
#include <jd297/list.h>
#include <jd297/lmap_sv.h>

#include "ir.h"

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
