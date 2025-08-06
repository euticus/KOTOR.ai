Great — you’re building a system that dynamically orchestrates huge narrative and mechanical variability, so this needs careful scaffolding. Below is **Phase 1** of a detailed, step-by-step **task pipeline for Augment Code** to build a robust and extensible AIDM system first, then connect it to Unreal Engine.

---

# 🧠 KOTOR.ai Task Pipeline – Phase 1: AIDM Core Engine

> **Goal:** Build a standalone AIDM pipeline that can ingest story seeds, generate full campaign plans (planets, NPCs, quests, enemies, loot), and be unit tested before UE integration.

---

## 📦 T-1: Setup & Project Structure

**Objective:** Lay down the Python backbone for AIDM outside UE.

**Tasks for Augment:**

* [ ] Create `aidm/` Python module with the following submodules:

  * `story_seed_interpreter.py`
  * `campaign_generator.py`
  * `planet_generator.py`
  * `npc_generator.py`
  * `encounter_generator.py`
  * `loot_generator.py`
  * `boss_generator.py`
  * `memory_manager.py`
* [ ] Create a `tests/` folder and add placeholder test cases for each generator module.
* [ ] Create `main.py` to run test generation from command line with CLI prompt input.

---

## 🧠 T-2: Story Seed Interpreter

**Objective:** Convert user input like "I want a 20-hour story about a Jedi exile hunting a Sith relic" into a `CampaignConfig`.

**Tasks for Augment:**

* [ ] Define `CampaignConfig` dataclass:

```python
@dataclass
class CampaignConfig:
    game_length_hours: int
    estimated_planets: int
    story_seed: str
    time_period: str
    alignment_focus: str  # "light", "dark", "neutral"
```

* [ ] Implement logic to:

  * Parse estimated hours → planet count
  * Extract key themes from seed (e.g., “Sith”, “relic”)
  * Set alignment focus heuristically
* [ ] Create simple CLI prompt to enter story seed and generate a `CampaignConfig` instance.

---

## 🌌 T-3: Campaign Generator

**Objective:** Generate a master campaign plan from the `CampaignConfig`.

**Tasks for Augment:**

* [ ] Define a `CampaignPlan` schema:

```python
@dataclass
class CampaignPlan:
    planets: List[PlanetData]
    final_boss: BossData
    main_quest_outline: str
```

* [ ] Implement campaign planner logic to:

  * Call `planet_generator.py` for each planet
  * Select a planet for the final boss
  * Write a high-level main quest outline
* [ ] Stub Wookieepedia API integration using cached Star Wars planet list
* [ ] Return structured JSON with all fields filled

---

## 🪐 T-4: Planet Generator

**Objective:** For each planet, define name, biome, layout names, and quests.

**Tasks for Augment:**

* [ ] Define `PlanetData` schema:

```python
@dataclass
class PlanetData:
    name: str
    biome: str
    layouts: List[MapLayout]
    main_quest: str
    side_quests: List[str]
```

* [ ] Generate up to 10 layout stubs (e.g., “Upper City”, “Temple Ruins”)
* [ ] Pull real planets using Wookieepedia API + fallback names
* [ ] Add 1–2 side quests per layout using LLM call
* [ ] Return to `CampaignPlan`

---

## 🧍 T-5: NPC Generator

**Objective:** Create all named NPCs with bios, alignment, and optional quests.

**Tasks for Augment:**

* [ ] Define `NPCData`:

```python
@dataclass
class NPCData:
    name: str
    faction: str
    alignment: str
    likes_player: bool
    role: str  # vendor, questgiver, follower, etc.
    backstory: str
    quest: Optional[QuestData]
```

* [ ] Implement logic to:

  * Place key NPCs in each layout
  * Randomize alignment and role
  * Generate 1–3 NPCs per layout with quests depending on config
* [ ] Track favor/relationship systems via dialogue seeds

---

## ⚔️ T-6: Encounter & Enemy Generator

**Objective:** Define enemy types per planet + dynamic spawn rules.

**Tasks for Augment:**

* [ ] Define `EnemyData`:

```python
@dataclass
class EnemyData:
    name: str
    species: str
    cr_rating: float
    abilities: List[str]
    loot_table: List[str]
```

* [ ] Implement:

  * 100+ enemy templates stored in `enemies.json`
  * Enemy selection logic per planet theme
  * Encounter builder that varies CR + formation
* [ ] Enemy stats should loosely map to D\&D rules but themed in Star Wars flavor

---

## 💎 T-7: Loot Generator

**Objective:** Generate 1,000+ scalable loot items using D\&D + Star Wars logic.

**Tasks for Augment:**

* [ ] Define `LootItem`:

```python
@dataclass
class LootItem:
    name: str
    rarity: str  # common, rare, epic, legendary
    item_type: str  # weapon, armor, mod, stim
    bonuses: Dict[str, int]
    flavor_text: str
```

* [ ] Write logic for exponential gear scaling
* [ ] Match loot themes to planet, faction, and CR of enemy
* [ ] Store in loot table JSON per tier (common → legendary)

---

## 👹 T-8: Final Boss Generator

**Objective:** Custom generate the final boss with unique mechanics and lore.

**Tasks for Augment:**

* [ ] Define `BossData`:

```python
@dataclass
class BossData:
    name: str
    species: str
    motivation: str
    mechanics: List[str]
    arena_description: str
    loot_drop: LootItem
```

* [ ] Generate boss with:

  * Unique backstory tied to story seed
  * 2–3 custom battle mechanics (e.g. phases, summons, force powers)
  * Final legendary item drop
* [ ] Return to `CampaignPlan.final_boss`

---

## 🧠 T-9: Memory Manager (Optional for Later)

**Objective:** Store persistent memory of player choices for prompt refinement.

**Tasks for Augment:**

* [ ] Implement local vector DB using `faiss` or `chromadb`
* [ ] Add logic for:

  * Storing NPC interactions
  * Tracking moral decisions
  * Feeding context window for continued prompt sessions

---

## 🧪 T-10: CLI Playground & Campaign Test

**Objective:** Make a CLI sandbox that lets you enter a story seed and preview a full campaign plan.

**Tasks for Augment:**

* [ ] In `main.py`:

  * Ask for user input (story seed + game length)
  * Generate full `CampaignPlan`
  * Print campaign summary, list planets, sample NPCs, boss

---

Once this is built and unit tested, you’ll have an LLM-ready worldbuilding engine that can slot into Unreal via API or pre-generated JSON files.

---

## ✅ Phase 1 Goal

> You should have a fully working AIDM outside Unreal that can:

* Ingest story seed
* Build dynamic campaigns with 15+ planets, 100+ enemies, 1,000s of loot
* Custom-generate NPCs, quests, and bosses
* Export a master JSON campaign file

---

Would you like to begin Phase 2 next (Unreal Engine C++/Blueprint integration + AIDM sandbox map)? Or should I expand on any of these tasks first with code examples?
