"""
Memory Manager Module

Implements persistent memory system for player choices using vector database
to track NPC interactions, moral decisions, and provide context for continued sessions.
"""

from dataclasses import dataclass, asdict
from typing import List, Dict, Optional, Any
import json
import os
import sqlite3
from datetime import datetime
import hashlib


@dataclass
class MemoryEntry:
    """Represents a single memory entry"""
    entry_id: str
    timestamp: datetime
    entry_type: str  # "npc_interaction", "moral_choice", "quest_completion", "dialogue"
    context: str  # Location, situation context
    content: str  # What actually happened
    emotional_weight: float  # -1.0 to 1.0, how positive/negative this was
    characters_involved: List[str]  # NPCs or entities involved
    consequences: List[str]  # What resulted from this action
    tags: List[str]  # Searchable tags
    campaign_id: str  # Which campaign this belongs to
    
    def to_dict(self) -> dict:
        data = asdict(self)
        data['timestamp'] = self.timestamp.isoformat()
        return data
    
    @classmethod
    def from_dict(cls, data: dict) -> 'MemoryEntry':
        data['timestamp'] = datetime.fromisoformat(data['timestamp'])
        return cls(**data)


@dataclass
class PlayerProfile:
    """Tracks player's moral alignment and decision patterns"""
    player_id: str
    light_side_points: int
    dark_side_points: int
    neutral_points: int
    decision_patterns: Dict[str, int]  # Types of decisions and frequency
    preferred_solutions: List[str]  # How player likes to solve problems
    relationship_tendencies: Dict[str, float]  # How player treats different NPC types
    moral_flexibility: float  # How consistent player is with their choices
    
    def get_alignment(self) -> str:
        """Calculate current alignment based on points"""
        total = self.light_side_points + self.dark_side_points + self.neutral_points
        if total == 0:
            return "neutral"
        
        light_ratio = self.light_side_points / total
        dark_ratio = self.dark_side_points / total
        
        if light_ratio > 0.6:
            return "light"
        elif dark_ratio > 0.6:
            return "dark"
        else:
            return "neutral"
    
    def to_dict(self) -> dict:
        return asdict(self)
    
    @classmethod
    def from_dict(cls, data: dict) -> 'PlayerProfile':
        return cls(**data)


class MemoryManager:
    """Manages persistent memory and player profiling"""
    
    def __init__(self, database_path: str = "aidm_memory.db"):
        self.database_path = database_path
        self.current_campaign_id: Optional[str] = None
        self.current_player_id: Optional[str] = None
        self._init_database()
    
    def _init_database(self):
        """Initialize SQLite database for memory storage"""
        
        with sqlite3.connect(self.database_path) as conn:
            cursor = conn.cursor()
            
            # Create memories table
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS memories (
                    entry_id TEXT PRIMARY KEY,
                    timestamp TEXT NOT NULL,
                    entry_type TEXT NOT NULL,
                    context TEXT NOT NULL,
                    content TEXT NOT NULL,
                    emotional_weight REAL NOT NULL,
                    characters_involved TEXT NOT NULL,
                    consequences TEXT NOT NULL,
                    tags TEXT NOT NULL,
                    campaign_id TEXT NOT NULL
                )
            ''')
            
            # Create player profiles table
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS player_profiles (
                    player_id TEXT PRIMARY KEY,
                    light_side_points INTEGER DEFAULT 0,
                    dark_side_points INTEGER DEFAULT 0,
                    neutral_points INTEGER DEFAULT 0,
                    decision_patterns TEXT DEFAULT '{}',
                    preferred_solutions TEXT DEFAULT '[]',
                    relationship_tendencies TEXT DEFAULT '{}',
                    moral_flexibility REAL DEFAULT 0.5
                )
            ''')
            
            # Create campaigns table
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS campaigns (
                    campaign_id TEXT PRIMARY KEY,
                    player_id TEXT NOT NULL,
                    campaign_name TEXT NOT NULL,
                    created_date TEXT NOT NULL,
                    last_played TEXT NOT NULL,
                    story_seed TEXT NOT NULL,
                    current_status TEXT DEFAULT 'active'
                )
            ''')
            
            conn.commit()
    
    def start_campaign(self, player_id: str, campaign_name: str, story_seed: str) -> str:
        """Start a new campaign and return campaign ID"""
        
        # Generate unique campaign ID
        campaign_data = f"{player_id}_{campaign_name}_{datetime.now().isoformat()}"
        campaign_id = hashlib.md5(campaign_data.encode()).hexdigest()[:12]
        
        self.current_campaign_id = campaign_id
        self.current_player_id = player_id
        
        # Store campaign info
        with sqlite3.connect(self.database_path) as conn:
            cursor = conn.cursor()
            cursor.execute('''
                INSERT INTO campaigns 
                (campaign_id, player_id, campaign_name, created_date, last_played, story_seed)
                VALUES (?, ?, ?, ?, ?, ?)
            ''', (campaign_id, player_id, campaign_name, 
                  datetime.now().isoformat(), datetime.now().isoformat(), story_seed))
            conn.commit()
        
        # Initialize player profile if it doesn't exist
        self._ensure_player_profile(player_id)
        
        return campaign_id
    
    def load_campaign(self, campaign_id: str) -> bool:
        """Load an existing campaign"""
        
        with sqlite3.connect(self.database_path) as conn:
            cursor = conn.cursor()
            cursor.execute('SELECT player_id FROM campaigns WHERE campaign_id = ?', (campaign_id,))
            result = cursor.fetchone()
            
            if result:
                self.current_campaign_id = campaign_id
                self.current_player_id = result[0]
                
                # Update last played
                cursor.execute('''
                    UPDATE campaigns SET last_played = ? WHERE campaign_id = ?
                ''', (datetime.now().isoformat(), campaign_id))
                conn.commit()
                
                return True
            
            return False
    
    def _ensure_player_profile(self, player_id: str):
        """Ensure player profile exists in database"""
        
        with sqlite3.connect(self.database_path) as conn:
            cursor = conn.cursor()
            cursor.execute('SELECT player_id FROM player_profiles WHERE player_id = ?', (player_id,))
            
            if not cursor.fetchone():
                cursor.execute('''
                    INSERT INTO player_profiles (player_id) VALUES (?)
                ''', (player_id,))
                conn.commit()
    
    def add_memory(self, entry_type: str, context: str, content: str,
                   emotional_weight: float = 0.0, characters_involved: List[str] = None,
                   consequences: List[str] = None, tags: List[str] = None) -> str:
        """Add a new memory entry"""
        
        if not self.current_campaign_id:
            raise ValueError("No active campaign. Call start_campaign() or load_campaign() first.")
        
        # Generate unique entry ID
        entry_data = f"{self.current_campaign_id}_{content}_{datetime.now().isoformat()}"
        entry_id = hashlib.md5(entry_data.encode()).hexdigest()[:16]
        
        memory = MemoryEntry(
            entry_id=entry_id,
            timestamp=datetime.now(),
            entry_type=entry_type,
            context=context,
            content=content,
            emotional_weight=emotional_weight,
            characters_involved=characters_involved or [],
            consequences=consequences or [],
            tags=tags or [],
            campaign_id=self.current_campaign_id
        )
        
        # Store in database
        with sqlite3.connect(self.database_path) as conn:
            cursor = conn.cursor()
            cursor.execute('''
                INSERT INTO memories 
                (entry_id, timestamp, entry_type, context, content, emotional_weight,
                 characters_involved, consequences, tags, campaign_id)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
            ''', (
                memory.entry_id, memory.timestamp.isoformat(), memory.entry_type,
                memory.context, memory.content, memory.emotional_weight,
                json.dumps(memory.characters_involved), json.dumps(memory.consequences),
                json.dumps(memory.tags), memory.campaign_id
            ))
            conn.commit()
        
        # Update player profile based on this memory
        self._update_player_profile(memory)
        
        return entry_id
    
    def _update_player_profile(self, memory: MemoryEntry):
        """Update player profile based on new memory"""
        
        if not self.current_player_id:
            return
        
        profile = self.get_player_profile(self.current_player_id)
        
        # Update alignment points based on emotional weight and type
        if memory.entry_type == "moral_choice":
            if memory.emotional_weight > 0.3:
                profile.light_side_points += 1
            elif memory.emotional_weight < -0.3:
                profile.dark_side_points += 1
            else:
                profile.neutral_points += 1
        
        # Track decision patterns
        if memory.entry_type not in profile.decision_patterns:
            profile.decision_patterns[memory.entry_type] = 0
        profile.decision_patterns[memory.entry_type] += 1
        
        # Update relationship tendencies based on character interactions
        for character in memory.characters_involved:
            if character not in profile.relationship_tendencies:
                profile.relationship_tendencies[character] = 0.0
            
            # Adjust relationship based on emotional weight
            profile.relationship_tendencies[character] += memory.emotional_weight * 0.1
            profile.relationship_tendencies[character] = max(-1.0, min(1.0, 
                profile.relationship_tendencies[character]))
        
        # Calculate moral flexibility (how consistent choices are)
        total_choices = sum(profile.decision_patterns.values())
        if total_choices > 5:  # Only calculate after enough data
            alignment_consistency = self._calculate_alignment_consistency(profile)
            profile.moral_flexibility = 1.0 - alignment_consistency
        
        # Save updated profile
        self._save_player_profile(profile)
    
    def _calculate_alignment_consistency(self, profile: PlayerProfile) -> float:
        """Calculate how consistent the player's alignment choices are"""
        
        total_points = profile.light_side_points + profile.dark_side_points + profile.neutral_points
        if total_points == 0:
            return 0.5
        
        # Calculate variance in alignment distribution
        light_ratio = profile.light_side_points / total_points
        dark_ratio = profile.dark_side_points / total_points
        neutral_ratio = profile.neutral_points / total_points
        
        # Higher consistency = one alignment dominates
        max_ratio = max(light_ratio, dark_ratio, neutral_ratio)
        return max_ratio
    
    def get_memories(self, entry_type: Optional[str] = None, 
                    character: Optional[str] = None,
                    tags: Optional[List[str]] = None,
                    limit: int = 50) -> List[MemoryEntry]:
        """Retrieve memories based on filters"""
        
        if not self.current_campaign_id:
            return []
        
        with sqlite3.connect(self.database_path) as conn:
            cursor = conn.cursor()
            
            query = 'SELECT * FROM memories WHERE campaign_id = ?'
            params = [self.current_campaign_id]
            
            if entry_type:
                query += ' AND entry_type = ?'
                params.append(entry_type)
            
            if character:
                query += ' AND characters_involved LIKE ?'
                params.append(f'%"{character}"%')
            
            query += ' ORDER BY timestamp DESC LIMIT ?'
            params.append(limit)
            
            cursor.execute(query, params)
            rows = cursor.fetchall()
            
            memories = []
            for row in rows:
                memory_data = {
                    'entry_id': row[0],
                    'timestamp': row[1],
                    'entry_type': row[2],
                    'context': row[3],
                    'content': row[4],
                    'emotional_weight': row[5],
                    'characters_involved': json.loads(row[6]),
                    'consequences': json.loads(row[7]),
                    'tags': json.loads(row[8]),
                    'campaign_id': row[9]
                }
                memories.append(MemoryEntry.from_dict(memory_data))
            
            return memories
    
    def get_player_profile(self, player_id: str) -> PlayerProfile:
        """Get player profile"""
        
        with sqlite3.connect(self.database_path) as conn:
            cursor = conn.cursor()
            cursor.execute('SELECT * FROM player_profiles WHERE player_id = ?', (player_id,))
            row = cursor.fetchone()
            
            if row:
                return PlayerProfile(
                    player_id=row[0],
                    light_side_points=row[1],
                    dark_side_points=row[2],
                    neutral_points=row[3],
                    decision_patterns=json.loads(row[4]),
                    preferred_solutions=json.loads(row[5]),
                    relationship_tendencies=json.loads(row[6]),
                    moral_flexibility=row[7]
                )
            else:
                # Return default profile
                return PlayerProfile(
                    player_id=player_id,
                    light_side_points=0,
                    dark_side_points=0,
                    neutral_points=0,
                    decision_patterns={},
                    preferred_solutions=[],
                    relationship_tendencies={},
                    moral_flexibility=0.5
                )
    
    def _save_player_profile(self, profile: PlayerProfile):
        """Save player profile to database"""
        
        with sqlite3.connect(self.database_path) as conn:
            cursor = conn.cursor()
            cursor.execute('''
                UPDATE player_profiles SET
                    light_side_points = ?,
                    dark_side_points = ?,
                    neutral_points = ?,
                    decision_patterns = ?,
                    preferred_solutions = ?,
                    relationship_tendencies = ?,
                    moral_flexibility = ?
                WHERE player_id = ?
            ''', (
                profile.light_side_points, profile.dark_side_points, profile.neutral_points,
                json.dumps(profile.decision_patterns), json.dumps(profile.preferred_solutions),
                json.dumps(profile.relationship_tendencies), profile.moral_flexibility,
                profile.player_id
            ))
            conn.commit()
    
    def get_context_for_prompt(self, character_name: Optional[str] = None,
                             recent_count: int = 10) -> str:
        """Generate context string for LLM prompts based on recent memories"""
        
        memories = self.get_memories(character=character_name, limit=recent_count)
        
        if not memories:
            return "No previous interactions recorded."
        
        context_parts = ["Recent campaign events:"]
        
        for memory in memories:
            context_parts.append(f"- {memory.content} (in {memory.context})")
            if memory.consequences:
                context_parts.append(f"  Consequences: {', '.join(memory.consequences)}")
        
        # Add player profile info
        if self.current_player_id:
            profile = self.get_player_profile(self.current_player_id)
            alignment = profile.get_alignment()
            context_parts.append(f"\nPlayer alignment tendency: {alignment}")
            
            if character_name and character_name in profile.relationship_tendencies:
                relationship = profile.relationship_tendencies[character_name]
                if relationship > 0.3:
                    context_parts.append(f"Player has a positive relationship with {character_name}")
                elif relationship < -0.3:
                    context_parts.append(f"Player has a negative relationship with {character_name}")
        
        return "\n".join(context_parts)
    
    def export_campaign_data(self, campaign_id: str, filename: str):
        """Export all campaign data to JSON file"""
        
        memories = self.get_memories()
        
        with sqlite3.connect(self.database_path) as conn:
            cursor = conn.cursor()
            cursor.execute('SELECT * FROM campaigns WHERE campaign_id = ?', (campaign_id,))
            campaign_row = cursor.fetchone()
        
        export_data = {
            "campaign_info": {
                "campaign_id": campaign_row[0],
                "player_id": campaign_row[1],
                "campaign_name": campaign_row[2],
                "created_date": campaign_row[3],
                "last_played": campaign_row[4],
                "story_seed": campaign_row[5],
                "status": campaign_row[6]
            },
            "memories": [memory.to_dict() for memory in memories],
            "player_profile": self.get_player_profile(campaign_row[1]).to_dict()
        }
        
        with open(filename, 'w', encoding='utf-8') as f:
            json.dump(export_data, f, indent=2, ensure_ascii=False)


# Example usage and testing
if __name__ == "__main__":
    # Test memory manager
    memory_manager = MemoryManager("test_memory.db")
    
    # Start a test campaign
    campaign_id = memory_manager.start_campaign("test_player", "Test Campaign", "Test story seed")
    print(f"Started campaign: {campaign_id}")
    
    # Add some test memories
    memory_manager.add_memory(
        "npc_interaction",
        "Tatooine Cantina",
        "Spoke with Twi'lek merchant about rare crystals",
        0.5,
        ["Bib'ala the Merchant"],
        ["Learned location of crystal cave"],
        ["merchant", "crystals", "tatooine"]
    )
    
    memory_manager.add_memory(
        "moral_choice",
        "Crystal Cave",
        "Chose to spare the cave's guardian creature",
        0.8,
        ["Cave Guardian"],
        ["Guardian became ally", "Gained rare crystal"],
        ["moral", "light_side", "creature"]
    )
    
    # Retrieve memories
    memories = memory_manager.get_memories()
    print(f"\nRetrieved {len(memories)} memories")
    
    # Get player profile
    profile = memory_manager.get_player_profile("test_player")
    print(f"Player alignment: {profile.get_alignment()}")
    print(f"Light side points: {profile.light_side_points}")
    
    # Get context for prompts
    context = memory_manager.get_context_for_prompt()
    print(f"\nContext for prompts:\n{context}")
    
    # Clean up test database
    os.remove("test_memory.db")
