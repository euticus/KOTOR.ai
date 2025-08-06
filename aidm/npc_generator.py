"""
NPC Generator Module

Creates named NPCs with bios, alignment, roles, and optional quests
for populating the campaign world.
"""

from dataclasses import dataclass
from typing import List, Optional, Dict
import random
from .story_seed_interpreter import CampaignConfig
from .planet_generator import PlanetData, MapLayout


@dataclass
class QuestData:
    """Represents a quest that can be given by an NPC"""
    title: str
    description: str
    quest_type: str  # "fetch", "kill", "escort", "investigate", "diplomacy"
    reward_type: str  # "credits", "item", "information", "reputation"
    difficulty: str  # "easy", "medium", "hard"
    estimated_time_minutes: int
    
    def to_dict(self) -> dict:
        return {
            "title": self.title,
            "description": self.description,
            "quest_type": self.quest_type,
            "reward_type": self.reward_type,
            "difficulty": self.difficulty,
            "estimated_time_minutes": self.estimated_time_minutes
        }
    
    @classmethod
    def from_dict(cls, data: dict) -> 'QuestData':
        return cls(**data)


@dataclass
class NPCData:
    """Complete data for a named NPC"""
    name: str
    species: str
    faction: str
    alignment: str  # "light", "dark", "neutral"
    likes_player: bool
    role: str  # "vendor", "questgiver", "follower", "guard", "civilian", "enemy"
    backstory: str
    personality_traits: List[str]
    location: str  # Which layout they're found in
    quest: Optional[QuestData]
    dialogue_style: str
    reputation_standing: int  # -100 to 100, how much they like/trust the player initially
    
    def to_dict(self) -> dict:
        return {
            "name": self.name,
            "species": self.species,
            "faction": self.faction,
            "alignment": self.alignment,
            "likes_player": self.likes_player,
            "role": self.role,
            "backstory": self.backstory,
            "personality_traits": self.personality_traits,
            "location": self.location,
            "quest": self.quest.to_dict() if self.quest else None,
            "dialogue_style": self.dialogue_style,
            "reputation_standing": self.reputation_standing
        }
    
    @classmethod
    def from_dict(cls, data: dict) -> 'NPCData':
        quest = QuestData.from_dict(data['quest']) if data['quest'] else None
        return cls(
            name=data['name'],
            species=data['species'],
            faction=data['faction'],
            alignment=data['alignment'],
            likes_player=data['likes_player'],
            role=data['role'],
            backstory=data['backstory'],
            personality_traits=data['personality_traits'],
            location=data['location'],
            quest=quest,
            dialogue_style=data['dialogue_style'],
            reputation_standing=data['reputation_standing']
        )


class NPCGenerator:
    """Generates NPCs for campaign worlds"""
    
    def __init__(self):
        # Star Wars species
        self.species = [
            "Human", "Twi'lek", "Rodian", "Wookiee", "Zabrak", "Togruta",
            "Nautolan", "Miraluka", "Cathar", "Mandalorian", "Chiss",
            "Bothan", "Mon Calamari", "Sullustan", "Duros", "Ithorian"
        ]
        
        # Factions
        self.factions = [
            "Republic", "Sith Empire", "Jedi Order", "Mandalorians",
            "Exchange", "Czerka Corporation", "Local Government", "Rebels",
            "Pirates", "Merchants Guild", "Independent", "Criminals"
        ]
        
        # Personality traits
        self.personality_traits = [
            "Brave", "Cowardly", "Greedy", "Generous", "Suspicious", "Trusting",
            "Aggressive", "Peaceful", "Ambitious", "Content", "Wise", "Foolish",
            "Loyal", "Treacherous", "Honest", "Deceptive", "Proud", "Humble"
        ]
        
        # Dialogue styles
        self.dialogue_styles = [
            "Formal", "Casual", "Aggressive", "Friendly", "Mysterious",
            "Sarcastic", "Nervous", "Confident", "Wise", "Childlike"
        ]
        
        # Name components for generating Star Wars-style names
        self.name_prefixes = [
            "Kor", "Vel", "Zar", "Mal", "Dar", "Jek", "Nex", "Kira", "Mira",
            "Bastila", "Jolee", "Canderous", "Mission", "Carth", "HK", "T3"
        ]
        
        self.name_suffixes = [
            "an", "ek", "us", "ia", "or", "ar", "el", "on", "ak", "shan",
            "47", "M4", "R2", "C3", "BB", "K2"
        ]
    
    def generate_npcs_for_planet(self, planet: PlanetData, 
                                config: CampaignConfig) -> List[NPCData]:
        """
        Generate all NPCs for a specific planet
        
        Args:
            planet: Planet data to generate NPCs for
            config: Campaign configuration
            
        Returns:
            List of generated NPCs
        """
        npcs = []
        
        # Generate 2-4 NPCs per layout
        for layout in planet.layouts:
            npc_count = random.randint(2, 4)
            
            for i in range(npc_count):
                npc = self._generate_single_npc(layout, planet, config)
                npcs.append(npc)
        
        # Ensure at least one quest giver per planet
        quest_givers = [npc for npc in npcs if npc.role == "questgiver"]
        if not quest_givers and npcs:
            # Convert a random NPC to a quest giver
            random_npc = random.choice(npcs)
            random_npc.role = "questgiver"
            random_npc.quest = self._generate_quest(layout, planet.difficulty_tier)
        
        return npcs
    
    def _generate_single_npc(self, layout: MapLayout, planet: PlanetData,
                           config: CampaignConfig) -> NPCData:
        """Generate a single NPC for a specific layout"""
        
        # Generate basic properties
        name = self._generate_name()
        species = random.choice(self.species)
        faction = self._select_faction(config, planet)
        alignment = self._select_alignment(config, faction)
        
        # Determine role based on layout type
        role = self._select_role(layout.layout_type)
        
        # Generate personality
        personality_traits = random.sample(self.personality_traits, 2)
        dialogue_style = random.choice(self.dialogue_styles)
        
        # Determine initial player relationship
        likes_player = self._determine_initial_relationship(alignment, config.alignment_focus)
        reputation_standing = self._calculate_initial_reputation(likes_player, alignment, config.alignment_focus)
        
        # Generate backstory
        backstory = self._generate_backstory(name, species, faction, role, planet.name)
        
        # Generate quest if appropriate
        quest = None
        if role == "questgiver" or (role in ["vendor", "civilian"] and random.random() < 0.3):
            quest = self._generate_quest(layout, planet.difficulty_tier)
        
        return NPCData(
            name=name,
            species=species,
            faction=faction,
            alignment=alignment,
            likes_player=likes_player,
            role=role,
            backstory=backstory,
            personality_traits=personality_traits,
            location=layout.name,
            quest=quest,
            dialogue_style=dialogue_style,
            reputation_standing=reputation_standing
        )
    
    def _generate_name(self) -> str:
        """Generate a Star Wars-style name"""
        if random.random() < 0.3:  # 30% chance for droid designation
            return f"{random.choice(['HK', 'T3', 'R2', 'C3', 'BB'])}-{random.randint(10, 99)}"
        else:
            return random.choice(self.name_prefixes) + random.choice(self.name_suffixes)
    
    def _select_faction(self, config: CampaignConfig, planet: PlanetData) -> str:
        """Select appropriate faction based on campaign and planet"""
        
        # Weight factions based on campaign alignment and time period
        if config.time_period == "old republic":
            weighted_factions = ["Republic", "Sith Empire", "Jedi Order", "Exchange", "Independent"]
        elif config.time_period == "galactic civil war":
            weighted_factions = ["Rebel Alliance", "Galactic Empire", "Independent", "Criminals"]
        else:
            weighted_factions = self.factions
        
        return random.choice(weighted_factions)
    
    def _select_alignment(self, config: CampaignConfig, faction: str) -> str:
        """Select alignment based on faction and campaign focus"""
        
        # Faction-based alignment tendencies
        faction_alignments = {
            "Jedi Order": "light",
            "Sith Empire": "dark",
            "Republic": "light",
            "Galactic Empire": "dark",
            "Rebels": "light",
            "Pirates": "dark",
            "Criminals": "dark"
        }
        
        if faction in faction_alignments:
            # 70% chance to match faction alignment
            if random.random() < 0.7:
                return faction_alignments[faction]
        
        # Otherwise random with slight bias toward campaign focus
        alignments = ["light", "neutral", "dark"]
        if config.alignment_focus != "neutral":
            alignments.append(config.alignment_focus)  # Add extra weight
        
        return random.choice(alignments)
    
    def _select_role(self, layout_type: str) -> str:
        """Select NPC role based on layout type"""
        
        role_weights = {
            "spaceport": ["vendor", "guard", "civilian", "questgiver"],
            "city": ["vendor", "civilian", "questgiver", "guard"],
            "settlement": ["civilian", "questgiver", "vendor"],
            "temple": ["guard", "questgiver", "civilian"],
            "ruins": ["enemy", "questgiver"],
            "wilderness": ["enemy", "civilian"],
            "dungeon": ["enemy", "guard"],
            "caves": ["enemy", "questgiver"]
        }
        
        possible_roles = role_weights.get(layout_type, ["civilian", "vendor", "questgiver"])
        return random.choice(possible_roles)
    
    def _determine_initial_relationship(self, alignment: str, player_focus: str) -> bool:
        """Determine if NPC initially likes the player"""
        
        if alignment == player_focus:
            return random.random() < 0.7  # 70% chance if aligned
        elif alignment == "neutral":
            return random.random() < 0.5  # 50% chance if neutral
        else:
            return random.random() < 0.2  # 20% chance if opposed
    
    def _calculate_initial_reputation(self, likes_player: bool, 
                                    alignment: str, player_focus: str) -> int:
        """Calculate initial reputation standing (-100 to 100)"""
        
        base = 0
        if likes_player:
            base = random.randint(20, 60)
        else:
            base = random.randint(-60, 20)
        
        # Adjust based on alignment compatibility
        if alignment == player_focus:
            base += random.randint(0, 20)
        elif alignment != "neutral" and alignment != player_focus:
            base -= random.randint(0, 30)
        
        return max(-100, min(100, base))
    
    def _generate_backstory(self, name: str, species: str, faction: str,
                          role: str, planet_name: str) -> str:
        """Generate a brief backstory for the NPC"""
        
        backstory_templates = {
            "vendor": f"{name} is a {species} merchant who has been trading on {planet_name} for years. "
                     f"As a member of the {faction}, they have built a reputation for fair deals.",
            
            "questgiver": f"{name} is a {species} who has lived on {planet_name} their entire life. "
                         f"Their connection to the {faction} has given them insight into local problems.",
            
            "guard": f"{name} is a {species} security officer working for the {faction}. "
                    f"They take their duty to protect {planet_name} very seriously.",
            
            "civilian": f"{name} is a {species} resident of {planet_name}. "
                       f"Though not officially part of the {faction}, they support their cause.",
            
            "enemy": f"{name} is a {species} operative working against local interests. "
                    f"Their allegiance to the {faction} puts them at odds with most inhabitants.",
            
            "follower": f"{name} is a {species} who has been searching for purpose. "
                       f"Their past with the {faction} has prepared them for adventure."
        }
        
        return backstory_templates.get(role, f"{name} is a {species} on {planet_name}.")
    
    def _generate_quest(self, layout: MapLayout, difficulty_tier: str) -> QuestData:
        """Generate a quest for an NPC to give"""
        
        quest_templates = {
            "fetch": [
                "Retrieve stolen supplies from bandits",
                "Collect rare materials from dangerous areas",
                "Recover lost family heirloom"
            ],
            "kill": [
                "Eliminate dangerous creatures threatening the area",
                "Stop criminal leader causing trouble",
                "Defeat corrupted droids"
            ],
            "escort": [
                "Safely escort merchant caravan",
                "Protect VIP during dangerous journey",
                "Guide refugees to safety"
            ],
            "investigate": [
                "Investigate mysterious disappearances",
                "Uncover corruption in local government",
                "Discover source of strange signals"
            ],
            "diplomacy": [
                "Negotiate peace between rival factions",
                "Convince leader to change policy",
                "Mediate trade dispute"
            ]
        }
        
        quest_type = random.choice(list(quest_templates.keys()))
        title = random.choice(quest_templates[quest_type])
        
        # Adjust difficulty and time based on tier
        difficulty_map = {"early": "easy", "mid": "medium", "late": "hard"}
        difficulty = difficulty_map.get(difficulty_tier, "medium")
        
        time_map = {"easy": 30, "medium": 45, "hard": 60}
        base_time = time_map[difficulty]
        estimated_time = base_time + random.randint(-10, 20)
        
        # Generate description
        description = f"A {difficulty} {quest_type} quest in the {layout.name} area. {title}."
        
        # Select reward type
        reward_types = ["credits", "item", "information", "reputation"]
        reward_type = random.choice(reward_types)
        
        return QuestData(
            title=title,
            description=description,
            quest_type=quest_type,
            reward_type=reward_type,
            difficulty=difficulty,
            estimated_time_minutes=estimated_time
        )


# Example usage and testing
if __name__ == "__main__":
    from .story_seed_interpreter import StorySeedInterpreter
    from .planet_generator import PlanetGenerator
    
    # Test NPC generation
    interpreter = StorySeedInterpreter()
    config = interpreter.parse_story_seed("Test 15-hour Jedi adventure")
    
    planet_gen = PlanetGenerator()
    planet = planet_gen.generate_planet(config, 0, "early", ["Tatooine"])
    
    npc_gen = NPCGenerator()
    npcs = npc_gen.generate_npcs_for_planet(planet, config)
    
    print(f"Generated {len(npcs)} NPCs for {planet.name}:")
    for npc in npcs[:3]:  # Show first 3
        print(f"- {npc.name} ({npc.species} {npc.role}) in {npc.location}")
        if npc.quest:
            print(f"  Quest: {npc.quest.title}")
