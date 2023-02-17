from copy import deepcopy
from json import dumps as json_str
from itertools import permutations

CARD_NAMES     = [ str(i) for i in range(2, 10) ] + [ "T", "J", "Q", "K", "A" ]
CHAR_VALUE_MAP = { CARD_NAMES[i]: i for i in range(len(CARD_NAMES)) }

def output_game(game):
    game_str = json_str(game, indent=4)

    with open("advanced_leduc_holdem.json", "w+") as game_file:
        game_file.write(game_str)

def showdown(p1card, p2card, board):
    p1 = CHAR_VALUE_MAP[p1card[0]]
    p2 = CHAR_VALUE_MAP[p2card[0]]
    b = CHAR_VALUE_MAP[ board[0] ]

    coeff = 1.0

    if p1 == p2:
        coeff = 0.0
    elif p2 == b or (p2 > p1 and p1 != b):
        coeff = -1.0

    return coeff

def gen_game():
    # cards = [
    #     "AD", "AH", "AS", "KD", "KH", "KS", "QD", "QH", "QS",
    #     "JD", "JH", "JS", "TD", "TH", "TS", "9D", "9H", "9S",
    #     "8D", "8H", "8S", "7D", "7H", "7S", "6D", "6H", "6S",
    #     "5D", "5H", "5S", "4D", "4H", "4S", "3D", "3H", "3S",
    #     "2D", "2H", "2S"
    # ]

    cards = [
        "AD", "AH", "AS", "AC", "KD", "KH", "KS", "KC", "QD", "QH", "QS", "QC",
        "JD", "JH", "JS", "JC", "TD", "TH", "TS", "TC", "9C", "9D", "9H", "9S"
    ]

    game = {
        "max_num_actions": 3,
        "max_regret": 6,
        "next": list(),
    }

    deal = permutations(cards, 2)

    for matchup in deal:
        p1_card, p2_card = matchup
        p1v = CHAR_VALUE_MAP[p1_card[0]]
        p2v = CHAR_VALUE_MAP[p2_card[0]]

        preflop = {
            "player": 0,
            "info_state": f"{p1_card}",
            "action_names": [ "check", "bet" ],
            "next": [
                {
                    "player": 1,
                    "info_state": f"{p2_card} check",
                    "action_names": [ "check", "bet" ],
                    "next": [
                        None, # Flop
                        {
                            "player": 0,
                            "info_state": f"{p1_card} check bet",
                            "action_names": [ "fold", "call", "raise" ],
                            "next": [
                                {
                                    "utility": [ -1.0, 1.0 ]
                                },
                                None, # Flop
                                {
                                    "player": 1,
                                    "info_state": f"{p2_card} check bet raise",
                                    "action_names": [ "fold", "call" ],
                                    "next": [
                                        {
                                            "utility": [ 2.0, -2.0 ]
                                        },
                                        None # Flop
                                    ]
                                }
                            ]
                        }
                    ]
                },
                {
                    "player": 1,
                    "info_state": f"{p2_card} bet",
                    "action_names": [ "fold", "call", "raise" ],
                    "next": [
                        {
                            "utility": [ 1.0, -1.0 ]
                        },
                        None, # Flop
                        {
                            "player": 0,
                            "info_state": f"{p1_card} bet raise",
                            "action_names": [ "fold", "call" ],
                            "next": [
                                {
                                    "utility": [ -2.0, 2.0 ]
                                },
                                None # Flop
                            ]
                        }
                    ]
                }
            ]
        }

        deck = deepcopy(cards)
        deck.remove(p1_card)
        deck.remove(p2_card)

        preflop_history = {
            "check check": (preflop["next"][0]["next"], 0, 2),
            "check bet call": (preflop["next"][0]["next"][1]["next"], 1, 4),
            "bet call": (preflop["next"][1]["next"], 1, 4),
            "check bet raise call": (preflop["next"][0]["next"][1]["next"][2]["next"], 1, 6),
            "bet raise call": (preflop["next"][1]["next"][2]["next"], 1, 6)
        }

        for history in preflop_history.keys():
            subgame, action, pot = preflop_history[history]
            flop_nature = {
                "next": list()
            }

            for board in deck:
                postflop = {
                    "player": 1,
                    "info_state": f"{p2_card} {history} {board}",
                    "action_names": [ "check", "bet" ],
                    "next": [
                        {
                            "player": 0,
                            "info_state": f"{p1_card} {history} {board} check",
                            "action_names": [ "check", "bet" ],
                            "next": [
                                {
                                    "utility": [ (pot/2) * showdown(p1_card, p2_card, board), -(pot/2) * showdown(p1_card, p2_card, board) ]
                                },
                                {
                                    "player": 1,
                                    "info_state": f"{p2_card} {history} {board} check bet",
                                    "action_names": [ "fold", "call", "raise" ],
                                    "next": [
                                        {
                                            "utility": [ (pot/2), -(pot/2) ]
                                        },
                                        {
                                            "utility": [ (pot/2 + 1) * showdown(p1_card, p2_card, board), -(pot/2 + 1) * showdown(p1_card, p2_card, board) ]
                                        },
                                        {
                                            "player": 0,
                                            "info_state": f"{p1_card} {history} {board} check bet raise",
                                            "action_names": [ "fold", "call" ],
                                            "next": [
                                                {
                                                    "utility": [ -(pot/2 + 1), (pot/2 + 1) ]
                                                },
                                                {
                                                     "utility": [ (pot/2 + 2) * showdown(p1_card, p2_card, board), -(pot/2 + 2) * showdown(p1_card, p2_card, board) ]
                                                }
                                            ]
                                        }
                                    ]
                                }
                            ]
                        },
                        {
                            "player": 0,
                            "info_state": f"{p1_card} {history} {board} bet",
                            "action_names": [ "fold", "call", "raise" ],
                            "next": [
                                {
                                    "utility": [ -(pot/2), (pot/2) ]
                                },
                                {
                                    "utility": [ (pot/2 + 1) * showdown(p1_card, p2_card, board), -(pot/2 + 1) * showdown(p1_card, p2_card, board) ]
                                },
                                {
                                    "player": 1,
                                    "info_state": f"{p2_card} {history} {board} bet raise",
                                    "action_names": [ "fold", "call" ],
                                    "next": [
                                        {
                                            "utility": [ (pot/2 + 1), -(pot/2 + 1) ]
                                        },
                                        {
                                            "utility": [ (pot/2 + 2) * showdown(p1_card, p2_card, board), -(pot/2 + 2) * showdown(p1_card, p2_card, board) ]
                                        }
                                    ]
                                }
                            ]
                        }
                    ]
                }

                flop_nature["next"].append(postflop)
            
            subgame[action] = flop_nature
        
        game["next"].append(preflop)
    
    return game

def main():
    game = gen_game()
    output_game(game)
    print("advanced_leduc_holdem.json was generated!")

if __name__ == "__main__":
    main()
