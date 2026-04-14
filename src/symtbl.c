#include <assert.h>
#include <stdlib.h>

#include <jd297/sv.h>
#include <jd297/lmap_sv.h>

#include "symtbl.h"
#include "ir.h"

extern Symtbl *symtbl_create(Symtbl *parent)
{
	Symtbl *symtbl = (Symtbl *)calloc(1, sizeof(Symtbl));

	assert(symtbl != NULL);

	symtbl->parent = parent;

	return symtbl;
}

extern void symtbl_free(Symtbl *symtbl)
{
	lmap_sv_free(&symtbl->namespace1);
	lmap_sv_free(&symtbl->namespace2);
	lmap_sv_free(&symtbl->namespace3);
	lmap_sv_free(&symtbl->namespace4);

	free(symtbl);
}

extern SymtblEntry *symtbl_get_entry(Symtbl *symtbl, sv_t *id, const SymtblEntryClass eclass)
{
	SymtblEntry *entry;

	while (1) {
		switch (eclass) {
			/* NAMESPACE 1 */
			case SYM_CLASS_OBJECT:
			case SYM_CLASS_FUNCTION:
			case SYM_CLASS_TYPEDEF_NAME:
			case SYM_CLASS_ENUM_CONSTANT: {
				entry = lmap_sv_get(&symtbl->namespace1, id);
			} break;
			/* NAMESPACE 2 */
			case SYM_CLASS_LABEL:
				return lmap_sv_get(&symtbl->namespace2, id);
			/* NAMESPACE 3 */
			case SYM_CLASS_TAG_OF_STRUCT:
			case SYM_CLASS_UNION:
			case SYM_CLASS_ENUMERATION: {
				entry = lmap_sv_get(&symtbl->namespace3, id);
			} break;
			/* NAMESPACE 4 */
			case SYM_CLASS_STRUCT_UNION_MEMBER:
				return lmap_sv_get(&symtbl->namespace4, id);
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

extern SymtblEntry *symtbl_add_entry(Symtbl *symtbl, sv_t id, IRDataType *dtype, const SymtblEntryClass eclass)
{
	SymtblEntry *entry = (SymtblEntry *)malloc(sizeof(SymtblEntry));

	assert(entry != NULL);

	entry->id = id;
	entry->dtype = dtype;
	entry->eclass = eclass;

	switch (eclass) {
		/* NAMESPACE 1 */
		case SYM_CLASS_OBJECT:
		case SYM_CLASS_FUNCTION:
		case SYM_CLASS_TYPEDEF_NAME:
		case SYM_CLASS_ENUM_CONSTANT: {
			assert(lmap_sv_add(&symtbl->namespace1, &entry->id, entry) == 0);
		} break;
		/* NAMESPACE 2 */
		case SYM_CLASS_LABEL: {
			assert(lmap_sv_add(&symtbl->namespace2, &entry->id, entry) == 0);
		} break;
		/* NAMESPACE 3 */
		case SYM_CLASS_TAG_OF_STRUCT:
		case SYM_CLASS_UNION:
		case SYM_CLASS_ENUMERATION: {
			assert(lmap_sv_add(&symtbl->namespace3, &entry->id, entry) == 0);
		} break;
		/* NAMESPACE 4 */
		case SYM_CLASS_STRUCT_UNION_MEMBER: {
			assert(lmap_sv_add(&symtbl->namespace4, &entry->id, entry) == 0);
		} break;
		default:
			assert(0 && "NOT REACHABLE");
	}
	
	return entry;
}
