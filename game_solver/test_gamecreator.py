from unittest import main, TestCase

import gamecreator as GC

class TestHandEvaluation(TestCase):
    
    def test_card_name_converted_to_card_value(self):
        value1 = GC.valuate_card("5")
        value2 = GC.valuate_card("K")
        value3 = GC.valuate_card("TH")
        value4 = GC.valuate_card("TD")
        
        self.assertEqual(3, value1)
        self.assertEqual(11, value2)
        self.assertEqual(8, value3)
        self.assertEqual(value3, value4)
    
    def test_showdown_pays_off_correctly_on_winner(self):
        low = "J"
        high = "K"
        
        evaluation = GC.showdown(low, high, 4)
        
        self.assertEqual(GC.constant_sum - 4, evaluation)
        
        evaluation = GC.showdown(high, low, 4)
        
        self.assertEqual(4, evaluation)

    def test_showdown_can_split_on_tie(self):
        value1 = "JH"
        value2 = "JD"

        evaluation = GC.showdown(value1, value2, 4)
        split_pot = int(GC.constant_sum / 2)

        self.assertEqual(split_pot, evaluation)

class TestCardTemplating(TestCase):
    
    def test_check_is_utility_node(self):
        util_node = { 10 }
        payoff_node = [ 2, 0 ]
        decision_node = {
            "player": 0,
            "info_state": "",
            "action_names": [ ],
            "next": [ ]
        }
        
        self.assertTrue(GC.is_util(util_node))
        
        self.assertFalse(GC.is_util(payoff_node))
        self.assertFalse(GC.is_util(decision_node))
    
    def test_check_is_payoff(self):
        util_node = { 10 }
        payoff_node = [ 2, 0 ]
        decision_node = {
            "player": 0,
            "info_state": "",
            "action_names": [ ],
            "next": [ ]
        }
        
        self.assertTrue(GC.is_payoff(payoff_node))
        
        self.assertFalse(GC.is_payoff(util_node))
        self.assertFalse(GC.is_payoff(decision_node))
        
    def test_convert_util_to_payoffs(self):
        util_node = { 7 }
        cards = ( "K", "Q" )
        
        payoff = GC.get_payoff(util_node, cards)
        
        self.assertEqual([ 7, GC.constant_sum - 7 ], payoff)
        
        cards = ( "Q", "K" )
        
        payoff = GC.get_payoff(util_node, cards)
        
        self.assertEqual([ GC.constant_sum - 7, 7 ], payoff)
        
    def test_subgame_template_from_utility_node(self):
        card_pair = ( "J", "Q" )
        
        subgame_template = { 10 }
        
        actual_subgame = GC.gen_subgame(card_pair, subgame_template)
        
        expected_subgame = { "utility": [ GC.constant_sum - 10, 10 ] }
        
        self.assertEqual(expected_subgame, actual_subgame)
    
    def test_subgame_template_generation(self):        
        card_pair = ( "J", "Q" )
        subgame_template = {
            "player": 0,
            "info_state": "%s message",
            "action_names": [ "A" ],
            "next": [
                {
                    "player": 1,
                    "info_state": "another %s message",
                    "action_names": [ "B", "C" ],
                    "next": [
                        [ 0, 2 ],
                        { 5 }
                    ]
                }
            ]
        }
        
        actual_subgame = GC.gen_subgame(card_pair, subgame_template)
        
        expected_subgame = {
            "player": 0,
            "info_state": "J message",
            "action_names": [ "A" ],
            "next": [
                {
                    "player": 1,
                    "info_state": "another Q message",
                    "action_names": [ "B", "C" ],
                    "next": [
                        { "utility": [ 0, 2 ] },
                        { "utility": [ GC.constant_sum - 5, 5 ] }
                    ]
                }
            ]
        }
        
        self.assertEqual(expected_subgame, actual_subgame)
        
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
                        { 2 },
                        {
                            "player": 0,
                            "info_state": "%s - Check Bet",
                            "action_names": [ "Fold", "Call" ],
                            "next": [
                                [ 2, 0 ],
                                { 3 }
                            ]
                        }
                    ]
                },
                {
                    "player": 1,
                    "info_state": "%s - Bet",
                    "action_names": [ "Fold", "Call" ],
                    "next": [
                        [ 0, 2 ],
                        { 3 }
                    ]
                }
            ]
        }
        
        actual_subgame = GC.gen_subgame(card_pair, subgame_template)
        
        expected_subgame = {
            "player": 0,
            "info_state": "J",
            "action_names": [ "Check", "Bet" ],
            "next": [
                {
                    "player": 1,
                    "info_state": "Q - Check",
                    "action_names": [ "Check", "Bet" ],
                    "next": [
                        { "utility": [ 0, 2 ] },
                        {
                            "player": 0,
                            "info_state": "J - Check Bet",
                            "action_names": [ "Fold", "Call" ],
                            "next": [
                                { "utility": [ 2, 0 ] },
                                { "utility": [ GC.constant_sum - 3, 3 ] }
                            ]
                        }
                    ]
                },
                {
                    "player": 1,
                    "info_state": "Q - Bet",
                    "action_names": [ "Fold", "Call" ],
                    "next": [
                        { "utility": [ 0, 2 ] },
                        { "utility": [ GC.constant_sum - 3, 3 ] }
                    ]
                }
            ]
        }
        
        self.assertEqual(expected_subgame, actual_subgame)
        
    def test_full_game_generation(self):
        game_template = {
            "player": 0,
            "info_state": "%s",
            "action_names": [ "Check", "Bet" ],
            "next": [
                {
                    "player": 1,
                    "info_state": "%s - Check",
                    "action_names": [ "Check", "Bet" ],
                    "next": [
                        { 2 },
                        {
                            "player": 0,
                            "info_state": "%s - Check Bet",
                            "action_names": [ "Fold", "Call" ],
                            "next": [
                                [ 2, 0 ],
                                { 3 }
                            ]
                        }
                    ]
                },
                {
                    "player": 1,
                    "info_state": "%s - Bet",
                    "action_names": [ "Fold", "Call" ],
                    "next": [
                        [ 0, 2 ],
                        { 3 }
                    ]
                }
            ]
        }
        
        game = GC.generate_game(game_template, cards=[ "J", "Q", "K" ])
        
        expected_subgame = {
            "player": 0,
            "info_state": "J",
            "action_names": [ "Check", "Bet" ],
            "next": [
                {
                    "player": 1,
                    "info_state": "K - Check",
                    "action_names": [ "Check", "Bet" ],
                    "next": [
                        { "utility": [ 0, 2 ] },
                        {
                            "player": 0,
                            "info_state": "J - Check Bet",
                            "action_names": [ "Fold", "Call" ],
                            "next": [
                                { "utility": [ 2, 0 ] },
                                { "utility": [ -1, 3 ] }
                            ]
                        }
                    ]
                },
                {
                    "player": 1,
                    "info_state": "K - Bet",
                    "action_names": [ "Fold", "Call" ],
                    "next": [
                        { "utility": [ 0, 2 ] },
                        { "utility": [ -1, 3 ] }
                    ]
                }
            ]
        }
        
        self.assertTrue(expected_subgame in game["next"])

if __name__ == "__main__":
    main()
