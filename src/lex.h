#ifndef JD297_CC_LEX_H
#define JD297_CC_LEX_H

#include <jd297/lex.h>

#include "tok.h"

extern Tok lex_tok;

extern char *lex_tell(void);

extern void lex_setpos(char *pos);

#endif
