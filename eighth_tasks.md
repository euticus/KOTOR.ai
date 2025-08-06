Perfect. Below is a fully structured **Unreal Engine development optimization pipeline** broken into tasks for Augment Code. These are plug-and-play instructions to make your **builds faster, testing automated, and development smoother**.

---

# 🛠 Augment Task List – KOTOR.ai Dev Streamlining (T-66 to T-80)

---

## ⚡ **Build Optimization & Compilation Speed**

### **T-66. Create a Fast-Compile Build Target**

* Add a new `KOTOR_FastEditorTarget.cs` with:

  ```cs
  bUseUnityBuild = true;
  bUsePCHFiles = true;
  bUseSharedPCHs = true;
  bBuildEditor = true;
  bCompileChaos = false;
  bCompileSimplygon = false;
  bCompileICU = false;
  bUseChaos = false;
  ```
* Make sure this target builds the minimal necessary modules for AIDM dev and sandbox play.
* Hook it into `.uproject`.

---

### **T-67. Trim Plugins in `.uproject` for Development Mode**

* Disable unused plugins like:

  * `OnlineSubsystemSteam`
  * `Niagara`
  * `LiveCoding`
  * `MRMesh`
* Leave Blueprint- and UI-related ones intact.

---

## 🔄 **Live Coding & Hot Reload**

### **T-68. Enable and Configure Live Coding**

* In `Editor Preferences → Live Coding`, set:

  * Enable Live Coding = ✅
  * Start Automatically = ✅
* Add C++ macro `LIVE_CODING_ENABLED` to key game systems (optional).
* Allow reload via `Ctrl+Alt+F11`.

---

## 🧪 **Automation Scripts & Testing**

### **T-69. Build Script: `run_kotor_game.bat`**

* Write a Windows batch file that:

  1. Builds game using `Build.bat`
  2. On success, launches `KOTOR_Clone.exe` with these CLI args:

     ```
     -windowed -log -dev -load=IntroPlanet -testing
     ```

---

### **T-70. Clean Script: `clean_kotor_build.bat`**

* Deletes:

  * `/Intermediate`
  * `/Binaries`
  * `/Saved`
* Only for troubleshooting corrupted builds.

---

### **T-71. Auto-Run Campaign Loader at Startup**

* Modify `UGameInstance::Init()` to:

  * Check for CLI arg `-testing`
  * If present, load campaign from:

    ```
    Saved/Campaigns/test_campaign.json
    ```
  * Call:

    ```cpp
    LoadCampaign(“Dantooine”);
    ```

---

## 📦 **Packaging & Launch Prep**

### **T-72. Add Script: `generate_campaign.py`**

* Python file that:

  * Loads AIDM
  * Creates and exports full JSON campaign to:

    ```
    Saved/Campaigns/test_campaign.json
    ```

---

### **T-73. Add Dev Shortcut: `main.py` Launcher**

* Python script that does:

  1. `generate_campaign.py`
  2. Calls `run_kotor_game.bat`
  3. Launches game into selected planet
* Optional CLI flags:

  * `--planet Dantooine`
  * `--mode testing`
  * `--random`

---

## 🧪 **Sandbox Testing Environment**

### **T-74. Create `L_DevTestMap` (Developer Playground Map)**

* One open map with:

  * Dialogue trigger zone
  * Meditation shrine
  * Loot pickup test
  * Enemy spawn arena
  * Galaxy map test terminal
  * Screenshot narrator booth
* Auto-load when `-dev` flag is passed in CLI.

---

## 🧘 **Force Vision Integration**

### **T-75. Add `BP_MeditationShrine` Actor**

* On overlap:

  * Triggers fade to `WBP_ForceVisionWidget`
  * Prompts AIDM for flashback or hallucination
* On exit, fade back to map

---

## 📸 **Photo Mode + Captioning**

### **T-76. Create `PhotoModeSubsystem`**

* On `F11`:

  * Pause game
  * Enable cinematic free camera
* On photo:

  * Capture screenshot
  * Run LLM prompt based on campaign context
  * Overlay caption with stylized font

---

## 🧠 **Narrative Auto-Test Harness**

### **T-77. Add `UAutoTestSubsystem`**

* On `-testing` CLI:

  * Loads a known campaign
  * Warps to intro planet
  * Auto-spawns:

    * 1 NPC
    * 1 quest
    * 1 enemy encounter
    * 1 loot drop
* Confirms each subsystem works (log check)

---

## 🧪 **Prefabs for Dev-Time Procedural Testing**

### **T-78. Create Layout Prefabs (`BP_TempleLayout`, `BP_SpaceportLayout`, etc.)**

* Add pre-baked layout Blueprints:

  * Modular geometry
  * Placeholder props
  * 5–10 spawn points (`BP_SpawnPoint`)
* Used by `BP_LevelPopulator` to spawn zones from AIDM

---

## 🔁 **Developer Quality of Life**

### **T-79. Add Keybind Profile: `IMC_KOTOREditor.json`**

* `E` → Interact
* `1–4` → Dialogue options
* `F` → Force Power
* `F11` → Photo Mode
* `M` → Meditate
* `Ctrl+Alt+F11` → Live Coding

---

### **T-80. Add AIDM Debug Panel (`WBP_DevPanel`)**

* Show:

  * Current campaign state
  * Active planet
  * Current alignment
  * Recent AIDM decisions
  * Buttons to:

    * Force side quest
    * Force enemy wave
    * Inject hallucination

---

## ✅ Final Outcome

With these 15 new tasks, you'll have:

* 🔁 Fast compile times
* 🚀 Auto-run game after build
* 🧪 Easy campaign injection
* 🧱 Modular dev test map
* 💻 One-click "generate + play" pipeline
* 🧘 Cinematics, screenshots, meditation, visions all testable in 30 seconds

---

