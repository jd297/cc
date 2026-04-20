/**
Source: https://gist.github.com/JD297

BSD 2-Clause License

Copyright (c) 2025, JD297
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef JD297_LOGGER_H
#define JD297_LOGGER_H

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#define L_NONE 0x0
#define L_EMERGENCY 0x1
#define L_ALERT 0x2
#define L_CRITICAL 0x3
#define L_ERROR 0x4
#define L_WARNING 0x5
#define L_NOTICE 0x6
#define L_INFO 0x7
#define L_DEBUG 0x8

extern void flog(FILE *stream, int level, const char *format, ...);

extern void flog_at(FILE *stream, int level, const char *pathname, int row, size_t col, const char *format, ...);

extern void flog_line(FILE *stream, int row, const char *begin);

extern void flog_ptr(FILE *stream, const char *begin, const char *pos, size_t len);

#endif

#ifdef JD297_LOGGER_IMPLEMENTATION

static void flog_level(FILE *stream, int level)
{
    switch (level) {
        case L_EMERGENCY: fprintf(stream, "emergency: "); return;
        case L_ALERT: fprintf(stream, "alert: "); return;
        case L_CRITICAL: fprintf(stream, "critical: "); return;
        case L_ERROR: fprintf(stream, "error: "); return;
        case L_WARNING: fprintf(stream, "warning: "); return;
        case L_NOTICE: fprintf(stream, "notice: "); return;
        case L_INFO: fprintf(stream, "info: "); return;
        case L_DEBUG: fprintf(stream, "debug: "); return;
        default: return;
    }
}

void flog(FILE *stream, int level, const char *format, ...)
{
    flog_level(stream, level);

    va_list ap;
	va_start(ap, format);
	vfprintf(stream, format, ap);
	va_end(ap);
}

void flog_at(FILE *stream, int level, const char *pathname, int row, size_t col, const char *format, ...)
{
    if (pathname != NULL) {
        fprintf(stream, "%s:", pathname);
    }
    
    if (row != 0) {
        fprintf(stream, "%d:", row);
    }

    if (col != 0) {
        fprintf(stream, "%zu:", col);
    }

    fprintf(stream, " ");

    flog_level(stream, level);

	va_list ap;
	va_start(ap, format);
	vfprintf(stream, format, ap);
	va_end(ap);
	
	fprintf(stream, "\n");
}

void flog_line(FILE *stream, int row, const char *begin)
{
    fprintf(stream, "%5d | ", row);
    
    while (*begin != '\0' && *begin != '\n') {
    	fprintf(stream, "%c", *begin++);
    }
	
	fprintf(stream, "\n");
}

void flog_ptr(FILE *stream, const char *begin, const char *pos, size_t len)
{
    fprintf(stream, "      | ");
    
    for (const char *i = begin; i < pos; ++i) {
        if (*i == '\t') {
            fprintf(stream, "\t");
        } else {
            fprintf(stream, " ");
        }
    }
    
    fprintf(stream, "^");
    
    for (size_t i = 1; i < len; ++i) {
        fprintf(stream, "~");
    }
    
    fprintf(stream, "\n");
}

#endif
