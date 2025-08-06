"""
Campaign Generator Module

Generates a master campaign plan from a CampaignConfig, coordinating
all other generators to create a cohesive campaign experience.
"""

from dataclasses import dataclass
from typing import List, Optional
import json
from .story_seed_interpreter import CampaignConfig
from .planet_generator import PlanetGenerator, PlanetData
from .boss_generator import BossGenerator, BossData


@dataclass
class CampaignPlan:
    """Complete campaign plan with all generated content"""
    config: CampaignConfig
    planets: List[PlanetData]
    final_boss: BossData
    main_quest_outline: str
    campaign_summary: str
    
    def to_dict(self) -> dict:
        """Convert to dictionary for JSON serialization"""
        return {
            "config": self.config.to_dict(),
            "planets": [planet.to_dict() for planet in self.planets],
            "final_boss": self.final_boss.to_dict(),
            "main_quest_outline": self.main_quest_outline,
            "campaign_summary": self.campaign_summary
        }
    
    def save_to_file(self, filename: str):
        """Save campaign plan to JSON file"""
        with open(filename, 'w', encoding='utf-8') as f:
            json.dump(self.to_dict(), f, indent=2, ensure_ascii=False)
    
    @classmethod
    def load_from_file(cls, filename: str) -> 'CampaignPlan':
        """Load campaign plan from JSON file"""
        with open(filename, 'r', encoding='utf-8') as f:
            data = json.load(f)
        
        # Reconstruct objects from dictionaries
        config = CampaignConfig(**data['config'])
        planets = [PlanetData.from_dict(p) for p in data['planets']]
        final_boss = BossData.from_dict(data['final_boss'])
        
        return cls(
            config=config,
            planets=planets,
            final_boss=final_boss,
            main_quest_outline=data['main_quest_outline'],
            campaign_summary=data['campaign_summary']
        )


class CampaignGenerator:
    """Orchestrates the generation of complete campaign plans"""
    
    def __init__(self):
        self.planet_generator = PlanetGenerator()
        self.boss_generator = BossGenerator()
        
        # Cache for Wookieepedia data (stub for now)
        self.star_wars_planets = [
            "Tatooine", "Coruscant", "Naboo", "Hoth", "Endor", "Dagobah",
            "Alderaan", "Kashyyyk", "Kamino", "Geonosis", "Mustafar",
            "Yavin 4", "Bespin", "Jakku", "Scarif", "Jedha", "Ryloth",
            "Mon Cala", "Mandalore", "Dathomir", "Korriban", "Tython"
        ]
    
    def generate_campaign(self, config: CampaignConfig) -> CampaignPlan:
        """
        Generate a complete campaign plan from configuration
        
        Args:
            config: Campaign configuration object
            
        Returns:
            Complete CampaignPlan with all generated content
        """
        print(f"Generating campaign: {config.story_seed}")
        print(f"Target: {config.game_length_hours} hours, {config.estimated_planets} planets")
        
        # Generate planets
        planets = self._generate_planets(config)
        
        # Select final boss planet (usually the last one)
        final_planet = planets[-1] if planets else None
        
        # Generate final boss
        final_boss = self.boss_generator.generate_boss(config, final_planet)
        
        # Create main quest outline
        main_quest_outline = self._generate_main_quest_outline(config, planets, final_boss)
        
        # Create campaign summary
        campaign_summary = self._generate_campaign_summary(config, planets, final_boss)
        
        return CampaignPlan(
            config=config,
            planets=planets,
            final_boss=final_boss,
            main_quest_outline=main_quest_outline,
            campaign_summary=campaign_summary
        )
    
    def _generate_planets(self, config: CampaignConfig) -> List[PlanetData]:
        """Generate all planets for the campaign"""
        planets = []
        
        for i in range(config.estimated_planets):
            # Determine planet progression (early, mid, late game)
            if i < config.estimated_planets // 3:
                difficulty_tier = "early"
            elif i < 2 * config.estimated_planets // 3:
                difficulty_tier = "mid"
            else:
                difficulty_tier = "late"
            
            planet = self.planet_generator.generate_planet(
                config=config,
                planet_index=i,
                difficulty_tier=difficulty_tier,
                available_planets=self.star_wars_planets
            )
            planets.append(planet)
        
        return planets
    
    def _generate_main_quest_outline(self, config: CampaignConfig, 
                                   planets: List[PlanetData], 
                                   final_boss: BossData) -> str:
        """Generate the main quest storyline outline"""
        
        # Create a narrative arc based on the story seed and planets
        if config.alignment_focus == "dark":
            quest_template = f"""
MAIN QUEST: {config.story_seed.title()}

ACT I - The Dark Path Begins
- Start on {planets[0].name}: Discover your dark potential
- Learn of {final_boss.name}'s influence across the galaxy
- Begin gathering power and followers

ACT II - Rise to Power  
- Visit {len(planets[1:-2])} worlds to build your dark empire
- Eliminate rivals and claim ancient Sith artifacts
- Uncover the location of {final_boss.name}

ACT III - The Final Confrontation
- Arrive at {planets[-1].name} for the ultimate showdown
- Face {final_boss.name} in {final_boss.arena_description}
- Claim your destiny as the new Dark Lord of the Sith
"""
        elif config.alignment_focus == "light":
            quest_template = f"""
MAIN QUEST: {config.story_seed.title()}

ACT I - The Call to Adventure
- Begin on {planets[0].name}: Discover the threat to the galaxy
- Learn of {final_boss.name}'s dark plans
- Gather allies and begin your heroic journey

ACT II - The Hero's Journey
- Travel to {len(planets[1:-2])} worlds to stop the darkness
- Save civilizations and gain powerful allies
- Uncover {final_boss.name}'s weakness

ACT III - The Light Triumphant
- Confront {final_boss.name} on {planets[-1].name}
- Rally your allies for the final battle in {final_boss.arena_description}
- Restore peace and balance to the galaxy
"""
        else:  # neutral
            quest_template = f"""
MAIN QUEST: {config.story_seed.title()}

ACT I - The Gray Path
- Start on {planets[0].name}: Navigate complex moral choices
- Discover {final_boss.name}'s true motivations
- Choose your own path between light and dark

ACT II - Walking the Line
- Visit {len(planets[1:-2])} worlds making difficult decisions
- Build relationships across all factions
- Prepare for the confrontation with {final_boss.name}

ACT III - Your Destiny
- Face {final_boss.name} on {planets[-1].name}
- Make the ultimate choice in {final_boss.arena_description}
- Shape the galaxy's future according to your vision
"""
        
        return quest_template.strip()
    
    def _generate_campaign_summary(self, config: CampaignConfig,
                                 planets: List[PlanetData],
                                 final_boss: BossData) -> str:
        """Generate a brief campaign summary"""
        
        planet_names = [p.name for p in planets[:3]]
        if len(planets) > 3:
            planet_names.append(f"and {len(planets) - 3} other worlds")
        
        summary = f"""
Campaign: {config.story_seed.title()}
Duration: {config.game_length_hours} hours
Era: {config.time_period.title()}
Alignment: {config.alignment_focus.title()} Side focus

Your journey will take you across {config.estimated_planets} worlds including {', '.join(planet_names)}.
The ultimate confrontation awaits with {final_boss.name}, {final_boss.motivation}.

This campaign emphasizes {config.alignment_focus} side choices and is set during the {config.time_period} era.
"""
        
        return summary.strip()


# Example usage and testing
if __name__ == "__main__":
    from .story_seed_interpreter import StorySeedInterpreter
    
    # Create a test campaign
    interpreter = StorySeedInterpreter()
    config = interpreter.parse_story_seed(
        "I want a 20-hour story about a Jedi exile hunting a Sith relic"
    )
    
    generator = CampaignGenerator()
    campaign = generator.generate_campaign(config)
    
    print("Generated Campaign:")
    print(campaign.campaign_summary)
    print(f"\nPlanets: {[p.name for p in campaign.planets]}")
    print(f"Final Boss: {campaign.final_boss.name}")
