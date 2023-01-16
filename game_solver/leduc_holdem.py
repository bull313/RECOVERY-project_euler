from copy import deepcopy
from json import dumps as json_str
from itertools import permutations

def output_game(game):
    game_str = json_str(game, indent=4)

    with open("leduc_holdem.json", "w+") as game_file:
        game_file.write(game_str)

def gen_game():
    cards = {
        "Jack of Spades": 0,
        "Jack of Hearts": 0,
        "Queen of Spades": 1,
        "Queen of Hearts": 1,
        "King of Spades": 2,
        "King of Hearts": 2
    }

    game = {
        "max_num_actions": 2,
        "max_regret": 4,
        "next": list(),
    }

    names = list(cards.keys())
    deal = permutations(names, 2)

    for matchup in deal:
        p1_card, p2_card = matchup
        p1v = cards[p1_card]
        p2v = cards[p2_card]
        showdown = lambda p1, p2, b: 0.0 if (p1 == p2) else \
            1.0 if ((p1 > p2 and p2 != b) or (p1 == b)) else -1.0

        preflop = {
            "player": 0,
            "info_state": f"Wake up with a {p1_card}",
            "action_names": [ "check", "bet" ],
            "next": [
                {
                    "player": 1,
                    "info_state": f"Wake up with a {p2_card} facing a check",
                    "action_names": [ "check", "bet" ],
                    "next": [
                        None, # Flop
                        {
                            "player": 0,
                            "info_state": f"Holding a {p1_card} facing a bet against a check",
                            "action_names": [ "fold", "call" ],
                            "next": [
                                {
                                    "utility": [ -1.0, 1.0 ]
                                },
                                None # Flop
                            ]
                        }
                    ]
                },
                {
                    "player": 1,
                    "info_state": f"Wake up with a {p2_card} facing a check",
                    "action_names": [ "fold", "call" ],
                    "next": [
                        {
                            "utility": [ 1.0, -1.0 ]
                        },
                        None # Flop
                    ]
                }
            ]
        }

        deck = deepcopy(names)
        deck.remove(p1_card)
        deck.remove(p2_card)

        preflop_history = {
            "check, check": (preflop["next"][0]["next"], 0, 2),
            "check bet call": (preflop["next"][0]["next"][1]["next"], 1, 4),
            "bet call": (preflop["next"][1]["next"], 1, 4),
        }

        for history in preflop_history.keys():
            subgame, action, pot = preflop_history[history]
            risk_return = pot / 2
            flop_nature = {
                "next": list()
            }

            for board in deck:
                bv = cards[board]

                postflop = {
                    "player": 0,
                    "info_state": f"Holding a {p1_card}; Action was [{history}] preflop; Flop is a {board}",
                    "action_names": [ "check", "bet" ],
                    "next": [
                        {
                            "player": 1,
                            "info_state": f"Holding a {p2_card}; Action was [{history}] preflop; Flop is a {board}; Facing a check",
                            "action_names": [ "check", "bet" ],
                            "next": [
                                {
                                    "utility": [ risk_return * showdown(p1v, p2v, bv), -risk_return * showdown(p1v, p2v, bv) ]
                                },
                                {
                                    "player": 0,
                                    "info_state": f"Holding a {p1_card}; Action was [{history}] preflop; Flop is a {board}; Facing a bet against a check",
                                    "action_names": [ "fold", "call" ],
                                    "next": [
                                        {
                                            "utility": [ -1.0, 1.0 ]
                                        },
                                        {
                                            "utility": [ (risk_return + 1) * showdown(p1v, p2v, bv), -(risk_return + 1) * showdown(p1v, p2v, bv) ]
                                        }
                                    ]
                                }
                            ]
                        },
                        {
                            "player": 1,
                            "info_state": f"Holding a {p2_card}; Action was [{history}] preflop; Flop is a {board}; Facing a bet",
                            "action_names": [ "fold", "call" ],
                            "next": [
                                {
                                    "utility": [ 1.0, -1.0 ]
                                },
                                {
                                    "utility": [ (risk_return + 1) * showdown(p1v, p2v, bv), -(risk_return + 1) * showdown(p1v, p2v, bv) ]
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
    print("leduc_holdem.json was generated!")

if __name__ == "__main__":
    main()
