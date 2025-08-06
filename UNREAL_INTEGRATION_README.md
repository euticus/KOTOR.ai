# KOTOR.ai - Unreal Engine 5 Integration

This document describes the Unreal Engine 5 integration for the AIDM (AI Dynamic Master) system.

## 🎯 Phase 2 Complete - Unreal Engine Integration

The AIDM system has been successfully integrated into Unreal Engine 5, providing a complete pipeline from Python campaign generation to playable Unreal Engine experiences.

## 📁 Project Structure

```
Source/KOTOR_Clone/
├── Public/
│   ├── AIDM/
│   │   ├── CampaignLoaderSubsystem.h     # Campaign JSON loading
│   │   ├── AIDirectorComponent.h         # Runtime content management
│   │   └── QuestManagerComponent.h       # Quest state management
│   ├── UI/
│   │   └── DialogueWidget.h              # NPC dialogue system
│   ├── Debug/
│   │   └── AIDMDebugWidget.h             # Development debug panel
│   └── Player/
│       └── AIDMPlayerCharacter.h         # Enhanced player character
├── Private/
│   ├── AIDM/
│   │   ├── CampaignLoaderSubsystem.cpp
│   │   ├── AIDirectorComponent.cpp
│   │   └── QuestManagerComponent.cpp
│   ├── UI/
│   │   └── DialogueWidget.cpp
│   ├── Debug/
│   │   └── AIDMDebugWidget.cpp
│   └── Player/
│       └── AIDMPlayerCharacter.cpp
└── KOTOR_Clone.Build.cs                 # Module dependencies

Content/
└── Campaigns/
    └── test_campaign.json               # Example campaign file
```

## 🔧 Core Components

### 1. Campaign Loader Subsystem
- **File**: `CampaignLoaderSubsystem.h/cpp`
- **Purpose**: Loads and deserializes AIDM JSON campaigns into Unreal Engine
- **Features**:
  - JSON parsing for campaign data
  - Planet, NPC, enemy, and boss data structures
  - Blueprint-accessible campaign information

### 2. AI Director Component
- **File**: `AIDirectorComponent.h/cpp`
- **Purpose**: Runtime agent for dynamic content spawning and management
- **Features**:
  - Dynamic NPC and enemy spawning
  - Spawn point management
  - Planet and layout transitions
  - Blueprint events for custom spawning logic

### 3. Quest Manager Component
- **File**: `QuestManagerComponent.h/cpp`
- **Purpose**: Manages quest state and progression
- **Features**:
  - Quest objective tracking
  - Save/load quest data
  - Quest completion events
  - Blueprint integration

### 4. Enhanced Player Character
- **File**: `AIDMPlayerCharacter.h/cpp`
- **Purpose**: Player character with AIDM integration
- **Features**:
  - Alignment tracking (Light/Dark/Neutral)
  - Experience and leveling system
  - NPC interaction system
  - Debug panel integration

### 5. UI System
- **File**: `DialogueWidget.h/cpp`
- **Purpose**: NPC dialogue and quest interfaces
- **Features**:
  - Dynamic dialogue generation
  - Quest offering system
  - Alignment-based dialogue options

### 6. Debug Tools
- **File**: `AIDMDebugWidget.h/cpp`
- **Purpose**: Development tools for AIDM monitoring
- **Features**:
  - Real-time campaign monitoring
  - Planet/layout switching
  - Quest debugging
  - Content spawning controls

## 🚀 Getting Started

### Prerequisites
- Unreal Engine 5.0+
- AIDM Python system (Phase 1)
- Generated campaign JSON files

### Setup Instructions

1. **Copy Source Files**
   ```
   Copy all files from Source/KOTOR_Clone/ to your UE5 project's Source directory
   ```

2. **Update Build Configuration**
   ```
   Add KOTOR_Clone module to your .uproject file:
   "Modules": [
     {
       "Name": "KOTOR_Clone",
       "Type": "Runtime"
     }
   ]
   ```

3. **Generate Project Files**
   ```
   Right-click your .uproject file and select "Generate Visual Studio project files"
   ```

4. **Compile Project**
   ```
   Open in Visual Studio and build the project
   ```

### Basic Usage

1. **Create Campaign**
   ```bash
   # Generate campaign using AIDM Python system
   python main.py --quick "Test UE5 integration" --hours 15
   ```

2. **Copy Campaign File**
   ```
   Copy generated JSON to Content/Campaigns/ directory
   ```

3. **Setup Player Character**
   ```cpp
   // In Blueprint or C++
   AIDMPlayerCharacter* Player = Cast<AIDMPlayerCharacter>(GetPawn());
   Player->LoadAndStartCampaign("Campaigns/test_campaign.json");
   ```

4. **Register Spawn Points**
   ```cpp
   // Create spawn points in your level
   FSpawnPointData SpawnPoint;
   SpawnPoint.Location = FVector(100, 200, 300);
   SpawnPoint.SpawnType = "NPC";
   SpawnPoint.LayoutName = "Anchorhead Spaceport";
   
   AIDirector->RegisterSpawnPoint(SpawnPoint);
   ```

## 🎮 Gameplay Features

### Campaign Loading
- Load AIDM-generated campaigns at runtime
- Automatic planet and layout progression
- Dynamic content scaling based on campaign data

### Dynamic Content Spawning
- NPCs spawn based on campaign data
- Enemies scale to planet difficulty
- Loot generation from AIDM system
- Quest objectives appear dynamically

### Player Progression
- Experience and leveling system
- Light/Dark side alignment tracking
- Quest completion rewards
- Persistent player stats

### Interactive Dialogue
- Dynamic NPC conversations
- Alignment-based dialogue options
- Quest offering through dialogue
- Branching conversation trees

### Quest System
- Objective tracking and completion
- Multiple quest types (fetch, kill, escort, etc.)
- Quest log UI
- Save/load quest progress

## 🛠 Development Tools

### Debug Panel (F1)
- Real-time campaign monitoring
- Planet/layout switching
- Content spawning controls
- Quest debugging tools

### Console Commands
```
// Load campaign
load_campaign Campaigns/my_campaign.json

// Change planet
planet 2

// Change layout
layout "Jedi Temple"

// Spawn content
spawn_content

// Start test quest
start_quest
```

### Blueprint Integration
All AIDM components are fully Blueprint-accessible:
- Campaign data structures
- Event delegates
- Spawning functions
- Quest management
- Player stats

## 📊 Data Flow

```
AIDM Python System → JSON Campaign File → Campaign Loader Subsystem → AI Director Component → Dynamic Content Spawning
                                                                    ↓
Player Character ← UI Widgets ← Quest Manager Component ← Quest Data
```

## 🔧 Customization

### Custom NPC Spawning
```cpp
// Override in Blueprint
APawn* OnSpawnNPC_Implementation(const FNPCData& NPCData, const FSpawnPointData& SpawnPoint)
{
    // Custom NPC spawning logic
    return SpawnedNPC;
}
```

### Custom Dialogue Options
```cpp
// Override in Blueprint
TArray<FDialogueOption> GenerateCustomDialogueOptions_Implementation(const FNPCData& NPCData)
{
    // Custom dialogue generation
    return CustomOptions;
}
```

### Custom Quest Types
```cpp
// Override quest objective creation
void CreateDefaultObjectives(FActiveQuest& Quest)
{
    // Custom objective logic based on quest type
}
```

## 🧪 Testing

### Test Campaign
A test campaign is included at `Content/Campaigns/test_campaign.json` with:
- 3 planets (Tatooine, Coruscant, Korriban)
- Multiple layouts per planet
- Sample NPCs and quests
- Final boss encounter

### Debug Features
- F1: Toggle debug panel
- J: Toggle quest log
- E: Interact with NPCs
- Real-time spawn point visualization
- Campaign data monitoring

## 🚀 Performance Considerations

### Optimization Features
- Spawn point pooling
- Dynamic content loading/unloading
- Quest data caching
- Efficient JSON parsing
- Memory management for large campaigns

### Scalability
- Supports campaigns with 3-15 planets
- Handles 100+ NPCs per campaign
- Manages 1000+ loot items
- Scales from 10-50 hour campaigns

## 🔮 Future Enhancements

### Planned Features
- 3D world generation from planet data
- Advanced AI behavior trees for NPCs
- Real-time campaign adaptation
- Multiplayer campaign support
- VR integration
- Advanced dialogue system with voice acting

### Integration Opportunities
- Save game system integration
- Achievement system
- Analytics and telemetry
- Mod support
- Steam Workshop integration

## 📄 License

This Unreal Engine integration is part of the KOTOR.ai project and follows the same licensing terms as the main project.

---

**The Force is strong with this integration!** 🌟

*Ready for Phase 3: Advanced AI and Real-time Adaptation*
