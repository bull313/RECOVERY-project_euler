#include "hardcoresolver.h"
#include "pytypes.h"

string pyobj_to_string(PyObject* pyobj) {
    uint32 i;
    PyObject* pyencoded_string;
    string bytes;
    string result;

    pyencoded_string = PyUnicode_AsEncodedString(pyobj, "UTF-8", "?");
    bytes = PyBytes_AsString(pyencoded_string);
    result = (string) malloc(( strlen(bytes) + 1 ) * sizeof(char));

    for (i = 0; i < strlen(bytes); ++i) {
        *(result + i) = *(bytes + i);
    }

    *(result + strlen(bytes)) = '\0';

    Py_XDECREF(pyencoded_string);

    return result;
}

list_string pylist_to_string_list(PyObject* pylist) {
    PYLIST_CONV(string, pyobj_to_string)
}

list_double pylist_to_double_list(PyObject* pylist) {
    PYLIST_CONV(double, PyFloat_AsDouble)
}

void update_player_prop(PyObject* v, game_node** node, game_tree* game) {
    uint8 player;

    INIT_GAME_NODE(*node, decision_node, DECISION_NODE)
    
    player = (uint8) PyLong_AsLong(v);
    ((decision_node*) *node)->player = player;
}

void update_info_state_prop(PyObject* v, game_node** node, game_tree* game) {
    uint32 i;
    sint32 info_state_addr;
    string info_state_name;
    info_state_strategy info_strategy;

    INIT_GAME_NODE(*node, decision_node, DECISION_NODE)

    info_state_name = pyobj_to_string(v);
    info_state_addr = -1;

    for (i = 0; i < game->info_state_names.length; ++i) {
        info_state_addr = ( BOOL( STR_EQUAL(
            info_state_name,
            *(game->info_state_names.data + i)
        ) ) == TRUE ) ? i : info_state_addr;
    }

    if (info_state_addr == -1) {
        list_string_add(&game->info_state_names, info_state_name);
        info_state_addr = game->info_state_names.length - 1;

        info_strategy.player = ((decision_node*) *node)->player;
        info_strategy.cumulative_regret = list_double_init();
        info_strategy.cumulative_strategy = list_double_init();
        info_strategy.current_strategy = list_double_init();
        info_strategy.reach_contribution = 0.0;

        list_info_state_strategy_add(&game->info_strategies, info_strategy);
    }

    ((decision_node*) *node)->info_state = info_state_addr;
}

void update_action_names_prop(PyObject* v, game_node** node, game_tree* game) {
    list_string action_names;

    INIT_GAME_NODE(*node, decision_node, DECISION_NODE)

    action_names = pylist_to_string_list(v);

    ((decision_node*) *node)->action_names = action_names;
}

void update_utility_prop(PyObject* v, game_node** node, game_tree* game) {
    list_double utility;

    INIT_GAME_NODE(*node, terminal_node, TERMINAL_NODE)
    
    utility = pylist_to_double_list(v);
    ((terminal_node*) *node)->utility = utility;
}

void update_next_prop(PyObject* v, game_node** node, game_tree* game) {
    double uniform_probability;
    PyObject* pychild;
    uint32 i;
    uint32 info_state;
    uint32 num_actions;
    game_node* child_node;
    info_state_strategy* info_strategy;

    INIT_GAME_NODE(*node, nature_node, NATURE_NODE)
    
    num_actions = PyObject_Length(v);
    ((nonterminal_game_node*) *node)->next = list_game_node_ptr_init();

    for (i = 0; i < num_actions; ++i) {
        pychild = PyList_GetItem(v, i);
        child_node = traverse_pydict(pychild, game);
        list_game_node_ptr_add(
            &((nonterminal_game_node*) *node)->next,
            child_node
        );
    }

    if ((*node)->type == DECISION_NODE) {
        info_state = ((decision_node*) *node)->info_state;
        uniform_probability = 1.0 / (double) num_actions;
        info_strategy = list_info_state_strategy_get(
            game->info_strategies,
            info_state
        );

        if (info_strategy->current_strategy.length == 0) {
            for (i = 0; i < PyObject_Length(v); ++i) {
                list_double_add(&info_strategy->cumulative_regret, 0.0);
                
                list_double_add(
                    &info_strategy->cumulative_strategy,
                    uniform_probability
                );
                
                list_double_add(
                    &info_strategy->current_strategy,
                    uniform_probability
                );
            }
        }
    }
}

void update_strategy_prop(PyObject* v, game_node** node, game_tree* game) {
    uint32 i;
    uint32 info_state;
    list_double strategy_distribution;
    info_state_strategy* info_strategy;

    strategy_distribution = pylist_to_double_list(v);
    info_state = ((decision_node*) *node)->info_state;
    info_strategy = list_info_state_strategy_get(
        game->info_strategies,
        info_state
    );

    if (info_strategy->current_strategy.length == 0) {
        for (i = 0; i < strategy_distribution.length; ++i) {
            list_double_add(&info_strategy->cumulative_regret, 0.0);
            
            list_double_add(
                &info_strategy->cumulative_strategy,
                *list_double_get(strategy_distribution, i)
            );
            
            list_double_add(
                &info_strategy->current_strategy,
                *list_double_get(strategy_distribution, i)
            );
        }
    }
}

uint8 parse_prop_name(string prop_name) {
    uint8 i;
    uint8 prop_idx;

    prop_idx = -1;

    for (i = 0; i < ARR_LEN(PROP_NAMES); ++i) {
        if (STR_EQUAL(PROP_NAMES[i], prop_name) == TRUE) {
            prop_idx = i;
        }
    }

    return prop_idx;
}

game_node* traverse_pydict(PyObject* pydict, game_tree* game) {
    PyObject* pykey;
    PyObject* pyvalue;
    Py_ssize_t pos;
    string prop_name;
    uint8 prop_idx;
    void (*prop_routine)(PyObject*, game_node**, game_tree*);
    game_node* node;

    pos = 0;
    node = NULL;

    while ( PyDict_Next(pydict, &pos, &pykey, &pyvalue) ) {
        prop_name = pyobj_to_string(pykey);
        prop_idx = parse_prop_name(prop_name);
        prop_routine = prop_routines[prop_idx];
        prop_routine(pyvalue, &node, game);
    }

    return node;
}

game_tree pydict_to_game_tree(PyObject* pydict) {
    game_tree game;

    game.info_state_names = list_string_init();
    game.info_strategies = list_info_state_strategy_init();
    game.root = traverse_pydict(pydict, &game);

    return game;
}

PyObject* get_next_nodes(list_game_node_ptr node_list, game_tree* game) {
    PyObject* pylist;
    PyObject* pynextdict;
    game_node* next;
    uint32 i;

    pylist = PyList_New(node_list.length);

    for (i = 0; i < node_list.length; ++i) {
        next = *list_game_node_ptr_get(node_list, i);
        pynextdict = traverse_output_game_tree(next, game);
        PyList_SetItem(pylist, i, pynextdict);
    }

    return pylist;
}

PyObject* decision_node_to_pydict(game_node* node, game_tree* game) {
    double action_probability;
    PyObject* pykey;
    PyObject* pyvalue;
    PyObject* pydict;
    PyObject* pystring;
    PyObject* pydouble;
    decision_node decision;
    string info_state_name;
    string action_name;
    info_state_strategy strategy;
    list_double average_strategy;
    uint32 i;

    pydict = PyDict_New();
    decision = *((decision_node*) node);

    pykey = Py_BuildValue(PY_STRING, "player");
    pyvalue = Py_BuildValue(PY_INT, decision.player);
    PyDict_SetItem(pydict, pykey, pyvalue);

    pykey = Py_BuildValue(PY_STRING, "info_state");
    info_state_name = *list_string_get(
        game->info_state_names,
        decision.info_state
    );
    pyvalue = Py_BuildValue(PY_STRING, info_state_name);
    PyDict_SetItem(pydict, pykey, pyvalue);

    pykey = Py_BuildValue(PY_STRING, "action_names");
    pyvalue = PyList_New(decision.action_names.length);
    
    for (i = 0; i < decision.action_names.length; ++i) {
        action_name = *list_string_get(decision.action_names, i);
        pystring = Py_BuildValue(PY_STRING, action_name);
        PyList_SetItem(pyvalue, i, pystring);
    }

    PyDict_SetItem(pydict, pykey, pyvalue);

    pykey = Py_BuildValue(PY_STRING, "strategy");
    pyvalue = PyList_New(decision.next.length);
    strategy = *list_info_state_strategy_get(
        game->info_strategies,
        decision.info_state
    );

    average_strategy = normalize_strategy(strategy.cumulative_strategy);

    for (i = 0; i < decision.next.length; ++i) {
        action_probability = *list_double_get(average_strategy, i);
        pydouble = Py_BuildValue(PY_DOUBLE, action_probability);
        PyList_SetItem(pyvalue, i, pydouble);
    }

    PyDict_SetItem(pydict, pykey, pyvalue);

    pykey = Py_BuildValue(PY_STRING, "next");
    pyvalue = get_next_nodes(decision.next, game);
    PyDict_SetItem(pydict, pykey, pyvalue);

    return pydict;
}

PyObject* nature_node_to_pydict(game_node* node, game_tree* game) {
    PyObject* pykey;
    PyObject* pyvalue;
    PyObject* pydict;
    nature_node nature;

    pydict = PyDict_New();
    nature = *((nature_node*) node);
    pykey = Py_BuildValue(PY_STRING, "next");
    pyvalue = get_next_nodes(nature.next, game);
    
    PyDict_SetItem(pydict, pykey, pyvalue);

    return pydict;
}

PyObject* terminal_node_to_pydict(game_node* node, game_tree* game) {
    double player_utility;
    PyObject* pykey;
    PyObject* pyvalue;
    PyObject* pydict;
    PyObject* pydouble;
    terminal_node terminal;
    uint32 i;

    pydict = PyDict_New();
    terminal = *((terminal_node*) node);

    pykey = Py_BuildValue(PY_STRING, "utility");
    pyvalue = PyList_New(terminal.utility.length);

    for (i = 0; i < terminal.utility.length; ++i) {
        player_utility = *list_double_get(terminal.utility, i);
        pydouble = Py_BuildValue(PY_DOUBLE, player_utility);
        PyList_SetItem(pyvalue, i, pydouble);
    }

    PyDict_SetItem(pydict, pykey, pyvalue);

    return pydict;
}

PyObject* traverse_output_game_tree(game_node* node, game_tree* game) {
    PyObject* (*node_to_pydict)(game_node*, game_tree*);
    PyObject* pydict;

    node_to_pydict = node_parsers[(int) node->type];
    pydict = node_to_pydict(node, game);

    return pydict;
}

PyObject* game_tree_to_pydict(game_tree game) {
    return traverse_output_game_tree(game.root, &game);
}

list_uint32 pytuple_to_uint32(PyObject* pytuple) {
    uint32 i;
    uint32 value;
    list_uint32 list;

    list = list_uint32_init();

    for (i = 0; i < PyObject_Length(pytuple); ++i) {
        value = (uint32) PyLong_AsLong(
            PyTuple_GetItem(pytuple, i)
        );

        list_uint32_add(&list, value);
    }

    return list;
}

PyObject* list_uint32_to_pytuple(list_uint32 arr) {
    uint32 i;
    uint32 value;
    PyObject* pyint;
    PyObject* pytuple;

    pytuple = PyTuple_New(arr.length);

    for (i = 0; i < arr.length; ++i) {
        value = *list_uint32_get(arr, i);
        pyint = Py_BuildValue(PY_INT, value);
        PyTuple_SetItem(pytuple, i, pyint);
    }

    return pytuple;
}
