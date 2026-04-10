#ifndef JD297_CC_LEX_H
#define JD297_CC_LEX_H

#include <jd297/lex.h>

#include "tok.h"

extern Tok lex_tok;

#define lex_tell() yytext

extern void lex_setpos(char *pos);

#endif
