# KOTOR.ai AIDM (AI Dynamic Master) System

A powerful AI-driven campaign generation system for creating dynamic Star Wars: Knights of the Old Republic style adventures.

## 🌟 Features

- **Story Seed Interpretation**: Convert natural language descriptions into structured campaign configurations
- **Dynamic Planet Generation**: Create diverse worlds with unique biomes, layouts, and quests
- **Intelligent NPC Creation**: Generate named characters with backstories, alignments, and quests
- **Scalable Encounter System**: 100+ enemy types with D&D-style challenge ratings
- **Advanced Loot Generation**: 1000+ possible items with exponential scaling and faction themes
- **Epic Boss Creation**: Unique final bosses with custom mechanics and legendary loot
- **Memory Management**: Track player choices and moral decisions across campaigns
- **CLI Playground**: Interactive command-line interface for testing and generation

## 🚀 Quick Start

### Installation

```bash
# Clone the repository
git clone <repository-url>
cd KOTOR.ai

# No external dependencies required! Uses Python standard library only.
```

### Basic Usage

```bash
# Interactive campaign generation
python main.py

# Quick generation from command line
python main.py --quick "I want a 20-hour Jedi adventure" --hours 20

# Run the test suite
python main.py --test

# See demonstration
python main.py --demo
```

### Example Story Seeds

- "I want a 20-hour story about a Jedi exile hunting a Sith relic"
- "Create a 30-hour dark side campaign where I build a Sith empire"
- "Generate a 15-hour neutral adventure during the Clone Wars"

## 📁 Project Structure

```
KOTOR.ai/
├── aidm/                          # Core AIDM modules
│   ├── __init__.py
│   ├── story_seed_interpreter.py  # Parse story seeds into configs
│   ├── campaign_generator.py      # Orchestrate campaign generation
│   ├── planet_generator.py        # Create planets and layouts
│   ├── npc_generator.py          # Generate NPCs and quests
│   ├── encounter_generator.py     # Create enemies and encounters
│   ├── loot_generator.py         # Generate scalable loot system
│   ├── boss_generator.py         # Create unique final bosses
│   └── memory_manager.py         # Track player choices
├── tests/                         # Unit tests
│   ├── __init__.py
│   ├── test_story_seed_interpreter.py
│   ├── test_campaign_generator.py
│   └── test_loot_generator.py
├── main.py                       # CLI interface
├── requirements.txt              # Dependencies (none for Phase 1!)
└── README.md                     # This file
```

## 🎮 How It Works

### 1. Story Seed Interpretation
```python
from aidm import StorySeedInterpreter

interpreter = StorySeedInterpreter()
config = interpreter.parse_story_seed("20-hour Jedi adventure")
# Automatically detects: game length, alignment focus, time period, planet count
```

### 2. Campaign Generation
```python
from aidm import CampaignGenerator

generator = CampaignGenerator()
campaign = generator.generate_campaign(config)
# Creates: planets, NPCs, quests, encounters, loot, final boss
```

### 3. Export and Save
```python
# Save campaign to JSON
campaign.save_to_file("my_campaign.json")

# Load campaign later
campaign = CampaignPlan.load_from_file("my_campaign.json")
```

## 🧪 Testing

Run the comprehensive test suite:

```bash
python main.py --test
```

Individual module testing:
```bash
python -m unittest tests.test_story_seed_interpreter
python -m unittest tests.test_campaign_generator
python -m unittest tests.test_loot_generator
```

## 📊 Generated Content Scale

- **Planets**: 3-15 per campaign (based on length)
- **NPCs**: 2-4 per planet layout (dozens per campaign)
- **Enemies**: 100+ base templates, scaled per planet
- **Loot Items**: 1000+ possible combinations
- **Quests**: Main quest + multiple side quests per planet
- **Layouts**: 3-6 areas per planet

## 🎯 Alignment System

The system supports three moral alignments:

- **Light Side**: Jedi, Republic, heroic choices
- **Dark Side**: Sith, Empire, power-focused choices  
- **Neutral**: Gray Jedi, smugglers, pragmatic choices

Campaigns adapt content based on player alignment focus.

## 🌌 Time Periods

Supports multiple Star Wars eras:

- **Old Republic**: KOTOR setting (default)
- **Clone Wars**: Prequel era
- **Galactic Civil War**: Original trilogy era
- **New Republic**: Sequel era

## 🔧 Configuration

### Campaign Configuration
```python
@dataclass
class CampaignConfig:
    game_length_hours: int      # Target campaign length
    estimated_planets: int      # Number of worlds to visit
    story_seed: str            # Original story description
    time_period: str           # Star Wars era
    alignment_focus: str       # Moral alignment emphasis
```

### Memory Tracking
```python
from aidm import MemoryManager

memory = MemoryManager()
campaign_id = memory.start_campaign("player_name", "Campaign Name", "story_seed")

# Track player choices
memory.add_memory("moral_choice", "Tatooine", "Spared the Tusken Raiders", 0.8)

# Get context for AI prompts
context = memory.get_context_for_prompt("NPC_Name")
```

## 🚧 Phase 1 Complete ✅

This Phase 1 implementation provides:

- ✅ Complete standalone AIDM system
- ✅ Story seed interpretation
- ✅ Dynamic campaign generation
- ✅ Planet, NPC, encounter, loot, and boss generation
- ✅ Memory management system
- ✅ CLI playground and testing
- ✅ JSON export/import
- ✅ Comprehensive unit tests

## 🔮 Phase 2 Preview

Coming next: Unreal Engine integration!

- C++/Blueprint integration
- Real-time campaign adaptation
- 3D world generation
- Interactive NPC dialogue
- Dynamic quest system
- Memory-driven storytelling

## 🤝 Contributing

This is a Phase 1 foundation. The system is designed to be:

- **Modular**: Each generator can be extended independently
- **Scalable**: Add new content templates easily
- **Testable**: Comprehensive unit test coverage
- **Extensible**: Ready for Phase 2 Unreal integration

## 📄 License

[Add your license here]

## 🙏 Acknowledgments

Built for the KOTOR.ai project - bringing AI-driven dynamic storytelling to the beloved Star Wars universe.

---

*"The Force will be with you... always." - Obi-Wan Kenobi*
