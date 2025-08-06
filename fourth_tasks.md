Excellent. Phase 4 is your *feature expansion arc* — turning KOTOR.ai from a solo RPG into a **dynamic multiplayer D\&D campaign simulator** with **space travel**, **procedural content**, and **cloud-state persistence**.

Let’s go slow and structured.

---

# 🌐 KOTOR.ai Task Pipeline – Phase 4: Multiplayer, Procedural Galaxies & Cloud Infrastructure

---

## 🧑‍🤝‍🧑 T-29: Multiplayer Party Mode

> **Goal:** Let multiple players play in the same campaign session as party members — each making their own choices.

### 🔧 Tasks for Augment:

* [ ] Enable **UE5 listen-server or dedicated server networking**
* [ ] Create `UCampaignSessionManager`:

  * Tracks all active players
  * Syncs current quest, layout, and story flags
  * Shares memory of LLM state across players
* [ ] Modify `PlayerCharacter`:

  * Add `PlayerIndex`, `PlayerName`, and `DecisionWeight`
  * Support dialogue votes (majority rules for dialogue options)
* [ ] Expose a **"Host Campaign"** / **"Join Campaign"** UI
* [ ] Add `AIDirectorComponent` to run on host only and replicate story updates to all clients

### 🧠 Narrative Control Options:

* Allow GM Mode: 1 player acts as AI override (optional)
* Enable party-based consequences (e.g. one player sides with Sith; affects entire story arc)

---

## 🚀 T-30: Space Travel & Combat Layer

> **Goal:** Add a new space layer — galaxy travel, space encounters, and ship upgrades.

### 🔧 Tasks for Augment:

* [ ] Create space scene level `L_GalaxyFlight`
* [ ] Implement `AShipPawn`:

  * Movement, shield, weapons, basic AI enemies
* [ ] Create encounter generator:

  * Random Sith ships, smuggler ambushes, minefields
  * Pull factions from `CampaignPlan`
* [ ] Link space encounter outcomes to story flags:

  * Success → new route, loot
  * Failure → damage, detour, hostage events
* [ ] Add modular **ship upgrades** system:

  * Hull plating, hyperspeed mods, crew slots
  * Buy upgrades from planets, win from bosses

---

## 🌍 T-31: Procedural Planet Generator

> **Goal:** Dynamically create fully custom planets based on biome + faction + LLM flavor

### 🔧 Tasks for Augment:

* [ ] Add `PlanetTemplateLibrary.json`:

  * Templates: `DesertOutpost`, `FungalForest`, `MiningAsteroid`, etc.
* [ ] Create `UProceduralPlanetBuilder`:

  * Input: Biome, Faction, TimePeriod, QuestType
  * Output: `PlanetData` + 3–10 layout blueprints
* [ ] Use modular tiles (Quixel or custom):

  * Auto-place spawners, terrain pieces, foliage, ambient SFX
* [ ] Spawn all map triggers dynamically (dialogue, loot, cutscene)

Optional: Run this in a CLI for headless generation.

---

## ☁️ T-32: Cloud Save + Persistent Campaigns

> **Goal:** Enable player campaigns to be saved, shared, and resumed across devices.

### 🔧 Tasks for Augment:

* [ ] Use Firebase or Supabase to store:

  * Player profiles
  * Serialized `CampaignPlan`
  * SaveGame state (NPC flags, quest progress, loot inventory)
* [ ] Add `UCloudSaveSubsystem`:

  * `UploadSave(FPlayerID)`
  * `DownloadSave(FPlayerID)`
* [ ] Auto-upload after every planet switch or boss fight
* [ ] Add campaign browser:

  * Let player view their past or current campaigns from a UI
  * Share campaign seeds with others

---

## 🧪 T-33: World-State Timeline + Replay System

> **Goal:** Let players view their campaign history and replay key decisions.

### 🔧 Tasks for Augment:

* [ ] Create `UWorldStateLogger`

  * Log: dialogue choices, moral events, quest branches, boss outcomes
* [ ] Add `WBP_TimelineWidget`

  * Visualize: timeline + decision points
  * Allow clicking nodes to read summaries
* [ ] Add Replay Mode:

  * Load planet layout in read-only mode
  * Rewatch cinematics + final boss cutscene

---

## 🔧 T-34: Procedural Quest Generator (On-Demand)

> **Goal:** Generate fresh side quests at any point with AIDM + alignment memory.

### 🔧 Tasks for Augment:

* [ ] Extend `AIDirectorComponent::RequestNewQuest(FQuestRequestParams)`

  * Includes: biome, party alignment, difficulty, faction rep
* [ ] LLM generates:

  * Title, objective, rewards, characters, consequences
* [ ] Trigger via:

  * Dialogue branches
  * Space anomalies
  * Planet ambient events
* [ ] Optional: “Infinite Mode” where campaign never ends, just keeps evolving

---

---

## ✅ Phase 4 Goal

> By the end of Phase 4, KOTOR.ai becomes a **multiplayer, persistent, replayable AI D\&D engine** with:

* Coop party play with unique decisions per player
* Galaxy map & space battles
* Procedural planet builder
* Cloud-saved campaigns & session replay
* Modular quest/encounter generator


---
