"""
Unit tests for Campaign Generator module
"""

import unittest
from aidm.story_seed_interpreter import CampaignConfig
from aidm.campaign_generator import CampaignGenerator


class TestCampaignGenerator(unittest.TestCase):
    
    def setUp(self):
        self.generator = CampaignGenerator()
        self.test_config = CampaignConfig(
            game_length_hours=20,
            estimated_planets=6,
            story_seed="Test Jedi adventure",
            time_period="old republic",
            alignment_focus="light"
        )
    
    def test_generate_campaign(self):
        """Test basic campaign generation"""
        campaign = self.generator.generate_campaign(self.test_config)
        
        self.assertEqual(len(campaign.planets), 6)
        self.assertIsNotNone(campaign.final_boss)
        self.assertIsNotNone(campaign.main_quest_outline)
        self.assertIsNotNone(campaign.campaign_summary)
        self.assertEqual(campaign.config, self.test_config)
    
    def test_planet_generation_progression(self):
        """Test that planets have proper difficulty progression"""
        campaign = self.generator.generate_campaign(self.test_config)
        
        # Check that we have early, mid, and late game planets
        difficulty_tiers = [planet.difficulty_tier for planet in campaign.planets]
        
        self.assertIn("early", difficulty_tiers)
        self.assertIn("mid", difficulty_tiers)
        self.assertIn("late", difficulty_tiers)
        
        # First planet should be early
        self.assertEqual(campaign.planets[0].difficulty_tier, "early")
        
        # Last planet should be late
        self.assertEqual(campaign.planets[-1].difficulty_tier, "late")
    
    def test_main_quest_outline_generation(self):
        """Test main quest outline generation for different alignments"""
        alignments = ["light", "dark", "neutral"]
        
        for alignment in alignments:
            with self.subTest(alignment=alignment):
                config = CampaignConfig(
                    game_length_hours=15,
                    estimated_planets=5,
                    story_seed=f"Test {alignment} adventure",
                    time_period="old republic",
                    alignment_focus=alignment
                )
                
                campaign = self.generator.generate_campaign(config)
                outline = campaign.main_quest_outline
                
                self.assertIsInstance(outline, str)
                self.assertGreater(len(outline), 100)  # Should be substantial
                self.assertIn("ACT I", outline)
                self.assertIn("ACT II", outline)
                self.assertIn("ACT III", outline)
    
    def test_campaign_serialization(self):
        """Test campaign plan serialization"""
        campaign = self.generator.generate_campaign(self.test_config)
        
        # Test to_dict conversion
        campaign_dict = campaign.to_dict()
        
        self.assertIn("config", campaign_dict)
        self.assertIn("planets", campaign_dict)
        self.assertIn("final_boss", campaign_dict)
        self.assertIn("main_quest_outline", campaign_dict)
        self.assertIn("campaign_summary", campaign_dict)
        
        # Check that planets are properly serialized
        self.assertEqual(len(campaign_dict["planets"]), 6)
        self.assertIsInstance(campaign_dict["planets"][0], dict)


if __name__ == '__main__':
    unittest.main()
