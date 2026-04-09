#include "lexer_c.h"
#include "token_type_c.h"
#include "token_c.h"
#include "lexer_c_keyword.h"

#include <jd297/sv.h>

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int lexer_c_match(Lexer_C *lexer, char expected);

static char lexer_c_advance(Lexer_C *lexer);

#define lexer_c_is_at_end(lexer_ptr) \
	*(lexer_ptr)->current == '\0'

#define lexer_c_peek(lexer_ptr, n) \
	*((lexer_ptr)->current + n)

#define lexer_c_set_token(lexer_ptr, token_ptr, type_val) \
	(token_ptr)->type = type_val; \
	(token_ptr)->view.value = (lexer_ptr)->start; \
	(token_ptr)->view.len = (lexer_ptr)->current - (lexer_ptr)->start

void lexer_c_create(Lexer_C *lexer, sv_t pathname, const char *source, Lexer_Mode_C mode)
{
	*lexer = (Lexer_C) {
		.start = source,
		.current = source,
		.loc = (Lexer_Location_C) {
			.pathname = pathname,
			.line = 1,
			.col = 0
		},
		.mode = mode
	};
}

TokenType_C lexer_c_next(Lexer_C *lexer, Token_C *token)
{
	lexer_c_next_again:

	switch (lexer_c_advance(lexer))
	{
        case '(':
			lexer_c_set_token(lexer, token, T_OPEN_PARENT);
			break;
		case ')':
			lexer_c_set_token(lexer, token, T_CLOSING_PARENT);
			break;
		case '[':
			lexer_c_set_token(lexer, token, T_OPEN_BRACKET);
			break;
		case ']':
			lexer_c_set_token(lexer, token, T_CLOSING_BRACKET);
			break;
		case '{':
			lexer_c_set_token(lexer, token, T_OPEN_BRACE);
			break;
		case '}':
			lexer_c_set_token(lexer, token, T_CLOSING_BRACE);
			break;
		case '~':
			lexer_c_set_token(lexer, token, T_TILDE);
			break;
		case ',':
			lexer_c_set_token(lexer, token, T_COMMA);
			break;
		case ':':
			lexer_c_set_token(lexer, token, T_COLON);
			break;
		case ';':
			lexer_c_set_token(lexer, token, T_SEMICOLON);
			break;
		case '?':
			lexer_c_set_token(lexer, token, T_TERNARY);
			break;
		case '#': {
			if (lexer->mode == LEXER_MODE_PREPROCESSOR) {
				assert(0 && "TODO: SKIP WS, try parse macro identifier");
			}

			lexer->start = lexer->current;

			if (lexer_c_next(lexer, token) != T_INTEGER_CONSTANT) {
				assert(0 && "TODO: error: invalid preprocessing directive");
			}
			
			size_t line = token->literal.lu;

			if (lexer_c_next(lexer, token) != T_STRING) {
				if (token->type != T_WS_NL) {
					/*
					 * TODO SKIP EOL / EOF
					 * TODO lexer->mode = LEXER_MODE_NORMAL;
					 * TODO goto lexer_c_next_again;
					 */
					assert(0 && "TODO: error: invalid filename for line marker directive");
				}
				
				assert(0 && "TODO: warning: this style of line directive is a GNU extension");
			}
			
			sv_t pathname = token->literal.sv;

			lexer->mode = LEXER_MODE_PREPROCESSOR;

			while (1) {
				lexer_c_next(lexer, token);
				
				if (token->type == T_WS_NL) {
					break;
				}
				
				if (token->type == T_EOF) {
					lexer->mode = LEXER_MODE_NORMAL;
					lexer_c_set_token(lexer, token, T_EOF);
					break;
				}
			}

			lexer->mode = LEXER_MODE_NORMAL;

			lexer->loc.pathname = pathname;
			
			lexer->loc.line = line;
			lexer->loc.col = 0;

			goto lexer_c_next_again;
		} break;
		case '=':
			lexer_c_set_token(lexer, token, lexer_c_match(lexer, '=') == 0 ? T_EQUAL_TO : T_ASSIGNMENT);
			break;
		case '*':
			lexer_c_set_token(lexer, token, lexer_c_match(lexer, '=') == 0 ? T_MULTIPLY_ASSIGN : T_MULTIPLY);
			break;
		case '%':
			lexer_c_set_token(lexer, token, lexer_c_match(lexer, '=') == 0 ? T_MODULUS_ASSIGN : T_MODULUS);
			break;
		case '^':
			lexer_c_set_token(lexer, token, lexer_c_match(lexer, '=') == 0 ? T_BITWISE_XOR_ASSIGN : T_BITWISE_XOR);
			break;
		case '!':
			lexer_c_set_token(lexer, token, lexer_c_match(lexer, '=') == 0 ? T_NOT_EQUAL_TO : T_LOGICAL_NOT);
			break;	
		case '|':
        	lexer_c_set_token(lexer, token, lexer_c_match(lexer, '=') == 0 ? T_BITWISE_OR_ASSIGN : lexer_c_match(lexer, '|') == 0 ? T_LOGICAL_OR : T_BITWISE_OR);
        	break;
    	case '&':
        	lexer_c_set_token(lexer, token, lexer_c_match(lexer, '=') == 0 ? T_BITWISE_AND_ASSIGN : lexer_c_match(lexer, '&') == 0 ? T_LOGICAL_AND : T_BITWISE_AND);
        	break;
    	case '+':
			lexer_c_set_token(lexer, token, lexer_c_match(lexer, '=') == 0 ? T_PLUS_ASSIGN : lexer_c_match(lexer, '+') == 0 ? T_INCREMENT : T_PLUS);
			break;
		case '-':
			lexer_c_set_token(lexer, token, lexer_c_match(lexer, '=') == 0 ? T_MINUS_ASSIGN : lexer_c_match(lexer, '-') == 0 ? T_DECREMENT : lexer_c_match(lexer, '>') == 0 ? T_ARROW : T_MINUS);
			break;
		case '<':
			lexer_c_set_token(lexer, token, lexer_c_match(lexer, '=') == 0 ? T_LESS_THAN_OR_EQUAL_TO : lexer_c_match(lexer, '<') == 0 ? lexer_c_match(lexer, '=') == 0 ? T_BITWISE_LEFTSHIFT_ASSIGN : T_BITWISE_LEFTSHIFT : T_LESS_THAN);
			break;
		case '>':
			lexer_c_set_token(lexer, token, lexer_c_match(lexer, '=') == 0 ? T_GREATER_THAN_OR_EQUAL_TO : lexer_c_match(lexer, '>') == 0 ? lexer_c_match(lexer, '=') == 0 ? T_BITWISE_RIGHTSHIFT_ASSIGN : T_BITWISE_RIGHTSHIFT : T_GREATER_THAN);
			break;
		case '/': {
			if (lexer_c_match(lexer, '/') == 0) {
				while (lexer_c_peek(lexer, 0) != '\n' && !(lexer_c_is_at_end(lexer))) {
					lexer_c_advance(lexer);
				}

				goto lexer_c_next_again;
			}

			if (lexer_c_match(lexer, '*') == 0) {
				while (1) {
					if (lexer_c_peek(lexer, 0) == '*' && lexer_c_peek(lexer, 1) == '/') {
						lexer_c_advance(lexer);
						lexer_c_advance(lexer);

						lexer->start = lexer->current;

						goto lexer_c_next_again;
					}

					if (lexer_c_is_at_end(lexer)) {
						printf(SV_FMT":%zu:%zu: error: unterminated /* comment\n", SV_PARAMS(&lexer->loc.pathname), lexer->loc.line, lexer->loc.col);

						lexer->start = lexer->current;
						lexer_c_set_token(lexer, token, T_EOF);
						return T_EOF;
					}
					
					if (lexer_c_peek(lexer, 0) == '\n') {
						++lexer->loc.line;
						lexer->loc.col = 0;

						lexer->current++;

						continue;
					}
					
					lexer_c_advance(lexer);
				}
			}
			
			lexer_c_set_token(lexer, token, lexer_c_match(lexer, '=') == 0 ? T_DIVIDE_ASSIGN : T_DIVIDE);
		} break;
		
		case '.': {
			switch (lexer_c_peek(lexer, 0)) {
				case '0': case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9':
					goto L_FLOATING_CONSTANT;
				case '.': {
					if (lexer_c_peek(lexer, 1) == '.') {
						lexer->current += 2;

						lexer_c_set_token(lexer, token, T_DOT_DOT_DOT);

						break;
					}
				}
				default:
					lexer_c_set_token(lexer, token, T_DOT);
			}
		} break;
		case 'L': {
			if (lexer_c_peek(lexer, 0) == '\'') {
				goto L_CHARACTER_CONSTANT;
			}

			if (lexer_c_peek(lexer, 0) == '\"') {
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
        	while (isalnum(*lexer->current) != 0  || *lexer->current == '_') {
        		lexer_c_advance(lexer);
        	}

			lexer_c_set_token(lexer, token, T_IDENTIFIER);

			const struct lexer_c_keyword_entry *e = 
				lexer_c_lookup_keyword(token->view.value, token->view.len);

			if (e != NULL) {
				token->type = e->type;
			}
        } break;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9': {
			char *endptr;
			unsigned long int lu;
			
			errno = 0;
			lu = strtoul(lexer->start, &endptr, 0);
			
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
			
			lexer->current = endptr;
			token->literal.lu = lu;
			lexer_c_set_token(lexer, token, T_INTEGER_CONSTANT);
		} break;
		L_FLOATING_CONSTANT: {
			char *endptr;
			long double Lf;
			
			errno = 0;
			Lf = strtold(lexer->start, &endptr);
			
			// TODO errno ERANGE
			//  warning: magnitude of floating-point constant too large for type 'double'
			
			// TODO F f | l L => advance
			// TODO error: invalid suffix 'ZZ' on floating constant

			lexer->current = endptr; // TODO advance location | check everywhere because it was probably forgoten sometimes !!
			token->literal.Lf = Lf;
			lexer_c_set_token(lexer, token, T_FLOATING_CONSTANT);
		} break;
		L_CHARACTER_CONSTANT:
			lexer_c_advance(lexer);
		case '\'': {
			enum {
				C_LIT_CHAR,
				C_LIT_ESC,
				C_LIT_HEX,
				C_LIT_OCT
			} type = C_LIT_CHAR;
		
			const char *end;

			for (end = lexer->current; *end != '\''; ++end) {
				if (*end == '\n' || *end == '\0') {
					printf(SV_FMT":%zu:%zu: error: missing terminating ' character\n", SV_PARAMS(&lexer->loc.pathname), lexer->loc.line, lexer->loc.col);
					
					lexer->start = lexer->current = end;

					goto lexer_c_next_again;
				}
			}

			switch (lexer_c_advance(lexer)) {
				case '\'': {
					printf(SV_FMT":%zu:%zu: error: empty character constant\n", SV_PARAMS(&lexer->loc.pathname), lexer->loc.line, lexer->loc.col);

					lexer->start = lexer->current;
			
					goto lexer_c_next_again;
				}
				case '\\': {
					switch (lexer_c_advance(lexer)) {
						case 'n': case 't': case 'v': case 'b':
						case 'r': case 'f': case 'a': case '\\':
						case '?': case '\'': case '\"': {
							if (lexer_c_advance(lexer) != '\'') {
								printf(SV_FMT":%zu:%zu: error: multi-character character constant\n", SV_PARAMS(&lexer->loc.pathname), lexer->loc.line, lexer->loc.col);
						
								lexer->start = lexer->current = end + 1;
						
								goto lexer_c_next_again;
							}
							
							type = C_LIT_ESC;
						} break;
						case 'x': {
							for (size_t i = 0; ; i++) {
								switch (lexer_c_advance(lexer)) {
									case '0': case '1': case '2': case '3': case '4':
									case '5': case '6': case '7': case '8': case '9':
									case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
									case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
										if (i >= 2) {
											printf(SV_FMT":%zu:%zu: error: hex escape sequence out of range\n", SV_PARAMS(&lexer->loc.pathname), lexer->loc.line, lexer->loc.col);

											lexer->start = lexer->current = end + 1;
											
											goto lexer_c_next_again;
										}

										continue;
									default: {
										printf(SV_FMT":%zu:%zu: error: \\x used with no following hex digits\n", SV_PARAMS(&lexer->loc.pathname), lexer->loc.line, lexer->loc.col);
							
										lexer->start = lexer->current = end + 1;
								
										goto lexer_c_next_again;
									}
									case '\'':
										if (i == 0) {
											printf(SV_FMT":%zu:%zu: error: \\x used with no following hex digits\n", SV_PARAMS(&lexer->loc.pathname), lexer->loc.line, lexer->loc.col);

											lexer->start = lexer->current = end + 1;
											
											goto lexer_c_next_again;
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
								switch (lexer_c_advance(lexer)) {
									case '0': case '1': case '2': case '3':
									case '4': case '5': case '6': case '7':
										if (i >= 2) {
											printf(SV_FMT":%zu:%zu: error: octal escape sequence out of range\n", SV_PARAMS(&lexer->loc.pathname), lexer->loc.line, lexer->loc.col);

											lexer->start = lexer->current = end + 1;
											
											goto lexer_c_next_again;
										}

										continue;
									default: {
										printf(SV_FMT":%zu:%zu: error: octal escape sequence used with no following octal digits\n", SV_PARAMS(&lexer->loc.pathname), lexer->loc.line, lexer->loc.col);
							
										lexer->start = lexer->current = end + 1;
								
										goto lexer_c_next_again;
									}
									case '\'':
										break;
								}

								type = C_LIT_OCT;

								break;
							}
						} break;
						default: {
							printf(SV_FMT":%zu:%zu: error: unknown escape sequence\n", SV_PARAMS(&lexer->loc.pathname), lexer->loc.line, lexer->loc.col);
						
							lexer->start = lexer->current = end + 1;
					
							goto lexer_c_next_again;
						}
					}
				} break;
				default: {
					if (lexer_c_advance(lexer) != '\'') {
						printf(SV_FMT":%zu:%zu: error: multi-character character constant\n", SV_PARAMS(&lexer->loc.pathname), lexer->loc.line, lexer->loc.col);
						
						lexer->start = lexer->current = end + 1;
				
						goto lexer_c_next_again;
					}
				} break;
			}
			
			switch (type) {
				case C_LIT_CHAR: {
					token->literal.lu = *(lexer->current - 2);
				} break;
				case C_LIT_ESC: {
					switch (*(lexer->current - 2)) {
						case 'n':  token->literal.lu = '\n'; break;
						case 't':  token->literal.lu = '\t'; break;
						case 'v':  token->literal.lu = '\v'; break;
						case 'b':  token->literal.lu = '\b'; break;
						case 'r':  token->literal.lu = '\r'; break;
						case 'f':  token->literal.lu = '\f'; break;
						case 'a':  token->literal.lu = '\a'; break;
						case '\\': token->literal.lu = '\\'; break;
						case '?':  token->literal.lu = '\?'; break;
						case '\'': token->literal.lu = '\''; break;
						case '\"': token->literal.lu = '\"'; break;
					}
				} break;
				case C_LIT_HEX: {
					char *endptr;

					token->literal.lu = strtoul(lexer->start + 3, &endptr, 16);
				} break;
				case C_LIT_OCT: {
					char *endptr;

					token->literal.lu = strtoul(lexer->start + 2, &endptr, 8);
				} break;
				default: assert(0 && "NOT REACHABLE");
			}

			lexer_c_set_token(lexer, token, T_CHARACTER_CONSTANT);
		} break;
		L_STRING_LITERAL:
			lexer_c_advance(lexer);
		case '\"': {
			char c;

			while ((c = lexer_c_advance(lexer)) != '\"') {
				if (c == '\\') {
					c = lexer_c_advance(lexer);
				}

				if (c == '\n' || c == '\0') {
					printf(SV_FMT":%zu:%zu: error: missing terminating \" character\n", SV_PARAMS(&lexer->loc.pathname), lexer->loc.line, lexer->loc.col);
					
					lexer->start = lexer->current;
		
					goto lexer_c_next_again;
				}
			}

			lexer_c_set_token(lexer, token, T_STRING);
			
			token->literal.sv = (sv_t) {
				.value = token->view.value + 1,
				.len = token->view.len - 2
			};
		} break;
		case '\n': {
			++lexer->loc.line;
			lexer->loc.col = 0;

			if (lexer->mode == LEXER_MODE_PREPROCESSOR) {
				lexer_c_set_token(lexer, token, T_WS_NL);
				break;
			}			

			lexer->start = lexer->current;

			goto lexer_c_next_again;
		} break;
		case ' ': case '\r': case '\t': case '\f': case '\v': {
			if (lexer->mode == LEXER_MODE_PREPROCESSOR) {
				lexer_c_set_token(lexer, token, T_WHITESPACE);
				break;
			}

			lexer->start = lexer->current;

			goto lexer_c_next_again;
		} break;
		case '\0': {
			lexer_c_set_token(lexer, token, T_EOF);
		} break;
		default: {
			printf(SV_FMT":%zu:%zu: error: unexpected character: \""SV_FMT"\"\n", SV_PARAMS(&lexer->loc.pathname), lexer->loc.line, lexer->loc.col, 1, lexer->start);
			
			lexer->start = lexer->current;
			lexer_c_set_token(lexer, token, T_UNKNOWN);
		} break;
	}

	lexer->start = lexer->current;

    return token->type;
}

static char lexer_c_advance(Lexer_C *lexer)
{
	++lexer->loc.col;

	return *(lexer->current++);
}

static int lexer_c_match(Lexer_C *lexer, char expected)
{
	if (*lexer->current != expected) {
		return -1;
	}

	lexer->current++;

	return 0;
}
