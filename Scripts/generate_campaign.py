#!/usr/bin/env python3
"""
KOTOR.ai - Campaign Generation Script
Copyright Epic Games, Inc. All Rights Reserved.

This script generates AIDM campaigns and exports them as JSON files for Unreal Engine.
"""

import json
import os
import sys
import argparse
import random
from datetime import datetime
from typing import Dict, List, Any, Optional

# Add the AIDM path to sys.path
script_dir = os.path.dirname(os.path.abspath(__file__))
aidm_path = os.path.join(script_dir, '..', 'AIDM')
sys.path.insert(0, aidm_path)

try:
    from campaign_generator import CampaignGenerator
    from npc_generator import NPCGenerator
    from quest_generator import QuestGenerator
    from loot_generator import LootGenerator
    from planet_generator import PlanetGenerator
except ImportError as e:
    print(f"Error importing AIDM modules: {e}")
    print("Make sure the AIDM directory is properly set up.")
    sys.exit(1)

class KOTORCampaignGenerator:
    """Enhanced campaign generator for KOTOR.ai"""
    
    def __init__(self):
        self.campaign_generator = CampaignGenerator()
        self.npc_generator = NPCGenerator()
        self.quest_generator = QuestGenerator()
        self.loot_generator = LootGenerator()
        self.planet_generator = PlanetGenerator()
        
        # KOTOR-specific templates
        self.kotor_planets = [
            "Taris", "Dantooine", "Tatooine", "Kashyyyk", "Manaan", 
            "Korriban", "Leviathan", "Star Forge", "Endar Spire", "Unknown World"
        ]
        
        self.kotor_factions = [
            "Republic", "Sith Empire", "Jedi Order", "Exchange", "Czerka Corporation",
            "Mandalorians", "Rakata", "Sand People", "Wookiees", "Selkath"
        ]
        
        self.kotor_themes = [
            "redemption", "fall_to_darkness", "ancient_mysteries", "war_aftermath",
            "force_awakening", "identity_crisis", "galactic_conflict", "lost_civilization"
        ]

    def generate_test_campaign(self, name: str = "TestCampaign", 
                             planets: Optional[List[str]] = None,
                             theme: str = "redemption") -> Dict[str, Any]:
        """Generate a test campaign for development"""
        
        if planets is None:
            planets = ["Taris", "Dantooine", "Korriban"]
        
        print(f"Generating test campaign: {name}")
        print(f"Planets: {', '.join(planets)}")
        print(f"Theme: {theme}")
        
        campaign_data = {
            "campaign_id": f"test_{name.lower().replace(' ', '_')}",
            "campaign_name": name,
            "description": f"Auto-generated test campaign with theme: {theme}",
            "version": "1.0",
            "created_date": datetime.now().isoformat(),
            "theme": theme,
            "planets": [],
            "npcs": [],
            "quests": [],
            "loot_tables": [],
            "factions": [],
            "timeline_events": [],
            "metadata": {
                "generator": "KOTOR.ai Campaign Generator",
                "generator_version": "1.0",
                "is_test_campaign": True,
                "auto_generated": True
            }
        }
        
        # Generate planets
        for planet_name in planets:
            planet_data = self._generate_planet(planet_name, theme)
            campaign_data["planets"].append(planet_data)
        
        # Generate NPCs for each planet
        for planet in campaign_data["planets"]:
            planet_npcs = self._generate_planet_npcs(planet["planet_id"], theme)
            campaign_data["npcs"].extend(planet_npcs)
        
        # Generate quests
        main_quest = self._generate_main_quest(planets, theme)
        campaign_data["quests"].append(main_quest)
        
        side_quests = self._generate_side_quests(planets, theme)
        campaign_data["quests"].extend(side_quests)
        
        # Generate loot tables
        loot_tables = self._generate_loot_tables(theme)
        campaign_data["loot_tables"].extend(loot_tables)
        
        # Generate factions
        factions = self._generate_factions(theme)
        campaign_data["factions"].extend(factions)
        
        # Generate timeline events
        timeline_events = self._generate_timeline_events(planets, theme)
        campaign_data["timeline_events"].extend(timeline_events)
        
        return campaign_data

    def _generate_planet(self, planet_name: str, theme: str) -> Dict[str, Any]:
        """Generate planet data"""
        
        planet_templates = {
            "Taris": {
                "description": "A once-great ecumenopolis, now in ruins after Malak's bombardment",
                "environment": "urban_ruins",
                "atmosphere": "grim",
                "population": "survivors",
                "government": "martial_law"
            },
            "Dantooine": {
                "description": "Peaceful agricultural world, home to a Jedi Enclave",
                "environment": "grasslands",
                "atmosphere": "serene",
                "population": "farmers",
                "government": "council"
            },
            "Korriban": {
                "description": "Ancient Sith homeworld, filled with dark side energy",
                "environment": "desert_tombs",
                "atmosphere": "ominous",
                "population": "sith_students",
                "government": "sith_academy"
            }
        }
        
        template = planet_templates.get(planet_name, {
            "description": f"A mysterious world in the {planet_name} system",
            "environment": "varied",
            "atmosphere": "neutral",
            "population": "mixed",
            "government": "independent"
        })
        
        return {
            "planet_id": planet_name.lower().replace(" ", "_"),
            "planet_name": planet_name,
            "description": template["description"],
            "environment_type": template["environment"],
            "atmosphere": template["atmosphere"],
            "population_type": template["population"],
            "government_type": template["government"],
            "faction_control": self._get_planet_faction(planet_name),
            "locations": self._generate_planet_locations(planet_name),
            "resources": self._generate_planet_resources(planet_name),
            "threats": self._generate_planet_threats(planet_name, theme),
            "accessibility": "available",
            "travel_cost": random.randint(100, 500),
            "travel_time": random.randint(1, 5)
        }

    def _generate_planet_npcs(self, planet_id: str, theme: str) -> List[Dict[str, Any]]:
        """Generate NPCs for a planet"""
        npcs = []
        
        # Generate key NPCs
        npc_roles = ["quest_giver", "merchant", "guard", "civilian", "informant"]
        
        for i, role in enumerate(npc_roles):
            npc_data = {
                "npc_id": f"{planet_id}_npc_{i+1}",
                "name": self._generate_npc_name(role),
                "role": role,
                "species": self._get_random_species(),
                "faction": self._get_npc_faction(planet_id, role),
                "location": planet_id,
                "dialogue_style": self._get_dialogue_style(role, theme),
                "personality_traits": self._generate_personality_traits(role),
                "inventory": self._generate_npc_inventory(role),
                "quests_given": [],
                "reputation_modifiers": {},
                "is_recruitable": role == "companion"
            }
            npcs.append(npc_data)
        
        return npcs

    def _generate_main_quest(self, planets: List[str], theme: str) -> Dict[str, Any]:
        """Generate main quest line"""
        
        quest_templates = {
            "redemption": {
                "title": "Path of Redemption",
                "description": "Discover your true identity and choose your destiny",
                "objective": "Uncover the truth about your past and decide the fate of the galaxy"
            },
            "ancient_mysteries": {
                "title": "Echoes of the Past",
                "description": "Investigate ancient ruins and uncover lost secrets",
                "objective": "Solve the mystery of the ancient civilization"
            }
        }
        
        template = quest_templates.get(theme, quest_templates["redemption"])
        
        return {
            "quest_id": "main_quest",
            "quest_name": template["title"],
            "description": template["description"],
            "quest_type": "main",
            "objective": template["objective"],
            "planets_involved": planets,
            "prerequisites": [],
            "rewards": {
                "experience": 5000,
                "credits": 1000,
                "items": ["lightsaber_crystal", "ancient_artifact"],
                "reputation": {"jedi": 100, "republic": 50}
            },
            "stages": self._generate_quest_stages(planets, theme),
            "is_repeatable": False,
            "alignment_impact": {"light_side": 50, "dark_side": -25}
        }

    def _generate_side_quests(self, planets: List[str], theme: str) -> List[Dict[str, Any]]:
        """Generate side quests"""
        side_quests = []
        
        quest_types = ["rescue", "delivery", "investigation", "combat", "diplomacy"]
        
        for i, planet in enumerate(planets):
            quest_type = quest_types[i % len(quest_types)]
            
            quest_data = {
                "quest_id": f"side_quest_{planet.lower()}_{quest_type}",
                "quest_name": f"{quest_type.title()} on {planet}",
                "description": f"A {quest_type} mission on {planet}",
                "quest_type": "side",
                "objective": f"Complete the {quest_type} objective",
                "planets_involved": [planet],
                "prerequisites": [],
                "rewards": {
                    "experience": random.randint(500, 1500),
                    "credits": random.randint(100, 500),
                    "items": [f"{quest_type}_reward"],
                    "reputation": {self._get_planet_faction(planet): random.randint(10, 50)}
                },
                "stages": [
                    {
                        "stage_id": f"stage_1",
                        "description": f"Begin the {quest_type} mission",
                        "objectives": [f"Travel to {planet}"],
                        "completion_conditions": ["arrive_at_planet"]
                    }
                ],
                "is_repeatable": False,
                "alignment_impact": {"light_side": random.randint(5, 15)}
            }
            
            side_quests.append(quest_data)
        
        return side_quests

    def _generate_loot_tables(self, theme: str) -> List[Dict[str, Any]]:
        """Generate loot tables"""
        return [
            {
                "table_id": "common_loot",
                "table_name": "Common Loot",
                "items": [
                    {"item_id": "credits", "weight": 50, "quantity_min": 10, "quantity_max": 50},
                    {"item_id": "medpac", "weight": 30, "quantity_min": 1, "quantity_max": 3},
                    {"item_id": "energy_cell", "weight": 20, "quantity_min": 1, "quantity_max": 2}
                ]
            },
            {
                "table_id": "rare_loot",
                "table_name": "Rare Loot",
                "items": [
                    {"item_id": "lightsaber_crystal", "weight": 10, "quantity_min": 1, "quantity_max": 1},
                    {"item_id": "ancient_artifact", "weight": 5, "quantity_min": 1, "quantity_max": 1},
                    {"item_id": "force_power_scroll", "weight": 15, "quantity_min": 1, "quantity_max": 1}
                ]
            }
        ]

    def _generate_factions(self, theme: str) -> List[Dict[str, Any]]:
        """Generate faction data"""
        factions = []
        
        for faction_name in self.kotor_factions[:5]:  # Use first 5 factions
            faction_data = {
                "faction_id": faction_name.lower().replace(" ", "_"),
                "faction_name": faction_name,
                "description": f"The {faction_name} faction",
                "alignment": self._get_faction_alignment(faction_name),
                "relationships": {},
                "territories": [],
                "resources": random.randint(1000, 10000),
                "military_strength": random.randint(100, 1000),
                "influence_level": random.randint(1, 10)
            }
            factions.append(faction_data)
        
        return factions

    def _generate_timeline_events(self, planets: List[str], theme: str) -> List[Dict[str, Any]]:
        """Generate timeline events"""
        events = []
        
        base_time = 0
        for i, planet in enumerate(planets):
            event_data = {
                "event_id": f"visit_{planet.lower()}",
                "event_name": f"Arrival at {planet}",
                "description": f"The party arrives at {planet}",
                "event_type": "travel",
                "timestamp": base_time + (i * 24),  # 24 hours between planets
                "planet": planet,
                "participants": ["player"],
                "consequences": [],
                "is_player_triggered": True
            }
            events.append(event_data)
        
        return events

    # Helper methods
    def _get_planet_faction(self, planet_name: str) -> str:
        faction_map = {
            "Taris": "republic",
            "Dantooine": "jedi_order", 
            "Korriban": "sith_empire",
            "Tatooine": "exchange",
            "Kashyyyk": "wookiees"
        }
        return faction_map.get(planet_name, "neutral")

    def _generate_planet_locations(self, planet_name: str) -> List[str]:
        location_templates = {
            "Taris": ["Upper City", "Lower City", "Undercity", "Sith Base"],
            "Dantooine": ["Jedi Enclave", "Courtyard", "Crystal Cave", "Ruins"],
            "Korriban": ["Sith Academy", "Valley of Dark Lords", "Tomb of Naga Sadow", "Tomb of Tulak Hord"]
        }
        return location_templates.get(planet_name, ["Spaceport", "City Center", "Outskirts"])

    def _generate_planet_resources(self, planet_name: str) -> List[str]:
        return ["credits", "equipment", "information", "allies"]

    def _generate_planet_threats(self, planet_name: str, theme: str) -> List[str]:
        threat_templates = {
            "Taris": ["rakghouls", "sith_patrols", "gangs"],
            "Dantooine": ["kath_hounds", "mandalorian_raiders"],
            "Korriban": ["sith_students", "dark_jedi", "tomb_guardians"]
        }
        return threat_templates.get(planet_name, ["hostile_wildlife", "bandits"])

    def _generate_npc_name(self, role: str) -> str:
        name_templates = {
            "quest_giver": ["Mission Vao", "Carth Onasi", "Bastila Shan"],
            "merchant": ["Janice Nall", "Kebla Yurt", "Eli Gand"],
            "guard": ["Sith Trooper", "Republic Soldier", "Security Officer"],
            "civilian": ["Citizen", "Refugee", "Worker"],
            "informant": ["Informant", "Spy", "Contact"]
        }
        names = name_templates.get(role, ["Unknown NPC"])
        return random.choice(names)

    def _get_random_species(self) -> str:
        species = ["Human", "Twi'lek", "Rodian", "Duros", "Ithorian", "Wookiee"]
        return random.choice(species)

    def _get_npc_faction(self, planet_id: str, role: str) -> str:
        return self._get_planet_faction(planet_id)

    def _get_dialogue_style(self, role: str, theme: str) -> str:
        styles = ["formal", "casual", "aggressive", "friendly", "mysterious"]
        return random.choice(styles)

    def _generate_personality_traits(self, role: str) -> List[str]:
        traits = ["brave", "cautious", "greedy", "helpful", "suspicious", "loyal"]
        return random.sample(traits, 2)

    def _generate_npc_inventory(self, role: str) -> List[str]:
        if role == "merchant":
            return ["medpac", "energy_cell", "blaster", "armor"]
        return ["credits", "personal_item"]

    def _generate_quest_stages(self, planets: List[str], theme: str) -> List[Dict[str, Any]]:
        stages = []
        for i, planet in enumerate(planets):
            stage = {
                "stage_id": f"stage_{i+1}",
                "description": f"Complete objectives on {planet}",
                "objectives": [f"Investigate {planet}", f"Find the artifact on {planet}"],
                "completion_conditions": ["objectives_complete"]
            }
            stages.append(stage)
        return stages

    def _get_faction_alignment(self, faction_name: str) -> str:
        alignment_map = {
            "Republic": "light",
            "Jedi Order": "light",
            "Sith Empire": "dark",
            "Exchange": "dark",
            "Mandalorians": "neutral"
        }
        return alignment_map.get(faction_name, "neutral")

    def export_campaign(self, campaign_data: Dict[str, Any], output_path: str) -> bool:
        """Export campaign data to JSON file"""
        try:
            os.makedirs(os.path.dirname(output_path), exist_ok=True)
            
            with open(output_path, 'w', encoding='utf-8') as f:
                json.dump(campaign_data, f, indent=2, ensure_ascii=False)
            
            print(f"Campaign exported successfully to: {output_path}")
            return True
            
        except Exception as e:
            print(f"Error exporting campaign: {e}")
            return False

def main():
    """Main function"""
    parser = argparse.ArgumentParser(description="Generate KOTOR.ai campaigns")
    parser.add_argument("--name", default="TestCampaign", help="Campaign name")
    parser.add_argument("--planets", nargs="+", default=["Taris", "Dantooine", "Korriban"], 
                       help="Planets to include")
    parser.add_argument("--theme", default="redemption", 
                       choices=["redemption", "fall_to_darkness", "ancient_mysteries", "war_aftermath"],
                       help="Campaign theme")
    parser.add_argument("--output", default="Content/Campaigns/TestCampaign.json", 
                       help="Output file path")
    parser.add_argument("--verbose", action="store_true", help="Verbose output")
    
    args = parser.parse_args()
    
    if args.verbose:
        print("KOTOR.ai Campaign Generator")
        print("=" * 40)
        print(f"Campaign Name: {args.name}")
        print(f"Planets: {', '.join(args.planets)}")
        print(f"Theme: {args.theme}")
        print(f"Output: {args.output}")
        print()
    
    # Generate campaign
    generator = KOTORCampaignGenerator()
    campaign_data = generator.generate_test_campaign(args.name, args.planets, args.theme)
    
    # Export to JSON
    success = generator.export_campaign(campaign_data, args.output)
    
    if success:
        print(f"✅ Campaign '{args.name}' generated successfully!")
        print(f"📁 Saved to: {args.output}")
        print(f"🌍 Planets: {len(campaign_data['planets'])}")
        print(f"👥 NPCs: {len(campaign_data['npcs'])}")
        print(f"📋 Quests: {len(campaign_data['quests'])}")
        print(f"⚔️ Factions: {len(campaign_data['factions'])}")
    else:
        print("❌ Campaign generation failed!")
        sys.exit(1)

if __name__ == "__main__":
    main()
