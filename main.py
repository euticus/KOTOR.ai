#!/usr/bin/env python3
"""
KOTOR.ai AIDM (AI Dynamic Master) - Main CLI Interface

This is the command-line interface for testing and using the AIDM system.
It allows you to generate complete campaigns from story seeds and export them.
"""

import argparse
import json
import sys
import os
from datetime import datetime

# Add the current directory to Python path for imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from aidm import (
    StorySeedInterpreter, CampaignGenerator, 
    MemoryManager, CampaignConfig
)


def print_banner():
    """Print the AIDM banner"""
    banner = """
╔══════════════════════════════════════════════════════════════╗
║                    KOTOR.ai AIDM System                     ║
║              AI Dynamic Master - Campaign Generator         ║
║                                                              ║
║  Generate dynamic Star Wars campaigns with AI-powered       ║
║  planets, NPCs, quests, encounters, loot, and bosses!       ║
╚══════════════════════════════════════════════════════════════╝
"""
    print(banner)


def interactive_campaign_generation():
    """Run interactive campaign generation"""
    print_banner()
    
    print("Welcome to the AIDM Campaign Generator!")
    print("Let's create your custom Star Wars campaign.\n")
    
    # Initialize components
    interpreter = StorySeedInterpreter()
    generator = CampaignGenerator()
    memory_manager = MemoryManager()
    
    # Get campaign configuration
    config = interpreter.create_interactive_config()
    
    print(f"\n{'='*60}")
    print("GENERATING CAMPAIGN...")
    print(f"{'='*60}")
    
    # Generate the campaign
    try:
        campaign = generator.generate_campaign(config)
        
        # Display campaign summary
        print("\n" + "="*60)
        print("CAMPAIGN GENERATED SUCCESSFULLY!")
        print("="*60)
        
        print(f"\n{campaign.campaign_summary}")
        
        print(f"\n{'='*40}")
        print("PLANETS:")
        print("="*40)
        
        for i, planet in enumerate(campaign.planets, 1):
            print(f"\n{i}. {planet.name} ({planet.biome.title()} World)")
            print(f"   Difficulty: {planet.difficulty_tier.title()}")
            print(f"   Government: {planet.government}")
            print(f"   Layouts: {len(planet.layouts)} areas")
            print(f"   Main Quest: {planet.main_quest}")
            print(f"   Side Quests: {len(planet.side_quests)}")
        
        print(f"\n{'='*40}")
        print("FINAL BOSS:")
        print("="*40)
        
        boss = campaign.final_boss
        print(f"\nName: {boss.title} {boss.name}")
        print(f"Species: {boss.species}")
        print(f"Challenge Rating: {boss.challenge_rating}")
        print(f"Motivation: {boss.motivation}")
        print(f"Arena: {boss.arena_description[:100]}...")
        print(f"Legendary Loot: {boss.loot_drop.name}")
        print(f"Battle Mechanics: {len(boss.mechanics)} unique mechanics")
        
        # Ask if user wants to save
        save_choice = input(f"\nWould you like to save this campaign? (y/n): ").lower().strip()
        
        if save_choice == 'y':
            # Generate filename
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            filename = f"campaign_{timestamp}.json"
            
            # Save campaign
            campaign.save_to_file(filename)
            print(f"\nCampaign saved to: {filename}")
            
            # Start memory tracking
            player_id = input("Enter your player name (for memory tracking): ").strip()
            if player_id:
                campaign_id = memory_manager.start_campaign(
                    player_id, 
                    f"Campaign {timestamp}", 
                    config.story_seed
                )
                print(f"Memory tracking started. Campaign ID: {campaign_id}")
        
        print(f"\n{'='*60}")
        print("Campaign generation complete!")
        print("="*60)
        
        return campaign
        
    except Exception as e:
        print(f"\nError generating campaign: {e}")
        return None


def quick_generation(story_seed: str, hours: int = 20):
    """Quick campaign generation from command line arguments"""
    
    print_banner()
    print(f"Quick Generation Mode")
    print(f"Story Seed: {story_seed}")
    print(f"Target Hours: {hours}")
    
    # Initialize components
    interpreter = StorySeedInterpreter()
    generator = CampaignGenerator()
    
    # Create config from story seed
    config = interpreter.parse_story_seed(story_seed)
    config.game_length_hours = hours  # Override with specified hours
    config.estimated_planets = max(3, min(15, hours // 3))
    
    print(f"\nParsed Configuration:")
    print(f"- Game Length: {config.game_length_hours} hours")
    print(f"- Estimated Planets: {config.estimated_planets}")
    print(f"- Time Period: {config.time_period}")
    print(f"- Alignment Focus: {config.alignment_focus}")
    
    print(f"\n{'='*50}")
    print("GENERATING CAMPAIGN...")
    print("="*50)
    
    try:
        campaign = generator.generate_campaign(config)
        
        # Generate filename and save
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"quick_campaign_{timestamp}.json"
        campaign.save_to_file(filename)
        
        print(f"\nCampaign generated and saved to: {filename}")
        print(f"\nCampaign Summary:")
        print(campaign.campaign_summary)
        
        print(f"\nPlanets: {[p.name for p in campaign.planets]}")
        print(f"Final Boss: {campaign.final_boss.title} {campaign.final_boss.name}")
        
        return campaign
        
    except Exception as e:
        print(f"\nError generating campaign: {e}")
        return None


def run_tests():
    """Run the test suite"""
    print_banner()
    print("Running AIDM Test Suite...")
    
    import unittest
    
    # Discover and run tests
    loader = unittest.TestLoader()
    start_dir = 'tests'
    suite = loader.discover(start_dir, pattern='test_*.py')
    
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    if result.wasSuccessful():
        print("\n✅ All tests passed!")
        return True
    else:
        print(f"\n❌ {len(result.failures)} test(s) failed, {len(result.errors)} error(s)")
        return False


def demo_mode():
    """Run a demonstration of the AIDM system"""
    print_banner()
    print("AIDM Demonstration Mode")
    print("="*50)
    
    demo_seeds = [
        "I want a 15-hour light side adventure about rescuing Jedi from Sith captivity",
        "Create a 25-hour dark side campaign where I build a Sith empire",
        "Generate a 20-hour neutral story about a smuggler caught between factions"
    ]
    
    interpreter = StorySeedInterpreter()
    generator = CampaignGenerator()
    
    for i, seed in enumerate(demo_seeds, 1):
        print(f"\nDemo {i}: {seed}")
        print("-" * 60)
        
        config = interpreter.parse_story_seed(seed)
        campaign = generator.generate_campaign(config)
        
        print(f"Generated: {len(campaign.planets)} planets")
        print(f"Final Boss: {campaign.final_boss.name}")
        print(f"Alignment: {config.alignment_focus}")
        print(f"Era: {config.time_period}")
        
        # Save demo campaign
        filename = f"demo_campaign_{i}.json"
        campaign.save_to_file(filename)
        print(f"Saved to: {filename}")


def main():
    """Main entry point"""
    parser = argparse.ArgumentParser(
        description="KOTOR.ai AIDM - AI Dynamic Master Campaign Generator",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python main.py                                    # Interactive mode
  python main.py --quick "Jedi exile story" --hours 20  # Quick generation
  python main.py --test                            # Run tests
  python main.py --demo                            # Run demonstration
        """
    )
    
    parser.add_argument(
        '--quick', 
        type=str, 
        help='Quick generation mode with story seed'
    )
    
    parser.add_argument(
        '--hours', 
        type=int, 
        default=20, 
        help='Target campaign length in hours (default: 20)'
    )
    
    parser.add_argument(
        '--test', 
        action='store_true', 
        help='Run the test suite'
    )
    
    parser.add_argument(
        '--demo', 
        action='store_true', 
        help='Run demonstration mode'
    )
    
    args = parser.parse_args()
    
    try:
        if args.test:
            success = run_tests()
            sys.exit(0 if success else 1)
        
        elif args.demo:
            demo_mode()
        
        elif args.quick:
            campaign = quick_generation(args.quick, args.hours)
            if not campaign:
                sys.exit(1)
        
        else:
            # Interactive mode (default)
            campaign = interactive_campaign_generation()
            if not campaign:
                sys.exit(1)
    
    except KeyboardInterrupt:
        print("\n\nOperation cancelled by user.")
        sys.exit(1)
    
    except Exception as e:
        print(f"\nUnexpected error: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
