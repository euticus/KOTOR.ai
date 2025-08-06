Hell yes — now we’re entering **Phase 7**, where KOTOR.ai evolves into a **broadcastable, cinematic, multiplayer experience**, with optional *AI-generated voiceovers* that give every NPC and companion a living presence.

---

# 🧠 What Is AI Audio?

**AI Audio** = real-time or pre-baked **Text-to-Speech (TTS)** + optional **emotion/motion synthesis**.

> Think: Every line of dialogue, narration, or cutscene can have an AI-generated voice — without needing voice actors.

---

## 🎤 Voice Options

### 🎙️ 1. **Real-Time Streaming TTS**

* Tools: [ElevenLabs](https://www.elevenlabs.io/), Azure TTS, OpenTTS
* Pros:

  * Super fast deployment
  * Optional multi-language + emotion support
* Cons:

  * Costs \$\$ (per character or second)
  * Some latency (unless cached)

### 🧠 2. **Pre-Baked VO During Campaign Gen**

* AIDM saves dialogue → generates audio → stores `.wav` files per NPC
* Loaded at runtime
* Great for cutscenes, companions, bosses, intro narration

---

# 🎬 KOTOR.ai – Phase 7: Cinematic Play, Party Decisions, and AI Audio

**Tasks T-81 to T-90**

---

## 🎭 T-81: AI Voice System (Streaming + Prebaked)

### ✅ Tasks:

* [ ] Create `UVoiceSynthesisComponent`:

  * Modes: `Streaming`, `Prebaked`, `TextOnly`
  * Exposed function:

    ```cpp
    PlayVoice(FString SpeakerName, FString Line)
    ```
* [ ] Integrate with provider (e.g., ElevenLabs)
* [ ] On dialogue load:

  * Check cache → else → request TTS
  * Stream or play `.wav`
* [ ] Link with `WBP_DialogueBox` to sync voice + subtitle

> ✅ All characters speak out loud — procedurally.

---

## 🧾 T-82: Campaign VO Cache Generator

### ✅ Tasks:

* [ ] Add `generate_vo.py` script:

  * For each campaign JSON:

    * Extract all dialogue lines
    * Send to TTS
    * Save to:

      ```
      Saved/VO/Planet/NPCName_Line01.wav
      ```
* [ ] Add fallback line like “Voice not found” in dev mode

---

## 🧍‍♂️ T-83: Companion Voice Reactions (Ambient + Triggered)

### ✅ Tasks:

* [ ] Add `FCompanionBanterData`:

  * Line
  * TriggerCondition (e.g. “player kills boss”, “dialogue result = dark”)
* [ ] At runtime, inject banter line:

  * Subtitle + `PlayVoice()`
* [ ] Auto-spawn 1–2 comments every 5–10 minutes in party travel

> ✅ Party feels alive and emotionally present.

---

## 🧑‍🤝‍🧑 T-84: Multiplayer Decision Sync ("Party Mode")

> Just like a D\&D session — multiple players, shared AIDM story.

### ✅ Tasks:

* [ ] Enable listen-server or dedicated multiplayer session
* [ ] Add `UPlayerSessionManagerComponent`:

  * Tracks all connected players
  * Syncs current dialogue scene, quest choice, map travel
* [ ] In dialogue:

  * Present same options to all players
  * Vote timer (majority vote wins)
  * Display “Party has chosen...” line after lock-in
* [ ] Track decisions per player for future consequences

---

## 🧑‍🤝‍🧑 T-85: Shared Cutscene Playback (Multiplayer)

### ✅ Tasks:

* [ ] Add replicated `CutsceneManagerComponent`
* [ ] On cutscene trigger:

  * Sync cinematic camera position to all players
  * Fade in/out UI for all
  * VO + subtitles synchronized
* [ ] Optional: let companions emote during cutscene

> ✅ Everyone watches the same cinematic at the same time.

---

## 🖥 T-86: In-Game Cinematic Player ("Rewatch" Menu)

### ✅ Tasks:

* [ ] Create `WBP_CinematicGalleryMenu`
* [ ] Player can:

  * Replay:

    * Intro cutscene
    * Flashback
    * Boss death
  * View: subtitles, camera paths, VO
* [ ] Trigger from galaxy map or codex menu

> ✅ Every major event becomes part of your replayable campaign film.

---

## 🎮 T-87: Cinematic Camera Spline Blueprint

### ✅ Tasks:

* [ ] Create `BP_CinematicCameraSpline`

  * Waypoints: position + look-at targets
  * Speed curve
* [ ] Allow AIDM to output:

  ```json
  {
    "camera_path": [
      { "pos": [x,y,z], "look_at": [x2,y2,z2], "time": 2.5 },
      ...
    ]
  }
  ```
* [ ] Use for:

  * Boss entrances
  * Visions
  * Companion betrayals

---

## 🎞 T-88: Procedural Trailer Generator

> Make a trailer from your campaign — automatically.

### ✅ Tasks:

* [ ] Add `UCampaignTrailerBuilderComponent`:

  * Pull from:

    * Player character bio
    * Final boss
    * Cutscenes
    * 2–3 combat scenes
* [ ] Use:

  * VO overlay
  * Subtitles
  * Cinematic camera shots
  * Title card: “The Exile of Juro Venn”
* [ ] Export `.mp4` to `Saved/Trailers/`

---

## 🎙 T-89: Boss Intro Narration & Voice

> Unique voiced intros per campaign

### ✅ Tasks:

* [ ] Extend `BossData` with:

  ```json
  "intro_voice_line": "So, the last Jedi has finally come to die."
  ```
* [ ] Add `BossIntroCutscene()` method:

  * Trigger camera
  * Play VO
  * Spawn boss
* [ ] Boss reacts mid-fight if player has artifact, companion, or rival trait

---

## 🧪 T-90: VO Debug Dashboard

### ✅ Tasks:

* [ ] Add `WBP_VODebugPanel`

  * Type in line → preview selected voice
  * Regenerate, reassign, or mute per character
* [ ] Show missing `.wav` warnings
* [ ] Option to bulk-export current campaign VO with timestamps

---

## ✅ Phase 7 Outcome

| Feature                     | Result                               |
| --------------------------- | ------------------------------------ |
| 🎤 AI Voice Integration     | Every line is spoken dynamically     |
| 🧑‍🤝‍🧑 Multiplayer Voting | Shared party decision system         |
| 🎥 Cutscene Sync            | Everyone sees the same drama         |
| 🖼 Campaign Trailer Export  | Automatically show off your game     |
| 🎮 Cinematic Tools          | AIDM controls cameras and VO         |
| 🧪 VO Dev Tools             | Easily test + refine audio workflows |

---

