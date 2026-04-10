#ifndef JD297_CC_SYMTBL_H
#define JD297_CC_SYMTBL_H

#include <jd297/sv.h>
#include <jd297/lmap_sv.h>

#include "ir.h"

typedef struct Symtbl Symtbl;

typedef enum {
	SYMTBL_ENTRY_FUNCTION,
	SYMTBL_ENTRY_LABEL,
	SYMTBL_ENTRY_LOCAL,
	SYMTBL_ENTRY_DATA,
	SYMTBL_ENTRY_UNION,
	SYMTBL_ENTRY_STRUCT,
	SYMTBL_ENTRY_TYPEDEF
} SymtblEntryType;

typedef struct {
	Symtbl *scope; /* TODO DRAFT */
} SymtblFunction;

typedef struct {
	size_t addr;
} SymtblLocal;

typedef struct {
	size_t lid;
} SymtblLabel;

typedef struct {
	sv_t *id;
	IRDataType *dtype;
	SymtblEntryType etype;

	union {
		SymtblFunction function;
		SymtblLocal local;
		/* SymtblLocal data; */ /* dtype should be enough */
		SymtblLabel label;
	} as;

	/* Lexer_Location_C loc; */ /* TODO DRAFT */
} SymtblEntry;

struct Symtbl {
	struct Symtbl *parent;

	SymtblEntry *self; /* TODO DRAFT */

	lmap_sv_t entries;
	lmap_sv_t labels;
	/* TODO maybe */
	/* lmap_sv_t locals; */
	/* lmap_sv_t ...; */
	
	int entered;
};

extern Symtbl *symtbl_create(Symtbl *parent);

extern void symtbl_free(Symtbl *symtbl);

extern SymtblEntry *symtbl_get_entry(Symtbl *symtbl, sv_t *id, const SymtblEntryType etype);

#define symtbl_get_function_entry(symtbl, id) symtbl_get_entry((symtbl), (id), SYMTBL_ENTRY_FUNCTION);

#define symtbl_get_local_entry(symtbl, id) symtbl_get_entry((symtbl), (id), SYMTBL_ENTRY_LOCAL);

#define symtbl_get_data_entry(symtbl, id) symtbl_get_entry((symtbl), (id), SYMTBL_ENTRY_DATA);

#define symtbl_get_label_entry(symtbl, id) symtbl_get_entry((symtbl), (id), SYMTBL_ENTRY_LABEL);

extern SymtblEntry *symtbl_add_entry(Symtbl *symtbl, sv_t *id, IRDataType *dtype, const SymtblEntryType etype);

#define symtbl_add_function_entry(symtbl, id, dtype) symtbl_add_entry((symtbl), (id), (dtype), SYMTBL_ENTRY_FUNCTION);

#define symtbl_add_local_entry(symtbl, id, dtype) symtbl_add_entry((symtbl), (id), (dtype), SYMTBL_ENTRY_LOCAL);

#define symtbl_add_data_entry(symtbl, id, dtype) symtbl_add_entry((symtbl), (id), (dtype), SYMTBL_ENTRY_DATA);

#define symtbl_add_label_entry(symtbl, id, dtype) symtbl_add_entry((symtbl), (id), (dtype), SYMTBL_ENTRY_LABEL);

#endif
