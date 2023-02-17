from json import load as json_load
from random import choices
from sys import argv

def main():
    filename = argv[1]
    player = argv[2]
    info_state = argv[3]

    game = None

    with open(filename, "r") as f:
        game = json_load(f)

    if player in game.keys() and info_state in game[player].keys():
        strategy = game[player][info_state]["strategy"]
        action_names = game[player][info_state]["action_names"]

        print(f"{info_state} strategy:\n")
        for i in range(len(action_names)):
            action = action_names[i]
            mix = strategy[i]
            print(f"\t{action}: {mix * 100.0:.2f}%")
        
        selection = choices(action_names, weights=strategy)[0]

        print(f"\nSelected action this turn: {selection}\n")
    else:
        print("Strategy not found\n")

if __name__ == "__main__":
    main()
