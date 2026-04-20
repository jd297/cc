#ifndef JD297_CC_LEX_H
#define JD297_CC_LEX_H

#include <jd297/lex.h>

#include "tok.h"

typedef struct {
	int lineno;
	char *line_begin;
	char *current;
} LexPos;

#define lex_pos_last (LexPos){ lex_lineno, lex_line_begin_p, yytext }

extern Tok lex_tok;

extern char *lex_line_begin_p;

extern int lex_lineno;

#define lex_col ((yytext - lex_line_begin_p) + 1)

extern LexPos lex_tell(void);

extern void lex_setpos(LexPos pos);

#endif
