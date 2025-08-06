#!/usr/bin/env python3
"""
KOTOR.ai - Master Development Launcher
Copyright Epic Games, Inc. All Rights Reserved.

This script provides a one-click solution to generate campaigns and launch the game.
"""

import os
import sys
import subprocess
import argparse
import random
import time
from pathlib import Path

class KOTORLauncher:
    """Master launcher for KOTOR.ai development"""
    
    def __init__(self):
        self.script_dir = Path(__file__).parent
        self.project_root = self.script_dir.parent
        self.campaigns_dir = self.project_root / "Content" / "Campaigns"
        
        # Available planets
        self.available_planets = [
            "Taris", "Dantooine", "Tatooine", "Kashyyyk", "Manaan", 
            "Korriban", "Leviathan", "Star Forge", "Endar Spire", "Unknown World"
        ]
        
        # Available themes
        self.available_themes = [
            "redemption", "fall_to_darkness", "ancient_mysteries", "war_aftermath",
            "force_awakening", "identity_crisis", "galactic_conflict", "lost_civilization"
        ]
        
        # Available modes
        self.available_modes = [
            "testing", "development", "demo", "full"
        ]

    def generate_campaign(self, name: str, planets: list, theme: str, output_path: str) -> bool:
        """Generate a campaign using the campaign generator"""
        print(f"🎮 Generating campaign: {name}")
        print(f"🌍 Planets: {', '.join(planets)}")
        print(f"🎭 Theme: {theme}")
        print()
        
        try:
            # Run the campaign generator
            cmd = [
                sys.executable,
                str(self.script_dir / "generate_campaign.py"),
                "--name", name,
                "--planets"] + planets + [
                "--theme", theme,
                "--output", output_path,
                "--verbose"
            ]
            
            result = subprocess.run(cmd, capture_output=True, text=True)
            
            if result.returncode == 0:
                print("✅ Campaign generation completed successfully!")
                return True
            else:
                print("❌ Campaign generation failed!")
                print(f"Error: {result.stderr}")
                return False
                
        except Exception as e:
            print(f"❌ Error running campaign generator: {e}")
            return False

    def launch_game(self, mode: str, planet: str = "", campaign_file: str = "", 
                   use_fast_build: bool = True, launch_editor: bool = True) -> bool:
        """Launch the game using the batch script"""
        print(f"🚀 Launching KOTOR.ai...")
        print(f"🎯 Mode: {mode}")
        if planet:
            print(f"🌍 Starting Planet: {planet}")
        if campaign_file:
            print(f"📋 Campaign: {campaign_file}")
        print()
        
        try:
            # Build the command
            cmd = [str(self.script_dir / "run_kotor_game.bat")]
            
            if use_fast_build:
                cmd.append("--fast")
            
            if launch_editor:
                cmd.append("--editor")
            else:
                cmd.append("--game")
            
            if campaign_file:
                cmd.extend(["--campaign", campaign_file])
            
            if planet:
                cmd.extend(["--planet", planet])
            
            # Add mode-specific flags
            if mode == "testing":
                cmd.append("--live-coding")
            elif mode == "development":
                cmd.append("--live-coding")
                cmd.append("--verbose")
            elif mode == "demo":
                cmd.append("--skip-build")
            
            # Run the batch script
            result = subprocess.run(cmd, shell=True)
            
            if result.returncode == 0:
                print("✅ Game launched successfully!")
                return True
            else:
                print("❌ Game launch failed!")
                return False
                
        except Exception as e:
            print(f"❌ Error launching game: {e}")
            return False

    def get_random_planet(self) -> str:
        """Get a random planet for testing"""
        return random.choice(self.available_planets)

    def get_random_theme(self) -> str:
        """Get a random theme for testing"""
        return random.choice(self.available_themes)

    def get_random_planets(self, count: int = 3) -> list:
        """Get random planets for testing"""
        return random.sample(self.available_planets, min(count, len(self.available_planets)))

    def create_quick_test_campaign(self) -> tuple:
        """Create a quick test campaign with random settings"""
        name = f"QuickTest_{int(time.time())}"
        planets = self.get_random_planets(3)
        theme = self.get_random_theme()
        output_path = str(self.campaigns_dir / f"{name}.json")
        
        return name, planets, theme, output_path

    def list_existing_campaigns(self) -> list:
        """List existing campaign files"""
        if not self.campaigns_dir.exists():
            return []
        
        campaigns = []
        for file_path in self.campaigns_dir.glob("*.json"):
            campaigns.append(file_path.name)
        
        return campaigns

    def interactive_mode(self):
        """Run in interactive mode"""
        print("🎮 KOTOR.ai Interactive Launcher")
        print("=" * 50)
        print()
        
        # Ask user what they want to do
        print("What would you like to do?")
        print("1. Generate new campaign and launch")
        print("2. Launch with existing campaign")
        print("3. Quick test (random campaign)")
        print("4. Launch without campaign")
        print("5. List existing campaigns")
        print()
        
        choice = input("Enter your choice (1-5): ").strip()
        
        if choice == "1":
            self._interactive_new_campaign()
        elif choice == "2":
            self._interactive_existing_campaign()
        elif choice == "3":
            self._interactive_quick_test()
        elif choice == "4":
            self._interactive_no_campaign()
        elif choice == "5":
            self._list_campaigns()
        else:
            print("Invalid choice. Exiting.")

    def _interactive_new_campaign(self):
        """Interactive new campaign creation"""
        print("\n📋 Creating New Campaign")
        print("-" * 30)
        
        name = input("Campaign name (default: TestCampaign): ").strip() or "TestCampaign"
        
        print(f"\nAvailable planets: {', '.join(self.available_planets)}")
        planets_input = input("Enter planets (space-separated, default: Taris Dantooine Korriban): ").strip()
        planets = planets_input.split() if planets_input else ["Taris", "Dantooine", "Korriban"]
        
        print(f"\nAvailable themes: {', '.join(self.available_themes)}")
        theme = input("Enter theme (default: redemption): ").strip() or "redemption"
        
        output_path = str(self.campaigns_dir / f"{name}.json")
        
        # Generate campaign
        if self.generate_campaign(name, planets, theme, output_path):
            # Launch game
            planet = planets[0] if planets else ""
            self.launch_game("development", planet, output_path)

    def _interactive_existing_campaign(self):
        """Interactive existing campaign selection"""
        campaigns = self.list_existing_campaigns()
        
        if not campaigns:
            print("\n❌ No existing campaigns found.")
            return
        
        print("\n📋 Existing Campaigns")
        print("-" * 30)
        for i, campaign in enumerate(campaigns, 1):
            print(f"{i}. {campaign}")
        
        try:
            choice = int(input("\nSelect campaign number: ")) - 1
            if 0 <= choice < len(campaigns):
                campaign_file = str(self.campaigns_dir / campaigns[choice])
                planet = input("Starting planet (optional): ").strip()
                self.launch_game("development", planet, campaign_file)
            else:
                print("Invalid selection.")
        except ValueError:
            print("Invalid input.")

    def _interactive_quick_test(self):
        """Interactive quick test"""
        print("\n⚡ Quick Test Mode")
        print("-" * 20)
        
        name, planets, theme, output_path = self.create_quick_test_campaign()
        
        print(f"Generated random campaign:")
        print(f"  Name: {name}")
        print(f"  Planets: {', '.join(planets)}")
        print(f"  Theme: {theme}")
        print()
        
        if self.generate_campaign(name, planets, theme, output_path):
            self.launch_game("testing", planets[0], output_path)

    def _interactive_no_campaign(self):
        """Interactive no campaign launch"""
        print("\n🚀 Launch Without Campaign")
        print("-" * 30)
        
        mode = input("Mode (testing/development/demo, default: development): ").strip() or "development"
        planet = input("Starting planet (optional): ").strip()
        
        self.launch_game(mode, planet)

    def _list_campaigns(self):
        """List existing campaigns"""
        campaigns = self.list_existing_campaigns()
        
        print("\n📋 Existing Campaigns")
        print("-" * 30)
        
        if campaigns:
            for campaign in campaigns:
                print(f"  📄 {campaign}")
        else:
            print("  No campaigns found.")
        
        print()

def main():
    """Main function"""
    parser = argparse.ArgumentParser(description="KOTOR.ai Master Development Launcher")
    parser.add_argument("--planet", help="Starting planet")
    parser.add_argument("--mode", choices=["testing", "development", "demo", "full"], 
                       default="development", help="Launch mode")
    parser.add_argument("--random", action="store_true", help="Generate random campaign")
    parser.add_argument("--campaign", help="Existing campaign file to use")
    parser.add_argument("--name", default="TestCampaign", help="Campaign name for new campaigns")
    parser.add_argument("--planets", nargs="+", help="Planets for new campaign")
    parser.add_argument("--theme", help="Theme for new campaign")
    parser.add_argument("--fast", action="store_true", help="Use fast build")
    parser.add_argument("--editor", action="store_true", default=True, help="Launch editor (default)")
    parser.add_argument("--game", action="store_true", help="Launch game instead of editor")
    parser.add_argument("--interactive", action="store_true", help="Run in interactive mode")
    parser.add_argument("--list-campaigns", action="store_true", help="List existing campaigns")
    
    args = parser.parse_args()
    
    launcher = KOTORLauncher()
    
    # Handle special modes
    if args.interactive:
        launcher.interactive_mode()
        return
    
    if args.list_campaigns:
        launcher._list_campaigns()
        return
    
    print("🎮 KOTOR.ai Master Launcher")
    print("=" * 40)
    print()
    
    campaign_file = ""
    planet = args.planet or ""
    
    # Handle campaign generation
    if args.random:
        print("🎲 Generating random campaign...")
        name, planets, theme, output_path = launcher.create_quick_test_campaign()
        if launcher.generate_campaign(name, planets, theme, output_path):
            campaign_file = output_path
            planet = planet or planets[0]
    elif args.campaign:
        campaign_file = args.campaign
        print(f"📋 Using existing campaign: {campaign_file}")
    elif args.planets or args.theme:
        # Generate new campaign with specified parameters
        name = args.name
        planets = args.planets or ["Taris", "Dantooine", "Korriban"]
        theme = args.theme or "redemption"
        output_path = str(launcher.campaigns_dir / f"{name}.json")
        
        if launcher.generate_campaign(name, planets, theme, output_path):
            campaign_file = output_path
            planet = planet or planets[0]
    
    # Launch the game
    launch_editor = args.editor and not args.game
    launcher.launch_game(args.mode, planet, campaign_file, args.fast, launch_editor)

if __name__ == "__main__":
    main()
