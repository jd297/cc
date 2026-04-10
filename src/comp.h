#ifndef JD297_CC_COMPILER_C_H
#define JD297_CC_COMPILER_C_H

#include <stddef.h>
#include <stdio.h>

typedef struct {
	FILE *input;
    FILE *output;
} Compiler_C;

extern int compiler_c_run(Compiler_C *compiler);

#endif
