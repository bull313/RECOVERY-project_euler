from copy import deepcopy
from itertools import permutations
from json import dumps
from sys import argv

num_players = 2
pot_size = 2
constant_sum = 2
cards = [ "J", "Q", "K", "A" ]

subgame_template = {
    "player": 0,
    "info_state": "%s",
    "action_names": [ "Check", "Bet" ],
    "next": [
        {
            "player": 1,
            "info_state": "%s - Check",
            "action_names": [ "Check", "Bet" ],
            "next": [
                { pot_size },
                {
                    "player": 0,
                    "info_state": "%s - Check Bet",
                    "action_names": [ "Fold", "Call" ],
                    "next": [
                        [ 0, pot_size ],
                        { pot_size + 1 }
                    ]
                }
            ]
        },
        {
            "player": 1,
            "info_state": "%s - Bet",
            "action_names": [ "Fold", "Call" ],
            "next": [
                [ pot_size, 0 ],
                { pot_size + 1 }
            ]
        }
    ]
}

def valuate_card(card):
    return { cards[i]: i for i in range(len(cards)) }[card]

def showdown(card1, card2, profit):
    loss = constant_sum - profit
    value1 = valuate_card(card1)
    value2 = valuate_card(card2)
    
    return profit if value1 > value2 else loss

def is_util(node):
    return type(node) is set

def is_payoff(node):
    return type(node) is list

def get_payoff(util_node, cards):
    profit = list(util_node)[0]
    value1 = showdown(*cards, profit)
    value2 = showdown(*cards[::-1], profit)
    
    return [ value1, value2 ]
    
def gen_subgame(card_pair, template):
    
    if is_util(template):
        return {
            "utility": get_payoff(template, card_pair)
        }
    
    if is_payoff(template):
        return {
            "utility": template
        }
    
    player = template["player"]
    
    template["info_state"] = template["info_state"] % card_pair[player]
    
    for i in range(len(template["next"])):
        template["next"][i] = gen_subgame(card_pair, template["next"][i])

    return template

def generate_game(template):
    subgame_list = list()
    deals = permutations(cards, num_players)
    
    for deal in deals:
        subgame_template = deepcopy(template)
        subgame = gen_subgame(deal, subgame_template)
        subgame_list.append(subgame)
        
    game = { "next": subgame_list }
    
    return game

def main():
    filename = argv[1]
    game = generate_game(subgame_template)
    gamestr = dumps(game, indent=4)
    
    with open(filename, "w+") as f:
        f.write(gamestr)
    
    print("Done!")

if __name__ == "__main__":
    main()
