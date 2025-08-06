Awesome. We’ll drop the **Player Wishes** mechanic and move forward with the rest — everything you’ve selected elevates KOTOR.ai from an innovative RPG to an **emotionally intelligent, emergent storytelling platform**.

---

# 🌟 KOTOR.ai – Phase 5 Task Breakdown

***Next-Gen Immersion, Systems Depth, and World Reactivity***
Tasks T-36 through T-45

---

## 🧬 T-36: LLM-Driven Character Creator

> *“Tell me who you want to be” – AIDM builds your character.*

### 🔧 Tasks for Augment:

* [ ] Add `WBP_CharacterCreationPanel` with natural language input box:

  * Prompt: "Describe your character"
* [ ] Create `CharacterGenerator.py`:

  * Inputs: name, class/flavor text, alignment, background
  * Outputs:

    * `PlayerStats` (HP, AC, skills, Force Affinity)
    * `StartingLoot`
    * `FCompanionData` for rival or bonded NPC
* [ ] Add `CharacterProfile.json` generator
* [ ] Automatically generate:

  * Origin planet
  * Personal goal (1–2 quest nodes)
  * NPC backstory hooks for AIDM memory injection

---

## 🧛 T-37: Dark Side Mutation System

> *The more power you draw, the more it changes you.*

### 🔧 Tasks for Augment:

* [ ] Add `UMutationManagerComponent` to PlayerCharacter
* [ ] Create corruption threshold mechanic:

  * Killing innocents, Force abuse, betrayal increase mutation score
* [ ] At set breakpoints:

  * Trigger hallucination events (LLM-generated)
  * Visually mutate player model (material swap, glowing eyes)
  * Add whispers (TTS VO lines from ghost Sith / inner voice)
* [ ] Optional: add “Redemption Path” to reduce mutation via quest chain

---

## 🧠 T-38: AIDM Narrative Pacing Self-Assessment

> *The story monitors itself for balance and tone.*

### 🔧 Tasks for Augment:

* [ ] Add `NarrativeRhythmAnalyzer.py`

  * Every 2 quests, summarize pacing:

    * Combat vs Dialogue vs Exploration %
    * Number of moral events
  * Sample prompt: “Summarize pacing for this RPG campaign so far”
* [ ] Based on output:

  * Inject “cooldown quest” (exploration or humor)
  * Trigger rising tension in next planet arc
* [ ] Store analysis in `CampaignLog.json`

---

## 🗣 T-39: Procedural Companion Banter Engine

> *Your companions talk to you, and each other.*

### 🔧 Tasks for Augment:

* [ ] Extend `UCompanionManagerComponent`:

  * Add `CompanionDialogueSlots[]`
  * Timer-based trigger (every X minutes, or on events)
* [ ] Add `CompanionBanterGenerator.py`

  * Inputs: active companions, last choice made, faction standing
  * Output: 2–4 line exchange
  * Injects alignment banter (Light Jedi + Dark Sith = arguments)
* [ ] Create `WBP_BanterSubtitleWidget`:

  * Float text above companions with optional TTS playback

---

## 🧿 T-40: Mythic Artifact System

> *Legendary items evolve as you do.*

### 🔧 Tasks for Augment:

* [ ] Add `FArtifactItem`:

  ```cpp
  FString Name;
  TArray<FPowerLevel> Evolutions;
  FString OriginPlanet;
  FString FlavorLore;
  ```
* [ ] Add to `LootGenerator.py`: 1–3 unique artifacts per long campaign
* [ ] Implement artifact evolution rules:

  * Tracks player alignment, kill count, Force use
  * At milestones → triggers form/ability changes
* [ ] Add visual/auditory effects (e.g. red glow, ghostly whispers)
* [ ] Optional: cutscene flashbacks via `UCutsceneGeneratorComponent`

---

## 🏛 T-41: Mini-Faction System

> *Diplomacy, betrayal, reputation, and political quests.*

### 🔧 Tasks for Augment:

* [ ] Define `FFactionData`:

  * Name, alignment, territory planets, faction leader
* [ ] Create `FactionReputationManagerComponent`

  * Track favor per faction
  * Display in UI: `WBP_FactionReputationWidget`
* [ ] Add faction-based branching:

  * Join, oppose, or manipulate faction interests
* [ ] Add faction-controlled layouts to procedural planet generator

---

## 📽 T-42: Procedural Cutscene Generator

> *Cinematics generated on demand from LLM output.*

### 🔧 Tasks for Augment:

* [ ] Extend `UCutsceneGeneratorComponent`
* [ ] Sample LLM input:

  > “The Jedi walks away from the burning temple while her old friend kneels beside the corpse.”
* [ ] Output parsed into:

  * Camera tracks (position, rotation, zoom)
  * Character animations or idle poses
  * Subtitle VO lines
* [ ] Support:

  * Intro cutscene
  * Flashback vision
  * Boss intro / death
* [ ] Optional: music track selector based on tone

---

## 🌍 T-43: Procedural Galaxy Campaign Expander

> *New planets emerge over time, even after the main arc.*

### 🔧 Tasks for Augment:

* [ ] Enable "Infinite Mode" in `CampaignConfig`
* [ ] Every 3–5 hours of play:

  * Trigger new event (galactic warfront, hidden Sith world, derelict Jedi ship)
  * Procedurally generate new planet with `UProceduralPlanetBuilder`
* [ ] Tie in artifact/companion/faction subquests to these new arcs
* [ ] Optional: “legacy mode” continuation of previous campaign

---

## 🛰 T-44: Cloud-Synced Campaign Browser

> *Access your campaigns anywhere. Share. Explore.*

### 🔧 Tasks for Augment:

* [ ] Build `WBP_CampaignBrowser`:

  * Lists: active campaigns, completed, public
  * Pulls from Supabase or Firebase
* [ ] Add `UCampaignSyncSubsystem`:

  * Sync campaign seed + progress
  * Download & auto-merge new AIDM outputs
* [ ] Allow sharing `CampaignLink` with friends (via hash ID)

---

## 🧪 T-45: Companion Web App (Next.js)

> *Track your character, chat with party, get LLM insights on story.*

### 🔧 Tasks for Augment:

* [ ] Build web app in Next.js + Supabase:

  * Auth system (match player ID)
  * Campaign viewer (quests, planet progress)
  * Dialogue log and NPC memories
* [ ] Add live party chat via WebSockets
* [ ] Optional: allow LLM sidebar to ask “What does the Force want me to do?” (player gets roleplay hint)

---

## ✅ Phase 5 Wrap-Up

By the end of Phase 5, your game will support:

| System                   | Result                                            |
| ------------------------ | ------------------------------------------------- |
| 🎭 LLM-Driven Characters | Custom backstories, personal quests               |
| 🧛 Mutation Engine       | Morality affects your body, powers, and story     |
| 🧠 Story Self-Regulation | Better pacing, genre fidelity                     |
| 🧍 Companion Banter      | Reactive, alive party interactions                |
| 🧿 Mythic Items          | Scaling legendary loot that responds to your path |
| 🏛 Mini-Factions         | Galactic diplomacy, emergent subplots             |
| 📽 Procedural Cutscenes  | Personalized cinematics                           |
| 🌌 Infinite Galaxy       | Campaigns that expand over time                   |
| ☁️ Cloud Save/Browser    | Seamless cross-device story access                |
| 📱 Web Companion App     | Remote campaign tracking & party chat             |

---

