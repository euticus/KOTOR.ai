"""
Loot Generator Module

Generates 1,000+ scalable loot items using D&D + Star Wars logic
with exponential gear scaling and thematic matching.
"""

from dataclasses import dataclass
from typing import List, Dict, Optional
import random
import json
from .story_seed_interpreter import CampaignConfig


@dataclass
class LootItem:
    """Represents a loot item with stats and flavor"""
    name: str
    rarity: str  # "common", "uncommon", "rare", "epic", "legendary"
    item_type: str  # "weapon", "armor", "mod", "stim", "misc", "credits"
    bonuses: Dict[str, int]  # stat bonuses like {"attack": 2, "defense": 1}
    flavor_text: str
    value_credits: int
    level_requirement: int
    faction_theme: Optional[str]  # "jedi", "sith", "republic", "imperial", etc.
    
    def to_dict(self) -> dict:
        return {
            "name": self.name,
            "rarity": self.rarity,
            "item_type": self.item_type,
            "bonuses": self.bonuses,
            "flavor_text": self.flavor_text,
            "value_credits": self.value_credits,
            "level_requirement": self.level_requirement,
            "faction_theme": self.faction_theme
        }
    
    @classmethod
    def from_dict(cls, data: dict) -> 'LootItem':
        return cls(**data)


class LootGenerator:
    """Generates scalable loot items for the campaign"""
    
    def __init__(self):
        # Rarity weights and multipliers
        self.rarity_weights = {
            "common": 0.50,
            "uncommon": 0.30,
            "rare": 0.15,
            "epic": 0.04,
            "legendary": 0.01
        }
        
        self.rarity_multipliers = {
            "common": 1.0,
            "uncommon": 1.5,
            "rare": 2.5,
            "epic": 4.0,
            "legendary": 7.0
        }
        
        # Base item templates
        self.weapon_templates = {
            "lightsaber": {
                "base_stats": {"attack": 8, "force_power": 2},
                "prefixes": ["Ancient", "Elegant", "Corrupted", "Master's", "Apprentice"],
                "suffixes": ["of Power", "of Balance", "of Fury", "of Wisdom", "of Darkness"]
            },
            "blaster": {
                "base_stats": {"attack": 5, "accuracy": 1},
                "prefixes": ["Heavy", "Precision", "Rapid-Fire", "Military", "Custom"],
                "suffixes": ["Rifle", "Pistol", "Carbine", "Cannon", "Repeater"]
            },
            "vibroblade": {
                "base_stats": {"attack": 6, "critical": 1},
                "prefixes": ["Vibrating", "Cortosis", "Durasteel", "Mandalorian", "Echani"],
                "suffixes": ["Sword", "Dagger", "Blade", "Knife", "Rapier"]
            }
        }
        
        self.armor_templates = {
            "robes": {
                "base_stats": {"defense": 3, "force_power": 3},
                "prefixes": ["Jedi", "Sith", "Master", "Padawan", "Dark"],
                "suffixes": ["Robes", "Vestments", "Garments", "Attire", "Mantle"]
            },
            "armor": {
                "base_stats": {"defense": 6, "health": 10},
                "prefixes": ["Combat", "Battle", "Assault", "Heavy", "Reinforced"],
                "suffixes": ["Armor", "Suit", "Plating", "Vest", "Gear"]
            },
            "clothing": {
                "base_stats": {"defense": 1, "charisma": 2},
                "prefixes": ["Formal", "Casual", "Diplomatic", "Noble", "Merchant"],
                "suffixes": ["Outfit", "Attire", "Garb", "Clothing", "Wear"]
            }
        }
        
        self.mod_templates = {
            "weapon_mod": {
                "base_stats": {"attack": 2},
                "prefixes": ["Targeting", "Power", "Precision", "Rapid", "Devastating"],
                "suffixes": ["Scope", "Cell", "Barrel", "Trigger", "Amplifier"]
            },
            "armor_mod": {
                "base_stats": {"defense": 2},
                "prefixes": ["Reinforced", "Energy", "Kinetic", "Adaptive", "Reactive"],
                "suffixes": ["Plating", "Shield", "Mesh", "Layer", "Coating"]
            },
            "utility_mod": {
                "base_stats": {"utility": 1},
                "prefixes": ["Advanced", "Military", "Stealth", "Medical", "Technical"],
                "suffixes": ["Interface", "Scanner", "Generator", "Processor", "Module"]
            }
        }
        
        # Faction themes
        self.faction_themes = {
            "jedi": {
                "colors": ["blue", "green", "silver", "white"],
                "materials": ["crystal", "metal", "energy"],
                "descriptors": ["peaceful", "balanced", "harmonious", "pure"]
            },
            "sith": {
                "colors": ["red", "black", "crimson", "dark"],
                "materials": ["obsidian", "dark metal", "corrupted crystal"],
                "descriptors": ["corrupted", "powerful", "menacing", "dark"]
            },
            "republic": {
                "colors": ["blue", "gold", "white", "silver"],
                "materials": ["durasteel", "plasteel", "composite"],
                "descriptors": ["military", "standard", "regulation", "official"]
            },
            "imperial": {
                "colors": ["gray", "black", "white", "red"],
                "materials": ["imperial steel", "plasteel", "composite"],
                "descriptors": ["imperial", "authoritarian", "efficient", "intimidating"]
            }
        }
    
    def generate_loot_table(self, planet_biome: str, faction: str, 
                          difficulty_tier: str, item_count: int = 100) -> List[LootItem]:
        """
        Generate a loot table for a specific context
        
        Args:
            planet_biome: The biome of the planet
            faction: Dominant faction on the planet
            difficulty_tier: "early", "mid", or "late" game
            item_count: Number of items to generate
            
        Returns:
            List of generated loot items
        """
        
        loot_table = []
        
        # Determine level range based on difficulty tier
        level_ranges = {
            "early": (1, 10),
            "mid": (8, 20),
            "late": (15, 30)
        }
        min_level, max_level = level_ranges[difficulty_tier]
        
        for _ in range(item_count):
            # Select item type
            item_type = self._select_item_type()
            
            # Generate item based on type
            if item_type == "weapon":
                item = self._generate_weapon(faction, min_level, max_level)
            elif item_type == "armor":
                item = self._generate_armor(faction, min_level, max_level)
            elif item_type == "mod":
                item = self._generate_mod(faction, min_level, max_level)
            elif item_type == "stim":
                item = self._generate_stim(min_level, max_level)
            elif item_type == "credits":
                item = self._generate_credits(min_level, max_level)
            else:  # misc
                item = self._generate_misc_item(planet_biome, faction, min_level, max_level)
            
            loot_table.append(item)
        
        return loot_table
    
    def _select_item_type(self) -> str:
        """Select item type based on weights"""
        type_weights = {
            "weapon": 0.25,
            "armor": 0.20,
            "mod": 0.15,
            "stim": 0.15,
            "misc": 0.15,
            "credits": 0.10
        }
        
        rand = random.random()
        cumulative = 0
        
        for item_type, weight in type_weights.items():
            cumulative += weight
            if rand <= cumulative:
                return item_type
        
        return "misc"  # fallback
    
    def _select_rarity(self, level: int) -> str:
        """Select rarity with higher level items having better rarity chances"""
        
        # Adjust weights based on level
        level_factor = min(level / 30.0, 1.0)  # 0.0 to 1.0
        
        adjusted_weights = {
            "common": max(0.1, 0.50 - level_factor * 0.3),
            "uncommon": 0.30,
            "rare": min(0.35, 0.15 + level_factor * 0.2),
            "epic": min(0.15, 0.04 + level_factor * 0.11),
            "legendary": min(0.10, 0.01 + level_factor * 0.09)
        }
        
        rand = random.random()
        cumulative = 0
        
        for rarity, weight in adjusted_weights.items():
            cumulative += weight
            if rand <= cumulative:
                return rarity
        
        return "common"  # fallback
    
    def _generate_weapon(self, faction: str, min_level: int, max_level: int) -> LootItem:
        """Generate a weapon item"""
        
        level = random.randint(min_level, max_level)
        rarity = self._select_rarity(level)
        
        # Select weapon type
        weapon_type = random.choice(list(self.weapon_templates.keys()))
        template = self.weapon_templates[weapon_type]
        
        # Generate name
        prefix = random.choice(template["prefixes"])
        suffix = random.choice(template["suffixes"])
        name = f"{prefix} {suffix}"
        
        # Scale stats based on level and rarity
        bonuses = {}
        multiplier = self.rarity_multipliers[rarity] * (1 + level * 0.1)
        
        for stat, base_value in template["base_stats"].items():
            bonuses[stat] = int(base_value * multiplier)
        
        # Add random bonus stats for higher rarities
        if rarity in ["rare", "epic", "legendary"]:
            bonus_stats = ["critical", "accuracy", "damage", "force_power"]
            extra_stat = random.choice(bonus_stats)
            if extra_stat not in bonuses:
                bonuses[extra_stat] = random.randint(1, 3)
        
        # Generate flavor text
        faction_theme = self._get_faction_theme(faction)
        flavor_text = self._generate_weapon_flavor(name, weapon_type, faction_theme, rarity)
        
        # Calculate value
        base_value = 100 * level
        value = int(base_value * self.rarity_multipliers[rarity])
        
        return LootItem(
            name=name,
            rarity=rarity,
            item_type="weapon",
            bonuses=bonuses,
            flavor_text=flavor_text,
            value_credits=value,
            level_requirement=level,
            faction_theme=faction
        )
    
    def _generate_armor(self, faction: str, min_level: int, max_level: int) -> LootItem:
        """Generate an armor item"""
        
        level = random.randint(min_level, max_level)
        rarity = self._select_rarity(level)
        
        # Select armor type
        armor_type = random.choice(list(self.armor_templates.keys()))
        template = self.armor_templates[armor_type]
        
        # Generate name
        prefix = random.choice(template["prefixes"])
        suffix = random.choice(template["suffixes"])
        name = f"{prefix} {suffix}"
        
        # Scale stats
        bonuses = {}
        multiplier = self.rarity_multipliers[rarity] * (1 + level * 0.08)
        
        for stat, base_value in template["base_stats"].items():
            bonuses[stat] = int(base_value * multiplier)
        
        # Add resistance bonuses for higher rarities
        if rarity in ["epic", "legendary"]:
            resistances = ["energy_resist", "kinetic_resist", "force_resist"]
            resistance = random.choice(resistances)
            bonuses[resistance] = random.randint(5, 15)
        
        # Generate flavor text
        faction_theme = self._get_faction_theme(faction)
        flavor_text = self._generate_armor_flavor(name, armor_type, faction_theme, rarity)
        
        # Calculate value
        base_value = 80 * level
        value = int(base_value * self.rarity_multipliers[rarity])
        
        return LootItem(
            name=name,
            rarity=rarity,
            item_type="armor",
            bonuses=bonuses,
            flavor_text=flavor_text,
            value_credits=value,
            level_requirement=level,
            faction_theme=faction
        )
    
    def _generate_mod(self, faction: str, min_level: int, max_level: int) -> LootItem:
        """Generate a modification item"""
        
        level = random.randint(min_level, max_level)
        rarity = self._select_rarity(level)
        
        # Select mod type
        mod_type = random.choice(list(self.mod_templates.keys()))
        template = self.mod_templates[mod_type]
        
        # Generate name
        prefix = random.choice(template["prefixes"])
        suffix = random.choice(template["suffixes"])
        name = f"{prefix} {suffix}"
        
        # Scale stats
        bonuses = {}
        multiplier = self.rarity_multipliers[rarity] * (1 + level * 0.05)
        
        for stat, base_value in template["base_stats"].items():
            bonuses[stat] = int(base_value * multiplier)
        
        flavor_text = f"A {rarity} modification that enhances equipment performance."
        
        # Calculate value
        base_value = 50 * level
        value = int(base_value * self.rarity_multipliers[rarity])
        
        return LootItem(
            name=name,
            rarity=rarity,
            item_type="mod",
            bonuses=bonuses,
            flavor_text=flavor_text,
            value_credits=value,
            level_requirement=level,
            faction_theme=faction
        )
    
    def _generate_stim(self, min_level: int, max_level: int) -> LootItem:
        """Generate a stimulant/consumable item"""
        
        level = random.randint(min_level, max_level)
        rarity = self._select_rarity(level)
        
        stim_types = [
            ("Medpac", {"health": 25}),
            ("Adrenal Strength", {"attack": 5}),
            ("Adrenal Stamina", {"defense": 5}),
            ("Force Stim", {"force_power": 10}),
            ("Battle Stim", {"critical": 3})
        ]
        
        stim_name, base_bonuses = random.choice(stim_types)
        
        # Scale bonuses
        bonuses = {}
        multiplier = self.rarity_multipliers[rarity] * (1 + level * 0.1)
        
        for stat, base_value in base_bonuses.items():
            bonuses[stat] = int(base_value * multiplier)
        
        name = f"{rarity.title()} {stim_name}"
        flavor_text = f"A {rarity} stimulant that provides temporary enhancement."
        
        # Calculate value
        base_value = 20 * level
        value = int(base_value * self.rarity_multipliers[rarity])
        
        return LootItem(
            name=name,
            rarity=rarity,
            item_type="stim",
            bonuses=bonuses,
            flavor_text=flavor_text,
            value_credits=value,
            level_requirement=level,
            faction_theme=None
        )
    
    def _generate_credits(self, min_level: int, max_level: int) -> LootItem:
        """Generate a credits reward"""
        
        level = random.randint(min_level, max_level)
        
        # Credits scale exponentially with level
        base_amount = 50 + (level * 25)
        variance = random.uniform(0.5, 2.0)
        amount = int(base_amount * variance)
        
        return LootItem(
            name=f"{amount} Credits",
            rarity="common",
            item_type="credits",
            bonuses={"credits": amount},
            flavor_text="Standard galactic currency.",
            value_credits=amount,
            level_requirement=1,
            faction_theme=None
        )
    
    def _generate_misc_item(self, biome: str, faction: str, 
                          min_level: int, max_level: int) -> LootItem:
        """Generate miscellaneous items"""
        
        level = random.randint(min_level, max_level)
        rarity = self._select_rarity(level)
        
        misc_items = [
            "Datapad", "Holocron", "Crystal", "Artifact", "Component",
            "Scanner", "Translator", "Beacon", "Recorder", "Tool"
        ]
        
        item_name = random.choice(misc_items)
        name = f"{rarity.title()} {item_name}"
        
        # Misc items provide utility bonuses
        bonuses = {"utility": random.randint(1, 5)}
        
        flavor_text = f"A {rarity} {item_name.lower()} with unknown properties."
        
        base_value = 30 * level
        value = int(base_value * self.rarity_multipliers[rarity])
        
        return LootItem(
            name=name,
            rarity=rarity,
            item_type="misc",
            bonuses=bonuses,
            flavor_text=flavor_text,
            value_credits=value,
            level_requirement=level,
            faction_theme=faction
        )
    
    def _get_faction_theme(self, faction: str) -> Dict:
        """Get theme data for a faction"""
        faction_lower = faction.lower()
        
        for theme_name, theme_data in self.faction_themes.items():
            if theme_name in faction_lower:
                return theme_data
        
        # Default neutral theme
        return {
            "colors": ["gray", "brown", "silver"],
            "materials": ["metal", "plastic", "composite"],
            "descriptors": ["standard", "basic", "functional"]
        }
    
    def _generate_weapon_flavor(self, name: str, weapon_type: str, 
                              theme: Dict, rarity: str) -> str:
        """Generate flavor text for weapons"""
        
        color = random.choice(theme["colors"])
        material = random.choice(theme["materials"])
        descriptor = random.choice(theme["descriptors"])
        
        templates = [
            f"This {descriptor} {weapon_type} gleams with {color} {material}.",
            f"A {rarity} {weapon_type} crafted from {color} {material}.",
            f"The {color} {material} of this {weapon_type} marks it as {descriptor}."
        ]
        
        return random.choice(templates)
    
    def _generate_armor_flavor(self, name: str, armor_type: str,
                             theme: Dict, rarity: str) -> str:
        """Generate flavor text for armor"""
        
        color = random.choice(theme["colors"])
        material = random.choice(theme["materials"])
        descriptor = random.choice(theme["descriptors"])
        
        templates = [
            f"This {descriptor} {armor_type} is reinforced with {color} {material}.",
            f"A {rarity} set of {armor_type} featuring {color} {material} plating.",
            f"The {color} {material} construction makes this {armor_type} both {descriptor} and protective."
        ]
        
        return random.choice(templates)


# Example usage and testing
if __name__ == "__main__":
    generator = LootGenerator()
    
    # Generate a sample loot table
    loot_table = generator.generate_loot_table("desert", "republic", "mid", 20)
    
    print("Sample Loot Table:")
    for item in loot_table[:10]:  # Show first 10 items
        print(f"- {item.name} ({item.rarity} {item.item_type})")
        print(f"  Bonuses: {item.bonuses}")
        print(f"  Value: {item.value_credits} credits")
        print(f"  Level: {item.level_requirement}")
        print()
