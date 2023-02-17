#include "hardcoresolver.h"

list_double normalize_strategy(list_double strategy) {
    double uniform_probability;
    double norm1;
    double probability;
    list_double normalized;
    uint32 i;

    normalized = list_double_init();
    uniform_probability = 1.0 / strategy.length;
    norm1 = 0.0;

    for (i = 0; i < strategy.length; ++i) {
        norm1 += *list_double_get(strategy, i);
        list_double_add(&normalized, uniform_probability);
    }

    if (norm1 > 0) {
        list_double_close(&normalized);
        normalized = list_double_init();

        for (i = 0; i < strategy.length; ++i) {
            probability = *list_double_get(strategy, i) / norm1;
            list_double_add(&normalized, probability);
        }
    }

    return normalized;
}

list_double get_negative_clipped(list_double list) {
    double clipped_value;
    uint32 i;
    list_double clipped_list;

    clipped_list = list_double_init();

    for (i = 0; i < list.length; ++i) {
        clipped_value = MAX( *list_double_get(list, i), 0 );
        list_double_add(&clipped_list, clipped_value);
    }

    return clipped_list;
}

void match_regret(info_state_strategy* strategy) {
    double weighted_prob;
    list_double cumulative_regret_plus;
    list_double regret_match_strat;
    uint32 i;

    cumulative_regret_plus = get_negative_clipped(strategy->cumulative_regret);
    regret_match_strat = normalize_strategy(cumulative_regret_plus);
    
    for (i = 0; i < regret_match_strat.length; ++i) {
        weighted_prob = *list_double_get(regret_match_strat, i);
        weighted_prob *= strategy->reach_contribution;

        list_double_set(
            &strategy->cumulative_strategy,
            i,
            *list_double_get(strategy->cumulative_strategy, i) + weighted_prob
        );
    }

    strategy->current_strategy = regret_match_strat;
}

double compute_reach_support(uint8 player, pcontrb_stack* pcs) {
    double probability;
    pcontrb probability_contribution;
    uint32 i;

    probability = 1.0;

    for (i = 0; i < pcs->size; ++i) {
        probability_contribution = *pcontrb_stack_peek(*pcs, i);

        if (probability_contribution.player != player) {
            probability *= probability_contribution.contribution;
        }
    }

    return probability;
}

double compute_reach_contribution(uint8 player, pcontrb_stack* pcs) {
    double probability;
    pcontrb probability_contribution;
    uint32 i;

    probability = 1.0;

    for (i = 0; i < pcs->size; ++i) {
        probability_contribution = *pcontrb_stack_peek(*pcs, i);

        if (probability_contribution.player == player) {
            probability *= probability_contribution.contribution;
        }
    }

    return probability;
}

list_double play_decision_node(game_node* n, game_tree* g, pcontrb_stack* cs) {
    double action_pr;
    double player_utility;
    double expected_player_utility;
    double regret;
    double curr_utility;
    double reach_contrb;
    double reach_support;
    list_double utility;
    list_double action_regrets;
    list_double action_utilities;
    list_double immediate_utility;
    decision_node decision;
    info_state_strategy* strategy;
    game_node* next_node;
    pcontrb player_contribution;
    uint8 player;
    uint32 action;
    uint32 i;
    uint32 info_state;

    decision = *((decision_node*) n);
    player = decision.player;
    info_state = decision.info_state;
    strategy = list_info_state_strategy_get(g->info_strategies, info_state);
    action_utilities = list_double_init();
    expected_player_utility = 0.0;
    utility = list_double_init();

    for (action = 0; action < NUM_PLAYERS; ++action) {
        list_double_add(&utility, 0.0);
    }

    for (action = 0; action < decision.next.length; ++action) {
        action_pr = *list_double_get(strategy->current_strategy, action);
        next_node = *list_game_node_ptr_get(decision.next, action);
        player_contribution.player = player;
        player_contribution.contribution = action_pr;
        pcontrb_stack_push(cs, player_contribution);
        immediate_utility = play_dual_game(next_node, g, cs);

        for (i = 0; i < immediate_utility.length; ++i) {
            curr_utility = *list_double_get(utility, i);
            curr_utility += *list_double_get(immediate_utility, i) * action_pr;

            list_double_set(&utility, i, curr_utility);
        }

        player_utility = *list_double_get(immediate_utility, player);
        expected_player_utility += player_utility * action_pr;
        list_double_add(&action_utilities, player_utility);
        pcontrb_stack_pop(cs);
    }

    reach_contrb = compute_reach_contribution(player, cs);
    reach_support = compute_reach_support(player, cs);
    strategy->reach_contribution += reach_contrb;
    action_regrets = list_double_init();

    for (action = 0; action < decision.next.length; ++action) {
        player_utility = *list_double_get(action_utilities, action);
        regret = reach_support * (player_utility - expected_player_utility);
        list_double_add(&action_regrets, regret);
    }

    for (action = 0; action < strategy->cumulative_regret.length; ++action) {
        regret = *list_double_get(strategy->cumulative_regret, action);
        regret += *list_double_get(action_regrets, action);
        
        list_double_set(&strategy->cumulative_regret, action, regret);
    }
    
    return utility;
}

list_double play_nature_node(game_node* n, game_tree* g, pcontrb_stack* cs) {
    double probability;
    double utility_value;
    list_double immediate_utility;
    list_double utility;
    nature_node nature;
    uint32 num_children;
    game_node* next_node;
    pcontrb nature_contribution;
    uint32 num_players;
    uint32 i;
    uint32 j;

    nature = *((nature_node*) n);
    num_children = nature.next.length;
    utility = list_double_init();
    num_players = 0;

    for (i = 0; i < num_children; ++i) {
        probability = 1.0 / (double) num_children;
        next_node = *list_game_node_ptr_get(nature.next, i);
        nature_contribution.player = NATURE_PLAYER_ID;
        nature_contribution.contribution = probability;
        pcontrb_stack_push(cs, nature_contribution);
        immediate_utility = play_dual_game(next_node, g, cs);
        pcontrb_stack_pop(cs);

        if (num_players == 0) {
            num_players = immediate_utility.length;

            for (j = 0; j < num_players; ++j) {
                list_double_add(&utility, 0.0);
            }
        }

        for (j = 0; j < num_players; ++j) {
            utility_value = *list_double_get(utility, j);
            utility_value += *list_double_get(immediate_utility, j);

            list_double_set(&utility, j, utility_value);
        }
    }

    return utility;
}

list_double play_terminal_node(game_node* n, game_tree* g, pcontrb_stack* cs) {
    list_double utility;
    terminal_node terminal;
    uint32 i;

    terminal = *((terminal_node*) n);
    utility = list_double_init();

    for (i = 0; i < terminal.utility.length; ++i) {
        list_double_add(&utility, *list_double_get(terminal.utility, i));
    }

    return utility;
}

list_double play_dual_game(game_node* n, game_tree* game, pcontrb_stack* pcs) {
    list_double (*node_player)(game_node*, game_tree*, pcontrb_stack*);
    list_double utility;

    node_player = node_play[(int) n->type];
    utility = node_player(n, game, pcs);

    return utility;
}

void update_strategies(game_tree* game) {
    uint32 i;
    info_state_strategy* strategy;

    for (i = 0; i < game->info_strategies.length; ++i) {
        strategy = list_info_state_strategy_get(game->info_strategies, i);
        match_regret(strategy);
        strategy->reach_contribution = 0.0;
    }
}

game_tree _solve_game(game_tree game, uint32* iters, uint32 timeout) {
    clock_t start;
    game_tree solution;
    pcontrb_stack parent_probabilities;

    start = clock();

    for (*iters = 0; SHOULD_KEEP_TRAINING(*iters, start, timeout); ++*iters) {
        parent_probabilities = pcontrb_stack_init();
        play_dual_game(game.root, &game, &parent_probabilities);
        update_strategies(&game);
        pcontrb_stack_close(&parent_probabilities);
    }

    return game;
}
