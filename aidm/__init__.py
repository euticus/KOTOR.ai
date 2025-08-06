"""
AIDM (AI Dynamic Master) - Core Engine for KOTOR.ai

This module provides the core functionality for generating dynamic campaigns,
planets, NPCs, encounters, loot, and bosses for the KOTOR.ai project.
"""

__version__ = "0.1.0"
__author__ = "KOTOR.ai Development Team"

# Core imports for easy access
from .story_seed_interpreter import StorySeedInterpreter, CampaignConfig
from .campaign_generator import CampaignGenerator, CampaignPlan
from .planet_generator import PlanetGenerator, PlanetData, MapLayout
from .npc_generator import NPCGenerator, NPCData, QuestData
from .encounter_generator import EncounterGenerator, EnemyData
from .loot_generator import LootGenerator, LootItem
from .boss_generator import BossGenerator, BossData
from .memory_manager import MemoryManager

__all__ = [
    "StorySeedInterpreter",
    "CampaignConfig", 
    "CampaignGenerator",
    "CampaignPlan",
    "PlanetGenerator",
    "PlanetData",
    "MapLayout",
    "NPCGenerator", 
    "NPCData",
    "QuestData",
    "EncounterGenerator",
    "EnemyData",
    "LootGenerator",
    "LootItem",
    "BossGenerator",
    "BossData",
    "MemoryManager"
]
