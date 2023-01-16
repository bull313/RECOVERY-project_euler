#ifndef _PYTYPES_H_
#define _PYTYPES_H_

#include <Python.h>

#include "types.h"
#include "util.h"

#define PY_STRING "s"
#define PY_INT "I"
#define PY_DICT "O"
#define PY_DOUBLE "d"
#define PY_DICTDOUBLE "Od"
#define PY_TUPLE "O"

#define PYLIST_CONV(type, typeconv) uint32 i; \
type item; \
list_ ## type list; \
PyObject* pyitem; \
\
list = list_ ## type ## _init(); \
\
for (i = 0; i < PyObject_Length(pylist); ++i) { \
    pyitem = PyList_GetItem(pylist, i); \
    item = typeconv(pyitem); \
    list_ ## type ## _add(&list, item); \
} \
\
return list; \

static const string PROP_NAMES[] = {
    "player",
    "info_state",
    "action_names",
    "utility",
    "next",
    "strategy"
};

string pyobj_to_string(PyObject*);
list_string pylist_to_string_list(PyObject*);
list_double pylist_to_double_list(PyObject*);
void update_player_prop(PyObject*, game_node**, game_tree*);
void update_info_state_prop(PyObject*, game_node**, game_tree*);
void update_action_names_prop(PyObject*, game_node**, game_tree*);
void update_utility_prop(PyObject*, game_node**, game_tree*);
void update_next_prop(PyObject*, game_node**, game_tree*);
void update_strategy_prop(PyObject*, game_node**, game_tree*);
uint8 parse_prop_name(string);
game_node* traverse_pydict(PyObject*, game_tree*);
game_tree pydict_to_game_tree(PyObject*);
PyObject* get_next_nodes(list_game_node_ptr, game_tree*);
PyObject* decision_node_to_pydict(game_node*, game_tree*);
PyObject* nature_node_to_pydict(game_node*, game_tree*);
PyObject* terminal_node_to_pydict(game_node*, game_tree*);
PyObject* traverse_output_game_tree(game_node*, game_tree*);
PyObject* game_tree_to_pydict(game_tree);
list_uint32 pytuple_to_uint32(PyObject*);
PyObject* list_uint32_to_pytuple(list_uint32);

static void (*prop_routines[])(PyObject*, game_node**, game_tree*) = {
    &update_player_prop,
    &update_info_state_prop,
    &update_action_names_prop,
    &update_utility_prop,
    &update_next_prop,
    &update_strategy_prop
};

static PyObject* (*node_parsers[])(game_node*, game_tree*) = {
    &decision_node_to_pydict,
    &nature_node_to_pydict,
    &terminal_node_to_pydict
};

#endif
