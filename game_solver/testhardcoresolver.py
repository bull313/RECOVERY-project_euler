from unittest import main as test
from unittest import TestCase

from hardcoresolver import *
from hardcoresolver_ctest import *

class Test(TestCase):

    def test_c_level_testing(self):
        c_test(self)

    def test_parse_json_file(self):
        game_data = load_game("/Users/bladick/Desktop/project_euler/" \
            + "cards/lab/poker_solver/test/test_game.json")
        
        expected_data = {
            "next": [
                {
                    "player": 0,
                    "info_state": "test 1",
                    "action_names": [ "one", "two" ],
                    "next": [
                        {
                            "utility": [ -1.0, 1.0 ]
                        },
                        {
            
                            "player": 1,
                            "info_state": "test 2",
                            "action_names": [ "three", "four" ],
                            "next": [
                                {
                                    "utility": [ 1.0, -1.0 ]
                                },
                                {
                    
                                    "utility": [ -2.0, 2.0 ]
                                }
                            ]
                        }
                    ]
                }
            ]
        }

        self.assertEqual(game_data, expected_data)

    def test_game_can_be_loaded_and_unloaded(self):
        game_data = load_game("/Users/bladick/Desktop/project_euler/" \
            + "cards/lab/poker_solver/test/test_game.json")

        solution = solve_game(game_data)

        store_game(solution)
        
if __name__ == "__main__":
    test()
