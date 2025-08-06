"""
Encounter & Enemy Generator Module

Defines enemy types per planet and dynamic spawn rules using D&D-style
challenge ratings adapted for Star Wars themes.
"""

from dataclasses import dataclass
from typing import List, Dict, Optional
import random
import json
from .story_seed_interpreter import CampaignConfig
from .planet_generator import PlanetData


@dataclass
class EnemyData:
    """Represents a single enemy type"""
    name: str
    species: str
    cr_rating: float  # Challenge Rating (D&D style)
    hit_points: int
    armor_class: int
    abilities: List[str]  # Special abilities/attacks
    loot_table: List[str]  # Possible loot drops
    biome_preference: List[str]  # Which biomes this enemy appears in
    faction: str
    description: str
    
    def to_dict(self) -> dict:
        return {
            "name": self.name,
            "species": self.species,
            "cr_rating": self.cr_rating,
            "hit_points": self.hit_points,
            "armor_class": self.armor_class,
            "abilities": self.abilities,
            "loot_table": self.loot_table,
            "biome_preference": self.biome_preference,
            "faction": self.faction,
            "description": self.description
        }
    
    @classmethod
    def from_dict(cls, data: dict) -> 'EnemyData':
        return cls(**data)


@dataclass
class EncounterData:
    """Represents a specific encounter with enemies"""
    encounter_id: str
    enemies: List[Dict[str, any]]  # List of {"enemy": EnemyData, "count": int}
    total_cr: float
    encounter_type: str  # "combat", "stealth", "social", "puzzle"
    location: str
    description: str
    special_conditions: List[str]  # Environmental hazards, etc.
    
    def to_dict(self) -> dict:
        return {
            "encounter_id": self.encounter_id,
            "enemies": [{"enemy": e["enemy"].to_dict(), "count": e["count"]} 
                       for e in self.enemies],
            "total_cr": self.total_cr,
            "encounter_type": self.encounter_type,
            "location": self.location,
            "description": self.description,
            "special_conditions": self.special_conditions
        }


class EncounterGenerator:
    """Generates enemies and encounters for campaign worlds"""
    
    def __init__(self):
        self.enemy_templates = self._create_enemy_templates()
        
        # CR scaling by difficulty tier
        self.cr_ranges = {
            "early": (0.25, 2.0),
            "mid": (1.0, 5.0),
            "late": (3.0, 10.0)
        }
        
        # Encounter types and their weights
        self.encounter_types = {
            "combat": 0.6,
            "stealth": 0.2,
            "social": 0.1,
            "puzzle": 0.1
        }
    
    def _create_enemy_templates(self) -> List[EnemyData]:
        """Create the base enemy templates (100+ enemies as specified)"""
        enemies = []
        
        # Humanoid enemies
        humanoid_species = ["Human", "Twi'lek", "Rodian", "Zabrak", "Weequay"]
        humanoid_roles = ["Soldier", "Scout", "Mercenary", "Assassin", "Officer"]
        
        for species in humanoid_species:
            for role in humanoid_roles:
                cr = random.uniform(0.5, 3.0)
                hp = int(10 + (cr * 15))
                ac = int(12 + (cr * 2))
                
                abilities = self._generate_abilities(role, cr)
                loot = self._generate_loot_table(role, cr)
                
                enemy = EnemyData(
                    name=f"{species} {role}",
                    species=species,
                    cr_rating=cr,
                    hit_points=hp,
                    armor_class=ac,
                    abilities=abilities,
                    loot_table=loot,
                    biome_preference=["urban", "desert", "forest"],
                    faction="Various",
                    description=f"A {species.lower()} serving as a {role.lower()}"
                )
                enemies.append(enemy)
        
        # Creature enemies
        creatures = [
            ("Krayt Dragon", "desert", 8.0, ["Acid Breath", "Tail Sweep"]),
            ("Rancor", "forest", 6.0, ["Grab", "Bite", "Rage"]),
            ("Wampa", "ice", 4.0, ["Ice Claws", "Camouflage"]),
            ("Nexu", "forest", 3.0, ["Pounce", "Night Vision"]),
            ("Dewback", "desert", 1.0, ["Charge", "Thick Hide"]),
            ("Bantha", "desert", 2.0, ["Trample", "Thick Hide"]),
            ("Kinrath", "forest", 2.5, ["Web", "Poison Bite"]),
            ("Tuk'ata", "volcanic", 5.0, ["Force Resistance", "Pack Hunter"])
        ]
        
        for name, biome, cr, abilities in creatures:
            hp = int(20 + (cr * 20))
            ac = int(10 + (cr * 1.5))
            
            enemy = EnemyData(
                name=name,
                species="Beast",
                cr_rating=cr,
                hit_points=hp,
                armor_class=ac,
                abilities=abilities,
                loot_table=["Beast Hide", "Claws", "Teeth"],
                biome_preference=[biome],
                faction="Wildlife",
                description=f"A dangerous {name.lower()} native to {biome} worlds"
            )
            enemies.append(enemy)
        
        # Droid enemies
        droid_types = [
            ("Security Droid", 1.5, ["Stun Blast", "Alarm"]),
            ("War Droid", 4.0, ["Heavy Blaster", "Armor Plating"]),
            ("Assassin Droid", 6.0, ["Stealth Mode", "Poison Dart"]),
            ("Mining Droid", 2.0, ["Drill Arm", "Explosive Charge"]),
            ("Protocol Droid", 0.25, ["Translation", "Distraction"])
        ]
        
        for name, cr, abilities in droid_types:
            hp = int(15 + (cr * 12))
            ac = int(14 + (cr * 1.5))
            
            enemy = EnemyData(
                name=name,
                species="Droid",
                cr_rating=cr,
                hit_points=hp,
                armor_class=ac,
                abilities=abilities,
                loot_table=["Scrap Metal", "Power Cell", "Circuit Board"],
                biome_preference=["urban", "volcanic", "ice"],
                faction="Various",
                description=f"An automated {name.lower()} with combat capabilities"
            )
            enemies.append(enemy)
        
        # Sith/Dark Side enemies
        sith_enemies = [
            ("Sith Apprentice", 5.0, ["Force Lightning", "Lightsaber Combat"]),
            ("Dark Jedi", 4.0, ["Force Push", "Lightsaber Throw"]),
            ("Sith Assassin", 6.0, ["Force Stealth", "Dual Sabers"]),
            ("Sith Marauder", 7.0, ["Force Rage", "Saber Fury"]),
            ("Sith Lord", 10.0, ["Force Storm", "Master Duelist"])
        ]
        
        for name, cr, abilities in sith_enemies:
            hp = int(25 + (cr * 18))
            ac = int(15 + (cr * 1.2))
            
            enemy = EnemyData(
                name=name,
                species="Human",
                cr_rating=cr,
                hit_points=hp,
                armor_class=ac,
                abilities=abilities + ["Force Sensitive"],
                loot_table=["Lightsaber Crystal", "Sith Holocron", "Dark Robes"],
                biome_preference=["urban", "volcanic", "desert"],
                faction="Sith Empire",
                description=f"A powerful {name.lower()} wielding the dark side"
            )
            enemies.append(enemy)
        
        return enemies
    
    def _generate_abilities(self, role: str, cr: float) -> List[str]:
        """Generate abilities based on enemy role and CR"""
        base_abilities = {
            "Soldier": ["Blaster Rifle", "Tactical Training"],
            "Scout": ["Stealth", "Tracking"],
            "Mercenary": ["Dual Weapons", "Combat Experience"],
            "Assassin": ["Sneak Attack", "Poison"],
            "Officer": ["Command", "Inspire Troops"]
        }
        
        abilities = base_abilities.get(role, ["Basic Attack"])
        
        # Add abilities based on CR
        if cr >= 2.0:
            abilities.append("Veteran Training")
        if cr >= 4.0:
            abilities.append("Elite Equipment")
        if cr >= 6.0:
            abilities.append("Master Combatant")
        
        return abilities
    
    def _generate_loot_table(self, role: str, cr: float) -> List[str]:
        """Generate loot table based on enemy role and CR"""
        base_loot = {
            "Soldier": ["Blaster Rifle", "Combat Armor", "Credits"],
            "Scout": ["Stealth Generator", "Binoculars", "Credits"],
            "Mercenary": ["Heavy Blaster", "Medpacs", "Credits"],
            "Assassin": ["Vibroblade", "Poison", "Credits"],
            "Officer": ["Command Codes", "Datapad", "Credits"]
        }
        
        loot = base_loot.get(role, ["Credits"])
        
        # Add rare loot based on CR
        if cr >= 3.0:
            loot.append("Rare Equipment")
        if cr >= 5.0:
            loot.append("Epic Item")
        
        return loot
    
    def generate_enemies_for_planet(self, planet: PlanetData, 
                                  config: CampaignConfig) -> List[EnemyData]:
        """
        Generate appropriate enemies for a specific planet
        
        Args:
            planet: Planet data to generate enemies for
            config: Campaign configuration
            
        Returns:
            List of enemies that can appear on this planet
        """
        suitable_enemies = []
        
        # Filter enemies by biome compatibility
        for enemy in self.enemy_templates:
            if (planet.biome in enemy.biome_preference or 
                "any" in enemy.biome_preference):
                
                # Scale enemy CR based on planet difficulty
                scaled_enemy = self._scale_enemy_for_difficulty(enemy, planet.difficulty_tier)
                suitable_enemies.append(scaled_enemy)
        
        # Add planet-specific enemies based on government/faction
        faction_enemies = self._generate_faction_enemies(planet, config)
        suitable_enemies.extend(faction_enemies)
        
        return suitable_enemies
    
    def _scale_enemy_for_difficulty(self, enemy: EnemyData, 
                                  difficulty_tier: str) -> EnemyData:
        """Scale an enemy's stats based on difficulty tier"""
        
        # Create a copy to avoid modifying the original
        scaled_enemy = EnemyData(
            name=enemy.name,
            species=enemy.species,
            cr_rating=enemy.cr_rating,
            hit_points=enemy.hit_points,
            armor_class=enemy.armor_class,
            abilities=enemy.abilities.copy(),
            loot_table=enemy.loot_table.copy(),
            biome_preference=enemy.biome_preference.copy(),
            faction=enemy.faction,
            description=enemy.description
        )
        
        # Apply scaling multipliers
        multipliers = {"early": 0.8, "mid": 1.0, "late": 1.3}
        multiplier = multipliers.get(difficulty_tier, 1.0)
        
        scaled_enemy.cr_rating *= multiplier
        scaled_enemy.hit_points = int(scaled_enemy.hit_points * multiplier)
        scaled_enemy.armor_class = int(scaled_enemy.armor_class * (1 + (multiplier - 1) * 0.5))
        
        # Add tier-specific abilities
        if difficulty_tier == "late" and "Elite" not in scaled_enemy.name:
            scaled_enemy.name = f"Elite {scaled_enemy.name}"
            scaled_enemy.abilities.append("Elite Training")
        
        return scaled_enemy
    
    def _generate_faction_enemies(self, planet: PlanetData, 
                                config: CampaignConfig) -> List[EnemyData]:
        """Generate enemies specific to the planet's faction/government"""
        faction_enemies = []
        
        # Government-based enemies
        if "Imperial" in planet.government:
            faction_enemies.extend(self._create_imperial_enemies(planet.difficulty_tier))
        elif "Republic" in planet.government:
            faction_enemies.extend(self._create_republic_enemies(planet.difficulty_tier))
        elif "Corporate" in planet.government:
            faction_enemies.extend(self._create_corporate_enemies(planet.difficulty_tier))
        
        return faction_enemies
    
    def _create_imperial_enemies(self, difficulty_tier: str) -> List[EnemyData]:
        """Create Imperial faction enemies"""
        base_cr = {"early": 1.0, "mid": 2.5, "late": 4.0}[difficulty_tier]
        
        return [
            EnemyData(
                name="Imperial Trooper",
                species="Human",
                cr_rating=base_cr,
                hit_points=int(20 + base_cr * 10),
                armor_class=int(14 + base_cr),
                abilities=["Blaster Rifle", "Imperial Training"],
                loot_table=["Imperial Credits", "Blaster Rifle", "Armor"],
                biome_preference=["urban", "desert", "ice"],
                faction="Galactic Empire",
                description="A loyal soldier of the Galactic Empire"
            )
        ]
    
    def _create_republic_enemies(self, difficulty_tier: str) -> List[EnemyData]:
        """Create Republic faction enemies (when corrupted/hostile)"""
        base_cr = {"early": 0.8, "mid": 2.0, "late": 3.5}[difficulty_tier]
        
        return [
            EnemyData(
                name="Corrupted Republic Guard",
                species="Human",
                cr_rating=base_cr,
                hit_points=int(18 + base_cr * 12),
                armor_class=int(13 + base_cr),
                abilities=["Republic Training", "Defensive Stance"],
                loot_table=["Republic Credits", "Security Codes", "Armor"],
                biome_preference=["urban", "forest"],
                faction="Corrupted Republic",
                description="A Republic guard turned to corruption"
            )
        ]
    
    def _create_corporate_enemies(self, difficulty_tier: str) -> List[EnemyData]:
        """Create Corporate faction enemies"""
        base_cr = {"early": 1.2, "mid": 2.8, "late": 4.5}[difficulty_tier]
        
        return [
            EnemyData(
                name="Corporate Security",
                species="Human",
                cr_rating=base_cr,
                hit_points=int(22 + base_cr * 8),
                armor_class=int(15 + base_cr * 0.8),
                abilities=["Corporate Equipment", "Non-Lethal Options"],
                loot_table=["Corporate Scrip", "Security Badge", "Stun Weapons"],
                biome_preference=["urban", "volcanic"],
                faction="Corporate",
                description="Well-equipped corporate security personnel"
            )
        ]
    
    def generate_encounter(self, enemies: List[EnemyData], target_cr: float,
                         location: str) -> EncounterData:
        """
        Generate a specific encounter with appropriate enemies
        
        Args:
            enemies: Available enemies for this encounter
            target_cr: Target challenge rating for the encounter
            location: Where the encounter takes place
            
        Returns:
            Complete encounter data
        """
        
        # Select encounter type
        encounter_type = self._select_encounter_type()
        
        # Build enemy group to match target CR
        enemy_group = self._build_enemy_group(enemies, target_cr)
        
        # Calculate total CR
        total_cr = sum(e["enemy"].cr_rating * e["count"] for e in enemy_group)
        
        # Generate description and conditions
        description = self._generate_encounter_description(enemy_group, location, encounter_type)
        special_conditions = self._generate_special_conditions(location, encounter_type)
        
        encounter_id = f"enc_{random.randint(1000, 9999)}"
        
        return EncounterData(
            encounter_id=encounter_id,
            enemies=enemy_group,
            total_cr=total_cr,
            encounter_type=encounter_type,
            location=location,
            description=description,
            special_conditions=special_conditions
        )
    
    def _select_encounter_type(self) -> str:
        """Select encounter type based on weights"""
        rand = random.random()
        cumulative = 0
        
        for enc_type, weight in self.encounter_types.items():
            cumulative += weight
            if rand <= cumulative:
                return enc_type
        
        return "combat"  # fallback
    
    def _build_enemy_group(self, enemies: List[EnemyData], 
                          target_cr: float) -> List[Dict[str, any]]:
        """Build a group of enemies that matches the target CR"""
        
        enemy_group = []
        remaining_cr = target_cr
        
        # Sort enemies by CR for better selection
        sorted_enemies = sorted(enemies, key=lambda e: e.cr_rating)
        
        while remaining_cr > 0.25 and len(enemy_group) < 8:  # Max 8 enemies per encounter
            # Select appropriate enemy
            suitable_enemies = [e for e in sorted_enemies if e.cr_rating <= remaining_cr * 1.2]
            
            if not suitable_enemies:
                break
            
            selected_enemy = random.choice(suitable_enemies)
            
            # Determine count (usually 1-3)
            max_count = min(3, int(remaining_cr / selected_enemy.cr_rating) + 1)
            count = random.randint(1, max_count)
            
            enemy_group.append({"enemy": selected_enemy, "count": count})
            remaining_cr -= selected_enemy.cr_rating * count
        
        return enemy_group
    
    def _generate_encounter_description(self, enemy_group: List[Dict[str, any]],
                                      location: str, encounter_type: str) -> str:
        """Generate description for the encounter"""
        
        enemy_names = []
        for group in enemy_group:
            count = group["count"]
            name = group["enemy"].name
            if count > 1:
                enemy_names.append(f"{count} {name}s")
            else:
                enemy_names.append(f"a {name}")
        
        enemy_list = ", ".join(enemy_names)
        
        type_descriptions = {
            "combat": f"You encounter {enemy_list} in {location}. They appear hostile!",
            "stealth": f"You spot {enemy_list} patrolling {location}. You might be able to sneak past.",
            "social": f"You meet {enemy_list} in {location}. They seem willing to talk.",
            "puzzle": f"You find {enemy_list} trapped in {location}. There might be a way to help them."
        }
        
        return type_descriptions.get(encounter_type, f"You encounter {enemy_list} in {location}.")
    
    def _generate_special_conditions(self, location: str, 
                                   encounter_type: str) -> List[str]:
        """Generate special environmental conditions for the encounter"""
        
        conditions = []
        
        # Location-based conditions
        if "cave" in location.lower():
            conditions.append("Limited visibility")
        if "lava" in location.lower() or "volcanic" in location.lower():
            conditions.append("Heat damage")
        if "ice" in location.lower() or "frozen" in location.lower():
            conditions.append("Slippery terrain")
        if "city" in location.lower():
            conditions.append("Civilian bystanders")
        
        # Type-based conditions
        if encounter_type == "stealth":
            conditions.append("Stealth advantage available")
        elif encounter_type == "social":
            conditions.append("Diplomacy possible")
        
        return conditions


# Example usage and testing
if __name__ == "__main__":
    from .story_seed_interpreter import StorySeedInterpreter
    from .planet_generator import PlanetGenerator
    
    # Test encounter generation
    interpreter = StorySeedInterpreter()
    config = interpreter.parse_story_seed("Test 15-hour adventure")
    
    planet_gen = PlanetGenerator()
    planet = planet_gen.generate_planet(config, 0, "early", ["Tatooine"])
    
    encounter_gen = EncounterGenerator()
    enemies = encounter_gen.generate_enemies_for_planet(planet, config)
    
    print(f"Generated {len(enemies)} enemy types for {planet.name}")
    
    # Generate a sample encounter
    encounter = encounter_gen.generate_encounter(enemies, 2.0, "Desert Outpost")
    print(f"\nSample Encounter: {encounter.description}")
    print(f"Total CR: {encounter.total_cr}")
    print(f"Special Conditions: {encounter.special_conditions}")
