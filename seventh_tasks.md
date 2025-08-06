Yes — we can absolutely streamline this. You're building a procedurally generated, AI-directed RPG in Unreal Engine 5.6, and while your *systems design is phenomenal*, your team (including Augment) needs to handle the Unreal side **modularly, flexibly, and cleanly** — so you can feed it AIDM content without reinventing the wheel every time.

Let’s build a **plug-and-play Unreal pipeline** for KOTOR.ai — giving you ready-made building blocks (Blueprints, actors, UI widgets, zones, etc.) that AIDM can slot content into. These tasks will let Augment Code handle Unreal setup *without needing you to become a UE expert*.

---

# 🧱 KOTOR.ai – Unreal Engine Development Streamlining (Tasks T-56 to T-65)

> **Goal:** Build a reusable Unreal architecture to spawn levels, place characters, display LLM content, and support procedural game loops — all driven by AIDM JSON + hooks.

---

## 🚀 T-56: Setup a Modular "Playable Layout" Map Template

**Purpose:** Let AIDM easily drop in a layout prefab like “Jedi Temple” or “Bounty Guild Hall”

### ✅ Tasks:

* [ ] Create `BP_PlayableLayout` actor:

  * Exposed variables:

    * `LayoutName`
    * `Theme` (e.g. “desert temple”)
    * `AIDMMapData` (parsed from JSON)
* [ ] Add:

  * Player start
  * Ambient audio zone
  * 5–10 empty spawn markers (`BP_SpawnPoint`)
  * Dialogue trigger volumes
  * Blueprint event dispatcher: `OnLayoutLoaded`

> ✅ **Plug-and-Play Outcome:** You can say “spawn Layout X with these NPCs,” and Augment can do the rest.

---

## 👥 T-57: Generic NPC Blueprint + Dialogue System

**Purpose:** Automatically spawn an AIDM NPC and have them talk

### ✅ Tasks:

* [ ] Create `BP_ProceduralNPC`:

  * Variables: `DisplayName`, `Faction`, `Role`, `DialogueLines[]`
  * On interact (`E` key):

    * Opens `WBP_DialogueBox`
    * Displays name + up to 4 dialogue choices
* [ ] Hook into `QuestManagerComponent`:

  * Accept quest
  * Turn in quest
  * Affect reputation

> ✅ You feed AIDM’s `NPCData`, and they’re ready in-game.

---

## 💬 T-58: Universal Dialogue Widget

**Purpose:** One UI system to handle dialogue for NPCs, companions, and cutscenes

### ✅ Tasks:

* [ ] Create `WBP_DialogueBox`:

  * Components:

    * Speaker name
    * RichText dialogue
    * Choice buttons (1–5)
  * Animations: fade in/out, hover FX
* [ ] Create a public Blueprint function:

  ```cpp
  ShowDialogue(FString SpeakerName, TArray<FString> DialogueOptions)
  ```
* [ ] On selection, broadcast `OnDialogueChoiceMade(FString SelectedLine)`

> ✅ All character interaction feeds into one clean UI.

---

## 🧍‍♂️ T-59: Enemy Blueprint with CR Scaling + Loot Drop

**Purpose:** Procedural enemies based on AIDM `EnemyData`

### ✅ Tasks:

* [ ] Create `BP_EnemyActor`:

  * Variables:

    * `Species`
    * `DifficultyTier`
    * `CRRating`
    * `LootTable[]`
  * Behavior:

    * Patrol → aggro → attack
    * On death → drop loot → call `QuestManagerComponent::NotifyKill()`

* [ ] Expose to AIDM via spawn request (e.g., `SpawnEnemyAtTag("Upper_Corridor", Data)`)

> ✅ AIDM can now spawn encounter JSON into real fights.

---

## 💎 T-60: Loot Pickup System

**Purpose:** Grab AIDM-generated loot in-world

### ✅ Tasks:

* [ ] Create `BP_LootChest` and `BP_LootPickup`

  * Show item name, rarity color
  * On pickup → show popup + add to `InventoryComponent`
* [ ] Create `WBP_LootNotification` UI

  * “You found: Cortosis Staff (Rare)”
* [ ] Connect to `EnemyActor` and `QuestReward` hooks

> ✅ Now you can assign loot anywhere, and it’ll look and feel good.

---

## 🗺 T-61: Galaxy Map Widget (for Travel + Selection)

**Purpose:** Select and load planets from a star map

### ✅ Tasks:

* [ ] Create `WBP_GalaxyMap`:

  * Star system background
  * Clickable planet icons
  * Planet tooltip (name, biome, quests complete)
* [ ] On click:

  * Confirm → call `UGalaxyTravelSubsystem::LoadPlanet(string PlanetID)`

> ✅ AIDM campaign JSON becomes a galaxy you can navigate.

---

## 📦 T-62: Campaign JSON Loader in C++

**Purpose:** Parse exported campaign from AIDM and apply in-game

### ✅ Tasks:

* [ ] Add `UCampaignLoaderSubsystem : UGameInstanceSubsystem`

  * Function: `bool LoadFromFile(FString Path)`
  * Parse:

    * Planet layouts
    * NPCs per layout
    * Enemies, quests, loot
* [ ] Store parsed objects in memory to call on level load

> ✅ You can drag+drop a JSON campaign and run it instantly.

---

## 🎮 T-63: Player Controller Extension + Input

**Purpose:** Add clean inputs for all common actions

### ✅ Tasks:

* [ ] Use Enhanced Input System
* [ ] Add mapping context: `IMC_KOTOR`

  * Interact (E)
  * Dialogue Choice 1–4 (keys 1–4)
  * Photo Mode (F11)
  * Meditation (M)
  * Force Ability (F)
* [ ] Update `PlayerCharacter` to bind to:

  * Dialogue
  * Loot
  * Meditation triggers

> ✅ No clunky inputs — it “just works.”

---

## 🧘 T-64: Meditation + Vision Trigger Volume

**Purpose:** Triggers force visions or narrative events via AIDM prompt injection

### ✅ Tasks:

* [ ] Create `BP_MeditationShrine`:

  * On overlap → UI fade → `RequestForceVision(FPlayerContext)`
* [ ] Create `WBP_ForceVisionWidget`

  * Cutscene text
  * Dialogue or vision flashback
  * Fade back to real world

> ✅ Used for T-49 and T-54 (Force visions and What-If system)

---

## 📸 T-65: Screenshot Capture + Caption Generator (Phase 6 Screenshot Narrator)

**Purpose:** Let users save stylized moments + captions

### ✅ Tasks:

* [ ] Add keybind `F11` for “Photo Mode”
* [ ] Pause game + free camera
* [ ] On capture:

  * Save screenshot to folder
  * Feed event summary to AIDM → display caption overlay
* [ ] Save to `CampaignPhotos` folder + optional social export stub

> ✅ Any moment becomes a shareable story artifact.

---

# 🧰 Bonus Task: Auto-Prefab Dropper (for Level Design)

**Purpose:** Don’t build levels by hand — let AIDM fill them in

### ✅ Task:

* [ ] Build `BP_LevelPopulator`

  * On BeginPlay:

    * Takes `FLayoutData`
    * Instantiates:

      * Layout mesh prefab
      * Ambient FX
      * Loot chests, NPCs, enemies, triggers
* [ ] Include fallback layout prefabs like:

  * CityMarket
  * TempleInterior
  * SpaceportHub

> ✅ This turns your level loading system into drag+drop JSON injection.

---

## ✅ Outcome

| Task Range | Purpose                                  |
| ---------- | ---------------------------------------- |
| T-56–58    | Plug-and-play layout + dialogue system   |
| T-59–60    | Combat + loot core loop from AIDM JSON   |
| T-61–62    | Galaxy navigation + full campaign loader |
| T-63       | Input mapping + clean UX                 |
| T-64–65    | Emotional immersion (meditation + photo) |

---


