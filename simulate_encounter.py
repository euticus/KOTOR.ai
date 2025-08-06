#!/usr/bin/env python3
"""
KOTOR.ai Scenario Testing Tool
Generates and simulates encounters for testing AIDM integration
"""

import json
import argparse
import random
import sys
from typing import Dict, List, Any, Optional
from dataclasses import dataclass, asdict
from pathlib import Path

@dataclass
class EncounterRequest:
    """Request parameters for encounter generation"""
    planet_type: str = "urban"
    enemy_type: str = "humanoid"
    player_level: int = 1
    difficulty: str = "easy"
    duration_minutes: int = 15
    encounter_type: str = "combat"
    biome: str = "urban"
    layout_type: str = "city"
    party_size: int = 1

@dataclass
class EnemyData:
    """Enemy data for encounters"""
    name: str
    species: str
    challenge_rating: float
    hit_points: int
    armor_class: int
    abilities: List[str]
    loot_table: List[str]
    ai_behavior: str
    spawn_weight: float

@dataclass
class EncounterResult:
    """Generated encounter data"""
    encounter_id: str
    encounter_type: str
    difficulty: str
    estimated_duration: int
    enemies: List[EnemyData]
    loot_rewards: List[Dict[str, Any]]
    environment: Dict[str, Any]
    objectives: List[str]
    success_conditions: List[str]
    failure_conditions: List[str]
    narrative_context: str

class EncounterGenerator:
    """Generates encounters based on AIDM parameters"""
    
    def __init__(self):
        self.enemy_templates = self._load_enemy_templates()
        self.loot_tables = self._load_loot_tables()
        self.environment_data = self._load_environment_data()
    
    def _load_enemy_templates(self) -> Dict[str, List[EnemyData]]:
        """Load enemy templates by type"""
        return {
            "humanoid": [
                EnemyData(
                    name="Sith Trooper",
                    species="Human",
                    challenge_rating=1.5,
                    hit_points=25,
                    armor_class=16,
                    abilities=["Blaster Rifle", "Tactical Training"],
                    loot_table=["credits", "blaster_parts", "armor_scraps"],
                    ai_behavior="aggressive",
                    spawn_weight=1.0
                ),
                EnemyData(
                    name="Dark Jedi",
                    species="Human",
                    challenge_rating=3.0,
                    hit_points=45,
                    armor_class=18,
                    abilities=["Force Lightning", "Lightsaber Combat", "Force Push"],
                    loot_table=["lightsaber_crystal", "force_artifact", "credits"],
                    ai_behavior="tactical",
                    spawn_weight=0.3
                ),
                EnemyData(
                    name="Mercenary",
                    species="Human",
                    challenge_rating=1.0,
                    hit_points=20,
                    armor_class=14,
                    abilities=["Dual Pistols", "Grenade"],
                    loot_table=["credits", "weapon_parts", "stims"],
                    ai_behavior="opportunistic",
                    spawn_weight=1.2
                )
            ],
            "droid": [
                EnemyData(
                    name="Security Droid",
                    species="Droid",
                    challenge_rating=1.2,
                    hit_points=30,
                    armor_class=17,
                    abilities=["Integrated Blaster", "Shield Generator"],
                    loot_table=["droid_parts", "energy_cells", "scrap_metal"],
                    ai_behavior="defensive",
                    spawn_weight=1.0
                ),
                EnemyData(
                    name="Assassin Droid",
                    species="Droid",
                    challenge_rating=2.5,
                    hit_points=35,
                    armor_class=19,
                    abilities=["Stealth Mode", "Poison Dart", "Self-Destruct"],
                    loot_table=["advanced_components", "targeting_system", "credits"],
                    ai_behavior="stealth",
                    spawn_weight=0.4
                )
            ],
            "beast": [
                EnemyData(
                    name="Kath Hound",
                    species="Beast",
                    challenge_rating=0.8,
                    hit_points=18,
                    armor_class=13,
                    abilities=["Pack Tactics", "Bite"],
                    loot_table=["beast_hide", "meat", "bones"],
                    ai_behavior="pack",
                    spawn_weight=1.5
                ),
                EnemyData(
                    name="Krayt Dragon",
                    species="Beast",
                    challenge_rating=8.0,
                    hit_points=200,
                    armor_class=22,
                    abilities=["Dragon Breath", "Tail Sweep", "Roar"],
                    loot_table=["krayt_pearl", "dragon_scales", "rare_crystals"],
                    ai_behavior="territorial",
                    spawn_weight=0.1
                )
            ]
        }
    
    def _load_loot_tables(self) -> Dict[str, List[Dict[str, Any]]]:
        """Load loot tables by category"""
        return {
            "credits": [
                {"name": "Small Credit Pouch", "value": 50, "rarity": "common"},
                {"name": "Credit Stash", "value": 200, "rarity": "uncommon"},
                {"name": "Republic Treasury", "value": 1000, "rarity": "rare"}
            ],
            "weapons": [
                {"name": "Vibroblade", "damage": "1d8", "rarity": "common"},
                {"name": "Blaster Rifle", "damage": "2d6", "rarity": "uncommon"},
                {"name": "Lightsaber", "damage": "2d8", "rarity": "legendary"}
            ],
            "armor": [
                {"name": "Padded Armor", "ac_bonus": 1, "rarity": "common"},
                {"name": "Combat Suit", "ac_bonus": 3, "rarity": "uncommon"},
                {"name": "Mandalorian Armor", "ac_bonus": 5, "rarity": "rare"}
            ]
        }
    
    def _load_environment_data(self) -> Dict[str, Dict[str, Any]]:
        """Load environment templates"""
        return {
            "urban": {
                "description": "Bustling city environment with buildings and crowds",
                "cover_options": ["buildings", "vehicles", "debris"],
                "hazards": ["traffic", "security cameras"],
                "lighting": "artificial",
                "sound_level": "high"
            },
            "desert": {
                "description": "Harsh desert landscape with sand dunes and rocky outcrops",
                "cover_options": ["rocks", "dunes", "ruins"],
                "hazards": ["sandstorms", "extreme heat"],
                "lighting": "natural",
                "sound_level": "low"
            },
            "forest": {
                "description": "Dense forest with thick vegetation and wildlife",
                "cover_options": ["trees", "undergrowth", "fallen logs"],
                "hazards": ["wild animals", "difficult terrain"],
                "lighting": "filtered",
                "sound_level": "medium"
            },
            "space_station": {
                "description": "High-tech space station with corridors and chambers",
                "cover_options": ["bulkheads", "consoles", "doorways"],
                "hazards": ["vacuum exposure", "system failures"],
                "lighting": "artificial",
                "sound_level": "low"
            }
        }
    
    def generate_encounter(self, request: EncounterRequest) -> EncounterResult:
        """Generate an encounter based on request parameters"""
        encounter_id = f"ENC_{random.randint(1000, 9999)}"
        
        # Calculate difficulty multiplier
        difficulty_multipliers = {
            "trivial": 0.5,
            "easy": 0.8,
            "medium": 1.0,
            "hard": 1.3,
            "deadly": 1.8
        }
        difficulty_mult = difficulty_multipliers.get(request.difficulty, 1.0)
        
        # Generate enemies
        enemies = self._generate_enemies(request, difficulty_mult)
        
        # Generate loot
        loot_rewards = self._generate_loot(request, enemies)
        
        # Generate environment
        environment = self._generate_environment(request)
        
        # Generate objectives
        objectives = self._generate_objectives(request)
        
        # Generate conditions
        success_conditions = self._generate_success_conditions(request, enemies)
        failure_conditions = self._generate_failure_conditions(request)
        
        # Generate narrative context
        narrative_context = self._generate_narrative_context(request, enemies)
        
        return EncounterResult(
            encounter_id=encounter_id,
            encounter_type=request.encounter_type,
            difficulty=request.difficulty,
            estimated_duration=request.duration_minutes,
            enemies=enemies,
            loot_rewards=loot_rewards,
            environment=environment,
            objectives=objectives,
            success_conditions=success_conditions,
            failure_conditions=failure_conditions,
            narrative_context=narrative_context
        )
    
    def _generate_enemies(self, request: EncounterRequest, difficulty_mult: float) -> List[EnemyData]:
        """Generate enemies for the encounter"""
        enemy_pool = self.enemy_templates.get(request.enemy_type, [])
        if not enemy_pool:
            enemy_pool = self.enemy_templates["humanoid"]  # Fallback
        
        # Calculate target challenge rating
        target_cr = request.player_level * difficulty_mult
        
        enemies = []
        current_cr = 0.0
        
        while current_cr < target_cr and len(enemies) < 10:  # Max 10 enemies
            # Select enemy based on weights and remaining CR budget
            available_enemies = [e for e in enemy_pool if e.challenge_rating <= (target_cr - current_cr + 0.5)]
            if not available_enemies:
                break
            
            # Weighted random selection
            weights = [e.spawn_weight for e in available_enemies]
            selected_enemy = random.choices(available_enemies, weights=weights)[0]
            
            # Scale enemy to player level
            scaled_enemy = self._scale_enemy(selected_enemy, request.player_level)
            enemies.append(scaled_enemy)
            current_cr += scaled_enemy.challenge_rating
        
        return enemies
    
    def _scale_enemy(self, enemy: EnemyData, player_level: int) -> EnemyData:
        """Scale enemy stats to player level"""
        level_mult = max(0.5, player_level / 5.0)
        
        scaled_enemy = EnemyData(
            name=enemy.name,
            species=enemy.species,
            challenge_rating=enemy.challenge_rating * level_mult,
            hit_points=int(enemy.hit_points * level_mult),
            armor_class=enemy.armor_class + max(0, player_level - 1),
            abilities=enemy.abilities.copy(),
            loot_table=enemy.loot_table.copy(),
            ai_behavior=enemy.ai_behavior,
            spawn_weight=enemy.spawn_weight
        )
        
        return scaled_enemy
    
    def _generate_loot(self, request: EncounterRequest, enemies: List[EnemyData]) -> List[Dict[str, Any]]:
        """Generate loot rewards"""
        loot_rewards = []
        
        # Base loot from enemies
        for enemy in enemies:
            for loot_category in enemy.loot_table:
                if loot_category in self.loot_tables:
                    loot_pool = self.loot_tables[loot_category]
                    if random.random() < 0.6:  # 60% chance for loot drop
                        loot_item = random.choice(loot_pool)
                        loot_rewards.append(loot_item)
        
        # Bonus loot based on difficulty
        if request.difficulty in ["hard", "deadly"]:
            bonus_loot = random.choice(self.loot_tables["weapons"])
            loot_rewards.append(bonus_loot)
        
        return loot_rewards
    
    def _generate_environment(self, request: EncounterRequest) -> Dict[str, Any]:
        """Generate environment data"""
        env_template = self.environment_data.get(request.biome, self.environment_data["urban"])
        
        environment = env_template.copy()
        environment["biome"] = request.biome
        environment["layout_type"] = request.layout_type
        environment["weather"] = random.choice(["clear", "overcast", "stormy"])
        environment["time_of_day"] = random.choice(["dawn", "day", "dusk", "night"])
        
        return environment
    
    def _generate_objectives(self, request: EncounterRequest) -> List[str]:
        """Generate encounter objectives"""
        objectives = []
        
        if request.encounter_type == "combat":
            objectives.append("Defeat all enemies")
            if random.random() < 0.3:
                objectives.append("Protect civilians")
        elif request.encounter_type == "stealth":
            objectives.append("Avoid detection")
            objectives.append("Reach the target location")
        elif request.encounter_type == "rescue":
            objectives.append("Rescue the hostages")
            objectives.append("Minimize casualties")
        else:
            objectives.append("Complete the mission")
        
        return objectives
    
    def _generate_success_conditions(self, request: EncounterRequest, enemies: List[EnemyData]) -> List[str]:
        """Generate success conditions"""
        conditions = []
        
        if enemies:
            conditions.append("All hostile enemies defeated")
        
        conditions.append("Player character survives")
        
        if request.party_size > 1:
            conditions.append("At least 50% of party members survive")
        
        return conditions
    
    def _generate_failure_conditions(self, request: EncounterRequest) -> List[str]:
        """Generate failure conditions"""
        conditions = [
            "Player character is defeated",
            "Mission timer expires"
        ]
        
        if request.encounter_type == "rescue":
            conditions.append("Hostages are killed")
        elif request.encounter_type == "stealth":
            conditions.append("Alarm is raised")
        
        return conditions
    
    def _generate_narrative_context(self, request: EncounterRequest, enemies: List[EnemyData]) -> str:
        """Generate narrative context for the encounter"""
        contexts = {
            "urban": "The bustling city streets suddenly turn dangerous as hostile forces emerge from the shadows.",
            "desert": "In the harsh desert wasteland, danger lurks behind every dune and rocky outcrop.",
            "forest": "The dense forest conceals threats that emerge from the undergrowth.",
            "space_station": "Aboard the space station, the sterile corridors echo with the sounds of conflict."
        }
        
        base_context = contexts.get(request.biome, "A dangerous encounter awaits.")
        
        if enemies:
            enemy_types = list(set(e.species for e in enemies))
            enemy_desc = ", ".join(enemy_types)
            base_context += f" You face {enemy_desc} forces in this confrontation."
        
        return base_context

def main():
    """Main CLI interface"""
    parser = argparse.ArgumentParser(description="KOTOR.ai Encounter Simulator")
    parser.add_argument("--planet-type", default="urban", help="Planet type (urban, desert, forest, space_station)")
    parser.add_argument("--enemy-type", default="humanoid", help="Enemy type (humanoid, droid, beast)")
    parser.add_argument("--player-level", type=int, default=1, help="Player level (1-20)")
    parser.add_argument("--difficulty", default="medium", help="Difficulty (trivial, easy, medium, hard, deadly)")
    parser.add_argument("--duration", type=int, default=15, help="Expected duration in minutes")
    parser.add_argument("--encounter-type", default="combat", help="Encounter type (combat, stealth, rescue)")
    parser.add_argument("--party-size", type=int, default=1, help="Party size")
    parser.add_argument("--output", help="Output file path")
    parser.add_argument("--format", default="json", choices=["json", "yaml"], help="Output format")
    
    args = parser.parse_args()
    
    # Create encounter request
    request = EncounterRequest(
        planet_type=args.planet_type,
        enemy_type=args.enemy_type,
        player_level=args.player_level,
        difficulty=args.difficulty,
        duration_minutes=args.duration,
        encounter_type=args.encounter_type,
        biome=args.planet_type,
        party_size=args.party_size
    )
    
    # Generate encounter
    generator = EncounterGenerator()
    result = generator.generate_encounter(request)
    
    # Convert to dict for serialization
    result_dict = asdict(result)
    
    # Output result
    if args.format == "json":
        output_text = json.dumps(result_dict, indent=2)
    else:
        try:
            import yaml
            output_text = yaml.dump(result_dict, default_flow_style=False)
        except ImportError:
            print("PyYAML not installed, falling back to JSON", file=sys.stderr)
            output_text = json.dumps(result_dict, indent=2)
    
    if args.output:
        with open(args.output, 'w') as f:
            f.write(output_text)
        print(f"Encounter saved to {args.output}")
    else:
        print(output_text)

if __name__ == "__main__":
    main()
