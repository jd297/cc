#include <assert.h>
#include <stddef.h>

#include "gen.h"
#include "ir.h"

codegen_run_func codegen_func;

IRPrimitiveDataType codegen_get_primitive_data_type(IRGenericPrimitiveDataType gtype)
{
	if (
		codegen_func == codegen_x86_64_run ||
		codegen_func == codegen_aarch64_run
	) {
		switch (gtype) {
			case IR_GENERIC_CHAR:
				return IR_S8_T;
			case IR_GENERIC_SIGNED_CHAR:
				return IR_S8_T;
			case IR_GENERIC_UNSIGNED_CHAR:
				return IR_U8_T;
			case IR_GENERIC_SHORT:
			case IR_GENERIC_SHORT_INT:
			case IR_GENERIC_SIGNED_SHORT:
			case IR_GENERIC_SIGNED_SHORT_INT:
				return IR_S16_T;
			case IR_GENERIC_UNSIGNED_SHORT:
			case IR_GENERIC_UNSIGNED_SHORT_INT:
				return IR_U16_T;
			case IR_GENERIC_INT:
			case IR_GENERIC_SIGNED:
			case IR_GENERIC_SIGNED_INT:
				return IR_S32_T;
			case IR_GENERIC_UNSIGNED:
			case IR_GENERIC_UNSIGNED_INT:
				return IR_U32_T;
			case IR_GENERIC_LONG:
			case IR_GENERIC_LONG_INT:
			case IR_GENERIC_SIGNED_LONG:
			case IR_GENERIC_SIGNED_LONG_INT:
				return IR_S64_T;
			case IR_GENERIC_UNSIGNED_LONG:
			case IR_GENERIC_UNSIGNED_LONG_INT:
				return IR_U64_T;
			case IR_GENERIC_FLOAT:
				return IR_F32_T;
			case IR_GENERIC_DOUBLE:
				return IR_F64_T;
			case IR_GENERIC_LONG_DOUBLE:
				return IR_F128_T;
			default:
				assert(0 && "NOT REACHABLE");
		}
	} else {
		assert(0 && "TODO not implemented: codegen_get_primitive_data_type for CODEGEN_FUNC");
	}
}

size_t codegen_get_type_size(IRDataType *dtype)
{
	if (
		codegen_func == codegen_x86_64_run ||
		codegen_func == codegen_aarch64_run
	) {
		switch (dtype->type) {
			case IR_TYPE_PRIMITIVE: {
				switch (dtype->as.primitive) {
					case IR_U8_T:
					case IR_S8_T:
						return 1;
					case IR_U16_T:
					case IR_S16_T:
						return 2;
					case IR_U32_T:
					case IR_S32_T:
						return 4;
					case IR_U64_T:
					case IR_S64_T:
					case IR_PTR_T:
						return 8;
					case IR_F32_T:
						return 4;
					case IR_F64_T:
						return 8;
					case IR_F128_T:
						return 16;
					default:
						assert(0 && "NOT REACHABLE");
				}
			}
			case IR_TYPE_ENUM:
				return 4;
			case IR_TYPE_STRUCT:
				return dtype->as.structure->size;
			case IR_TYPE_ARRAY:
				return dtype->as.array.size * codegen_get_type_size(dtype->as.array.to);
			case IR_TYPE_POINTER:
				return 8;
			default:
				assert(0 && "NOT REACHABLE");
		}
	} else {
		assert(0 && "TODO not implemented: platform_size for CODEGEN_FUNC");
	}
}
