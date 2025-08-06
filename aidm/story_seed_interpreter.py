"""
Story Seed Interpreter Module

Converts user input like "I want a 20-hour story about a Jedi exile hunting a Sith relic"
into a structured CampaignConfig object.
"""

from dataclasses import dataclass
import re
from typing import Optional


@dataclass
class CampaignConfig:
    """Configuration object for campaign generation"""
    game_length_hours: int
    estimated_planets: int
    story_seed: str
    time_period: str
    alignment_focus: str  # "light", "dark", "neutral"
    
    def to_dict(self) -> dict:
        """Convert to dictionary for JSON serialization"""
        return {
            "game_length_hours": self.game_length_hours,
            "estimated_planets": self.estimated_planets,
            "story_seed": self.story_seed,
            "time_period": self.time_period,
            "alignment_focus": self.alignment_focus
        }


class StorySeedInterpreter:
    """Interprets natural language story seeds into structured campaign configs"""
    
    def __init__(self):
        # Keywords for detecting alignment focus
        self.light_keywords = ["jedi", "republic", "light", "hero", "save", "protect", "peace"]
        self.dark_keywords = ["sith", "empire", "dark", "revenge", "power", "dominate", "destroy"]
        
        # Keywords for time periods
        self.time_periods = {
            "old republic": ["old republic", "kotor", "revan", "malak"],
            "clone wars": ["clone wars", "anakin", "obi-wan", "separatist"],
            "galactic civil war": ["empire", "rebel", "luke", "vader", "death star"],
            "new republic": ["new republic", "first order", "resistance", "kylo"]
        }
    
    def parse_story_seed(self, story_seed: str) -> CampaignConfig:
        """
        Parse a natural language story seed into a CampaignConfig
        
        Args:
            story_seed: Natural language description of desired campaign
            
        Returns:
            CampaignConfig object with parsed parameters
        """
        story_seed = story_seed.lower().strip()
        
        # Extract game length
        game_length = self._extract_game_length(story_seed)
        
        # Calculate estimated planets (roughly 1 planet per 3-4 hours)
        estimated_planets = max(3, min(15, game_length // 3))
        
        # Determine time period
        time_period = self._determine_time_period(story_seed)
        
        # Determine alignment focus
        alignment_focus = self._determine_alignment_focus(story_seed)
        
        return CampaignConfig(
            game_length_hours=game_length,
            estimated_planets=estimated_planets,
            story_seed=story_seed,
            time_period=time_period,
            alignment_focus=alignment_focus
        )
    
    def _extract_game_length(self, story_seed: str) -> int:
        """Extract game length from story seed, default to 20 hours"""
        # Look for patterns like "20 hour", "30-hour", "15 hours"
        hour_patterns = [
            r'(\d+)[-\s]*hours?',
            r'(\d+)[-\s]*hr',
            r'(\d+)h\b'
        ]
        
        for pattern in hour_patterns:
            match = re.search(pattern, story_seed)
            if match:
                return int(match.group(1))
        
        # Default to 20 hours if not specified
        return 20
    
    def _determine_time_period(self, story_seed: str) -> str:
        """Determine the Star Wars time period from story context"""
        for period, keywords in self.time_periods.items():
            if any(keyword in story_seed for keyword in keywords):
                return period
        
        # Default to Old Republic era (KOTOR setting)
        return "old republic"
    
    def _determine_alignment_focus(self, story_seed: str) -> str:
        """Determine the moral alignment focus of the campaign"""
        light_score = sum(1 for keyword in self.light_keywords if keyword in story_seed)
        dark_score = sum(1 for keyword in self.dark_keywords if keyword in story_seed)

        # If there's a clear winner, use it
        if dark_score > light_score:
            return "dark"
        elif light_score > dark_score:
            return "light"
        # If tied but both have keywords, prefer light (heroic default)
        elif light_score > 0 and dark_score > 0:
            return "light"
        # If tied at zero or equal, check for specific patterns
        elif light_score == dark_score:
            # Check for specific light-side patterns
            if any(word in story_seed for word in ["vs empire", "rebel", "rescue", "help"]):
                return "light"
            # Default to neutral only if no clear indicators
            return "neutral"
        else:
            return "neutral"
    
    def create_interactive_config(self) -> CampaignConfig:
        """Create a campaign config through interactive CLI prompts"""
        print("=== KOTOR.ai Campaign Generator ===")
        print("Let's create your custom Star Wars campaign!\n")
        
        # Get story seed
        story_seed = input("Describe your campaign idea: ")
        
        # Parse the seed
        config = self.parse_story_seed(story_seed)
        
        # Allow user to modify parsed values
        print(f"\nParsed Configuration:")
        print(f"Game Length: {config.game_length_hours} hours")
        print(f"Estimated Planets: {config.estimated_planets}")
        print(f"Time Period: {config.time_period}")
        print(f"Alignment Focus: {config.alignment_focus}")
        
        modify = input("\nWould you like to modify any values? (y/n): ").lower().strip()
        
        if modify == 'y':
            # Allow modifications
            new_length = input(f"Game length ({config.game_length_hours}): ").strip()
            if new_length.isdigit():
                config.game_length_hours = int(new_length)
                config.estimated_planets = max(3, min(15, config.game_length_hours // 3))
            
            new_period = input(f"Time period ({config.time_period}): ").strip()
            if new_period:
                config.time_period = new_period
            
            new_alignment = input(f"Alignment focus ({config.alignment_focus}): ").strip()
            if new_alignment in ["light", "dark", "neutral"]:
                config.alignment_focus = new_alignment
        
        return config


# Example usage and testing
if __name__ == "__main__":
    interpreter = StorySeedInterpreter()
    
    # Test with example story seeds
    test_seeds = [
        "I want a 20-hour story about a Jedi exile hunting a Sith relic",
        "Create a 30 hour dark side campaign where I play as a Sith apprentice",
        "15-hour adventure during the Clone Wars helping the Republic"
    ]
    
    for seed in test_seeds:
        print(f"\nTesting: {seed}")
        config = interpreter.parse_story_seed(seed)
        print(f"Result: {config}")
