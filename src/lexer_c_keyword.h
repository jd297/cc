/* ANSI-C code produced by gperf version 3.1 */
/* Command-line: gperf --output-file=src/lexer_c_keyword.h defs/keywords  */
/* Computed positions: -k'1,3' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gperf@gnu.org>."
#endif

#line 11 "defs/keywords"
struct lexer_c_keyword_entry { const char *name; TokenType_C type; };
#include <string.h>
/* maximum key range = 42, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
lexer_c_hash_keyword (register const char *str, register size_t len)
{
  static const unsigned char asso_values[] =
    {
      45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
      45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
      45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
      45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
      45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
      45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
      45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
      45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
      45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
      45, 45, 45, 45, 45, 45, 45, 30, 10, 10,
      25, 20,  0, 10, 45,  5, 45, 45, 25, 45,
      10, 10,  5, 45,  0,  5,  0,  0,  0, 20,
      45, 45, 25, 45, 45, 45, 45, 45
    };
  register unsigned int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
      case 2:
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

const struct lexer_c_keyword_entry *
lexer_c_lookup_keyword (register const char *str, register size_t len)
{
  enum
    {
      TOTAL_KEYWORDS = 32,
      MIN_WORD_LENGTH = 2,
      MAX_WORD_LENGTH = 8,
      MIN_HASH_VALUE = 3,
      MAX_HASH_VALUE = 44
    };

  static const struct lexer_c_keyword_entry wordlist[] =
    {
      {""}, {""}, {""},
#line 26 "defs/keywords"
      {"for", T_FOR},
      {""}, {""},
#line 32 "defs/keywords"
      {"return", T_REGISTER},
#line 28 "defs/keywords"
      {"if", T_IF},
#line 29 "defs/keywords"
      {"int", T_INT},
#line 42 "defs/keywords"
      {"void", T_VOID},
#line 40 "defs/keywords"
      {"union", T_UNION},
#line 37 "defs/keywords"
      {"struct", T_STRUCT},
#line 39 "defs/keywords"
      {"typedef", T_TYPEDEF},
#line 41 "defs/keywords"
      {"unsigned", T_UNSIGNED},
#line 27 "defs/keywords"
      {"goto", T_GOTO},
#line 25 "defs/keywords"
      {"float", T_FLOAT},
#line 38 "defs/keywords"
      {"switch", T_SWITCH},
      {""},
#line 31 "defs/keywords"
      {"register", T_REGISTER},
#line 15 "defs/keywords"
      {"case", T_CASE},
#line 33 "defs/keywords"
      {"short", T_SHORT},
#line 34 "defs/keywords"
      {"signed", T_SIGNED},
      {""}, {""},
#line 23 "defs/keywords"
      {"enum", T_ENUM},
#line 17 "defs/keywords"
      {"const", T_CONST},
#line 24 "defs/keywords"
      {"extern", T_EXTERN},
#line 20 "defs/keywords"
      {"do", T_DO},
#line 18 "defs/keywords"
      {"continue", T_CONTINUE},
#line 22 "defs/keywords"
      {"else", T_ELSE},
#line 44 "defs/keywords"
      {"while", T_WHILE},
#line 21 "defs/keywords"
      {"double", T_DOUBLE},
#line 19 "defs/keywords"
      {"default", T_DEFAULT},
#line 43 "defs/keywords"
      {"volatile", T_VOLATILE},
#line 13 "defs/keywords"
      {"auto", T_AUTO},
#line 14 "defs/keywords"
      {"break", T_BREAK},
#line 35 "defs/keywords"
      {"sizeof", T_SIZEOF},
      {""}, {""},
#line 30 "defs/keywords"
      {"long", T_LONG},
      {""},
#line 36 "defs/keywords"
      {"static", T_STATIC},
      {""}, {""},
#line 16 "defs/keywords"
      {"char", T_CHAR}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register unsigned int key = lexer_c_hash_keyword (str, len);

      if (key <= MAX_HASH_VALUE)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
            return &wordlist[key];
        }
    }
  return 0;
}
