"""
Unit tests for Loot Generator module
"""

import unittest
from aidm.loot_generator import LootGenerator, LootItem


class TestLootGenerator(unittest.TestCase):
    
    def setUp(self):
        self.generator = LootGenerator()
    
    def test_generate_loot_table(self):
        """Test basic loot table generation"""
        loot_table = self.generator.generate_loot_table("desert", "republic", "mid", 50)
        
        self.assertEqual(len(loot_table), 50)
        
        # Check that all items are LootItem instances
        for item in loot_table:
            self.assertIsInstance(item, LootItem)
            self.assertIsInstance(item.name, str)
            self.assertIn(item.rarity, ["common", "uncommon", "rare", "epic", "legendary"])
            self.assertIsInstance(item.bonuses, dict)
            self.assertGreater(item.value_credits, 0)
    
    def test_rarity_distribution(self):
        """Test that rarity distribution is reasonable"""
        loot_table = self.generator.generate_loot_table("urban", "jedi", "early", 100)
        
        rarity_counts = {}
        for item in loot_table:
            rarity_counts[item.rarity] = rarity_counts.get(item.rarity, 0) + 1
        
        # Common should be most frequent
        self.assertGreater(rarity_counts.get("common", 0), rarity_counts.get("legendary", 0))
        
        # Should have some variety
        self.assertGreaterEqual(len(rarity_counts), 3)
    
    def test_item_type_variety(self):
        """Test that different item types are generated"""
        loot_table = self.generator.generate_loot_table("forest", "sith", "late", 100)
        
        item_types = set(item.item_type for item in loot_table)
        
        # Should have multiple item types
        self.assertGreaterEqual(len(item_types), 3)
        
        # Should include basic types
        expected_types = {"weapon", "armor", "mod", "stim", "credits", "misc"}
        self.assertTrue(item_types.issubset(expected_types))
    
    def test_level_scaling(self):
        """Test that items scale properly with difficulty tier"""
        early_loot = self.generator.generate_loot_table("desert", "republic", "early", 20)
        late_loot = self.generator.generate_loot_table("desert", "republic", "late", 20)
        
        # Late game items should generally have higher level requirements
        early_levels = [item.level_requirement for item in early_loot]
        late_levels = [item.level_requirement for item in late_loot]
        
        avg_early_level = sum(early_levels) / len(early_levels)
        avg_late_level = sum(late_levels) / len(late_levels)
        
        self.assertGreater(avg_late_level, avg_early_level)
    
    def test_weapon_generation(self):
        """Test weapon-specific generation"""
        weapon = self.generator._generate_weapon("jedi", 10, 15)
        
        self.assertEqual(weapon.item_type, "weapon")
        self.assertIsInstance(weapon.bonuses, dict)
        self.assertGreaterEqual(weapon.level_requirement, 10)
        self.assertLessEqual(weapon.level_requirement, 15)
        self.assertIn("attack", weapon.bonuses)
    
    def test_armor_generation(self):
        """Test armor-specific generation"""
        armor = self.generator._generate_armor("republic", 5, 10)
        
        self.assertEqual(armor.item_type, "armor")
        self.assertIsInstance(armor.bonuses, dict)
        self.assertGreaterEqual(armor.level_requirement, 5)
        self.assertLessEqual(armor.level_requirement, 10)
        self.assertIn("defense", armor.bonuses)
    
    def test_credits_generation(self):
        """Test credits generation"""
        credits = self.generator._generate_credits(1, 30)
        
        self.assertEqual(credits.item_type, "credits")
        self.assertEqual(credits.rarity, "common")
        self.assertIn("credits", credits.bonuses)
        self.assertGreater(credits.bonuses["credits"], 0)
    
    def test_item_serialization(self):
        """Test LootItem serialization"""
        item = LootItem(
            name="Test Sword",
            rarity="rare",
            item_type="weapon",
            bonuses={"attack": 5},
            flavor_text="A test weapon",
            value_credits=1000,
            level_requirement=10,
            faction_theme="jedi"
        )
        
        item_dict = item.to_dict()
        
        self.assertEqual(item_dict["name"], "Test Sword")
        self.assertEqual(item_dict["rarity"], "rare")
        self.assertEqual(item_dict["bonuses"]["attack"], 5)
        
        # Test deserialization
        restored_item = LootItem.from_dict(item_dict)
        self.assertEqual(restored_item.name, item.name)
        self.assertEqual(restored_item.bonuses, item.bonuses)


if __name__ == '__main__':
    unittest.main()
