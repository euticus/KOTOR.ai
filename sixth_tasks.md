Perfect — we’ll exclude **Legacy Characters** and **Painting Archive**, and move forward with a Phase 6 roadmap that incorporates the remaining cutting-edge systems. This phase is focused on *emergent world simulation, subconscious storytelling, and emotional immersion*.

---

# 🌌 KOTOR.ai – Phase 6: World Simulation, Subconscious Narrative, and Player-Driven Atmosphere

**Tasks T-46 to T-55**

---

## 🌍 T-46: Dynamic World-State Simulation Engine

> *Factions, warzones, resource scarcity, and planet instability.*

### 🔧 Tasks for Augment:

* [ ] Create `UWorldStateSimulator` subsystem:

  * Factions, economy, weather, war states per planet
* [ ] Run update tick every real-time hour or major story beat
* [ ] Planets track:

  * Ownership (e.g. Republic → Sith)
  * Economic strain (prices rise/fall)
  * Civilian/NPC morale
  * Environmental decay or weather shifts
* [ ] Inject into AIDM prompts:

  * Quest flavor changes based on planet mood/state
  * “Taris is under martial law after a Sith raid. The market is closed.”
* [ ] Display summaries in `WBP_GalacticIntelPanel`

---

## 🧠 T-47: NPC Memory & Social Web Engine

> *NPCs track what you did, what they heard, and who told them.*

### 🔧 Tasks for Augment:

* [ ] Create `UNPCMemoryMatrixComponent`
* [ ] Every important NPC stores:

  * First impression
  * Player actions involving them
  * Gossip received from other NPCs (propagation logic)
* [ ] Add simple gossip propagation system:

  * e.g. “Player executed Jedi” → “Friend of Jedi” hears → reacts next encounter
* [ ] Add memory UI to dev panel (for debugging)
* [ ] Hook into faction system and dialogue tree branching

---

## 📖 T-48: Auto-Generated Codex & Narrative Log

> *The game writes a poetic log of your entire campaign.*

### 🔧 Tasks for Augment:

* [ ] Add `NarrativeLogGenerator.py`:

  * Summarize quests, major decisions, betrayals, item acquisitions
* [ ] Write entries in codex style:

  > *“The Jedi fell. The blade that ended him bore the same crest he once wore on his belt.”*
* [ ] Store in `PlayerCodex.json`
* [ ] Display via `WBP_CodexJournal` UI

  * Allow search + tag filters

---

## 🧘 T-49: Meditation Events & Force Visions

> *Let the player enter “pause states” for character reflection.*

### 🔧 Tasks for Augment:

* [ ] Create interactable `MeditationSpot` actor
* [ ] On activation:

  * Freeze UI, enable atmospheric camera mode
  * AIDM generates:

    * Flashback memory
    * Vision or hallucination
    * Cryptic moral dilemma
* [ ] Add `WBP_ForceVisionPromptWidget` to show scene dialogue or player choices
* [ ] May set internal flag or alter alignment subtly

---

## 🛰 T-50: Galactic News System

> *Simulate the HoloNet with weekly events and headlines.*

### 🔧 Tasks for Augment:

* [ ] Create `GalacticNewsGenerator.py`

  * Pulls from:

    * Player actions (e.g., “Jedi Temple Destroyed”)
    * World-state sim (e.g., “Trade embargo on Nar Shaddaa”)
  * Includes faction spin/bias
* [ ] Add `WBP_HoloNewsPanel`:

  * Scrollable news UI
  * Radio-style TTS reading (optional)
* [ ] Time-based: updates every X minutes or major story event

---

## 🧠 T-51: AIDM Mood & Tone Regulator

> *Let the narrative evolve tonally over time — from hopeful to grim to triumphant.*

### 🔧 Tasks for Augment:

* [ ] Create `NarrativeToneTracker`:

  * Scans events for emotion signals:

    * Tragedy, betrayal, hope, anger
* [ ] Inject into AIDM prompt:

  > “The tone has been grim lately. Introduce a moment of levity and surprise.”
* [ ] Affect:

  * Quest tone
  * Companion dialogue
  * Lighting/music/camera behavior

---

## 🧩 T-52: Moral Echo System

> *The galaxy slowly reshapes in response to your morality.*

### 🔧 Tasks for Augment:

* [ ] Track global alignment across entire campaign
* [ ] Trigger subtle environmental shifts:

  * Lighting warms or dims
  * Planets grow lush or desolate
  * Enemies evolve toward rage/zealotry vs manipulation/cunning
* [ ] Add `UEchoSceneTriggerVolume`

  * Randomly triggers hallucinations, ghost NPCs, or reactions
  * “Echo of your past self” gives commentary
* [ ] Echo effects increase over time (subtle at first, overt later)

---

## 📽 T-53: Cinematic Screenshot Narrator

> *Turn emotional moments into stylized stills with AI-written captions.*

### 🔧 Tasks for Augment:

* [ ] Create photo mode toggle (`F11`)
* [ ] Pause game, enter free camera mode
* [ ] On capture:

  * Grab last 30s of logs + quest data
  * Feed into LLM:

    > “Describe this moment poetically”
  * Output a caption
* [ ] Display with:

  * Screenshot
  * Stylized font overlay
  * Option to save to `CampaignPhotos/` or gallery

---

## 🔁 T-54: “What If” Vision System

> *Let the player peek into alternate timelines.*

### 🔧 Tasks for Augment:

* [ ] At select shrines or events, allow “What If” trigger
* [ ] Create `UAlternateRealitySimulator`

  * Modifies campaign log temporarily (e.g., “What if player sided with Sith?”)
  * AIDM outputs brief cutscene or vision sequence
* [ ] Return to real world — no state changes, but reveals insights
* [ ] Optional: unlock new dialogue or hidden quests if vision is completed

---

## 🦠 T-55: Force Parasite / Hidden Entity Mechanic

> *Introduce a subtle narrative force that may or may not be manipulating you.*

### 🔧 Tasks for Augment:

* [ ] Add internal `UEntityTrackerComponent`

  * Tracks whisper triggers, hallucinations, subtle combat changes
* [ ] AIDM creates a backstory over time (e.g., “Ancient Sith spirit within your blade”)
* [ ] Random events begin occurring:

  * Enemy suddenly hesitates
  * Companion fears you
* [ ] Discovery Quest:

  * If player investigates, unlocks:

    * Boss encounter
    * Choice to accept, destroy, or absorb entity

---

## ✅ Phase 6 Goals

| System                 | Result                                             |
| ---------------------- | -------------------------------------------------- |
| 🌍 World Sim           | Planet state evolves based on war, economy, player |
| 🧠 NPC Memory          | Deep social consequences & gossip propagation      |
| 📖 Narrative Log       | In-game poetic storybook                           |
| 🧘 Meditation          | Reflective Force-based inner arcs                  |
| 🛰 HoloNews            | Diegetic feedback & worldbuilding                  |
| 🎭 Tone Shifter        | Cinematic tonal evolution of story                 |
| 🧩 Moral Echoes        | World subtly changes based on you                  |
| 📸 Screenshot Narrator | Save emotional visual moments                      |
| 🔁 What-If Simulator   | “Peek behind the curtain” decisions                |
| 🦠 Hidden Entity       | Optional horror/meta-narrative thread              |


