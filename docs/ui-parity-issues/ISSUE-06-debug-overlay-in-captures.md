# ISSUE-06: Debug Overlay Visible in Captures

## Severity

P1

## Observed Differences

- PPSSPP debug text is visible in screenshots.
- Overlay introduces non-game visual noise and skews parity review.

## Likely Root Cause

- Emulator debug stats enabled during capture workflow.
- No standardized screenshot capture profile/checklist.

## Proposed Solutions

1. Define capture profile settings for PPSSPP parity screenshots.
2. Add pre-capture checklist step to disable all overlays.
3. Store capture settings in docs for repeatability.

## Implementation Checklist

- [x] 1. Document exact PPSSPP settings for parity captures.
- [x] 2. Add no-overlay verification step before screenshot.
- [x] 3. Re-capture baseline pregame and ingame screens.
- [x] 4. Replace previous debug-overlay images in review set.

## Current Progress

- Added explicit capture-profile document: `docs/PPSSPP_CAPTURE_PROFILE.md`.
- Updated parity test plan to require both PPSSPP overlay-off state and `debug_overlay = false` in PSP-latro settings.
- Added runtime setting guard for in-game debug overlay draw path (`g_settings.debug_overlay`) so capture workflow can reliably suppress overlay text.

## Evidence

- New capture profile/checklist doc created: `docs/PPSSPP_CAPTURE_PROFILE.md`.
- Test plan now references the capture profile and explicit no-overlay requirements: `docs/UI_PARITY_TEST_PLAN.md`.
- Updated overlay-free parity captures provided (2026-05-15):
	- Pregame baseline capture (top-right debug text absent).
	- Ingame baseline capture (top-right debug text absent).

## Completion Note

Issue closure criteria satisfied by runtime overlay gating + updated clean baseline captures.

## Done Definition

- Parity screenshots contain only game UI, no emulator debug text.
