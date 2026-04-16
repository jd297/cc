#ifndef JD297_CC_CODEGEN_H
#define JD297_CC_CODEGEN_H

#include <stdio.h>

#include "ir.h"

typedef void (*codegen_run_func)(FILE *output);

extern codegen_run_func codegen_func;

extern IRPrimitiveDataType codegen_get_primitive_data_type(IRGenericPrimitiveDataType gtype);

extern size_t codegen_get_type_size(IRDataType *dtype);

extern void codegen_x86_64_run(FILE *output);

extern void codegen_aarch64_run(FILE *output);

#endif
