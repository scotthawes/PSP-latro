# Project documentation

This folder collects repository documentation, developer guides, and improvement trackers.

Files

- `DEVGUIDE_PSP3000.md` — Developer guide and workflows for PSP-3000 builds and assets.
- `COPILOT_GUIDE.md` — Copilot/AI usage guide and reproducibility rules.
- `CONSOLIDATED_STATUS.md` — Consolidated current state, missing features, and prioritized improvements.
- `psxplace/` — (optional) archived PSX-Place threads and fetch artifacts.

Removed / Consolidated

- `IMPROVEMENTS.md`, `IMPROVEMENTS2.md`, `IMPROVEMENTS_SUMMARY.md`, and `MISSING_FEATURES.md` were consolidated into `CONSOLIDATED_STATUS.md` to reduce duplication and centralize triage. The originals remain available in git history.

Guidance

- Keep authored asset documentation and how-tos in this folder.
- `assets/src/` should contain source art/audio; `assets/out/` holds generated files.
- Prefer small, focused docs; long procedural guides belong in `docs/` but reference scripts in `scripts/`.

If you want, I can split the consolidated status into smaller per-area trackers (audio, graphics, assets) after triage.
