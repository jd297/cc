/**
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

#ifndef JD297_LIST_H
#define JD297_LIST_H

#include <stddef.h>

typedef struct list_node_t {
    void *value;
    struct list_node_t *next;
    struct list_node_t *prev;
} list_node_t;

typedef struct {
    list_node_t *begin;
    list_node_t *end;
} list_t;

extern int list_create(list_t *list);

extern void list_free(list_t *list);

extern int list_clear(list_t *list);

extern int list_empty(list_t *list);

extern list_node_t *list_insert(list_t *list, list_node_t *pos, void *value);

extern list_node_t *list_erase(list_t *list, list_node_t *pos);

extern list_node_t *list_insert_range(list_t *list, list_node_t *pos, list_node_t *begin, list_node_t *end);

#define list_begin(list) (list)->begin
#define list_end(list) (list)->end
#define list_next(node) (node)->next
#define list_prev(node) (node)->prev

#endif

#ifdef JD297_LIST_IMPLEMENTATION

#include <stdlib.h>

int list_create(list_t *list)
{
	list->end = NULL;

	list->begin = calloc(1, sizeof(list_node_t));

	if (list->begin == NULL) {
		return -1;
	}
	
	list->end = list->begin;

	return 0;
}

int list_empty(list_t *list)
{
	return list->begin == list->end;
}

list_node_t *list_insert(list_t *list, list_node_t *pos, void *value)
{
	list_node_t *node = calloc(1, sizeof(list_node_t));
	
	if (node == NULL) {
		return NULL;
	}

	if (pos == list->begin) {
		list->begin = node;
	} else {
		pos->prev->next = node;
	}

	node->value = value;

	node->prev = pos->prev;
	node->next = pos;
	pos->prev = node;

	return node;
}

list_node_t *list_erase(list_t *list, list_node_t *pos)
{
	list_node_t *next;
	
	if (list->end == pos->next) {
			next = list->end;
	} else {
		pos->next->prev = pos->prev;
		next = pos->prev->next = pos->next;
	}
	
	if (list->begin == pos) {
		list->begin = next;
	}
	
	free(pos);
	
	return next;
}

list_node_t *list_insert_range(list_t *list, list_node_t *pos, list_node_t *begin, list_node_t *end)
{
	list_node_t *it;

	for (it = begin; it != end; it = list_next(it)) {
		pos = list_insert(list, pos == list_end(list) ? pos : list_next(pos), it->value);
	}
	
	return pos;
}

void list_free(list_t *list)
{
	for (list_node_t *it = list->begin; it != NULL; ) {
		list_node_t *next = it->next;

		free(it);

		it = next;
	}
}

int list_clear(list_t *list)
{
	list_free(list);
	
	return list_create(list);
}

#endif
