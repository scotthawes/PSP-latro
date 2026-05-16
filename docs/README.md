# Project documentation

This folder collects repository documentation, developer guides, and improvement trackers.

Files

- `DEVGUIDE_PSP3000.md` — Developer guide and workflows for PSP-3000 builds and assets.
- `COPILOT_GUIDE.md` — Copilot/AI usage guide and reproducibility rules.
- `CONSOLIDATED_STATUS.md` — Consolidated current state, missing features, and prioritized improvements.
- `UI_PARITY_ISSUE_COLLATION.md` — Master list of screenshot parity issues, priorities, and global checklist.
- `UI_PARITY_TEST_PLAN.md` — End-to-end parity validation plan, test matrix, and exit criteria.
- `TARGET_VS_PSP_COMPARISON_2026-05-16.md` — Consolidated target-versus-PSP post-parity comparison verdict by scene.
- `UI_COMPARISON_ISSUE_COLLATION.md` — Post-parity issue tracker for remaining current-versus-reference UI gaps.
- `UI_COMPARISON_TEST_PLAN.md` — Test plan and evidence matrix for post-parity comparison pass.
- `MENU_FEATURE_WORKFLOW.md` — Task list and implementation/diagnostic workflow for menu and boot-path feature work.
- `MENU_DESIGN_RESOURCES.md` — Source/reference map for menu design (original game scripts, PSPSDK samples, runtime notes).
- `ui-parity-issues/` — One document per pregame/ingame parity issue with root cause, solutions, and implementation checklist.
- `ui-comparison-issues/` — One document per remaining post-parity UI difference with plan, checklist, and testing attachments.
- `psxplace/` — (optional) archived PSX-Place threads and fetch artifacts.

Removed / Consolidated

- `IMPROVEMENTS.md`, `IMPROVEMENTS2.md`, `IMPROVEMENTS_SUMMARY.md`, and `MISSING_FEATURES.md` were consolidated into `CONSOLIDATED_STATUS.md` to reduce duplication and centralize triage. The originals remain available in git history.

Guidance

- Keep authored asset documentation and how-tos in this folder.
- `assets/src/` should contain source art/audio; `assets/out/` holds generated files.
- Prefer small, focused docs; long procedural guides belong in `docs/` but reference scripts in `scripts/`.

If you want, I can split the consolidated status into smaller per-area trackers (audio, graphics, assets) after triage.
