## 🚀 Build & Launch Streamlining Tasks (Expanded)

### T-201. Create `UGameStartupSubsystem`
- [ ] Run during `GameInstance` init phase.
- [ ] Auto-load:
  - AI Director singleton
  - Music subsystem
  - AnimationTagMap
- [ ] Call `SpawnStartupActors()` to populate test scene if enabled.

### T-202. Write `AutoMapLoader.cpp`
- [ ] On game startup (non-editor), read `DefaultStartupMap.ini`
- [ ] Load that level with `UGameplayStatics::OpenLevel()`

### T-203. Create `EditorPrepUtilityWidget` (UMG)
- [ ] Buttons:
  - Spawn BP_PlayerCharacter
  - Add enemies or group from test presets
  - Spawn AI Director HUD / debug tools
- [ ] Extras:
  - Add nav mesh volume, sun light, skylight with default settings

### T-204. Default Viewport Prep
- [ ] On BeginPlay (GameMode or StartupSubsys):
  - Call `ClientSetHUD(BP_AIDirectorHUD)`
  - Add debug panels to viewport

### T-205. Create CLI Launch Scripts
- [ ] `run_game.cmd` (Windows):
  - Opens target map directly, bypassing editor
- [ ] `run_game.sh` (Mac):
  - Wraps launch command with options

### T-206. Add Console Hotkey Bindings
- [ ] In `DefaultInput.ini`:
  - Enable console key: `~`
  - Bind keys:
    - `F5`: Reload scene
    - `F10`: Trigger BossIntro manually

---