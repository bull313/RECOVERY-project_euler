#ifndef _TYPES_H_
#define _TYPES_H_

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "primitivetypes.h"
#include "list.h"
#include "stack.h"

#define STRING_EQUAL(str1, str2) ( (strcmp(str1, str2) == 0) ? TRUE : FALSE )
#define NODE_DATA \
    node_type type;
#define NONTERM_NODE_DATA \
    node_type type; \
    list_game_node_ptr next;
#define INIT_GAME_NODE(node, ntype, type_val) if (node == NULL) { \
    ntype game_data; \
    game_data.type = type_val; \
    node = malloc(sizeof(ntype)); \
    memcpy(node, &game_data, sizeof(ntype)); \
}

LIST_DEFINE(string)
LIST_DEFINE(double)
LIST_DEFINE(uint32)

typedef enum {
    DECISION_NODE,
    NATURE_NODE, 
    TERMINAL_NODE
} node_type;

typedef struct {
    NODE_DATA
} game_node;

typedef game_node* game_node_ptr;

LIST_DEFINE(game_node_ptr)

typedef struct {
    NONTERM_NODE_DATA
} nonterminal_game_node;

typedef struct {
    NONTERM_NODE_DATA
    uint8 player;
    uint32 info_state;
    list_string action_names;
} decision_node;

typedef struct {
    NONTERM_NODE_DATA
} nature_node;

typedef struct {
    NODE_DATA
    list_double utility;
} terminal_node;

typedef struct {
    uint8 player;
    list_double cumulative_regret;
    list_double cumulative_strategy;
    double reach_contribution;
    list_double current_strategy;
} info_state_strategy;

LIST_DEFINE(info_state_strategy);

typedef struct {
    game_node* root;
    list_string info_state_names;
    list_info_state_strategy info_strategies;
} game_tree;

typedef struct {
    uint8 player;
    double contribution;
} pcontrb;

STACK_DEFINE(pcontrb)

#endif
