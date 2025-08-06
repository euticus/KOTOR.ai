Perfect — now that **Phase 1** sets up the AIDM world-generation pipeline, we can move to **Phase 2: Unreal Engine Integration**.

> **Goal:** Create a robust Unreal playground where the AIDM-generated campaign can be visualized, navigated, and interacted with.

---

# 🚀 KOTOR.ai Task Pipeline – Phase 2: Unreal Engine Sandbox + Runtime Hooks

---

## 🌍 T-11: UE5 Sandbox World Setup

**Objective:** Create a modular test world that can simulate AIDM campaigns.

**Tasks for Augment:**

* [ ] Create a new persistent level called `AIDM_TestWorld`.
* [ ] Build 3 example "map layouts":

  * `CityLayout_A`
  * `TempleLayout_B`
  * `SpaceportLayout_C`
* [ ] Each map should support:

  * 10–20 spawn points for NPCs/enemies
  * Blueprint markers for item drops, quest triggers, etc.
* [ ] Use streaming levels or World Partition for scalable loading

---

## 📦 T-12: CampaignPlan Loader (C++)

**Objective:** Load a `CampaignPlan.json` file exported by AIDM into Unreal.

**Tasks for Augment:**

* [ ] Create C++ class: `UCampaignLoaderSubsystem : UGameInstanceSubsystem`
* [ ] Implement JSON deserialization (use `FJsonObjectConverter`)
* [ ] Map:

  * `PlanetData` → Level list
  * `NPCData` → Spawned character actors
  * `EnemyData` → AI character classes
  * `LootItem` → Item pickups
* [ ] Expose `LoadCampaign(FString JsonFilePath)` to Blueprint

---

## 🧠 T-13: AIDirectorComponent (Runtime Agent)

**Objective:** Build an in-engine representation of the AI Director.

**Tasks for Augment:**

* [ ] Create `UAIDirectorComponent` (attachable to player or singleton actor)
* [ ] Responsibilities:

  * Parse campaign state
  * Spawn content dynamically as player enters zones
  * Trigger cutscenes or scripted events
* [ ] Add Blueprint-exposed functions:

  * `RequestNewSideQuest()`
  * `SpawnEnemyEncounter()`
  * `GetPlanetOverview()`
  * `GetNextMainQuestObjective()`

---

## 🧍 T-14: Dynamic NPC & Enemy Spawner

**Objective:** Procedurally spawn content as players enter layout areas.

**Tasks for Augment:**

* [ ] Create `ANPCSpawner` and `AEnemySpawner` actors
* [ ] Use `OnActorBeginOverlap` with volumes to trigger spawns
* [ ] Pull data from `CampaignLoaderSubsystem`
* [ ] Set NPC properties dynamically (faction, dialogue, quest availability)
* [ ] Adjust enemy CR based on party level (simple scalar for now)

---

## 💬 T-15: Dialogue & Quest Widget System

**Objective:** Display LLM-generated quest and dialogue text in a UI.

**Tasks for Augment:**

* [ ] Create `WBP_DialogueWidget`:

  * NPC name
  * Branching choices (support for 1–5 responses)
  * Quest acceptance prompt
* [ ] Create `WBP_QuestLogWidget`:

  * Track main + side quests
  * Quest objectives + completion
* [ ] Hook into NPC interaction (keypress E, raytrace or overlap trigger)

---

## 🧠 T-16: QuestManagerComponent (C++)

**Objective:** Manage quest state, flags, and story-state across levels.

**Tasks for Augment:**

* [ ] Create `UQuestManagerComponent`
* [ ] Track:

  * Accepted quests
  * Completed quests
  * Active objectives
  * Flags for global decision state
* [ ] Interface with:

  * `DialogueWidget` (to accept quests)
  * `AIDirectorComponent` (to update prompts or future quests)
  * `EnemySpawner` (to trigger ambushes, objectives, etc.)

---

## 🎮 T-17: PlayerCharacter Upgrade

**Objective:** Extend the player blueprint/class to support interaction with AIDM content.

**Tasks for Augment:**

* [ ] Add `UQuestManagerComponent`
* [ ] Add companion slots for future `UCompanionManagerComponent`
* [ ] Add interaction interface for `INPCInteractable`
* [ ] Display floating nameplates above NPCs or quest givers

---

## 🧪 T-18: Runtime Debug Panel (Dev Only)

**Objective:** Debug AIDM generation & decisions in real-time.

**Tasks for Augment:**

* [ ] Create `WBP_AIDM_DebugPanel`
* [ ] Display:

  * Current quest node
  * Player alignment
  * Current planet name
  * Next planned event
* [ ] Add dev hotkey (e.g., F10) to toggle visibility

---

## 📜 T-19: Import & Play a Campaign

**Objective:** Test real AIDM-generated JSON inside the UE sandbox.

**Tasks for Augment:**

* [ ] Export test JSON campaign from AIDM (Phase 1)
* [ ] Place it in `Content/Data/Campaigns/`
* [ ] Call `LoadCampaign()` on BeginPlay of `AIDM_TestWorld`
* [ ] Walk through layout:

  * Talk to generated NPCs
  * Fight spawned enemies
  * Accept and complete quests

---

## 🎬 T-20: Final Boss Arena Loader

**Objective:** Load the final boss arena and spawn the boss from JSON.

**Tasks for Augment:**

* [ ] Create level: `FinalBoss_Arena`
* [ ] Load `BossData` from `CampaignPlan.final_boss`
* [ ] Spawn custom boss actor with:

  * Unique mesh/material
  * Boss intro dialogue
  * Battle music
  * Trigger cutscene or fight phase intro
* [ ] Drop boss loot upon defeat and trigger end-game sequence

---

## ✅ Phase 2 Goal

> Build an Unreal testbed that:

* Loads a full campaign
* Spawns modular layouts
* Displays LLM-generated NPCs, quests, dialogue, and loot
* Supports procedural combat, loot drops, and progression
* Prepares for end-to-end cinematic and boss generation

---