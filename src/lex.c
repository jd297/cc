#define JD297_LEX_IMPLEMENTATION
#include <jd297/lex.h>
#undef JD297_LEX_IMPLEMENTATION
#include <jd297/sv.h>

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lex.h"
#include "lex_kw.h"
#include "tok.h"

#define lex_start_p yytext_ptr
#define lex_current_p yy_c_buf_p

#define lex_eof() (*lex_current_p == YY_NULL)

#define lex_advance() (*lex_current_p++)

#define lex_peek(n) *(lex_current_p + (n))

#define lex_tok_len() (lex_current_p - lex_start_p)

#define lex_emit_single(tok, val) tok = (val); goto LEX_EMIT_SINGLE

#define lex_emit_multi(tok, val) tok = (val); goto LEX_EMIT_MULTI

#define lex_emit_literal(tok, val) tok = (val); goto LEX_EMIT_LITERAL

#define lex_emit_cache() lex_cache = 0; goto LEX_EMIT_CACHE

static int lex_cache = 0;

static int lex_match(char c)
{
	if (*lex_current_p != c) {
		return -1;
	}

	lex_current_p++;

	return 0;
}

extern void lex_setpos(char *pos)
{
	/* TODO if (pos == yytext) {
		lex_cache = 1;
	}
	*/

	lex_current_p = pos;
}

Tok lex_tok;

extern int yylex(void)
{
	int tok; /* TODO TokenType_C ?? */
	/* TODO col feature const char *lex_line_begin_p = lex_current_p; */

	if (lex_cache) {
		lex_emit_cache();
	}

LEX_AGAIN:
	lex_start_p = lex_current_p;

	switch (lex_advance())
	{
		default:
			fprintf(stderr, "lexical error: unexpected character: \'%c\'\n", *lex_start_p);

			lex_emit_single(tok, T_UNKNOWN);
		case '\0':
			lex_current_p = lex_start_p;

			return YY_NULL;
        case '(':
		case ')':
		case '[':
		case ']':
		case '{':
		case '}':
		case '~':
		case ',':
		case ':':
		case ';':
		case '?':
			lex_emit_single(tok, *lex_start_p);
		case '#': {
			assert(0 && "TODO not implemented CPP directive");
		} break;
		case '=':
			lex_emit_multi(tok, lex_match('=') == 0 ? T_EQUAL_TO : T_ASSIGNMENT);
		case '*':
			lex_emit_multi(tok, lex_match('=') == 0 ? T_MULTIPLY_ASSIGN : T_MULTIPLY);
		case '%':
			lex_emit_multi(tok, lex_match('=') == 0 ? T_MODULUS_ASSIGN : T_MODULUS);
		case '^':
			lex_emit_multi(tok, lex_match('=') == 0 ? T_BITWISE_XOR_ASSIGN : T_BITWISE_XOR);
		case '!':
			lex_emit_multi(tok, lex_match('=') == 0 ? T_NOT_EQUAL_TO : T_LOGICAL_NOT);
		case '|':
        	lex_emit_multi(tok, lex_match('=') == 0 ? T_BITWISE_OR_ASSIGN : lex_match('|') == 0 ? T_LOGICAL_OR : T_BITWISE_OR);
    	case '&':
        	lex_emit_multi(tok, lex_match('=') == 0 ? T_BITWISE_AND_ASSIGN : lex_match('&') == 0 ? T_LOGICAL_AND : T_BITWISE_AND);
    	case '+':
			lex_emit_multi(tok, lex_match('=') == 0 ? T_PLUS_ASSIGN : lex_match('+') == 0 ? T_INCREMENT : T_PLUS);
		case '-':
			lex_emit_multi(tok, lex_match('=') == 0 ? T_MINUS_ASSIGN : lex_match('-') == 0 ? T_DECREMENT : lex_match('>') == 0 ? T_ARROW : T_MINUS);
		case '<':
			lex_emit_multi(tok, lex_match('=') == 0 ? T_LESS_THAN_OR_EQUAL_TO : lex_match('<') == 0 ? lex_match('=') == 0 ? T_BITWISE_LEFTSHIFT_ASSIGN : T_BITWISE_LEFTSHIFT : T_LESS_THAN);
		case '>':
			lex_emit_multi(tok, lex_match('=') == 0 ? T_GREATER_THAN_OR_EQUAL_TO : lex_match('>') == 0 ? lex_match('=') == 0 ? T_BITWISE_RIGHTSHIFT_ASSIGN : T_BITWISE_RIGHTSHIFT : T_GREATER_THAN);
		case '/': {
			if (lex_match('/') == 0) {
				while (lex_peek(0) != '\n' && !lex_eof()) {
					lex_advance();
				}

				goto LEX_AGAIN;
			}

			if (lex_match('*') == 0) {
				while (1) {
					if (lex_peek(0) == '*' && lex_peek(1) == '/') {
						lex_advance();
						lex_advance();

						lex_start_p = lex_current_p;

						goto LEX_AGAIN;
					}

					if (lex_eof()) {
						fprintf(stderr, "lexical error: unterminated /* comment\n");

						lex_emit_single(tok, *lex_start_p);
					}
					
					if (lex_peek(0) == '\n') {
						/*
						TODO REIMPLEMENT THIS FEATURE
						++lexer->loc.line;
						lexer->loc.col = 0;
						*/

						lex_current_p++;

						continue;
					}
					
					lex_advance();
				}
			}
			
			lex_emit_multi(tok, lex_match('=') == 0 ? T_DIVIDE_ASSIGN : T_DIVIDE);
		}
		case '.': {
			switch (lex_peek(0)) {
				case '0': case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9':
					goto L_FLOATING_CONSTANT;
				case '.': {
					if (lex_peek(1) == '.') {
						lex_current_p += 2;

						lex_emit_multi(tok, T_DOT_DOT_DOT);
					}
				}
				default:
					lex_emit_single(tok, *lex_start_p);
			}
		}
		case 'L': {
			if (lex_peek(0) == '\'') {
				goto L_CHARACTER_CONSTANT;
			}

			if (lex_peek(0) == '\"') {
				goto L_STRING_LITERAL;
			}
		}
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        case 'G': case 'H': case 'I': case 'J': case 'K': 
        case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
        case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
        case 'Y': case 'Z':
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
        case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
        case 's': case 't': case 'u': case 'v': case 'w': case 'x':
        case 'y': case 'z': 
        case '_': {
        	const struct lex_kw_entry *kw;

        	while (isalnum(*lex_current_p) != 0  || *lex_current_p == '_') {
        		lex_advance();
        	}

			kw = lex_lookup_kw(lex_start_p, lex_tok_len());

			lex_emit_multi(tok, kw == NULL ? T_IDENTIFIER : kw->type);
        }
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9': {
			char *endptr;
			unsigned long int lu;
			
			errno = 0;
			lu = strtoul(lex_start_p, &endptr, 0);
			
			// TODO errno ERANGE
			// TODO error: integer literal is too large to be represented in any integer type
			
			// TODO u U + l L => advance
			// TODO error: invalid suffix 'ZZ' on integer constant

			switch (*endptr) {
				case 'f': case 'F':
				case 'e': case 'E':
				case '.':
					goto L_FLOATING_CONSTANT;
			}
			
			lex_current_p = endptr;
			lex_tok.literal.lu = lu;

			lex_emit_literal(tok, T_INTEGER_CONSTANT);
		}
L_FLOATING_CONSTANT: {
			char *endptr;
			long double Lf;
			
			errno = 0;
			Lf = strtold(lex_start_p, &endptr);
			
			// TODO errno ERANGE
			//  warning: magnitude of floating-point constant too large for type 'double'
			
			// TODO F f | l L => advance
			// TODO error: invalid suffix 'ZZ' on floating constant

			lex_current_p = endptr; // TODO advance location | check everywhere because it was probably forgoten sometimes !!
			lex_tok.literal.Lf = Lf;

			lex_emit_literal(tok, T_FLOATING_CONSTANT);
		}
L_CHARACTER_CONSTANT:
			lex_advance();
		case '\'': {
			enum {
				C_LIT_CHAR,
				C_LIT_ESC,
				C_LIT_HEX,
				C_LIT_OCT
			} type = C_LIT_CHAR;
		
			char *end;

			for (end = lex_current_p; *end != '\''; ++end) {
				if (*end == '\n' || *end == '\0') {
					fprintf(stderr, "lexical error: missing terminating ' character\n");
					
					lex_start_p = lex_current_p = end;

					goto LEX_AGAIN;
				}
			}

			switch (lex_advance()) {
				case '\'': {
					fprintf(stderr, "lexical error: empty character constant\n");

					lex_start_p = lex_current_p;
			
					goto LEX_AGAIN;
				}
				case '\\': {
					switch (lex_advance()) {
						case 'n': case 't': case 'v': case 'b':
						case 'r': case 'f': case 'a': case '\\':
						case '?': case '\'': case '\"': {
							if (lex_advance() != '\'') {
								fprintf(stderr, "lexical error: multi-character character constant\n");
						
								lex_start_p = lex_current_p = end + 1;
						
								goto LEX_AGAIN;
							}
							
							type = C_LIT_ESC;
						} break;
						case 'x': {
							for (size_t i = 0; ; i++) {
								switch (lex_advance()) {
									case '0': case '1': case '2': case '3': case '4':
									case '5': case '6': case '7': case '8': case '9':
									case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
									case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
										if (i >= 2) {
											fprintf(stderr, "lexical error: hex escape sequence out of range\n");

											lex_start_p = lex_current_p = end + 1;
											
											goto LEX_AGAIN;
										}

										continue;
									default: {
										fprintf(stderr, "lexical error: \\x used with no following hex digits\n");
							
										lex_start_p = lex_current_p = end + 1;
								
										goto LEX_AGAIN;
									}
									case '\'':
										if (i == 0) {
											fprintf(stderr, "lexical error: \\x used with no following hex digits\n");

											lex_start_p = lex_current_p = end + 1;
											
											goto LEX_AGAIN;
										}
										
										break;
								}

								type = C_LIT_HEX;

								break;
							}
						} break;
						case '0': case '1': case '2': case '3':
						case '4': case '5': case '6': case '7': {
							for (size_t i = 0; ; i++) {
								switch (lex_advance()) {
									case '0': case '1': case '2': case '3':
									case '4': case '5': case '6': case '7':
										if (i >= 2) {
											fprintf(stderr, "lexical error: octal escape sequence out of range\n");

											lex_start_p = lex_current_p = end + 1;
											
											goto LEX_AGAIN;
										}

										continue;
									default: {
										fprintf(stderr, "lexical error: octal escape sequence used with no following octal digits\n");
							
										lex_start_p = lex_current_p = end + 1;
								
										goto LEX_AGAIN;
									}
									case '\'':
										break;
								}

								type = C_LIT_OCT;

								break;
							}
						} break;
						default: {
							fprintf(stderr, "lexical error: unknown escape sequence\n");
						
							lex_start_p = lex_current_p = end + 1;
					
							goto LEX_AGAIN;
						}
					}
				} break;
				default: {
					if (lex_advance() != '\'') {
						fprintf(stderr, "lexical error: multi-character character constant\n");
						
						lex_start_p = lex_current_p = end + 1;
				
						goto LEX_AGAIN;
					}
				} break;
			}
			
			switch (type) {
				case C_LIT_CHAR: {
					lex_tok.literal.lu = *(lex_current_p - 2);
				} break;
				case C_LIT_ESC: {
					switch (*(lex_current_p - 2)) {
						case 'n':  lex_tok.literal.lu = '\n'; break;
						case 't':  lex_tok.literal.lu = '\t'; break;
						case 'v':  lex_tok.literal.lu = '\v'; break;
						case 'b':  lex_tok.literal.lu = '\b'; break;
						case 'r':  lex_tok.literal.lu = '\r'; break;
						case 'f':  lex_tok.literal.lu = '\f'; break;
						case 'a':  lex_tok.literal.lu = '\a'; break;
						case '\\': lex_tok.literal.lu = '\\'; break;
						case '?':  lex_tok.literal.lu = '\?'; break;
						case '\'': lex_tok.literal.lu = '\''; break;
						case '\"': lex_tok.literal.lu = '\"'; break;
					}
				} break;
				case C_LIT_HEX: {
					char *endptr;

					lex_tok.literal.lu = strtoul(lex_start_p + 3, &endptr, 16);
				} break;
				case C_LIT_OCT: {
					char *endptr;

					lex_tok.literal.lu = strtoul(lex_start_p + 2, &endptr, 8);
				} break;
				default: assert(0 && "NOT REACHABLE");
			}

			lex_emit_literal(tok, T_CHARACTER_CONSTANT);
		}
L_STRING_LITERAL:
			lex_advance();
		case '\"': {
			char c;

			while ((c = lex_advance()) != '\"') {
				if (c == '\\') {
					c = lex_advance();
				}

				if (c == '\n' || c == '\0') {
					fprintf(stderr, "lexical error: missing terminating \" character\n");
					
					lex_start_p = lex_current_p;
		
					goto LEX_AGAIN;
				}
			}

			lex_tok.literal.sv = (sv_t) {
				.value = lex_start_p + 1,
				.len = lex_tok_len() - 2
			};

			lex_emit_literal(tok, T_STRING);
		}
		case '\n': {
			/*
			TODO REIMPLEMENT THIS FEATURE
			++lexer->loc.line;
			lexer->loc.col = 0;
			*/

			/*
			TODO CURRENTLY NO CPP
			if (lexer->mode == LEXER_MODE_PREPROCESSOR) {
				lex_emit_multi(tok, T_WS_NL);
				break;
			}
			*/

			goto LEX_AGAIN;
		} break;
		case ' ': case '\r': case '\t': case '\f': case '\v': {
			/*
			TODO CURRENTLY NO CPP
			if (lexer->mode == LEXER_MODE_PREPROCESSOR) {
				lex_emit_multi(tok, T_WHITESPACE);
				break;
			}
			*/

			goto LEX_AGAIN;
		} break;
	}

LEX_EMIT_MULTI:
	lex_tok.literal.sv = (sv_t) {
		.value = lex_start_p,
		.len = lex_tok_len()
	};

	/*FALLTHROUGH*/
LEX_EMIT_LITERAL:
	yyleng = lex_tok_len();

	/*FALLTHROUGH*/
LEX_EMIT_SINGLE:
	/* TODO col += lex_current_p - lex_line_begin_p */

	return lex_tok.type = tok;

LEX_EMIT_CACHE:
	return lex_tok.type;
}
