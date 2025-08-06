# KOTOR.ai – Phase 8 Deep UE5 Tasks: AI Animation + Procedural Music

## 🎭 AI Animation Tasks

### T-101. Implement UProceduralPerformanceComponent (C++)
- [ ] Create `UProceduralPerformanceComponent`
- [ ] Define `FPerformanceMetadata` struct (emotion, tone, line)
- [ ] Write logic to select and play AnimMontage based on metadata
- [ ] Expose component to Blueprints

### T-102. Build AnimationTagMap UObject
- [ ] Create `UAnimationTagMap` that maps emotion + tone → AnimMontage
- [ ] Load from `AnimationTagMap.json` or inline editor setup

### T-103. Integrate VO Playback with Performance Component
- [ ] Add `UVoiceSynthesisComponent` trigger to play dialogue
- [ ] Sync voice playback with montage trigger

### T-104. Facial Animation (MetaHuman)
- [ ] Use Control Rig curves to map expressions and visemes
- [ ] Trigger facial curves via `FaceComponent->SetExpressionCurve()`
- [ ] Optionally add phoneme syncing from TTS line

### T-105. Create BP_AnimationTestConsole
- [ ] Expose dropdowns for emotion/tone/test line
- [ ] Test `PlayPerformance()` on a test dummy NPC

### T-106. Create Companion Reaction System
- [ ] Build `TriggerCompanionEmote(FString EventTag)` API
- [ ] Trigger idle or expressive montages on companions
- [ ] Trigger based on quests, dialogue, morality shifts

### T-107. Boss Intro Blueprint
- [ ] Create `BP_BossIntroSequencer`
- [ ] Include cinematic camera pan
- [ ] Add boss VO + pose montage
- [ ] Trigger music hit from boss metadata


## 🎵 Procedural Music Tasks

### T-108. Create UProceduralMusicSubsystem (C++)
- [ ] Define `TMap<FString, USoundBase*> MusicStems`
- [ ] Load stems from `MusicStemMap.json`
- [ ] Expose control API: `PlayStem`, `FadeOut`, `BlendMusic`

### T-109. Implement Runtime Music Layering
- [ ] Use `UGameplayStatics::SpawnSound2D()` with fade-ins
- [ ] Store references in `TMap<FString, UAudioComponent*> ActiveStems`
- [ ] Provide fade-in/out utility

### T-110. Link AIDM Narrative Tags to Music
- [ ] Accept tone/alignment/scene JSON
- [ ] Parse into blend presets
- [ ] Switch stem set accordingly

### T-111. Create WBP_MusicDebugPanel (UMG)
- [ ] Dropdowns for biome, tone, combat/boss toggles
- [ ] Sliders for stem volume
- [ ] Log events to screen

### T-112. Optional: Upgrade to MetaSound
- [ ] Build MetaSound graph with parameterized inputs
- [ ] Route `EnemyDensity`, `EmotionLevel`, etc. into inputs
- [ ] Control tempo and modulation with combat state

