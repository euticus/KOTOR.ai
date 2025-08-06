"""
Planet Generator Module

Generates detailed planet data including biomes, layouts, and quests
for each world in the campaign.
"""

from dataclasses import dataclass
from typing import List, Optional, Dict
import random
from .story_seed_interpreter import CampaignConfig


@dataclass
class MapLayout:
    """Represents a specific area/map within a planet"""
    name: str
    layout_type: str  # "city", "wilderness", "dungeon", "spaceport", "temple"
    description: str
    estimated_time_hours: float
    key_features: List[str]
    
    def to_dict(self) -> dict:
        return {
            "name": self.name,
            "layout_type": self.layout_type,
            "description": self.description,
            "estimated_time_hours": self.estimated_time_hours,
            "key_features": self.key_features
        }
    
    @classmethod
    def from_dict(cls, data: dict) -> 'MapLayout':
        return cls(**data)


@dataclass
class PlanetData:
    """Complete data for a planet in the campaign"""
    name: str
    biome: str
    climate: str
    population: str
    government: str
    layouts: List[MapLayout]
    main_quest: str
    side_quests: List[str]
    difficulty_tier: str  # "early", "mid", "late"
    planet_index: int
    lore_description: str
    
    def to_dict(self) -> dict:
        return {
            "name": self.name,
            "biome": self.biome,
            "climate": self.climate,
            "population": self.population,
            "government": self.government,
            "layouts": [layout.to_dict() for layout in self.layouts],
            "main_quest": self.main_quest,
            "side_quests": self.side_quests,
            "difficulty_tier": self.difficulty_tier,
            "planet_index": self.planet_index,
            "lore_description": self.lore_description
        }
    
    @classmethod
    def from_dict(cls, data: dict) -> 'PlanetData':
        layouts = [MapLayout.from_dict(layout) for layout in data['layouts']]
        return cls(
            name=data['name'],
            biome=data['biome'],
            climate=data['climate'],
            population=data['population'],
            government=data['government'],
            layouts=layouts,
            main_quest=data['main_quest'],
            side_quests=data['side_quests'],
            difficulty_tier=data['difficulty_tier'],
            planet_index=data['planet_index'],
            lore_description=data['lore_description']
        )


class PlanetGenerator:
    """Generates detailed planet data for campaigns"""
    
    def __init__(self):
        # Planet biome templates
        self.biomes = {
            "desert": {
                "climates": ["arid", "hot", "dry"],
                "features": ["sand dunes", "canyons", "oases", "ancient ruins"],
                "layouts": ["spaceport", "settlement", "caves", "ruins"]
            },
            "forest": {
                "climates": ["temperate", "humid", "mild"],
                "features": ["dense forests", "rivers", "wildlife", "hidden temples"],
                "layouts": ["village", "temple", "wilderness", "canopy city"]
            },
            "ice": {
                "climates": ["frozen", "cold", "harsh"],
                "features": ["glaciers", "ice caves", "frozen lakes", "blizzards"],
                "layouts": ["research station", "ice caves", "frozen city", "bunker"]
            },
            "urban": {
                "climates": ["controlled", "artificial", "varied"],
                "features": ["skyscrapers", "undercity", "traffic", "technology"],
                "layouts": ["upper city", "lower city", "industrial", "government"]
            },
            "ocean": {
                "climates": ["tropical", "humid", "stormy"],
                "features": ["islands", "underwater cities", "coral reefs", "storms"],
                "layouts": ["floating city", "underwater base", "island", "platform"]
            },
            "volcanic": {
                "climates": ["hot", "dangerous", "unstable"],
                "features": ["lava flows", "geysers", "mining operations", "crystals"],
                "layouts": ["mining facility", "lava tubes", "crystal caves", "observatory"]
            }
        }
        
        # Government types
        self.governments = [
            "Republic Democracy", "Imperial Autocracy", "Corporate Oligarchy",
            "Tribal Council", "Military Junta", "Theocracy", "Anarchist Collective",
            "Feudal System", "Trade Federation", "Rebel Alliance"
        ]
        
        # Population sizes
        self.populations = [
            "Uninhabited", "Sparse (Thousands)", "Small (Hundreds of Thousands)",
            "Medium (Millions)", "Large (Billions)", "Massive (Trillions)"
        ]
    
    def generate_planet(self, config: CampaignConfig, planet_index: int,
                       difficulty_tier: str, available_planets: List[str]) -> PlanetData:
        """
        Generate a complete planet for the campaign
        
        Args:
            config: Campaign configuration
            planet_index: Index of this planet in the campaign (0-based)
            difficulty_tier: "early", "mid", or "late" game difficulty
            available_planets: List of canonical Star Wars planet names
            
        Returns:
            Complete PlanetData object
        """
        
        # Select planet name (prefer canonical, fallback to generated)
        if planet_index < len(available_planets):
            planet_name = available_planets[planet_index]
        else:
            planet_name = self._generate_planet_name()
        
        # Select biome based on story themes and progression
        biome = self._select_biome(config, planet_index, difficulty_tier)
        biome_data = self.biomes[biome]
        
        # Generate basic planet properties
        climate = random.choice(biome_data["climates"])
        population = random.choice(self.populations)
        government = random.choice(self.governments)
        
        # Generate layouts (3-6 per planet based on game length)
        target_layouts = min(6, max(3, config.game_length_hours // config.estimated_planets))
        layouts = self._generate_layouts(biome_data, target_layouts, difficulty_tier)
        
        # Generate quests
        main_quest = self._generate_main_quest(config, planet_name, biome, planet_index)
        side_quests = self._generate_side_quests(layouts, difficulty_tier)
        
        # Generate lore description
        lore_description = self._generate_lore_description(
            planet_name, biome, climate, population, government
        )
        
        return PlanetData(
            name=planet_name,
            biome=biome,
            climate=climate,
            population=population,
            government=government,
            layouts=layouts,
            main_quest=main_quest,
            side_quests=side_quests,
            difficulty_tier=difficulty_tier,
            planet_index=planet_index,
            lore_description=lore_description
        )
    
    def _generate_planet_name(self) -> str:
        """Generate a Star Wars-style planet name"""
        prefixes = ["Kor", "Nal", "Zar", "Vel", "Kesh", "Ord", "Dxun", "Mek", "Telos"]
        suffixes = ["iban", "ador", "oth", "aan", "esh", "aris", "ion", "us", "or"]
        
        return random.choice(prefixes) + random.choice(suffixes)
    
    def _select_biome(self, config: CampaignConfig, planet_index: int, 
                     difficulty_tier: str) -> str:
        """Select appropriate biome based on campaign progression"""
        
        # Early game: more hospitable biomes
        if difficulty_tier == "early":
            return random.choice(["urban", "forest", "desert"])
        
        # Mid game: varied biomes
        elif difficulty_tier == "mid":
            return random.choice(["ocean", "forest", "desert", "urban"])
        
        # Late game: more dangerous biomes
        else:
            return random.choice(["volcanic", "ice", "desert", "urban"])
    
    def _generate_layouts(self, biome_data: Dict, target_count: int, 
                         difficulty_tier: str) -> List[MapLayout]:
        """Generate map layouts for the planet"""
        layouts = []
        layout_types = biome_data["layouts"]
        features = biome_data["features"]
        
        # Base time per layout (scales with difficulty)
        base_time = {"early": 1.5, "mid": 2.0, "late": 2.5}[difficulty_tier]
        
        for i in range(target_count):
            layout_type = random.choice(layout_types)
            
            # Generate layout name
            if layout_type == "spaceport":
                name = f"{random.choice(['Central', 'Main', 'Orbital'])} Spaceport"
            elif layout_type == "city" or layout_type == "settlement":
                name = f"{random.choice(['New', 'Old', 'Central'])} {random.choice(['City', 'Settlement', 'District'])}"
            elif layout_type == "temple":
                name = f"{random.choice(['Ancient', 'Forgotten', 'Sacred'])} Temple"
            elif layout_type == "ruins":
                name = f"{random.choice(['Lost', 'Ancient', 'Buried'])} Ruins"
            else:
                name = f"{layout_type.title()} {random.choice(['Alpha', 'Beta', 'Gamma', 'Prime'])}"
            
            # Generate description and features
            layout_features = random.sample(features, min(3, len(features)))
            description = f"A {layout_type} area featuring {', '.join(layout_features[:2])}"
            
            # Vary time based on layout type
            time_multiplier = {"spaceport": 0.8, "city": 1.2, "wilderness": 1.5, 
                             "dungeon": 1.3, "temple": 1.1}.get(layout_type, 1.0)
            
            layout = MapLayout(
                name=name,
                layout_type=layout_type,
                description=description,
                estimated_time_hours=base_time * time_multiplier,
                key_features=layout_features
            )
            layouts.append(layout)
        
        return layouts
    
    def _generate_main_quest(self, config: CampaignConfig, planet_name: str,
                           biome: str, planet_index: int) -> str:
        """Generate the main quest for this planet"""
        
        quest_templates = {
            "early": [
                f"Investigate mysterious signals coming from {planet_name}",
                f"Establish contact with the local government on {planet_name}",
                f"Rescue stranded allies from {planet_name}'s hostile environment"
            ],
            "mid": [
                f"Uncover ancient secrets hidden beneath {planet_name}",
                f"Prevent a civil war from erupting on {planet_name}",
                f"Retrieve a powerful artifact from {planet_name}'s ruins"
            ],
            "late": [
                f"Confront the dark presence corrupting {planet_name}",
                f"Rally the forces of {planet_name} for the final battle",
                f"Unlock the ultimate power hidden within {planet_name}"
            ]
        }
        
        difficulty = "early" if planet_index < 3 else "mid" if planet_index < 7 else "late"
        return random.choice(quest_templates[difficulty])
    
    def _generate_side_quests(self, layouts: List[MapLayout], 
                            difficulty_tier: str) -> List[str]:
        """Generate side quests for the planet"""
        
        quest_count = len(layouts) // 2 + 1  # Roughly 1 side quest per 2 layouts
        
        side_quest_templates = [
            "Help local merchants deal with smuggler problems",
            "Investigate strange creature sightings",
            "Recover stolen goods from bandits",
            "Mediate a dispute between rival factions",
            "Explore abandoned facilities for valuable technology",
            "Rescue missing persons from dangerous areas",
            "Uncover corruption in the local government",
            "Defend settlements from hostile forces"
        ]
        
        return random.sample(side_quest_templates, min(quest_count, len(side_quest_templates)))
    
    def _generate_lore_description(self, name: str, biome: str, climate: str,
                                 population: str, government: str) -> str:
        """Generate rich lore description for the planet"""
        
        return f"""
{name} is a {biome} world with a {climate} climate, home to a {population.lower()} population 
governed by a {government.lower()}. The planet's unique characteristics have shaped both 
its inhabitants and their culture over millennia.

The world's {biome} environment has created distinct challenges and opportunities for those 
who call it home. Ancient ruins and mysterious phenomena suggest a rich history that 
predates current civilization, hinting at secrets waiting to be uncovered by those 
brave enough to explore its depths.
""".strip()


# Example usage and testing
if __name__ == "__main__":
    from .story_seed_interpreter import StorySeedInterpreter
    
    # Test planet generation
    interpreter = StorySeedInterpreter()
    config = interpreter.parse_story_seed("Test 15-hour Jedi adventure")
    
    generator = PlanetGenerator()
    available_planets = ["Tatooine", "Coruscant", "Naboo"]
    
    planet = generator.generate_planet(config, 0, "early", available_planets)
    
    print(f"Generated Planet: {planet.name}")
    print(f"Biome: {planet.biome}")
    print(f"Layouts: {[layout.name for layout in planet.layouts]}")
    print(f"Main Quest: {planet.main_quest}")
    print(f"Side Quests: {planet.side_quests}")
