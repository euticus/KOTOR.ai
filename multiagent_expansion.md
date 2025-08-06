# KOTOR.ai – Phase 9: AIDM Federation Model + Modular LLM Sub-Agent Tasks

## 🤖 AIDM Modular Agent Delegation Tasks

### T-301. Architect Sub-Agent Registry
- [ ] Create `FAgentRegistry` struct that defines roles, providers, and endpoint URLs
- [ ] Support at least 10 sub-agents: NarrativeArchitect, DialogueWriter, CharacterDesigner, PlanetDesigner, EncounterBuilder, LootCaster, MemoryTracker, SceneScorer, CinematicPlanner, GameplaySynthesizer
- [ ] Add fallback routing logic in `AIDirectorComponent`

### T-302. Define LLM Agent Protocol Schema
- [ ] Create `AgentInputPayload` and `AgentOutputPayload` structs
- [ ] Document JSON templates for:
  - QuestSaga (`PlanetList`, `MainArc`, `StoryBeats`)
  - CharacterProfile (`Name`, `Backstory`, `Stats`, `Gear`, `Alignment`)
  - DialogueTree (`Speaker`, `Responses[]`, `MoralTags`)
  - EncounterProfile (`CRLevel`, `Enemies[]`, `LootDropChance`)
  - PlanetDefinition (`Biome`, `Hazards`, `FactionControl`, `Layouts[]`)

### T-303. Add AgentRoutingSystem to AIDirector
- [ ] In `AIDirectorComponent`, add `RouteToAgent(FString TaskType)`
- [ ] Call appropriate provider: OpenAI, Claude, Local via REST
- [ ] Include prompt templating fallback (if no JSON schema detected)

### T-304. Delegate to Specialized Sub-Agents
- [ ] Replace existing quest/planet/NPC generation with routed versions
- [ ] Add benchmarking log to track which agent handles which call
- [ ] Add provider tags to output JSON for tracking

### T-305. Implement MemoryTracker Sub-Agent
- [ ] Log quest flags, alignment decisions, major NPC interactions
- [ ] Store in SaveGame JSON or local `.sav` format
- [ ] Expose query method: `HasPlayerDone(FString Key)`

### T-306. Setup Agent Test Harness (Python CLI)
- [ ] Add `test_agents.py` with CLI menu for:
  - Test DialogueWriter prompt
  - Generate 5 QuestPlanets with NarrativeArchitect
  - Validate EncounterBuilder CR scaling
- [ ] Print formatted JSON and store test outputs in `/agent_tests/`

### T-307. Optional: LLM Router + Switcher
- [ ] Add `LLMRouterSubsystem` with provider preference map
- [ ] Use metadata like latency, cost, and temperature to route calls
- [ ] Allow override via config or `.env`: `FORCE_PROVIDER=claude`

### T-308. Optional: Prompt Debug Panel
- [ ] UMG widget to test prompt output from each sub-agent
- [ ] Dropdown for agent role, text area for prompt body
- [ ] Shows last JSON response and provider info

### T-309. Expand GameStartupSubsystem
- [ ] Preload sub-agent registry
- [ ] Warm up AIDM routing
- [ ] Inject test saga if flag `InjectTestStory=True` in config

---

Would you like to generate the JSON schema docs next? Or wire up example `.prompt` templates for each agent?
