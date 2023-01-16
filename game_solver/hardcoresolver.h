#ifndef _HARDCORESOLVER_H_
#define _HARDCORESOLVER_H_

#include <stdio.h>
#include <time.h>
#include "util.h"
#include "types.h"
#include "random.h"

#define NUM_PLAYERS 2
#define NATURE_PLAYER_ID 255
#define SHOULD_KEEP_TRAINING(num_iterations, start_time, timeout) (\
    ( ( (clock() - start_time) / CLOCKS_PER_SEC ) <= timeout ) \
    && (num_iterations < 1000000) \
)

list_double normalize_strategy(list_double);
list_double get_negative_clipped(list_double);
void match_regret(info_state_strategy*);
double compute_reach_contribution(uint8, pcontrb_stack*);
double compute_reach_support(uint8, pcontrb_stack*);
list_double play_decision_node(game_node*, game_tree*, pcontrb_stack*);
list_double play_nature_node(game_node*, game_tree*, pcontrb_stack*);
list_double play_terminal_node(game_node*, game_tree*, pcontrb_stack*);
list_double play_dual_game(game_node*, game_tree*, pcontrb_stack*);
void update_strategies(game_tree*);
game_tree _solve_game(game_tree, uint32*, uint32);

static list_double (*node_play[])(game_node*, game_tree*, pcontrb_stack*) = {
    &play_decision_node,
    &play_nature_node,
    &play_terminal_node
};

#endif
