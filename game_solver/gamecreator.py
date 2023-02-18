from copy import deepcopy
from itertools import permutations
from json import dumps
from sys import argv

board_size = 1
card_values = [ str(i) for i in range(2, 10) ] + [ "T", "J", "Q", "K", "A" ]
num_players = 2
pot_size = 2
constant_sum = 2
game_cards = [ "J", "Q", "K" ]

kuhn_template = {
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

raise_kuhn_template = {
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
                    "action_names": [ "Fold", "Call", "Raise" ],
                    "next": [
                        [ 0, pot_size ],
                        { pot_size + 1 },
                        {
                            "player": 1,
                            "info_state": "%s - Check Bet Raise",
                            "action_names": [ "Fold", "Call" ],
                            "next": [
                                [ 3, -1 ],
                                { pot_size + 2 }
                            ]
                        }
                    ]
                }
            ]
        },
        {
            "player": 1,
            "info_state": "%s - Bet",
            "action_names": [ "Fold", "Call", "Raise" ],
            "next": [
                [ pot_size, 0 ],
                { pot_size + 1 },
                {
                    "player": 0,
                    "info_state": "%s - Bet Raise",
                    "action_names": [ "Fold", "Call" ],
                    "next": [
                        [ -1, 3 ],
                        { pot_size + 2 }
                    ]
                }
            ]
        }
    ]
}

subgame_template = ( kuhn_template )

def valuate_card(card, board=None):
    hand_type = 0

    if board is None:
        board = set()
    
    board_values = set([ b[0] for b in board ])
    
    if card[0] in board_values:
        hand_type = 1

    return (hand_type << 4) | { card_values[i]: i for i in range(len(card_values)) }[card[0]]

def showdown(card1, card2, profit, board=None):
    split = int(constant_sum / 2)
    loss = constant_sum - profit
    value1 = valuate_card(card1, board)
    value2 = valuate_card(card2, board)
    
    return profit if value1 > value2 \
        else split if value1 == value2 \
        else loss

def is_util(node):
    return type(node) is set or ( type(node) is dict and len(node.keys()) == 1 and list(node.keys())[0] == "utility" )

def is_payoff(node):
    return type(node) is list

def get_payoff(util_node, cards, board=None):
    profit = list(util_node)[0]
    value1 = showdown(*cards, profit, board)
    value2 = showdown(*cards[::-1], profit, board)
    
    return [ value1, value2 ]
    
def gen_subgame(card_pair, template, board=None):

    if type(template) is int:
        return template
    
    if is_util(template):
        return {
            "utility": get_payoff(template, card_pair, board)
        }
    
    if is_payoff(template):
        return {
            "utility": template
        }
    
    player = template["player"]

    if template["info_state"].count("%s") == 1:
        template["info_state"] = template["info_state"] % card_pair[player]
    else:
        template["info_state"] = template["info_state"] % (card_pair[player], board)
    
    for i in range(len(template["next"])):
        template["next"][i] = gen_subgame(card_pair, template["next"][i], board)

    return template

def add_action_history(info_state, board, action_history):
    no_history = " -" not in info_state
    player_card_str = f"{info_state} -" if no_history else info_state[:info_state.index(" -") + 2]
    info_state_suffix = "" if no_history else f"- {info_state[(len(player_card_str) + 1):]}"
    history_str = ' '.join(action_history)

    return f"{player_card_str} {history_str} - {board} {info_state_suffix}".strip()

def adjust_payoffs_and_actions(template, pot, board, action_history):

    if is_util(template):

        if template["utility"][0] != template["utility"][1]:
            loss = int( (pot - constant_sum) / num_players )
            winner_idx = 0 if template["utility"][0] > template["utility"][1] else 1
            loser_idx = (winner_idx + 1) % 2

            template["utility"][loser_idx] -= loss
            template["utility"][winner_idx] += loss
    
    else:
        template["info_state"] = add_action_history(template["info_state"], board, action_history)
        for i in range(len(template["next"])):
            template["next"][i] = adjust_payoffs_and_actions(template["next"][i], pot, board, action_history)

    return template

def append_postflop_subgame(preflop, postflop, board, action_history=None):

    if action_history is None:
        action_history = list()

    if type(preflop) is int:
        postflop_subgame = adjust_payoffs_and_actions(deepcopy(postflop), preflop, board, action_history)
        return postflop_subgame
    
    if is_util(preflop):
        return preflop
    
    else:
        for i in range(len(preflop["next"])):
            action = preflop["action_names"][i]
            action_history.append(action)
            preflop["next"][i] = append_postflop_subgame(preflop["next"][i], postflop, board, action_history)
            action_history.pop()
    
    return preflop

def generate_game(*templates, cards=None):
    global game_cards

    if cards is None:
        cards = game_cards

    subgame_list = list()
    deals = permutations(cards, num_players)

    if len(templates) == 1:
        template = templates[0]
    
        for deal in deals:
            subgame_template = deepcopy(template)
            subgame = gen_subgame(deal, subgame_template)
            subgame_list.append(subgame)

    elif len(templates) == 2:
        preflop = templates[0]
        postflop = templates[1]

        for deal in deals:

            preflop_subgame = gen_subgame(deal, deepcopy(preflop))
            deck = deepcopy(cards)
            
            for deal_card in deal:
                deck.remove(deal_card)
            
            boards = permutations(deck, board_size)

            for board in boards:
                board_card = board[0]
                postflop_subgame = gen_subgame(deal, deepcopy(postflop), board_card)
                subgame = append_postflop_subgame(deepcopy(preflop_subgame), postflop_subgame, board_card)
                subgame_list.append(subgame)
        
    game = { "next": subgame_list }

    return game

def main():
    filename = argv[1]
    game = generate_game(*subgame_template)
    gamestr = dumps(game, indent=4)
    
    with open(filename, "w+") as f:
        f.write(gamestr)
    
    print("Done!")

if __name__ == "__main__":
    main()
