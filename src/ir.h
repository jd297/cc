#ifndef JD297_CC_IR_H
#define JD297_CC_IR_H

#include <stddef.h>

#include <jd297/sv.h>
#include <jd297/list.h>
#include <jd297/lmap_sv.h>

typedef struct IRDataType IRDataType;

typedef enum {
	IR_GENERIC_CHAR,
	IR_GENERIC_SIGNED_CHAR,
	IR_GENERIC_UNSIGNED_CHAR,
	IR_GENERIC_SHORT,
	IR_GENERIC_SHORT_INT,
	IR_GENERIC_SIGNED_SHORT,
	IR_GENERIC_SIGNED_SHORT_INT,
	IR_GENERIC_UNSIGNED_SHORT,
	IR_GENERIC_UNSIGNED_SHORT_INT,
	IR_GENERIC_INT,
	IR_GENERIC_SIGNED,
	IR_GENERIC_SIGNED_INT,
	IR_GENERIC_UNSIGNED,
	IR_GENERIC_UNSIGNED_INT,
	IR_GENERIC_LONG,
	IR_GENERIC_LONG_INT,
	IR_GENERIC_SIGNED_LONG,
	IR_GENERIC_SIGNED_LONG_INT,
	IR_GENERIC_UNSIGNED_LONG,
	IR_GENERIC_UNSIGNED_LONG_INT,
	IR_GENERIC_FLOAT,
	IR_GENERIC_DOUBLE,
	IR_GENERIC_LONG_DOUBLE
} IRGenericPrimitiveDataType;

typedef enum {
	IR_U8_T,
	IR_S8_T,
	IR_U16_T,
	IR_S16_T,
	IR_U32_T,
	IR_S32_T,
	IR_U64_T,
	IR_S64_T,
	IR_F32_T,
	IR_F64_T,
	IR_F128_T,
	IR_PTR_T
} IRPrimitiveDataType;

typedef struct {
	size_t offset;
	IRDataType *to;
} IRStructEntry;

typedef struct {
	size_t size;
	lmap_sv_t *entries; /* TODO ?? */
} IRStructDataType;

typedef struct {
	size_t size;
	IRDataType *to;
} IRArrayDataType;

typedef struct {
	IRDataType *to;
} IRPointerDataType;

typedef enum {
	IR_TYPE_PRIMITIVE,
	IR_TYPE_ENUM,
	IR_TYPE_STRUCT,
	IR_TYPE_ARRAY,
	IR_TYPE_POINTER,
	IR_TYPE_NONE
} IRDataArgType;

#define IR_QUALIFIER_FLAG_NONE     0x00
#define IR_QUALIFIER_FLAG_CONST    0x01
#define IR_QUALIFIER_FLAG_VOLATILE 0x02

#define IR_STORAGE_FLAG_NONE       0x00
#define IR_STORAGE_FLAG_AUTO       0x01
#define IR_STORAGE_FLAG_REGISTER   0x02
#define IR_STORAGE_FLAG_STATIC     0x04
#define IR_STORAGE_FLAG_EXTERN     0x08
#define IR_STORAGE_FLAG_TYPEDEF    0x10

struct IRDataType {
	IRDataArgType type;

	int qualifier_flags;
	int storage_flags;

	union {
		IRPrimitiveDataType primitive;
		lmap_sv_t *enumeration;
		IRStructDataType *structure;
		IRArrayDataType array;
		IRPointerDataType pointer;
	} as;
};

IRDataType *ir_dtype_assign(IRDataType *src);

IRDataType *ir_dtype_from_primitive(IRPrimitiveDataType primitive, int qualifier_flags, int storage_flags);

void ir_dtype_wrap_pointer(IRDataType *ptr, IRDataType *to);

typedef union {
	int d;
	unsigned int u;
	long int ld;
	unsigned long int lu;
	float f;
	double lf;
	long double Lf;
	sv_t sv;
} IRLiteral;

extern IRLiteral ir_literal_from_d(int d);
extern IRLiteral ir_literal_from_u(unsigned int u);
extern IRLiteral ir_literal_from_ld(long int ld);
extern IRLiteral ir_literal_from_lu(unsigned long int lu);
extern IRLiteral ir_literal_from_f(float d);
extern IRLiteral ir_literal_from_lf(double d);
extern IRLiteral ir_literal_from_Lf(long double Lf);
extern IRLiteral ir_literal_from_sv(sv_t sv);

typedef struct IRSSAEnt IRSSAEnt;

typedef struct {
	list_t *code;
	list_t *ssa;
	IRSSAEnt *ssa_latest;
	IRSSAEnt *switch_expression;
	list_node_t *switch_stmt_section;
	list_node_t *switch_test_section;
	list_node_t *code_current;
	size_t stack_offset;
	size_t label_tmp;
	size_t label_func_end;
	size_t label_iter_begin;
	size_t label_iter_end;
	size_t label_select_begin;
	size_t label_select_end;
	size_t label_str;
	IRDataType *function_return_type;
} IR_CTX;

extern IR_CTX *ir_ctx;

extern int ir_ctx_create(void);
extern void ir_ctx_destroy(void);

typedef enum {
	IR_OC_FUNC_BEGIN,
	IR_OC_FUNC_END,
	IR_OC_IMM,
	IR_OC_PUSH,
	IR_OC_POP,
	IR_OC_SAL,
	IR_OC_SAR,
	IR_OC_ADD,
	IR_OC_SUB,
	IR_OC_MUL,
	IR_OC_DIV,
	IR_OC_MOD,
	IR_OC_OR,
	IR_OC_XOR,
	IR_OC_NOT,
	IR_OC_AND,
	IR_OC_EQ,
	IR_OC_NEQ,
	IR_OC_GT,
	IR_OC_LT,
	IR_OC_GTE,
	IR_OC_LTE,
	IR_OC_LABEL,
	IR_OC_JMP,
	IR_OC_JMP_ZERO,
	IR_OC_JMP_NOT_ZERO,
	IR_OC_RET,
	IR_OC_LOCAL,
	IR_OC_LOAD,
	IR_OC_STORE,
	IR_OC_STACK_ALLOC,
	IR_OC_STACK_DEALLOC,
	IR_OC_PARAM,
	IR_OC_CALL,
	IR_OC_STRING,
	IR_OC_LOAD_STRING
} IROpCode;

typedef enum {
	IR_ATYPE_NUM,
	IR_ATYPE_ADDR,
	IR_ATYPE_LIT,
	IR_ATYPE_REG,
	IR_ATYPE_STACK,
	IR_ATYPE_MEM,
	IR_ATYPE_STR,
	IR_ATYPE_VIEW,
	IR_ATYPE_SSA,
	IR_ATYPE_NONE
} IRArgType;
	
struct IRSSAEnt {
	IRArgType type;
	const IRDataType *dtype;
	
	union {
		size_t num;
		size_t str;
		size_t *addr;
		IRLiteral literal;
		size_t reg;
		size_t *stack;
		sv_t mem;
		sv_t *view;
		IRSSAEnt *ssa;
	} as;
};

typedef struct {
	IROpCode op;
	const IRDataType *dtype;
	IRSSAEnt *arg1;
	IRSSAEnt *arg2;
	IRSSAEnt *result;
} IRCode;

extern IRSSAEnt *ir_ssa_latest(void);
extern IRSSAEnt *ir_ssa_default(const IRDataType *dtype);
extern IRSSAEnt *ir_ssa_from_view(sv_t *view);
extern IRSSAEnt *ir_ssa_from_num(size_t num);
extern IRSSAEnt *ir_ssa_from_str(size_t str);
extern IRSSAEnt *ir_ssa_from_stack(size_t *stack);
extern IRSSAEnt *ir_ssa_from_addr(size_t *addr);
extern IRSSAEnt *ir_ssa_from_literal(IRLiteral literal, const IRDataType *dtype);
extern IRSSAEnt *ir_ssa_from_reg(size_t reg, const IRDataType *dtype);
extern IRSSAEnt *ir_ssa_from_ssa(IRSSAEnt *ssa);

extern void ir_emit(IROpCode op, const IRDataType *dtype, IRSSAEnt *result, IRSSAEnt *arg1, IRSSAEnt *arg2);

extern const IRDataType *ir_arithmetic_dtype(const IRDataType *lhs, const IRDataType *rhs);

extern void ir_dump(void);

#endif

