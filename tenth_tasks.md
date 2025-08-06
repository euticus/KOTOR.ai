Perfect. Here’s the roadmap for **Phase 8**, focusing on two cinematic systems that will make KOTOR.ai feel like a living, breathing, emotionally reactive film:

* 🎭 **AI-Driven Animation**
* 🎵 **Procedural Soundtrack & Music Direction**

---

# 🎬 KOTOR.ai – Phase 8: Dynamic Animation & Adaptive Soundtrack

**Tasks T-91 to T-100**

---

## 🎭 AI-Driven Animation System

### **T-91. Create `UProceduralPerformanceComponent`**

> Controls body + face animation synced with AIDM dialogue.

✅ Tasks:

* [ ] Attach to `BP_ProceduralNPC`, `BP_Enemy`, and `BP_Companion`
* [ ] Accept metadata:

  ```cpp
  struct FPerformanceData {
      FString Emotion;      // e.g. "angry", "hopeful"
      FString DialogueTone; // e.g. "mocking", "urgent"
      FString ActorRole;    // "boss", "companion"
  };
  ```
* [ ] Based on inputs:

  * Select appropriate **animation montage** or idle loop
  * Play synced with voice line start

---

### **T-92. Build Animation Tag Table**

> Maps AIDM emotion tags to montages you already have or will add.

✅ Tasks:

* [ ] Create `AnimationTagMap.json`:

```json
{
  "talk_angry": "TalkAngry_Loop",
  "mocking": "LaughMocking_Loop",
  "dark_reveal": "PointForward_Slow",
  "despair": "SitHeadDown_Loop"
}
```

* [ ] Integrate with `UProceduralPerformanceComponent`

---

### **T-93. Add Auto-Face Animation Trigger (UE Native)**

> Sync lip and face gestures to TTS voice line.

✅ Tasks:

* [ ] Use `FaceFX`, Control Rig, or built-in MetaHuman expression curve
* [ ] When voice line is played:

  * Trigger facial expressions based on `Emotion`
  * Optional: use phoneme curve from TTS line

---

### **T-94. Add Companion Emote Trigger System**

> Let party members physically react to player choices.

✅ Tasks:

* [ ] Add function:

  ```cpp
  void TriggerCompanionEmote(FString EventTag); // e.g. "boss_defeated", "artifact_found"
  ```
* [ ] Emotes include:

  * Nods, cheers, fear recoils, eye rolls
* [ ] Triggered when:

  * Quest branches
  * Morality choices
  * Combat ends

---

### **T-95. Add Boss Cutscene Entrance Animation Blueprint**

> Make boss intros unforgettable.

✅ Tasks:

* [ ] Create `BP_BossEntranceSequence`:

  * Animates:

    * Boss spawn pose (e.g. saber ignite, step forward)
    * Player camera reaction
  * Sync with VO and music hit
* [ ] Trigger on `FinalBossData` load

---

---

## 🎵 Procedural Soundtrack System

### **T-96. Create `UProceduralMusicSubsystem`**

> Master controller for all soundtrack blending.

✅ Tasks:

* [ ] On BeginPlay:

  * Load music stems:

    * `base_pad`, `combat_drums`, `dark_motif`, `light_motif`, `tension_synth`, etc.
* [ ] On scene change:

  * Read narrative tone from AIDM:

    ```json
    "tone": "hopeful", "alignment": "dark", "scene": "boss_intro"
    ```
  * Blend active stems:

    ```cpp
    PlayStem("dark_motif", volume=0.6);
    FadeOut("light_motif");
    ```

---

### **T-97. Define Music Layers and Stem Types**

> Classify and organize all your music stems.

✅ Tasks:

* [ ] Create `MusicStemMap.json`:

```json
{
  "ambient_forest": ["pad_low", "wind", "mystery_chimes"],
  "combat_sith": ["drums", "brass_stabs", "choir"],
  "boss_finale": ["force_theme", "choir", "rising_strings"]
}
```

* [ ] Associate with:

  * Biomes
  * Encounter type
  * Player alignment

---

### **T-98. Create Realtime Music Blending Blueprint**

> Lets music change without hard cuts.

✅ Tasks:

* [ ] Use `USoundCue` or `MetaSound`
* [ ] Create:

  * `BP_MusicController`
  * Accepts JSON or C++ struct for active stems
* [ ] Fade in/out smoothly between layers using `SetVolumeMultiplier`

---

### **T-99. Link Music to Combat + Morality + Decisions**

> Let moment-to-moment gameplay shift the soundtrack.

✅ Tasks:

* [ ] Monitor events:

  * Player alignment
  * Combat outcomes
  * Boss phase transitions
* [ ] Trigger audio shift:

  * Moral descent → dark chimes layer fades in
  * Artifact acquired → heroic string pad rises
  * Sith betrayal → remove “hope” motif layer

---

### **T-100. Create `WBP_MusicDebugger` for Dev Use**

> Toggle/test music layers live in editor.

✅ Tasks:

* [ ] Display current active layers
* [ ] Add dropdowns to:

  * Force "combat mode"
  * Test "light/dark transition"
* [ ] Log blend operations

---


