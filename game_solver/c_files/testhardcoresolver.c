#include "testhardcoresolver.h"
#include "pytypes.h"
#include "hardcoresolver.h"
#include "types.h"

uint32 test_list_string_operations() {
    uint32 test_status;
    string new_item;
    string get_item;
    list_string list;

    test_status = 0;
    list = list_string_init();

    test_status = ASSERT_TRUE((list.length == 0), test_status, 1);
    test_status = ASSERT_TRUE(( *(list.data) == NULL ), test_status, 2);

    new_item = "This is a test!";
    list_string_add(&list, new_item);

    test_status = ASSERT_TRUE(
        STR_EQUAL(*(list.data), "This is a test!"),
        test_status,
        3
    );

    new_item = "This is another test!";
    list_string_add(&list, new_item);

    test_status = ASSERT_TRUE(
        STR_EQUAL(*(list.data), "This is a test!"),
        test_status,
        4
    );

    test_status = ASSERT_TRUE(
        STR_EQUAL(*(list.data + 1), "This is another test!"),
        test_status,
        5
    );

    get_item = *list_string_get(list, 1);
    
    test_status = ASSERT_TRUE(
        STR_EQUAL(get_item, new_item),
        test_status,
        6
    );

    list_string_set(&list, 0, "The set test!");

    get_item = *list_string_get(list, 0);
    
    test_status = ASSERT_TRUE(
        STR_EQUAL(get_item, "The set test!"),
        test_status,
        7
    );

    list_string_close(&list);
    
    return test_status;
}

uint32 test_prop_name_lookup() {
    uint32 test_status;
    uint8 index;
    string prop;

    test_status = 0;
    prop = "player";
    index = parse_prop_name(prop);
    test_status = ASSERT_TRUE((index == 0), test_status, 1);

    prop = "action_names";
    index = parse_prop_name(prop);
    test_status = ASSERT_TRUE((index == 2), test_status, 2);

    prop = "next";
    index = parse_prop_name(prop);
    test_status = ASSERT_TRUE((index == 4), test_status, 3);

    return test_status;
}

void pydict_node_update_init(game_tree* game, uint32* test_status) {
    game->info_state_names = list_string_init();
    game->info_strategies = list_info_state_strategy_init();

    *test_status = ASSERT_TRUE(
        (game->info_state_names.length == 0),
        *test_status,
        10);

    *test_status = ASSERT_TRUE(
        (game->info_strategies.length == 0),
        *test_status,
        11
    );
}

uint32 test_pydict_player_node_update() {
    uint32 test_status;
    game_node* node;
    game_tree game;
    uint32 player;
    PyObject* pyplayerint;

    test_status = 0;
    node = NULL;
    player = 3;
    pyplayerint = Py_BuildValue(PY_INT, player);

    pydict_node_update_init(&game, &test_status);
    update_player_prop(pyplayerint, &node, &game);

    test_status = ASSERT_TRUE(
        (node->type == DECISION_NODE),
        test_status,
        1);

    test_status = ASSERT_TRUE(
        (((decision_node*) node)->player == 3),
        test_status,
        2
    );

    return test_status;
}

uint32 test_pydict_info_state_node_update() {
    uint32 test_status;
    game_node* node;
    game_tree game;
    string info_state_name;
    PyObject* pyinfostatestring;

    test_status = 0;
    node = NULL;
    info_state_name = "Test info state";
    pyinfostatestring = Py_BuildValue(PY_STRING, info_state_name);

    pydict_node_update_init(&game, &test_status);
    update_info_state_prop(pyinfostatestring, &node, &game);

    test_status = ASSERT_TRUE(
        (node->type == DECISION_NODE),
        test_status,
        1
    );

    test_status = ASSERT_TRUE(
        (((decision_node*) node)->info_state == 0),
        test_status,
        2
    );

    test_status = ASSERT_TRUE(
        STR_EQUAL(
            *list_string_get(game.info_state_names, 0),
            "Test info state"
        ),
        test_status,
        3
    );

    return test_status;
}

uint32 test_pydict_action_names_node_update() {
    uint32 i;
    uint32 test_status;
    game_node* node;
    game_tree game;
    PyObject* pyactionnamelist;

    string action_names[2] = { "in", "out" };

    test_status = 0;
    node = NULL;
    pyactionnamelist = PyList_New(2);

    for (i = 0; i < 2; ++i) {
        PyList_SetItem(
            pyactionnamelist,
            i, 
            Py_BuildValue(PY_STRING, action_names[i])
        );
    }

    pydict_node_update_init(&game, &test_status);
    update_action_names_prop(pyactionnamelist, &node, &game);

    test_status = ASSERT_TRUE((node->type == DECISION_NODE), test_status, 1);
    test_status = ASSERT_TRUE(
        (((decision_node*) node)->action_names.length == 2),
        test_status,
        2
    );

    test_status = ASSERT_TRUE(
        STR_EQUAL(
            *list_string_get(((decision_node*) node)->action_names, 0),
            "in"
        ),
        test_status,
        3
    );

    test_status = ASSERT_TRUE(
        STR_EQUAL(
            *list_string_get(((decision_node*) node)->action_names, 1),
            "out"
        ), 
        test_status, 
        4
    );

    return test_status;
}

uint32 test_pydict_utility_node_update() {
    uint32 i;
    uint32 test_status;
    game_node* node;
    game_tree game;
    PyObject* pyutilitylist;

    double utilities[2] = { 0.33, 0.66 };

    test_status = 0;
    node = NULL;
    pyutilitylist = PyList_New(2);

    for (i = 0; i < 2; ++i) {
        PyList_SetItem(
            pyutilitylist,
            i, 
            Py_BuildValue(PY_DOUBLE, utilities[i])
        );
    }

    pydict_node_update_init(&game, &test_status);
    update_utility_prop(pyutilitylist, &node, &game);

    test_status = ASSERT_TRUE((node->type == TERMINAL_NODE), test_status, 1);
    test_status = ASSERT_TRUE(
        (((terminal_node*) node)->utility.length == 2),
        test_status,
        2
    );

    test_status = ASSERT_TRUE(
        (*list_double_get(((terminal_node*) node)->utility, 0) == 0.33),
        test_status,
        3
    );

    test_status = ASSERT_TRUE(
        (*list_double_get(((terminal_node*) node)->utility, 1) == 0.66),
        test_status,
        4
    );

    return test_status;
}

uint32 test_normalize_strategy() {
    uint32 test_status;
    list_double cumulative_strategy;
    list_double normalized_strategy;

    test_status = 0;
    cumulative_strategy = list_double_init();
    list_double_add(&cumulative_strategy, 1.0);
    list_double_add(&cumulative_strategy, 3.0);

    normalized_strategy = normalize_strategy(cumulative_strategy);

    test_status = ASSERT_TRUE(
        ( *list_double_get(normalized_strategy, 0) == 0.25 ),
        test_status,
        1
    );

    test_status = ASSERT_TRUE(
        ( *list_double_get(normalized_strategy, 1) == 0.75 ),
        test_status,
        2
    );

    list_double_close(&cumulative_strategy);
    cumulative_strategy = list_double_init();
    list_double_add(&cumulative_strategy, 0);
    list_double_add(&cumulative_strategy, 0);
    normalized_strategy = normalize_strategy(cumulative_strategy);

    test_status = ASSERT_TRUE(
        ( *list_double_get(normalized_strategy, 0) == 0.5 ),
        test_status,
        3
    );

    test_status = ASSERT_TRUE(
        ( *list_double_get(normalized_strategy, 1) == 0.5 ),
        test_status,
        4
    );

    return test_status;
}

uint32 test_pcontrb_stack_operations() {
    uint32 test_status;
    pcontrb new_item;
    pcontrb pop_item;
    pcontrb peek_item;
    pcontrb_stack stack;

    test_status = 0;
    stack = pcontrb_stack_init();

    test_status = ASSERT_TRUE((stack.size == 0), test_status, 1);

    new_item.player = 1;
    new_item.contribution = 2.3;
    pcontrb_stack_push(&stack, new_item);

    test_status = ASSERT_TRUE((stack.size == 1), test_status, 2);
    test_status = ASSERT_TRUE((stack.data->player == 1), test_status, 3);

    test_status = ASSERT_TRUE((stack.data->contribution == 2.3),
        test_status,
        4
    );

    new_item.player = 4;
    new_item.contribution = 5.6;
    pcontrb_stack_push(&stack, new_item);

    test_status = ASSERT_TRUE((stack.size == 2), test_status, 5);
    test_status = ASSERT_TRUE((stack.data->player == 1), test_status, 6);

    test_status = ASSERT_TRUE(
        (stack.data->contribution == 2.3),
        test_status,
        7
    );

    test_status = ASSERT_TRUE(
        ((stack.data + 1)->player == 4),
        test_status,
        8
    );

    test_status = ASSERT_TRUE(
        ((stack.data + 1)->contribution == 5.6),
        test_status,
        9
    );

    pop_item = *pcontrb_stack_pop(&stack);

    test_status = ASSERT_TRUE((stack.size == 1), test_status, 10);
    test_status = ASSERT_TRUE((stack.data->player == 1), test_status, 11);

    test_status = ASSERT_TRUE(
        (stack.data->contribution == 2.3),
        test_status,
        12
    );

    test_status = ASSERT_TRUE(
        (pop_item.player == 4),
        test_status,
        13
    );

    test_status = ASSERT_TRUE(
        (pop_item.contribution == 5.6),
        test_status,
        14
    );

    peek_item = *pcontrb_stack_peek(stack, 0);

    test_status = ASSERT_TRUE((stack.size == 1), test_status, 15);
    test_status = ASSERT_TRUE((stack.data->player == 1), test_status, 16);

    test_status = ASSERT_TRUE(
        (stack.data->contribution == 2.3),
        test_status,
        17
    );

    test_status = ASSERT_TRUE(
        (peek_item.player == 1),
        test_status,
        18
    );

    test_status = ASSERT_TRUE(
        (peek_item.contribution == 2.3),
        test_status,
        19
    );

    pcontrb_stack_close(&stack);
    
    return test_status;
}

uint32 test_negative_clip() {
    list_double unclipped_list;
    list_double clipped_list;
    uint32 test_status;

    test_status = 0;

    unclipped_list = list_double_init();
    list_double_add(&unclipped_list, 1);
    list_double_add(&unclipped_list, 0);
    list_double_add(&unclipped_list, 10);

    clipped_list = get_negative_clipped(unclipped_list);

    test_status = ASSERT_TRUE(
        (*list_double_get(clipped_list, 0) == 1),
        test_status,
        1
    );

    test_status = ASSERT_TRUE(
        (*list_double_get(clipped_list, 1) == 0),
        test_status,
        2
    );

    test_status = ASSERT_TRUE(
        (*list_double_get(clipped_list, 2) == 10),
        test_status,
        3
    );

    list_double_close(&unclipped_list);

    unclipped_list = list_double_init();
    list_double_add(&unclipped_list, -1);
    list_double_add(&unclipped_list, 0);
    list_double_add(&unclipped_list, 10);

    clipped_list = get_negative_clipped(unclipped_list);

    test_status = ASSERT_TRUE(
        (*list_double_get(clipped_list, 0) == 0),
        test_status,
        4
    );

    test_status = ASSERT_TRUE(
        (*list_double_get(clipped_list, 1) == 0),
        test_status,
        5
    );

    test_status = ASSERT_TRUE(
        (*list_double_get(clipped_list, 2) == 10),
        test_status,
        6
    );

    list_double_close(&unclipped_list);

    unclipped_list = list_double_init();
    list_double_add(&unclipped_list, -1);
    list_double_add(&unclipped_list, 0);
    list_double_add(&unclipped_list, -10);

    clipped_list = get_negative_clipped(unclipped_list);

    test_status = ASSERT_TRUE(
        (*list_double_get(clipped_list, 0) == 0),
        test_status,
        7
    );

    test_status = ASSERT_TRUE(
        (*list_double_get(clipped_list, 1) == 0),
        test_status,
        8
    );

    test_status = ASSERT_TRUE(
        (*list_double_get(clipped_list, 2) == 0),
        test_status,
        9
    );

    list_double_close(&unclipped_list);

    return test_status;
}

uint32 test_regret_match_strategy() {
    uint32 test_status;
    info_state_strategy strategy;
    uint32 i;

    test_status = 0;

    strategy.cumulative_regret = list_double_init();
    strategy.cumulative_strategy = list_double_init();
    strategy.current_strategy = list_double_init();
    strategy.reach_contribution = 0.5;

    list_double_add(&strategy.cumulative_regret, 0.02);
    list_double_add(&strategy.cumulative_regret, -0.08);
    list_double_add(&strategy.cumulative_regret, -0.1);

    list_double_add(&strategy.current_strategy, 0.5);
    list_double_add(&strategy.current_strategy, 0.4);
    list_double_add(&strategy.current_strategy, 0.1);

    list_double_add(&strategy.cumulative_strategy, 0.2);
    list_double_add(&strategy.cumulative_strategy, 0.2);
    list_double_add(&strategy.cumulative_strategy, 0.2);

    match_regret(&strategy);

    test_status = ASSERT_TRUE(
        (strategy.reach_contribution == 0.5),
        test_status,
        1
    );

    test_status = ASSERT_TRUE(
        (*list_double_get(strategy.cumulative_regret, 0) == 0.02),
        test_status,
        2
    );
    
    test_status = ASSERT_TRUE(
        (*list_double_get(strategy.cumulative_regret, 1) == -0.08),
        test_status,
        3
    );

    test_status = ASSERT_TRUE(
        (*list_double_get(strategy.cumulative_regret, 2) == -0.1),
        test_status,
        4
    );

    test_status = ASSERT_TRUE(
        (*list_double_get(strategy.current_strategy, 0) == 1.0),
        test_status,
        5
    );
    
    test_status = ASSERT_TRUE(
        (*list_double_get(strategy.current_strategy, 1) == 0.0),
        test_status,
        6
    );

    test_status = ASSERT_TRUE(
        (*list_double_get(strategy.current_strategy, 2) == 0.0),
        test_status,
        7
    );

    test_status = ASSERT_TRUE(
        (*list_double_get(strategy.cumulative_strategy, 0) == 0.7),
        test_status,
        8
    );
    
    test_status = ASSERT_TRUE(
        (*list_double_get(strategy.cumulative_strategy, 1) == 0.2),
        test_status,
        9
    );

    test_status = ASSERT_TRUE(
        (*list_double_get(strategy.cumulative_strategy, 2) == 0.2),
        test_status,
        10
    );

    list_double_close(&strategy.cumulative_regret);
    list_double_close(&strategy.cumulative_strategy);
    list_double_close(&strategy.current_strategy);

    return test_status;
}

uint32 test_compute_forcing_reach() {
    uint32 test_status;
    pcontrb contrb;
    pcontrb_stack stack;
    double p1_contribution;
    double p2_contribution;

    test_status = 0;

    stack = pcontrb_stack_init();
    
    contrb.player = 0;
    contrb.contribution = 0.1;
    pcontrb_stack_push(&stack, contrb);

    contrb.player = 1;
    contrb.contribution = 0.2;
    pcontrb_stack_push(&stack, contrb);

    contrb.player = 0;
    contrb.contribution = 0.3;
    pcontrb_stack_push(&stack, contrb);

    contrb.player = 1;
    contrb.contribution = 0.4;
    pcontrb_stack_push(&stack, contrb);

    p1_contribution = compute_reach_contribution(1, &stack);
    p2_contribution = compute_reach_contribution(0, &stack);

    test_status = ASSERT_TRUE(
        EQ_WITHIN(p1_contribution, 0.03, 3),
        test_status,
        1
    );

    test_status = ASSERT_TRUE(
        EQ_WITHIN(p2_contribution, 0.08, 3),
        test_status,
        2
    );

    pcontrb_stack_close(&stack);

    stack = pcontrb_stack_init();

    p1_contribution = compute_reach_contribution(1, &stack);
    p2_contribution = compute_reach_contribution(0, &stack);

    test_status = ASSERT_TRUE(
        EQ_WITHIN(p1_contribution, 1.0, 8),
        test_status,
        3
    );

    test_status = ASSERT_TRUE(
        EQ_WITHIN(p2_contribution, 1.0, 8),
        test_status,
        4
    );

    pcontrb_stack_close(&stack);

    return test_status;
}

uint32 _run_test_suite() {
    uint32 i;
    uint32 test_code;
    uint32 result;

    result = FALSE;

    for (i = 0; i < ARR_LEN(tests); ++i) {
        test_code = (*tests[i])();

        if (test_code != 0) {
            result = ((i + 1) << 16) | test_code;
            break;
        }
    }

    return result;
}
