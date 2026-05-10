# Copilot Agents for PSP-latro

This folder contains small artifacts to help developer-facing automation and AI prompts.

Agents

- AssetBuilder
  - Script: `scripts/asset_builder/build_assets.sh`
  - Trigger: commits to `assets/src/**` or manual
  - Output: `assets/out/` and `assets/out/manifest.json`
  - Usage: run the script locally or via `.github/workflows/assetbuilder.yml`

- PSPBuilder
  - Script: `scripts/pspbuilder/build_with_docker.sh`
  - Trigger: pushes/PRs that touch `src/**` or `Makefile`
  - Output: `pspalatro.elf`, `EBOOT.PBP` (in `build/` or repo root)
  - Notes: uses `pspdev/psp-sdk` Docker image when available.

- TestRunner
  - Script: `scripts/testrunner/run_smoke_test.sh`
  - Trigger: PRs / manual dispatch
  - Output: `test-output/` (expected/actual/diff images)
  - Notes: PPSSPP required for full capture; baseline created on first run.

- PerfProfiler (placeholder)
  - Intended to run instrumented builds and collect frame-time/memory traces.

- PR Reviewer Skill
  - Checks: `cppcheck`, `clang-format` style, asset-size budgets.

- LocalizationSync
  - Sync/localize strings into `src/` or an external locale file.

- Copilot Agent
  - Prompt template: `.copilot/prompts/copilot-agent.yml`
  - Runner script: `.copilot/tools/run_agent.sh`
  - Purpose: Render prompt templates deterministically and produce artifact bundles (prompt + metadata + optional output) under `.copilot/artifacts/`.
  - Usage: `./.copilot/tools/run_agent.sh .copilot/prompts/copilot-agent.yml myrun "Summarize repo status" "src/"`

- PSP Build
  - Skill: `.copilot/skills/psp-build/SKILL.md`
  - Use: Building, packaging, reproducible Docker builds, linking issues.

- PSP Graphics
  - Skill: `.copilot/skills/psp-graphics/SKILL.md`
  - Use: GU/GUM rendering, batching, texture formats, cache writebacks.

- PSP Audio
  - Skill: `.copilot/skills/psp-audio/SKILL.md`
  - Use: OGG/Vorbis decoding, ringbuffer, audio callbacks, underrun handling.

- PSP Assets
  - Skill: `.copilot/skills/psp-assets/SKILL.md`
  - Use: Asset conversions, texture atlases, image/audio transforms, Git LFS guidance.

- PSP Deploy
  - Skill: `.copilot/skills/psp-deploy/SKILL.md`
  - Use: pack-pbp, PARAM.SFO, USB/FTP deployment and CI packaging.

- PSP Debugging
  - Skill: `.copilot/skills/psp-debugging/SKILL.md`
  - Use: Device-only regressions, logs, screenshot diffs, emulator vs hardware comparisons.

- PSP Performance
  - Skill: `.copilot/skills/psp-performance/SKILL.md`
  - Use: Frame timing, profiling, cache and memory tuning, overclock guidance.

How to use

- Ask the assistant to "Run AssetBuilder" or run the script directly.
- Use the prompt templates in `.copilot/PROMPTS.md` when requesting actions from the AI.
