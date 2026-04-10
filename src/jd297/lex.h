#ifndef JD297_LEX_H
#define JD297_LEX_H

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define YY_NULL 0

#define yyconst const

#ifndef YY_TYPEDEF_YY_SIZE_T
#define YY_TYPEDEF_YY_SIZE_T
typedef size_t yy_size_t;
#endif

#ifndef YY_TYPEDEF_YY_BUFFER_STATE
#define YY_TYPEDEF_YY_BUFFER_STATE
typedef struct yy_buffer_state *YY_BUFFER_STATE;
#endif

#define YY_END_OF_BUFFER_CHAR 0

#ifndef YY_STRUCT_YY_BUFFER_STATE
#define YY_STRUCT_YY_BUFFER_STATE
struct yy_buffer_state {
	FILE *yy_input_file;
	char *yy_ch_buf;
	char *yy_buf_pos;
	yy_size_t yy_buf_size;
	yy_size_t yy_n_chars;
	int yy_at_bol;
	int yy_bs_lineno;
	int yy_bs_column;
	int yy_buffer_status;

#define YY_BUFFER_NEW 0
#define YY_BUFFER_NORMAL 1
#define YY_BUFFER_EOF_PENDING 2
};
#endif /* !YY_STRUCT_YY_BUFFER_STATE */

#define yyalloc(size)			malloc((size))
#define yyrealloc(ptr, size)	realloc((char *)(ptr), (size))
#define yyfree(ptr)				free((ptr))

extern char *yytext;
#define yytext_ptr yytext

extern yy_size_t yyleng;

extern int yylex(void);

extern YY_BUFFER_STATE yy_create_buffer(FILE *file, int size);

extern void yy_switch_to_buffer(YY_BUFFER_STATE new_buffer);

extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

extern void yypush_buffer_state(YY_BUFFER_STATE new_buffer);

extern void yypop_buffer_state(void);

extern void yy_flush_buffer(YY_BUFFER_STATE b);

extern YY_BUFFER_STATE yy_scan_buffer(char * base, yy_size_t  size);

#endif

#ifdef JD297_LEX_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *yyin, *yyout;

char *yytext;

yy_size_t yyleng;

#define EOB_ACT_CONTINUE_SCAN 0
#define EOB_ACT_END_OF_FILE 1
#define EOB_ACT_LAST_MATCH 2

#define YY_MORE_ADJ 0

#ifndef YY_READ_BUF_SIZE
#define YY_READ_BUF_SIZE 8192
#endif

#ifndef YY_FATAL_ERROR
#define YY_FATAL_ERROR(msg)		yy_fatal_error((msg))
#endif

#ifndef YY_EXIT_FAILURE
#define YY_EXIT_FAILURE 2
#endif

#define YY_CURRENT_BUFFER ( (yy_buffer_stack) \
                          ? (yy_buffer_stack)[(yy_buffer_stack_top)] \
                          : NULL)

#define YY_CURRENT_BUFFER_LVALUE (yy_buffer_stack)[(yy_buffer_stack_top)]

static char yy_hold_char;
static yy_size_t yy_n_chars;

static char *yy_c_buf_p = NULL;

static size_t yy_buffer_stack_top = 0;
static size_t yy_buffer_stack_max = 0;
static YY_BUFFER_STATE *yy_buffer_stack = NULL;

static void yy_fatal_error(yyconst char* msg);

static void yy_load_buffer_state(void);

static void yy_init_buffer(YY_BUFFER_STATE b, FILE *file);

static void yyensure_buffer_stack(void);

#ifndef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
                { \
                errno=0; \
                while ( (result = fread(buf, 1, max_size, yyin))==0 && ferror(yyin)) \
                        { \
                        if( errno != EINTR) \
                                { \
                                YY_FATAL_ERROR( "input in flex scanner failed" ); \
                                break; \
                                } \
                        errno=0; \
                        clearerr(yyin); \
                        } \
                }\
\

#endif

static void yy_fatal_error(yyconst char* msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(YY_EXIT_FAILURE);
}

static void yy_init_buffer(YY_BUFFER_STATE b, FILE *file)
{
	int oerrno = errno;

	yy_flush_buffer(b);

	b->yy_input_file = file;
	/* b->yy_fill_buffer = 1; */ /* TODO ?? */

	if (b != YY_CURRENT_BUFFER){
		b->yy_bs_lineno = 1;
		b->yy_bs_column = 0;
	}

	errno = oerrno;
}

static void yy_load_buffer_state(void)
{
	(yy_n_chars) = YY_CURRENT_BUFFER_LVALUE->yy_n_chars;
	(yytext_ptr) = (yy_c_buf_p) = YY_CURRENT_BUFFER_LVALUE->yy_buf_pos;
	yyin = YY_CURRENT_BUFFER_LVALUE->yy_input_file;
	(yy_hold_char) = *(yy_c_buf_p);
}

static void yyensure_buffer_stack(void)
{
        yy_size_t num_to_alloc;

        if (!(yy_buffer_stack)) {
                num_to_alloc = 8; /* TODO JD297_LEX_INIT_STACK_ALLOC_SIZE */

                (yy_buffer_stack) = (struct yy_buffer_state**)yyalloc
                                                                (num_to_alloc * sizeof(struct yy_buffer_state*)
                                                                );
                if ( ! (yy_buffer_stack) )
                        YY_FATAL_ERROR( "out of dynamic memory in yyensure_buffer_stack()" );

                memset((yy_buffer_stack), 0, num_to_alloc * sizeof(struct yy_buffer_state*));

                (yy_buffer_stack_max) = num_to_alloc;
                (yy_buffer_stack_top) = 0;
                return;
        }

        if ((yy_buffer_stack_top) >= ((yy_buffer_stack_max)) - 1){

                /* Increase the buffer to prepare for a possible push. */
                int grow_size = 8; /* TODO JD297_LEX_GROW_STACK_ALLOC_SIZE */

                num_to_alloc = (yy_buffer_stack_max) + grow_size;
                (yy_buffer_stack) = (struct yy_buffer_state**)yyrealloc
                                                                ((yy_buffer_stack),
                                                                num_to_alloc * sizeof(struct yy_buffer_state*)
                                                                );
                if ( ! (yy_buffer_stack) )
                        YY_FATAL_ERROR( "out of dynamic memory in yyensure_buffer_stack()" );

                /* zero only the new slots.*/
                memset(yy_buffer_stack + (yy_buffer_stack_max), 0, grow_size * sizeof(struct yy_buffer_state*));
                yy_buffer_stack_max = num_to_alloc;
        }
}

extern void yy_flush_buffer(YY_BUFFER_STATE b)
{
	if (b == NULL) {
		return;
	}

	b->yy_n_chars = 0;

	b->yy_ch_buf[0] = YY_END_OF_BUFFER_CHAR;
	b->yy_ch_buf[1] = YY_END_OF_BUFFER_CHAR;

	b->yy_buf_pos = &b->yy_ch_buf[0];

	/* b->yy_at_bol = 1; */ /* TODO ?? */
	b->yy_buffer_status = YY_BUFFER_NEW;

	if (b == YY_CURRENT_BUFFER) {
		yy_load_buffer_state();
	}
}

extern YY_BUFFER_STATE yy_create_buffer(FILE *file, int size)
{
	YY_BUFFER_STATE b;

	b = (YY_BUFFER_STATE)yyalloc(sizeof(struct yy_buffer_state));

	if (b == NULL) {
		YY_FATAL_ERROR("out of dynamic memory in yy_create_buffer()");
	}

	b->yy_buf_size = size;

	b->yy_ch_buf = (char *)yyalloc(b->yy_buf_size + 2);

	if (b->yy_ch_buf == NULL) {
		YY_FATAL_ERROR("out of dynamic memory in yy_create_buffer()");
	}

	yy_init_buffer(b, file);

	return b;
}

extern void yy_switch_to_buffer  (YY_BUFFER_STATE  new_buffer )
{
    
	/* TODO. We should be able to replace this entire function body
	 * with
	 *		yypop_buffer_state();
	 *		yypush_buffer_state(new_buffer);
     */
	yyensure_buffer_stack ();
	/*if ( YY_CURRENT_BUFFER == new_buffer )
		return;*/

	if ( YY_CURRENT_BUFFER )
		{
		/* Flush out information for old buffer. */
		*(yy_c_buf_p) = (yy_hold_char);
		YY_CURRENT_BUFFER_LVALUE->yy_buf_pos = (yy_c_buf_p);
		YY_CURRENT_BUFFER_LVALUE->yy_n_chars = (yy_n_chars);
		}

	YY_CURRENT_BUFFER_LVALUE = new_buffer;
	yy_load_buffer_state();

	/* We don't actually know whether we did this switch during
	 * EOF (yywrap()) processing, but the only time this flag
	 * is looked at is after yywrap() is called, so it's safe
	 * to go ahead and always set it.
	 */
	/* (yy_did_buffer_switch_on_eof) = 1; */
}

extern void yy_delete_buffer(YY_BUFFER_STATE b)
{
	if ( ! b )
                return;

        if ( b == YY_CURRENT_BUFFER ) /* Not sure if we should pop here. */ /* TODO ?? */
                YY_CURRENT_BUFFER_LVALUE = (YY_BUFFER_STATE) 0;

        /* if ( b->yy_is_our_buffer ) */ /* TODO DELETE it's our buffer anyway */
                yyfree((void *) b->yy_ch_buf  );

        yyfree((void *) b  );
}

extern void yypush_buffer_state(YY_BUFFER_STATE new_buffer)
{
        if (new_buffer == NULL)
                return;

        yyensure_buffer_stack();

        /* This block is copied from yy_switch_to_buffer. */
        if ( YY_CURRENT_BUFFER )
                {
                /* Flush out information for old buffer. */
                *(yy_c_buf_p) = (yy_hold_char);
                YY_CURRENT_BUFFER_LVALUE->yy_buf_pos = (yy_c_buf_p);
                YY_CURRENT_BUFFER_LVALUE->yy_n_chars = (yy_n_chars);
                }

        /* Only push if top exists. Otherwise, replace top. */
        if (YY_CURRENT_BUFFER)
                (yy_buffer_stack_top)++;
        YY_CURRENT_BUFFER_LVALUE = new_buffer;

        /* copied from yy_switch_to_buffer. */
        yy_load_buffer_state( );
        /* (yy_did_buffer_switch_on_eof) = 1; */ /* TODO DELETE we don't support yywrap() anyway */
}

extern void yypop_buffer_state(void)
{
        if (!YY_CURRENT_BUFFER)
            return;

        yy_delete_buffer(YY_CURRENT_BUFFER );
        YY_CURRENT_BUFFER_LVALUE = NULL;
        if ((yy_buffer_stack_top) > 0)
                --(yy_buffer_stack_top);

        if (YY_CURRENT_BUFFER) {
                yy_load_buffer_state( );
                /* (yy_did_buffer_switch_on_eof) = 1; */ /* TODO DELETE we don't support yywrap() anyway */
        }
}

YY_BUFFER_STATE yy_scan_buffer(char * base, yy_size_t  size)
{
	YY_BUFFER_STATE b;
    
	if ( size < 2 ||
	     base[size-2] != YY_END_OF_BUFFER_CHAR ||
	     base[size-1] != YY_END_OF_BUFFER_CHAR )
		/* They forgot to leave room for the EOB's. */
		return 0;

	b = (YY_BUFFER_STATE) yyalloc(sizeof( struct yy_buffer_state )  );
	if ( ! b )
		YY_FATAL_ERROR( "out of dynamic memory in yy_scan_buffer()" );

	b->yy_buf_size = size - 2;	/* "- 2" to take care of EOB's */
	b->yy_buf_pos = b->yy_ch_buf = base;
	/* b->yy_is_our_buffer = 0; */
	b->yy_input_file = 0;
	b->yy_n_chars = b->yy_buf_size;
	/* b->yy_is_interactive = 0; */
	b->yy_at_bol = 1;
	/* b->yy_fill_buffer = 0; */
	b->yy_buffer_status = YY_BUFFER_NEW;

	yy_switch_to_buffer(b);

	return b;
}

#endif
