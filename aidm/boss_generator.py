"""
Boss Generator Module

Creates unique final bosses with custom mechanics, lore, and legendary loot drops
tied to the campaign's story seed and themes.
"""

from dataclasses import dataclass
from typing import List, Optional, Dict
import random
from .story_seed_interpreter import CampaignConfig
from .planet_generator import PlanetData
from .loot_generator import LootItem


@dataclass
class BossData:
    """Complete data for a campaign's final boss"""
    name: str
    title: str
    species: str
    motivation: str
    backstory: str
    mechanics: List[str]  # Special battle mechanics
    phases: List[Dict[str, any]]  # Battle phases with different abilities
    arena_description: str
    loot_drop: LootItem
    challenge_rating: float
    hit_points: int
    armor_class: int
    special_abilities: List[str]
    weaknesses: List[str]
    minions: List[str]  # Types of minions they can summon
    
    def to_dict(self) -> dict:
        return {
            "name": self.name,
            "title": self.title,
            "species": self.species,
            "motivation": self.motivation,
            "backstory": self.backstory,
            "mechanics": self.mechanics,
            "phases": self.phases,
            "arena_description": self.arena_description,
            "loot_drop": self.loot_drop.to_dict(),
            "challenge_rating": self.challenge_rating,
            "hit_points": self.hit_points,
            "armor_class": self.armor_class,
            "special_abilities": self.special_abilities,
            "weaknesses": self.weaknesses,
            "minions": self.minions
        }
    
    @classmethod
    def from_dict(cls, data: dict) -> 'BossData':
        loot_drop = LootItem.from_dict(data['loot_drop'])
        return cls(
            name=data['name'],
            title=data['title'],
            species=data['species'],
            motivation=data['motivation'],
            backstory=data['backstory'],
            mechanics=data['mechanics'],
            phases=data['phases'],
            arena_description=data['arena_description'],
            loot_drop=loot_drop,
            challenge_rating=data['challenge_rating'],
            hit_points=data['hit_points'],
            armor_class=data['armor_class'],
            special_abilities=data['special_abilities'],
            weaknesses=data['weaknesses'],
            minions=data['minions']
        )


class BossGenerator:
    """Generates unique final bosses for campaigns"""
    
    def __init__(self):
        # Boss archetypes based on alignment
        self.boss_archetypes = {
            "dark": {
                "titles": ["Dark Lord", "Sith Master", "Shadow Emperor", "Fallen One", "Destroyer"],
                "motivations": [
                    "seeks ultimate power through ancient Sith artifacts",
                    "wants to corrupt the galaxy with dark side energy",
                    "plans to rule through fear and domination",
                    "desires revenge against those who wronged them",
                    "aims to unlock forbidden Sith knowledge"
                ],
                "species": ["Human", "Zabrak", "Chiss", "Miraluka", "Sith Pureblood"],
                "abilities": ["Force Lightning", "Force Choke", "Dark Rage", "Sith Sorcery", "Mind Control"]
            },
            "light": {
                "titles": ["Fallen Jedi", "Corrupted Master", "Lost Guardian", "Broken Knight", "Twisted Sage"],
                "motivations": [
                    "believes the galaxy must be saved through extreme measures",
                    "thinks they alone can bring true peace",
                    "was corrupted while trying to do good",
                    "seeks to prevent a greater evil through lesser evils",
                    "wants to reshape the Force itself"
                ],
                "species": ["Human", "Twi'lek", "Togruta", "Miraluka", "Cathar"],
                "abilities": ["Force Push", "Healing Corruption", "Light Bind", "Purifying Fire", "Mind Shield"]
            },
            "neutral": {
                "titles": ["Gray Master", "Balance Keeper", "Void Walker", "Neutral Lord", "Equilibrium"],
                "motivations": [
                    "seeks to maintain balance between light and dark",
                    "wants to eliminate both Jedi and Sith",
                    "plans to create a new Force philosophy",
                    "aims to transcend traditional Force limitations",
                    "desires to reset galactic civilization"
                ],
                "species": ["Human", "Zabrak", "Nautolan", "Chiss", "Miraluka"],
                "abilities": ["Force Balance", "Void Touch", "Neutral Ground", "Phase Shift", "Reality Warp"]
            }
        }
        
        # Battle mechanics templates
        self.battle_mechanics = {
            "phase_transition": "Boss gains new abilities and changes tactics at 75%, 50%, and 25% health",
            "environmental_hazard": "Arena features dangerous environmental elements that activate periodically",
            "minion_summoning": "Boss summons waves of minions throughout the fight",
            "force_powers": "Boss uses powerful Force abilities that affect the entire battlefield",
            "weapon_mastery": "Boss demonstrates mastery of multiple weapon types during combat",
            "defensive_stance": "Boss alternates between aggressive and defensive phases",
            "area_denial": "Boss creates zones of danger that players must avoid",
            "mind_games": "Boss uses psychological warfare and illusions",
            "power_absorption": "Boss can absorb and redirect player abilities",
            "berserker_rage": "Boss becomes more dangerous as health decreases"
        }
        
        # Arena types
        self.arena_types = {
            "sith_temple": "A dark temple filled with Sith artifacts and dangerous energy",
            "jedi_ruins": "Ancient Jedi ruins with crumbling architecture and Force echoes",
            "space_station": "A massive space station with artificial gravity and energy barriers",
            "volcanic_chamber": "A chamber within an active volcano with lava flows and unstable ground",
            "crystal_cave": "A cave filled with Force-sensitive crystals that amplify abilities",
            "throne_room": "An elaborate throne room with multiple levels and hidden passages",
            "starship_bridge": "The bridge of a massive starship with control panels and viewports",
            "ancient_arena": "A gladiatorial arena with mechanical traps and obstacles",
            "force_nexus": "A location where the Force is unusually strong and unpredictable",
            "orbital_platform": "A platform in space with artificial atmosphere and gravity"
        }
    
    def generate_boss(self, config: CampaignConfig, 
                     final_planet: Optional[PlanetData] = None) -> BossData:
        """
        Generate a unique final boss for the campaign
        
        Args:
            config: Campaign configuration
            final_planet: The planet where the final battle takes place
            
        Returns:
            Complete boss data
        """
        
        # Determine boss alignment (usually opposite of player focus)
        boss_alignment = self._determine_boss_alignment(config.alignment_focus)
        archetype = self.boss_archetypes[boss_alignment]
        
        # Generate basic boss properties
        name = self._generate_boss_name(config.story_seed)
        title = random.choice(archetype["titles"])
        species = random.choice(archetype["species"])
        motivation = random.choice(archetype["motivations"])
        
        # Generate backstory tied to story seed
        backstory = self._generate_backstory(name, title, species, motivation, config)
        
        # Generate battle mechanics (2-3 unique mechanics)
        mechanics = random.sample(list(self.battle_mechanics.keys()), 3)
        mechanic_descriptions = [self.battle_mechanics[mech] for mech in mechanics]
        
        # Generate battle phases
        phases = self._generate_battle_phases(archetype, mechanics)
        
        # Generate arena
        arena_type = random.choice(list(self.arena_types.keys()))
        arena_description = self._generate_arena_description(arena_type, final_planet)
        
        # Generate stats based on campaign length (longer campaigns = stronger boss)
        challenge_rating = self._calculate_boss_cr(config.game_length_hours)
        hit_points = int(100 + (challenge_rating * 50))
        armor_class = int(18 + (challenge_rating * 0.5))
        
        # Generate abilities and weaknesses
        special_abilities = self._generate_special_abilities(archetype, challenge_rating)
        weaknesses = self._generate_weaknesses(boss_alignment, mechanics)
        minions = self._generate_minion_types(boss_alignment, final_planet)
        
        # Generate legendary loot drop
        loot_drop = self._generate_legendary_loot(name, title, boss_alignment, config)
        
        return BossData(
            name=name,
            title=title,
            species=species,
            motivation=motivation,
            backstory=backstory,
            mechanics=mechanic_descriptions,
            phases=phases,
            arena_description=arena_description,
            loot_drop=loot_drop,
            challenge_rating=challenge_rating,
            hit_points=hit_points,
            armor_class=armor_class,
            special_abilities=special_abilities,
            weaknesses=weaknesses,
            minions=minions
        )
    
    def _determine_boss_alignment(self, player_focus: str) -> str:
        """Determine boss alignment based on player focus"""
        
        if player_focus == "dark":
            # Dark side players face light side or neutral opponents
            return random.choice(["light", "neutral"])
        elif player_focus == "light":
            # Light side players face dark side opponents
            return "dark"
        else:  # neutral
            # Neutral players can face any alignment
            return random.choice(["light", "dark", "neutral"])
    
    def _generate_boss_name(self, story_seed: str) -> str:
        """Generate a boss name, potentially incorporating story seed elements"""
        
        # Extract key words from story seed for inspiration
        seed_words = story_seed.lower().split()
        
        # Star Wars-style name components
        prefixes = ["Darth", "Lord", "Master", "Admiral", "General", "Emperor", "Overlord"]
        
        # Generate base name
        name_parts = [
            ["Mal", "Dar", "Vel", "Kor", "Zar", "Nex", "Vex", "Keth", "Mor", "Sav"],
            ["ak", "us", "an", "or", "el", "ix", "on", "ar", "ek", "oth"]
        ]
        
        base_name = random.choice(name_parts[0]) + random.choice(name_parts[1])
        
        # Sometimes add a title prefix
        if random.random() < 0.7:
            prefix = random.choice(prefixes)
            return f"{prefix} {base_name}"
        else:
            return base_name
    
    def _generate_backstory(self, name: str, title: str, species: str,
                          motivation: str, config: CampaignConfig) -> str:
        """Generate detailed backstory for the boss"""
        
        backstory_template = f"""
{name}, known as {title}, is a {species} who {motivation}. 

Born during the {config.time_period} era, {name} was once a promising individual who 
showed great potential in the Force. However, their path took a dark turn when they 
discovered ancient secrets that changed their perspective on the galaxy.

Their current plan involves {motivation.lower()}, which directly threatens the stability 
of the galaxy. The events described in "{config.story_seed}" are merely the beginning 
of their grand design.

{name} has spent years gathering followers and resources, preparing for this moment when 
they can finally implement their vision for the galaxy's future. They see themselves 
not as a villain, but as a necessary force for change in a corrupt and stagnant galaxy.
"""
        
        return backstory_template.strip()
    
    def _generate_battle_phases(self, archetype: Dict, mechanics: List[str]) -> List[Dict[str, any]]:
        """Generate battle phases with different abilities and tactics"""
        
        phases = []
        
        # Phase 1: 100-75% health
        phase1 = {
            "name": "Opening Assault",
            "health_range": "100-75%",
            "abilities": random.sample(archetype["abilities"], 2),
            "tactics": "Aggressive attacks to test the player's strength",
            "special_effects": "None"
        }
        phases.append(phase1)
        
        # Phase 2: 75-50% health
        phase2 = {
            "name": "Escalation",
            "health_range": "75-50%",
            "abilities": random.sample(archetype["abilities"], 3),
            "tactics": "Introduces environmental hazards and summons minions",
            "special_effects": "Arena becomes more dangerous"
        }
        phases.append(phase2)
        
        # Phase 3: 50-25% health
        phase3 = {
            "name": "Desperation",
            "health_range": "50-25%",
            "abilities": archetype["abilities"][:4],
            "tactics": "Uses most powerful abilities and becomes more unpredictable",
            "special_effects": "All mechanics active simultaneously"
        }
        phases.append(phase3)
        
        # Phase 4: 25-0% health
        phase4 = {
            "name": "Final Stand",
            "health_range": "25-0%",
            "abilities": archetype["abilities"],
            "tactics": "Unleashes full power in a last desperate attempt",
            "special_effects": "Boss gains new ultimate abilities"
        }
        phases.append(phase4)
        
        return phases
    
    def _generate_arena_description(self, arena_type: str, 
                                  final_planet: Optional[PlanetData]) -> str:
        """Generate detailed arena description"""
        
        base_description = self.arena_types[arena_type]
        
        if final_planet:
            location_detail = f" Located on {final_planet.name}, this {arena_type} reflects the planet's {final_planet.biome} environment."
        else:
            location_detail = f" This {arena_type} serves as the perfect backdrop for an epic confrontation."
        
        environmental_features = {
            "sith_temple": "Dark energy pulses through the walls, and ancient Sith statues watch the battle",
            "jedi_ruins": "Broken lightsaber crystals litter the ground, still glowing faintly with residual energy",
            "space_station": "Stars wheel past the massive viewports as the station slowly rotates",
            "volcanic_chamber": "Lava bubbles and flows create natural barriers and hazards",
            "crystal_cave": "Force-sensitive crystals resonate with the combatants' emotions",
            "throne_room": "Elaborate architecture provides multiple levels and tactical advantages",
            "starship_bridge": "Control panels spark and systems fail as the battle intensifies",
            "ancient_arena": "Mechanical traps activate randomly, adding chaos to the fight",
            "force_nexus": "The Force itself seems alive here, amplifying all abilities",
            "orbital_platform": "The artificial gravity can be manipulated during the battle"
        }
        
        feature_detail = environmental_features.get(arena_type, "The environment itself becomes part of the battle")
        
        return f"{base_description}{location_detail} {feature_detail}."
    
    def _calculate_boss_cr(self, game_length_hours: int) -> float:
        """Calculate boss challenge rating based on campaign length"""
        
        # Longer campaigns should have more powerful bosses
        base_cr = 8.0  # Minimum boss CR
        length_bonus = (game_length_hours - 10) * 0.2  # +0.2 CR per hour over 10
        
        return max(base_cr, min(15.0, base_cr + length_bonus))
    
    def _generate_special_abilities(self, archetype: Dict, cr: float) -> List[str]:
        """Generate special abilities based on archetype and CR"""
        
        base_abilities = archetype["abilities"].copy()
        
        # Add CR-based abilities
        if cr >= 10.0:
            base_abilities.extend(["Legendary Actions", "Legendary Resistance"])
        if cr >= 12.0:
            base_abilities.extend(["Lair Actions", "Regional Effects"])
        if cr >= 14.0:
            base_abilities.append("Reality Manipulation")
        
        return base_abilities
    
    def _generate_weaknesses(self, alignment: str, mechanics: List[str]) -> List[str]:
        """Generate boss weaknesses for strategic gameplay"""
        
        alignment_weaknesses = {
            "dark": ["Light side Force powers", "Emotional connections", "Overconfidence"],
            "light": ["Dark side corruption", "Moral dilemmas", "Protecting innocents"],
            "neutral": ["Extreme emotions", "Unbalanced tactics", "Commitment to ideals"]
        }
        
        base_weaknesses = alignment_weaknesses[alignment]
        
        # Add mechanic-specific weaknesses
        mechanic_weaknesses = {
            "environmental_hazard": "Can be turned against the boss",
            "minion_summoning": "Vulnerable while summoning",
            "force_powers": "Force exhaustion after major abilities",
            "phase_transition": "Brief vulnerability during transitions"
        }
        
        for mechanic in mechanics:
            if mechanic in mechanic_weaknesses:
                base_weaknesses.append(mechanic_weaknesses[mechanic])
        
        return base_weaknesses
    
    def _generate_minion_types(self, alignment: str, 
                             final_planet: Optional[PlanetData]) -> List[str]:
        """Generate types of minions the boss can summon"""
        
        alignment_minions = {
            "dark": ["Sith Assassins", "Dark Jedi", "Corrupted Beasts", "Shadow Troopers"],
            "light": ["Fallen Jedi", "Corrupted Guardians", "Light Spirits", "Purified Droids"],
            "neutral": ["Gray Guards", "Balanced Constructs", "Void Wraiths", "Neutral Sentinels"]
        }
        
        minions = alignment_minions[alignment].copy()
        
        # Add planet-specific minions if available
        if final_planet:
            if final_planet.biome == "volcanic":
                minions.append("Lava Elementals")
            elif final_planet.biome == "ice":
                minions.append("Ice Wraiths")
            elif final_planet.biome == "forest":
                minions.append("Corrupted Wildlife")
            elif final_planet.biome == "urban":
                minions.append("Security Droids")
        
        return minions
    
    def _generate_legendary_loot(self, boss_name: str, boss_title: str,
                               alignment: str, config: CampaignConfig) -> LootItem:
        """Generate a legendary item dropped by the boss"""
        
        # Create a unique legendary item based on the boss
        item_name = f"{boss_name}'s Legacy"
        
        # Determine item type based on alignment
        if alignment == "dark":
            item_type = "weapon"
            base_bonuses = {"attack": 15, "force_power": 10, "dark_side": 5}
        elif alignment == "light":
            item_type = "armor"
            base_bonuses = {"defense": 12, "force_power": 8, "light_side": 5}
        else:  # neutral
            item_type = "mod"
            base_bonuses = {"balance": 10, "force_power": 12, "versatility": 8}
        
        flavor_text = f"The final remnant of {boss_title} {boss_name}'s power. This legendary {item_type} pulses with the Force energy of its former owner, granting incredible abilities to those worthy enough to wield it."
        
        # Legendary items are very valuable
        value = 50000 + (config.game_length_hours * 1000)
        
        return LootItem(
            name=item_name,
            rarity="legendary",
            item_type=item_type,
            bonuses=base_bonuses,
            flavor_text=flavor_text,
            value_credits=value,
            level_requirement=config.game_length_hours,
            faction_theme=alignment
        )


# Example usage and testing
if __name__ == "__main__":
    from .story_seed_interpreter import StorySeedInterpreter
    from .planet_generator import PlanetGenerator
    
    # Test boss generation
    interpreter = StorySeedInterpreter()
    config = interpreter.parse_story_seed("I want a 25-hour story about hunting a Sith relic")
    
    planet_gen = PlanetGenerator()
    final_planet = planet_gen.generate_planet(config, 5, "late", ["Korriban"])
    
    boss_gen = BossGenerator()
    boss = boss_gen.generate_boss(config, final_planet)
    
    print(f"Generated Boss: {boss.title} {boss.name}")
    print(f"Species: {boss.species}")
    print(f"Motivation: {boss.motivation}")
    print(f"Challenge Rating: {boss.challenge_rating}")
    print(f"Legendary Loot: {boss.loot_drop.name}")
    print(f"Arena: {boss.arena_description[:100]}...")
    print(f"Mechanics: {boss.mechanics[:2]}")
