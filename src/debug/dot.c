#include <stdio.h>
#include <stddef.h>

#include "debug/dot.h"
#include "debug/util.h"
#include "parse.h"

static void dot_traverse_tree(ParseTreeNode *this_node);

extern void dot_run(ParseTreeNode *translation_unit)
{
	printf("digraph graphname {\n");

	dot_traverse_tree(translation_unit);

	printf("}\n");
}

static void dot_traverse_tree(ParseTreeNode *this_node)
{
	size_t i;
	const char *typestr;
	
	typestr = parse_tree_type_to_str(this_node->type);
	
	if (parse_tree_type_has_token(this_node->type) && this_node->tok.type != T_EOF) {
		printf("\t_%p [shape=record]\n", (void *)this_node);
		printf("\t_%p [label=\"%s", (void *)this_node, typestr);
		
		printf(" | { TokType | %s }", tok_type_to_str(this_node->tok.type));
		
		if (tok_type_has_literal(this_node->tok.type)) {
			printf(" | { Literal | ");
			tok_type_print_literal(this_node->tok);
			printf(" }");
		}

		if (this_node->sym != NULL) {
			printf(" | { SYM | [TODO] }");
		}

		if (this_node->dtype != NULL) {
			printf(" | { DTYPE | { ");
			ir_print_dtype(this_node->dtype);
			printf("} }");
		}

		printf("\"]\n");
	} else {
		printf("\t_%p [label=\"%s\"]\n",(void *)this_node, typestr);
	}

	for (i = 0; i < this_node->num; ++i) {
		ParseTreeNode *child = this_node->elements[i];

		printf("\t_%p -> _%p\n", (void *)this_node, (void *)child);
		
		dot_traverse_tree(child);
	}
}
