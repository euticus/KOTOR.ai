Excellent. Now that AIDM is running and integrated into Unreal, **Phase 3** focuses on adding immersion, responsiveness, and long-term extensibility.

> **Goal:** Create rich interaction systems (voice, companions, cinematics), procedural world logic (space travel, galaxy map), and expandability (memory, modding).

---

# 🌌 KOTOR.ai Task Pipeline – Phase 3: Immersion, Intelligence & Modding

---

## 🔊 T-21: Voice & TTS Integration

**Objective:** Bring NPC dialogue to life via AI-generated voice.

**Tasks for Augment:**

* [ ] Create `VoiceSynthesisComponent`
* [ ] Integrate with a TTS provider (e.g., ElevenLabs, Azure Speech, OpenTTS)
* [ ] For each `NPCData`, add:

  ```json
  "voice_id": "vader_like_01"
  ```
* [ ] On dialogue start:

  * Send current NPC line to TTS
  * Stream back and play via `UAudioComponent`
* [ ] Optional: cache audio per line to avoid API rate costs

---

## 👥 T-22: Companion System (Party AI)

**Objective:** Add recruitable companions with dialogue and combat roles.

**Tasks for Augment:**

* [ ] Add `UCompanionManagerComponent` to PlayerCharacter
* [ ] Define `FCompanionData`:

  ```cpp
  FString Name;
  FString Role; // Tank, Healer, DPS, etc
  FString Alignment;
  TArray<FString> LikesDislikes;
  ```
* [ ] Recruit via LLM quest or NPC trigger
* [ ] In combat:

  * Follow tactical orders (Attack target, Hold, Heal player)
* [ ] Out of combat:

  * Offer commentary (triggered by triggers or player actions)
  * Track loyalty or influence

---

## 🧠 T-23: Narrative Memory & Context Engine

**Objective:** Track key decisions, moral alignment, and long-term narrative consequences.

**Tasks for Augment:**

* [ ] Implement `NarrativeMemoryManager`

  * Stores:

    * Faction reputation
    * NPC relationships
    * Player moral events
* [ ] Optional: use `faiss` or `chromadb` to store embeddings of quest outcomes
* [ ] Inject memory snippets into LLM prompts during generation (e.g. “Player previously spared Sith informant”)
* [ ] Display current alignment and narrative consequences in `WBP_AIDM_DebugPanel`

---

## 🛰 T-24: Galaxy Map System

**Objective:** Let the player travel between planets (visual + logic).

**Tasks for Augment:**

* [ ] Create `WBP_GalaxyMapWidget`

  * Display all planets from `CampaignPlan.planets`
  * Hover to show brief info (biome, faction, quest status)
  * Click to trigger travel confirmation
* [ ] Create `UGalaxyTravelSubsystem`:

  * Saves player progress per planet
  * Manages fade/loading to correct streaming level
  * Handles event triggers between travels (ambush, companion dialog)

---

## 🧪 T-25: AIDM Scenario Testing Tools

**Objective:** Build a dev interface to generate, simulate, and debug mini-scenarios.

**Tasks for Augment:**

* [ ] Create CLI tool: `simulate_encounter.py`

  * Input: planet type, enemy type, player level
  * Output: encounter JSON (enemy CR, layout, loot table)
* [ ] Create UE blueprint callable node:

  * `SimulateLLMScenario(FScenarioRequestData)`
  * Returns: `FScenarioResultData`
* [ ] Add in-editor debug spawn: “Generate Side Quest” or “Force Boss Cutscene”

---

## 🎞 T-26: Cinematic Event Generator

**Objective:** Auto-create cinematic cutscenes tied to quests or bosses.

**Tasks for Augment:**

* [ ] Create `ACinematicDirectorActor`

  * Plays Matinee/Sequencer files
  * Can dynamically assign:

    * Dialogue
    * Camera pans
    * Music
* [ ] Add to `CampaignPlan`:

  ```json
  "cutscenes": {
    "intro": "jedi_council_briefing",
    "midpoint": "betrayal_of_mentor",
    "finale": "boss_defeat"
  }
  ```
* [ ] Add triggers in world to play appropriate cutscenes

---

## 🧰 T-27: Player-Controlled Prompt Injection (Optional Sandbox Mode)

**Objective:** Let advanced players inject prompt guidance in-game.

**Tasks for Augment:**

* [ ] Add UI toggle: `Sandbox Mode`
* [ ] In `WBP_AIDM_DebugPanel`, let player type prompt like:

  * “Spawn hidden Sith assassin”
  * “Force conflict between companions”
* [ ] Inject into `AIDirectorComponent` and regenerate scenario live

---

## 🧩 T-28: Modding API & Scripting Hooks

**Objective:** Allow external users to write their own quests, enemies, or loot.

**Tasks for Augment:**

* [ ] Expose data assets for:

  * Enemy templates
  * Dialogue snippets
  * Loot tables
* [ ] Add scripting engine (Lua or Python) to:

  * Trigger events
  * Override behaviors
  * Run on startup or triggers
* [ ] Document mod folder structure:

  ```
  /Mods/
   ├─ CustomLoot/
   ├─ Quests/
   ├─ CustomEnemies/
  ```

---

## ✅ Phase 3 Goal

> By the end of Phase 3, KOTOR.ai should support:

* Spoken LLM-generated dialogue
* Recruitable party with loyalty/interactions
* Persistent player morality & story consequences
* Galaxy travel system across procedurally defined planets
* Debuggable scenario sandbox and event spawner
* Auto-generated boss cutscenes and branching events
* Expandable mods and scripting hooks

---


