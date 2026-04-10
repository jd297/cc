#include <assert.h>
#include <stdlib.h>

#include <jd297/sv.h>
#include <jd297/lmap_sv.h>

#include "symtbl.h"
#include "ir.h"

Symtbl *symtbl_create(Symtbl *parent)
{
	Symtbl *symtbl = (Symtbl *)calloc(1, sizeof(Symtbl));

	assert(symtbl != NULL);

	symtbl->parent = parent;

	return symtbl;
}

void symtbl_free(Symtbl *symtbl)
{
	lmap_sv_free(&symtbl->entries);
	lmap_sv_free(&symtbl->labels);

	free(symtbl);
}

SymtblEntry *symtbl_get_entry(Symtbl *symtbl, sv_t *id, const SymtblEntryType etype)
{
	SymtblEntry *entry;

	while (1) {
		switch (etype) {
			case SYMTBL_ENTRY_FUNCTION:
			case SYMTBL_ENTRY_LOCAL:
			case SYMTBL_ENTRY_DATA:
				entry = lmap_sv_get(&symtbl->labels, id);
				break;
			case SYMTBL_ENTRY_LABEL:
				entry = lmap_sv_get(&symtbl->labels, id);
				break;
			case SYMTBL_ENTRY_UNION:
				assert(0 && "TODO not implemented with (SYMTBL_ENTRY_UNION)");
			case SYMTBL_ENTRY_STRUCT:
				assert(0 && "TODO not implemented with (SYMTBL_ENTRY_STRUCT)");
			case SYMTBL_ENTRY_TYPEDEF:
				assert(0 && "TODO not implemented with (SYMTBL_ENTRY_TYPEDEF)");
			default:
				assert(0 && "NOT REACHABLE");
		}
		
		if (entry != NULL) {
			return entry;
		}

		if (symtbl->parent == NULL) {
			return NULL;
		}

		symtbl = symtbl->parent;
	}
}

SymtblEntry *symtbl_add_entry(Symtbl *symtbl, sv_t *id, IRDataType *dtype, const SymtblEntryType etype)
{
	SymtblEntry *entry = (SymtblEntry *)malloc(sizeof(SymtblEntry));

	assert(entry != NULL);

	entry->id = id;
	entry->dtype = dtype;
	entry->etype = etype;

	switch (etype) {
		case SYMTBL_ENTRY_FUNCTION:
		case SYMTBL_ENTRY_LOCAL:
		case SYMTBL_ENTRY_DATA:
			assert(lmap_sv_add(&symtbl->entries, id, entry) == 0);
			break;
		case SYMTBL_ENTRY_LABEL:
			assert(lmap_sv_add(&symtbl->labels, id, entry) == 0);
			break;
		case SYMTBL_ENTRY_UNION:
			assert(0 && "TODO not implemented with (SYMTBL_ENTRY_UNION)");
		case SYMTBL_ENTRY_STRUCT:
			assert(0 && "TODO not implemented with (SYMTBL_ENTRY_STRUCT)");
		case SYMTBL_ENTRY_TYPEDEF:
			assert(0 && "TODO not implemented with (SYMTBL_ENTRY_TYPEDEF)");
		default:
			assert(0 && "NOT REACHABLE");
	}
	
	return entry;
}
