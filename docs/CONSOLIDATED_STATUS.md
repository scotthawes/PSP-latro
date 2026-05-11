# PSP-latro — Consolidated Status, Missing Features & Improvements

Purpose
-------
This document consolidates the repository's current state, the primary missing
features (vs. the original Balatro v0.11), and prioritized improvements that
should be tracked and actioned. Redundant or overlapping documents in `docs/`
have been merged into this file to provide a single source of truth.

Scope
-----
- Current implemented surface (what works now)
- Missing major features to reach parity with the original
- High/medium/low priority improvements and recommended verification steps
- Auditability rules for any changes that are not part of the original game

1) Current state (summary)
---------------------------
- Source: `src/` contains the full PSP-latro C port (unity build via `src/main.c`).
- Build: `Makefile` builds `pspalatro.elf` and packages `EBOOT.PBP` (pack-pbp/pspsdk).
- Assets: `assets/src/` holds originals; `assets/out/` holds generated PSP-ready assets (16-bit PNGs, atlases, .ogg). Generated assets are LFS-tracked where appropriate.
- Gameplay: core mechanics (cards, tarot, planet cards, editions, booster packs) are largely present. Many jokers and some booster/feature systems remain stubbed.
- Testing/CI: scaffolds exist for AssetBuilder, PSPBuilder and TestRunner; Copilot artifacts and helper scripts are under `.copilot/`.

2) Missing features (high-level)
--------------------------------
These features are present in the original Balatro v0.11 but not implemented or only partially implemented in PSP-latro.

- Deck selection (unlockable decks) — not implemented. (15 decks in original)
- Vouchers / permanent upgrades (store/shop modifiers) — not implemented.
- Spectral cards (pack type exists but individual cards not implemented).
- Challenges mode (preset runs) — not implemented.
- Rich Boss Blind variety & per-card debuff system — currently a single generic boss blind only.
- Tags system (skip rewards, run modifiers) — not implemented.
- Stake / difficulty system (multiple stakes with modifiers) — not implemented.
- Save & progression (persistent unlocks, run history) — not implemented.
- Seeded runs (reproducible runs from a seed) — not implemented.
- ~55 Jokers are defined but remain `// NOT IMPLEMENTED` in code; they need data and logic implementation.

3) High-priority known issues & fixes
------------------------------------
- Background music silent on PSP hardware (device-only regression). Files of interest: `src/audio.c` (`audio_load_ogg`, `audio_update`, `audio_callback`). Recommended immediate actions:
  - Add `fopen()` null checks and return error codes in `audio_load_ogg()`.
  - Use `ov_test_callbacks()` as a precheck and call `ov_open_callbacks()` on the decode thread.
  - Instrument `ov_read()` return values and ringbuffer fill levels; write logs to `logs/audio.log` for device reproductions.
  - Ensure `audio_callback()` zero-fills `buf` on underrun.
  - Verify with PPSSPP and on-device builds, collect logs and overlay ringbuffer state.

- Audio underrun outputs garbage samples (fixed in Round 2): ensure zero-fill in underrun branch remains present and logged.

4) Performance & UX improvements (medium priority)
------------------------------------------------
- Replace full dcache flushes (`sceKernelDcacheWritebackInvalidateAll`) in hot-path draw macros with `sceKernelDcacheWritebackRange()` targeted writes.
- Increase `MAX_QUADS` (e.g., → 500) to reduce flush frequency and lower draw call count.
- Consider moving OGG decode to a dedicated low-priority kernel thread to decouple audio decode from the main loop.
- Default `settings.ini` quick wins: `overclock=true` (PSP-3000) and `speed=2` for snappier animations.
- UI polish: format large numbers with thousands separators, use `font_big` for key HUD text, map analog stick to navigation.

5) Housekeeping & repo hygiene
-----------------------------
- Generated assets and large binaries should live under `assets/out/` or `build/` and be tracked with Git LFS. Avoid committing generated binaries to `master` history when possible.
- Copilot/AI artifacts and prompts: keep templates and runners under `.copilot/` and generated outputs under `.copilot/artifacts/` on the dedicated branch `feature/copilot-artifacts`.

6) Non-original changes — auditability and policy
-------------------------------------------------
Any change that is not part of the original Balatro v0.11 source (data, scripts, docs, CI, generated assets) must be clearly auditable. The following guidelines are required for auditability:

- Record metadata for each generated artifact (store as JSON in `.copilot/artifacts/`):

```json
{
  "prompt_file": "...",
  "prompt_version": "1.0",
  "model": "<model-id>",
  "date": "YYYY-MM-DDThh:mm:ssZ",
  "regenerate_command": "./.copilot/tools/run_agent.sh ...",
  "container": "pspdev/psp-sdk:latest",
  "commit": "<git-sha>"
}
```

- For generated code/patches: include the generating command, the exact script used, and the environment (container image + tag) in the PR description.
- Branching: keep generated artifacts on `feature/copilot-artifacts` or similar; do not merge artifact blobs into `master` unless manually reviewed and accepted.
- For any binary or large asset added to the repo, provide the source (original asset), an automated script that reproduces the transform, and the exact command used.

Common non-original locations (examples)
- `.copilot/` — prompt templates, runners, skills (AI integration helpers)
- `scripts/` — asset builder, packer, deploy scripts
- `build/`, `assets/out/` — generated binaries and PSP-ready assets
- `.github/workflows/` — CI jobs that may produce artifacts

7) Files consolidated / removed (audit note)
------------------------------------------------
To reduce duplication the following files have been consolidated into this document and removed from `docs/`:

- `docs/IMPROVEMENTS.md`
- `docs/IMPROVEMENTS2.md`
- `docs/IMPROVEMENTS_SUMMARY.md`
- `docs/MISSING_FEATURES.md`

These files remain available in the repository history (use `git log -- docs/<file>` or `git show <sha>:docs/<file>` to retrieve the originals). The removed content was merged here to make triage and prioritization easier.

8) Recommended next actions (small wins)
--------------------------------------
1. ~~Implement the `audio.c` fixes and add `logs/audio.log` instrumentation; verify on-device.~~ ✅ Done (PR #14)
2. ~~Replace full dcache flushes with range-scoped writebacks and increase `MAX_QUADS`.~~ ✅ Done
3. **Implement start screen and menu system** — see §10 below.
4. Add a CI job (label-triggered) to regenerate artifacts in a pinned container and upload artifacts for review.
5. Create `docs/PSP3000_CHECKLIST.md` (device tests) and add the profiling harness per `DEVGUIDE_PSP3000.md` suggestions.

9) Contact / workflow
---------------------
If you want, I can open a PR that includes this consolidated doc and the deletion of the redundant files. I can also attach an audit-ready JSON manifest that lists every file moved/removed and the commands used.

10) Start screen & menu system
------------------------------
**Priority: High (next feature)**

The game currently drops straight into gameplay with no title screen or top-level navigation. A proper menu system is needed before the game feels shippable.

Scope:
- **Title/start screen** — PSPalatro logo, "Press X to Start" prompt, background art or animated background, background music playing from boot.
- **Main menu** — New Run, (Continue Run — gated on save system), Options, Quit. Navigable with D-pad/analog stick; X to confirm, O to back.
- **Options submenu** — at minimum: music volume, SFX volume (stubs ok for now), display overclock toggle if settings.ini supports it.
- **Run setup screen** — deck selection placeholder (single deck for now), stake selection placeholder, seed entry (stub).
- **Transition animations** — simple fade-in/fade-out between screens using the existing GU pipeline.
- **Input handling** — leverage `src/game_input.c` and `src/input.c`; debounce repeat presses.
- **State machine** — add a `GAME_STATE_MENU` (and sub-states) alongside the existing game states in `src/game.c` / `src/global.h`.

Files likely affected: `src/game.c`, `src/global.h`, `src/draw.c`, `src/game_input.c`, `src/callbacks.c`, `src/main.c`.

--
This file is the canonical snapshot of the repository state as of this commit. For prior historical versions, consult the git history of the removed files listed above.
