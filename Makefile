.POSIX:

CC            = cc
CFLAGS        = -Wall -Wextra -Wpedantic -g -I src
LDFLAGS       = 

PERF          = gperf

TARGET        = cc
PREFIX        = /usr/local
BINDIR        = $(PREFIX)/bin
MANDIR        = $(PREFIX)/man
SRCDIR        = src
BUILDDIR      = build
DEFSDIR       = defs

OBJFILES      = $(BUILDDIR)/lex.o $(BUILDDIR)/lmap.o $(BUILDDIR)/parse.o\
                $(BUILDDIR)/cpp.o\
                $(BUILDDIR)/vector.o $(BUILDDIR)/logger.o $(BUILDDIR)/comp.o\
                $(BUILDDIR)/list.o $(BUILDDIR)/opt.o $(BUILDDIR)/ir.o\
                $(BUILDDIR)/x86_64.o $(BUILDDIR)/aarch64.o\
                $(BUILDDIR)/gen.o\
                $(BUILDDIR)/sv.o $(BUILDDIR)/lmap_sv.o\
                $(BUILDDIR)/toolchain.o\
                $(BUILDDIR)/OpenBSD.o $(BUILDDIR)/GNULinux.o\
                $(BUILDDIR)/symtbl.o

HEADERS       = $(SRCDIR)/jd297/lmap.h $(SRCDIR)/jd297/vector.h\
                $(SRCDIR)/lex.h $(SRCDIR)/parse.h\
                $(SRCDIR)/cpp.h $(SRCDIR)/tok.h\
                $(SRCDIR)/jd297/logger.h\
                $(SRCDIR)/comp.h $(SRCDIR)/jd297/list.h\
                $(SRCDIR)/opt.h $(SRCDIR)/ir.h $(SRCDIR)/gen.h\
                $(SRCDIR)/jd297/sv.h $(SRCDIR)/jd297/lmap_sv.h\
                $(SRCDIR)/toolchain.h\
                $(SRCDIR)/symtbl.h

all: $(BUILDDIR)/$(TARGET)

$(BUILDDIR)/$(TARGET): $(OBJFILES) $(BUILDDIR)/cc.o
	$(CC) -o $@ $(OBJFILES) $(BUILDDIR)/cc.o $(LDFLAGS)

$(BUILDDIR)/cc.o: $(HEADERS) $(SRCDIR)/cc.c
	$(CC) $(CFLAGS) -c -o $@ $(SRCDIR)/cc.c

$(BUILDDIR)/lex.o: $(HEADERS) $(SRCDIR)/lex.c $(SRCDIR)/lex_kw.h
	$(CC) $(CFLAGS) -Wno-missing-field-initializers -c -o $@ $(SRCDIR)/lex.c

$(BUILDDIR)/lmap.o: $(HEADERS) $(SRCDIR)/lmap.c
	$(CC) $(CFLAGS) -c -o $@ $(SRCDIR)/lmap.c

$(BUILDDIR)/parse.o: $(HEADERS) $(SRCDIR)/parse.c
	$(CC) $(CFLAGS) -c -o $@ $(SRCDIR)/parse.c

$(BUILDDIR)/cpp.o: $(HEADERS) $(SRCDIR)/cpp.c
	$(CC) $(CFLAGS) -c -o $@ $(SRCDIR)/cpp.c

$(BUILDDIR)/vector.o: $(HEADERS) $(SRCDIR)/vector.c
	$(CC) $(CFLAGS) -c -o $@ $(SRCDIR)/vector.c

$(BUILDDIR)/logger.o: $(HEADERS) $(SRCDIR)/logger.c
	$(CC) $(CFLAGS) -c -o $@ $(SRCDIR)/logger.c

$(BUILDDIR)/comp.o: $(HEADERS) $(SRCDIR)/comp.c
	$(CC) $(CFLAGS) -c -o $@ $(SRCDIR)/comp.c

$(BUILDDIR)/list.o: $(HEADERS) $(SRCDIR)/list.c
	$(CC) $(CFLAGS) -c -o $@ $(SRCDIR)/list.c

$(BUILDDIR)/opt.o: $(HEADERS) $(SRCDIR)/opt.c
	$(CC) $(CFLAGS) -c -o $@ $(SRCDIR)/opt.c

$(BUILDDIR)/ir.o: $(HEADERS) $(SRCDIR)/ir.c
	$(CC) $(CFLAGS) -c -o $@ $(SRCDIR)/ir.c

$(BUILDDIR)/symtbl.o: $(HEADERS) $(SRCDIR)/symtbl.c
	$(CC) $(CFLAGS) -c -o $@ $(SRCDIR)/symtbl.c

$(BUILDDIR)/sv.o: $(HEADERS) $(SRCDIR)/sv.c
	$(CC) $(CFLAGS) -c -o $@ $(SRCDIR)/sv.c

$(BUILDDIR)/lmap_sv.o: $(HEADERS) $(SRCDIR)/lmap_sv.c
	$(CC) $(CFLAGS) -c -o $@ $(SRCDIR)/lmap_sv.c

$(BUILDDIR)/toolchain.o: $(HEADERS) $(SRCDIR)/toolchain.c
	$(CC) $(CFLAGS) -c -o $@ $(SRCDIR)/toolchain.c

$(BUILDDIR)/OpenBSD.o: $(HEADERS) $(SRCDIR)/toolchain/OpenBSD.c
	$(CC) $(CFLAGS) -c -o $@ $(SRCDIR)/toolchain/OpenBSD.c

$(BUILDDIR)/GNULinux.o: $(HEADERS) $(SRCDIR)/toolchain/GNULinux.c
	$(CC) $(CFLAGS) -c -o $@ $(SRCDIR)/toolchain/GNULinux.c

$(SRCDIR)/lex_kw.h: $(DEFSDIR)/keywords
	$(PERF) --output-file=$@ $(DEFSDIR)/keywords

$(BUILDDIR)/gen.o: $(HEADERS) $(SRCDIR)/gen.c
	$(CC) $(CFLAGS) -c -o $@ $(SRCDIR)/gen.c

$(BUILDDIR)/x86_64.o: $(HEADERS) $(SRCDIR)/gen/x86_64.c
	$(CC) $(CFLAGS) -c -o $@ $(SRCDIR)/gen/x86_64.c

$(BUILDDIR)/aarch64.o: $(HEADERS) $(SRCDIR)/gen/aarch64.c
	$(CC) $(CFLAGS) -c -o $@ $(SRCDIR)/gen/aarch64.c

clean:
	rm -f $(BUILDDIR)/*

install: $(BUILDDIR)/$(TARGET)
	cp $(BUILDDIR)/$(TARGET) $(BINDIR)/$(TARGET)

uninstall:
	rm -f $(BINDIR)/$(TARGET)
