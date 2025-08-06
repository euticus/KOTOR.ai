"""
Unit tests for Story Seed Interpreter module
"""

import unittest
from aidm.story_seed_interpreter import StorySeedInterpreter, CampaignConfig


class TestStorySeedInterpreter(unittest.TestCase):
    
    def setUp(self):
        self.interpreter = StorySeedInterpreter()
    
    def test_parse_basic_story_seed(self):
        """Test parsing a basic story seed"""
        seed = "I want a 20-hour story about a Jedi exile hunting a Sith relic"
        config = self.interpreter.parse_story_seed(seed)
        
        self.assertEqual(config.game_length_hours, 20)
        self.assertEqual(config.estimated_planets, 6)  # 20 // 3 = 6
        self.assertEqual(config.time_period, "old republic")
        self.assertEqual(config.alignment_focus, "light")
    
    def test_extract_game_length(self):
        """Test game length extraction from various formats"""
        test_cases = [
            ("30 hour adventure", 30),
            ("15-hour campaign", 15),
            ("25 hours of gameplay", 25),
            ("10h story", 10),
            ("no time specified", 20)  # default
        ]
        
        for seed, expected_hours in test_cases:
            with self.subTest(seed=seed):
                result = self.interpreter._extract_game_length(seed)
                self.assertEqual(result, expected_hours)
    
    def test_determine_time_period(self):
        """Test time period detection"""
        test_cases = [
            ("kotor style adventure", "old republic"),
            ("clone wars era story", "clone wars"),
            ("empire vs rebels", "galactic civil war"),
            ("first order threat", "new republic"),
            ("generic star wars", "old republic")  # default
        ]
        
        for seed, expected_period in test_cases:
            with self.subTest(seed=seed):
                result = self.interpreter._determine_time_period(seed)
                self.assertEqual(result, expected_period)
    
    def test_determine_alignment_focus(self):
        """Test alignment focus detection"""
        test_cases = [
            ("jedi hero saves the galaxy", "light"),
            ("sith lord seeks power", "dark"),
            ("smuggler makes deals", "neutral"),
            ("dark jedi vs sith", "dark"),  # dark keywords win
            ("republic vs empire", "light")  # light keywords win
        ]
        
        for seed, expected_alignment in test_cases:
            with self.subTest(seed=seed):
                result = self.interpreter._determine_alignment_focus(seed)
                self.assertEqual(result, expected_alignment)
    
    def test_campaign_config_serialization(self):
        """Test CampaignConfig serialization"""
        config = CampaignConfig(
            game_length_hours=25,
            estimated_planets=8,
            story_seed="test seed",
            time_period="old republic",
            alignment_focus="neutral"
        )
        
        config_dict = config.to_dict()
        
        self.assertEqual(config_dict["game_length_hours"], 25)
        self.assertEqual(config_dict["estimated_planets"], 8)
        self.assertEqual(config_dict["story_seed"], "test seed")
        self.assertEqual(config_dict["time_period"], "old republic")
        self.assertEqual(config_dict["alignment_focus"], "neutral")


if __name__ == '__main__':
    unittest.main()
