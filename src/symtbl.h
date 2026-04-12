#ifndef JD297_CC_SYMTBL_H
#define JD297_CC_SYMTBL_H

#include <jd297/sv.h>
#include <jd297/lmap_sv.h>

#include "ir.h"

typedef struct Symtbl Symtbl;

typedef enum {
	SYM_CLASS_OBJECT,
	SYM_CLASS_FUNCTION,
	SYM_CLASS_TYPEDEF_NAME,
	SYM_CLASS_ENUM_CONSTANT,
	SYM_CLASS_LABEL,
	SYM_CLASS_TAG_OF_STRUCT,
	SYM_CLASS_UNION,
	SYM_CLASS_ENUMERATION,
	SYM_CLASS_STRUCT_UNION_MEMBER,
} SymtblEntryClass;

typedef struct {
	sv_t *id;
	IRDataType *dtype;
	SymtblEntryClass eclass;

	union {
		Symtbl *scope;
		size_t addr;
		size_t label_id;
	} as;

	/* TODO token location */
} SymtblEntry;

struct Symtbl {
	Symtbl *parent;

	lmap_sv_t namespace1; /* OBJECT, FUNCTION, TYPEDEF_NAME, ENUM_CONSTANT */	
	lmap_sv_t namespace2; /* LABEL */
	lmap_sv_t namespace3; /* TAGS_OF_STRUCT, UNION, ENUMERATION */
	lmap_sv_t namespace4; /* STRUCT_UNION_MEMBER */
};

extern Symtbl *symtbl_create(Symtbl *parent);

extern void symtbl_free(Symtbl *symtbl);

extern SymtblEntry *symtbl_get_entry(Symtbl *symtbl, sv_t *id, const SymtblEntryClass eclass);

#define symtbl_get_object_entry(symtbl, id) symtbl_get_entry((symtbl), (id), SYM_CLASS_OBJECT);
#define symtbl_get_function_entry(symtbl, id) symtbl_get_entry((symtbl), (id), SYM_CLASS_FUNCTION);
#define symtbl_get_typedef_name_entry(symtbl, id) symtbl_get_entry((symtbl), (id), SYM_CLASS_TYPEDEF_NAME);
#define symtbl_get_enum_constant_entry(symtbl, id) symtbl_get_entry((symtbl), (id), SYM_CLASS_ENUM_CONSTANT);
#define symtbl_get_label_entry(symtbl, id) symtbl_get_entry((symtbl), (id), SYM_CLASS_LABEL);
#define symtbl_get_tag_of_struct_entry(symtbl, id) symtbl_get_entry((symtbl), (id), SYM_CLASS_TAG_OF_STRUCT);
#define symtbl_get_union_entry(symtbl, id) symtbl_get_entry((symtbl), (id), SYM_CLASS_UNION);
#define symtbl_get_enumeration_entry(symtbl, id) symtbl_get_entry((symtbl), (id), SYM_CLASS_ENUMERATION);
#define symtbl_get_struct_union_member_entry(symtbl, id) symtbl_get_entry((symtbl), (id), SYM_CLASS_STRUCT_UNION_MEMBER);

extern SymtblEntry *symtbl_add_entry(Symtbl *symtbl, sv_t *id, IRDataType *dtype, const SymtblEntryClass eclass);

#define symtbl_add_object_entry(symtbl, id) symtbl_add_entry((symtbl), (id), SYM_CLASS_OBJECT);
#define symtbl_add_function_entry(symtbl, id) symtbl_add_entry((symtbl), (id), SYM_CLASS_FUNCTION);
#define symtbl_add_typedef_name_entry(symtbl, id) symtbl_add_entry((symtbl), (id), SYM_CLASS_TYPEDEF_NAME);
#define symtbl_add_enum_constant_entry(symtbl, id) symtbl_add_entry((symtbl), (id), SYM_CLASS_ENUM_CONSTANT);
#define symtbl_add_label_entry(symtbl, id) symtbl_add_entry((symtbl), (id), SYM_CLASS_LABEL);
#define symtbl_add_tag_of_struct_entry(symtbl, id) symtbl_add_entry((symtbl), (id), SYM_CLASS_TAG_OF_STRUCT);
#define symtbl_add_union_entry(symtbl, id) symtbl_add_entry((symtbl), (id), SYM_CLASS_UNION);
#define symtbl_add_enumeration_entry(symtbl, id) symtbl_add_entry((symtbl), (id), SYM_CLASS_ENUMERATION);
#define symtbl_add_struct_union_member_entry(symtbl, id) symtbl_add_entry((symtbl), (id), SYM_CLASS_STRUCT_UNION_MEMBER);

#endif
