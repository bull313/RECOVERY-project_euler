from json import load as json_load
from json import dumps as json_str
from sys import argv
from random import choices

strategies = dict()

def traverse_game(subgame):
    global strategies

    for key in subgame:

        if key == "info_state":
            player = subgame["player"]
            info_state = subgame[key]
            action_names = subgame["action_names"]
            strategy = subgame["strategy"]

            if player not in strategies.keys():
                strategies.update({ player: dict() })
            
            strat_weights = [ round(sv, 2) for sv in strategy ]
            random_selection = choices(action_names, weights=strat_weights)[0]

            strategies[player].update({
                info_state: {
                        "action_names": action_names,
                        "strategy": strategy,
                        "random_selection": random_selection
                    }
            })
        
        if key == "next":
            
            for next_game in subgame["next"]:
                traverse_game(next_game)

def get_clean(solution):
    global strategies

    strategies = dict()
    traverse_game(solution)

    return strategies

def get_cleaned_solution(soln_file):
    solution = None

    with open(soln_file, "r") as f:
        solution = json_load(f)
    
    return get_clean(solution)

def main():
    soln_file = argv[1]
    cleaned_solution = get_cleaned_solution(soln_file)

    with open(argv[1][:-(5 + len("solution"))] + "strategy.json", "w+") as f:
        f.write(json_str(cleaned_solution, indent=4))

if __name__ == "__main__":
    main()
