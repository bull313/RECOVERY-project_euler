from json import load as json_load
from json import dumps as json_str
from os.path import exists
from os.path import splitext
from re import search
from sys import argv

from solution import get_clean

import _hardcoresolver as C_IMPL

DEFAULT_TIMEOUT = 5.0

filepath = None

def load_game(game_file):
    global filepath

    filepath = game_file
    game_data = None
    
    with open(game_file, "r") as game:
        game_data = json_load(game)
    
    return game_data

def solve_game(game, timeout):
    if timeout == None:
        timeout = DEFAULT_TIMEOUT

    return C_IMPL.solve_game(game, timeout)

def store_game(game):
    global filepath

    soln_filename = f"{splitext(filepath)[0]}_solution.json"

    if splitext(filepath)[0].endswith("_solution"):
        soln_filename = f"{splitext(filepath)[0]}.json"

    soln_str = json_str(game, indent=4)

    with open(soln_filename, "w+") as soln_file:
        soln_file.write(soln_str)
    
    return soln_filename

def traverse_game(subgame, player):
    info_states = set()

    if "info_state" in subgame.keys():
        
        if subgame["player"] == player:
            info_states.add(subgame["info_state"])

    if "next" in subgame.keys():
        
        for next_node in subgame["next"]:
            info_states |= traverse_game(next_node, player)

    return info_states

def analyze_game(game, num_players):
    info_states = 0

    for player in range(num_players):
        player_info_states = traverse_game(game, player)
        info_states = max(len(player_info_states), info_states)
    
    return info_states

def get_game_regret(game):
    max_num_actions = None
    max_regret = None

    if "max_num_actions" in game.keys():
        max_num_actions = int(game.pop("max_num_actions"))
    if "max_regret" in game.keys():
        max_regret = int(game.pop("max_regret"))
    
    return max_num_actions, max_regret

def get_worst_case_regret(actions, regret, info_states, iterations):
    return regret * ( (actions / (iterations)) ** 0.5 ) * info_states

def main():
    global filepath

    filepath = argv[1]
    timeout = None
    clean = "--clean" in argv

    if len(argv) > 3 and argv[2] == "--timeout":
        timeout = float(argv[3])

    if exists(filepath):
        game = load_game(filepath)

        max_num_actions, max_regret = get_game_regret(game)
        num_info_states = analyze_game(game, 2)
        solution, num_iterations = solve_game(game, timeout)
        soln_file = store_game(solution)

        print(f"Solver underwent {num_iterations} iterations")

        if None not in { max_regret, max_num_actions }:
            wr = get_worst_case_regret(
                max_num_actions,
                max_regret,
                num_info_states,
                num_iterations
            )

            print(f"Each player has no more than {num_info_states} " \
                + "information states")
            print(f"This solution's worst regret is no larger than { wr }")

        print(f"Game solution stored in {soln_file}")

        if clean:
            cleaned_soln = get_clean(solution)
            clean_soln_str = json_str(cleaned_soln, indent=4)
            clean_file = f"{splitext(filepath)[0]}_strategy.json"

            if clean_file[:-len("_strategy.json")].endswith("solution"):
                clean_file = clean_file[:-len("_strategy.json") - len("solution")] \
                    + "strategy.json"

            with open(clean_file, "w+") as cf:
                cf.write(clean_soln_str)

            print(f"Cleaned strategy file stored at {clean_file}")

    else:
        print(f"Game {filepath} could not be found")

if __name__ == "__main__":
    main()
