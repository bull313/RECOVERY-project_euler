#ifndef _TESTHARDCORESOLVER_H_
#define _TESTHARDCORESOLVER_H_

#include <stdio.h>
#include "types.h"

#define ASSERT_TRUE(condition, test_status, err_code) ( \
    (BOOL(condition) == TRUE || test_status > 0) ? test_status : err_code \
)
#define EQ_WITHIN(value1, value2, round_place) ( \
    value1 >= value2 - (round_place / 100.0) \
    && value1 <= value2 + (round_place / 100.0) )

uint32 _run_test_suite();
uint32 test_list_string_operations();
uint32 test_prop_name_lookup();
void pydict_node_update_init(game_tree*, uint32*);
uint32 test_pydict_player_node_update();
uint32 test_pydict_info_state_node_update();
uint32 test_pydict_action_names_node_update();
uint32 test_pydict_utility_node_update();
uint32 test_normalize_strategy();
uint32 test_pcontrb_stack_operations();
uint32 test_negative_clip();
uint32 test_regret_match_strategy();
uint32 test_compute_forcing_reach();

static uint32 (*tests[])() = {
    &test_list_string_operations,                       // Test Case 1
    &test_prop_name_lookup,                             // Test Case 2
    &test_pydict_player_node_update,                    // Test Case 3
    &test_pydict_info_state_node_update,                // Test Case 4
    &test_pydict_action_names_node_update,              // Test Case 5
    &test_pydict_utility_node_update,                   // Test Case 6
    &test_normalize_strategy,                           // Test Case 7
    &test_pcontrb_stack_operations,                     // Test Case 8
    &test_negative_clip,                                // Test Case 9
    &test_regret_match_strategy,                        // Test Case 10
    &test_compute_forcing_reach                         // Test Case 11
};

#endif
