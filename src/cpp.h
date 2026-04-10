#ifndef JD297_CC_CPP
#define JD297_CC_CPP

#include <stddef.h>
#include <stdio.h>

#include <jd297/lmap_sv.h>
#include <jd297/vector.h>

#define JD297_CC_PREPROCESSOR_C_H____DISABLED

typedef struct {
    vector_t *include_dirs;
    vector_t *source_files;
    lmap_t *defines;
    FILE *output;
} Preprocessor_C;

extern int preprocessor_c_run(Preprocessor_C *preprocessor);

extern int preprocessor_c_parse_file(Preprocessor_C *preprocessor, const char* pathname);

#ifndef JD297_CC_PREPROCESSOR_C_H____DISABLED
extern int preprocessor_c_parse_lexer(Preprocessor_C *preprocessor, Lexer_C *lexer, const char* end);

extern int preprocessor_c_parse_next(Preprocessor_C *preprocessor, Lexer_C *lexer);

extern int preprocessor_c_parse_default(Preprocessor_C *preprocessor, Lexer_C *lexer, Token_C *token);
extern int preprocessor_c_parse_include(Preprocessor_C *preprocessor, Lexer_C *lexer, Token_C *token);
extern int preprocessor_c_parse_identifier(Preprocessor_C *preprocessor, Lexer_C *lexer, Token_C *token);
extern int preprocessor_c_parse_define(Preprocessor_C *preprocessor, Lexer_C *lexer, Token_C *token);
extern int preprocessor_c_parse_undef(Preprocessor_C *preprocessor, Lexer_C *lexer, Token_C *token);
extern int preprocessor_c_parse_conditional(Preprocessor_C *preprocessor, Lexer_C *lexer, Token_C *token);
extern int preprocessor_c_parse_line(Preprocessor_C *preprocessor, Lexer_C *lexer, Token_C *token);
extern int preprocessor_c_parse_error(Preprocessor_C *preprocessor, Lexer_C *lexer, Token_C *token);
extern int preprocessor_c_parse_pragma(Preprocessor_C *preprocessor, Lexer_C *lexer, Token_C *token);

#endif

#endif
