## 🚀 Auto-Launch & Build Streamlining Tasks

### T-201. Create `UGameStartupSubsystem`
- [ ] Initialize: AIDirector, ProceduralMusicSubsystem, AnimationTagMap
- [ ] Call `SpawnStartupActors()`
- [ ] Load default map if none selected

### T-202. Create `AutoMapLoader.cpp`
- [ ] Write logic to get map name from config
- [ ] Load startup map on game start if not in Editor

### T-203. Create `EditorPrepUtilityWidget` (Blueprint)
- [ ] Button to spawn:
  - BP_PlayerCharacter
  - BP_EnemyGroupTest
  - BP_MusicDebugPanel
  - BP_AIDirectorHUD
- [ ] Auto-add nav mesh + lights (optional)

### T-204. Default Viewport Setup
- [ ] In GameMode BeginPlay or StartupSubsystem:
  - `ClientSetHUD(BP_AIDirectorHUD)`
  - Add WBP panels to viewport

### T-205. Create CLI Run Wrappers
- [ ] `run_game.cmd` (Windows): starts game with target map
- [ ] `run_game.sh` (Mac): launches app bundle with argument

### T-206. Add Console Command Support
- [ ] Enable `~` in `DefaultInput.ini`
- [ ] Add bindings for:
  - `F5` reload
  - `F10` boss intro trigger


## 🤖 MCP Integration + Codex Automation Tasks

### T-301. Add GitHub CI/CD Support for UE5
- [ ] Create `.github/workflows/ue5_build_test.yml`
- [ ] Use Windows runner with Visual Studio 2022 preinstalled
- [ ] Install UnrealBuildTool and call `RunUAT.bat`
- [ ] Set up cache for `Saved/Intermediate` folders to speed up builds

### T-302. Auto-run Build + Tests Post Commit
- [ ] Add steps to run:
  - `Build.bat` for Win64 client
  - `UE5Editor-Cmd.exe` to run Integration Tests
- [ ] Use `--exec="Automation RunTests KOTORTestSuite.*"`
- [ ] Archive and upload `Saved/Logs/` and `Saved/TestReports/`

### T-303. Hook MCP via Codex to Patch Failing Tests
- [ ] On test failure, pipe logs to Codex API
- [ ] Ask: "Fix test T-106 if it fails due to null pointer in CompanionEmote logic"
- [ ] Auto-generate patch PR via Codex suggestions

### T-304. Prompt Engineering for MCP Bug Resolution
- [ ] Create `.codexrc` or MCP context profile:
  - Include folder map
  - Include prompt template: "Refactor, fix, and test this .cpp file"
- [ ] Add custom `.prompt` files to trigger MCP on critical paths (e.g. `AIDirector.cpp`, `ProceduralMusicSubsystem.cpp`)

### T-305. CI Badge Reporting + Slack Hook
- [ ] Add `ci-badge.sh` script to generate build status badges
- [ ] Webhook to Slack or Discord to post when build fails or tests break

### T-306. Codex Auto-Test Expansion
- [ ] Pass each `.cpp` file to Codex with prompt: “Write integration tests in Unreal Automation Framework for this module.”
- [ ] Auto-commit `.cpp` or `.uasset` test files into `Tests/`

---