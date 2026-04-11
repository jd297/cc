#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "comp.h"
#include "parse.h"
#include "symtbl.h"
#include "ir.h"
#include "irgen.h"
#include "opt.h"
#include "gen.h"

#include <jd297/lex.h>
#include <jd297/lmap_sv.h>
#include <jd297/list.h>

int compiler_c_run(Compiler_C *compiler)
{
	int yyparse_result;
	char *src;
	size_t bytes, nread;

	(void)fseek(compiler->input, 0L, SEEK_END);

	bytes = ftell(compiler->input) + 2; /* lex/flex portability (... + 2) */

	(void)fseek(compiler->input, 0L, SEEK_SET);

	if ((src = (char *)malloc(bytes)) == NULL) {
		return -1;
	}

	src[bytes-2] = '\0'; /* lex/flex portability */
	src[bytes-1] = '\0'; /* lex/flex portability */

	while ((nread = fread(src, sizeof(char), bytes, compiler->input)) == 0 && ferror(compiler->input)) {
		if(errno != EINTR) {
			break;
        }
        
        errno=0;

        clearerr(compiler->input);
    }

	if (ferror(compiler->input)) {
		return -1;
	}

	if (nread != (bytes - 2)) { /* SANITYCHECK | LOST BYTES SOMEHOW */
		return -1;
	}

	fclose(compiler->input);

	YY_BUFFER_STATE yystate = yy_scan_buffer(src, bytes);

	yyparse_result = yyparse();

	if (yyparse_result == -1) {
		fprintf(stderr, "%d errors generated.", parse_error_count);
		return -1;
	}

	codegen_func = codegen_x86_64_run; /* TODO HARD */

	/*
	Symtbl *symtbl_root = symtbl_create(NULL);
	
	Semantic_C_CTX semantic_ctx = {
		.error_count = 0,
		.symtbl = symtbl_root
	};

	semantic_c_run(&semantic_ctx, translation_unit);

	if (semantic_ctx.error_count > 0) {
		fprintf(stderr, "%d errors generated.", semantic_ctx.error_count);

		return -1;
	}
	*/

	IR_CTX *ir_ctx;

	assert((ir_ctx = ir_ctx_create()) != NULL);

	irgen_c_run(ir_ctx, parse_result);

	assert(optimizer_run(ir_ctx) == 0);

	codegen_func(ir_ctx, compiler->output);

	parse_tree_node_destroy(parse_result);

	yy_delete_buffer(yystate);

	ir_ctx_destroy(ir_ctx);

    return 0;
}
