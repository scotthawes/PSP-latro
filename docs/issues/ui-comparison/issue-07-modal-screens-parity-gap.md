# issue-07: Modal Screens Parity Gap (Settings, Language, Credits, Run Setup, Game Over)

## Severity

P1

## Observed Differences

- Current comparison set is incomplete for modal-heavy screens.
- Settings tabs, language screen, credits pages, run setup, and game over need explicit parity checks against reference.

## Reference Attachments

- assets/ui_db/balatro_game_options.jpg
- assets/ui_db/balatro_video_options.jpg
- assets/ui_db/balatro_graphics_options.jpg
- assets/ui_db/balatro_audio_options.jpg
- assets/ui_db/balatro_language_options.jpg
- assets/ui_db/balatro_credits.jpg
- assets/ui_db/balatro_credits_2.jpg
- assets/ui_db/custom_game_rules.jpg
- assets/ui_db/failure_game_over.jpg

## Likely Root Cause

- Modal parity has not been validated in the same structured workflow as gameplay scenes.

## Implementation Plan

1. Build a modal capture checklist and capture all required current screens.
2. Compare panel geometry, tab bars, spacing, and button treatment.
3. Patch shared modal tokens first, then screen-specific deltas.

## Implementation Checklist

- [ ] 1. Capture all modal screens listed in UI_COMPARISON_TEST_PLAN.
- [x] 2. Compare tab strip, panel radii, borders, and button geometry.
- [x] 3. Tune label density and spacing for readability parity.
- [x] 4. Validate language list and credits layout against target.
- [ ] 5. Validate run setup and game over modal composition.
- [ ] 6. Re-capture and attach before/after evidence.

## Progress Notes

- Added dedicated Language and Credits modal overlays inside Options flow for parity iteration.
- Language modal supports up/down navigation and select/back close behavior.
- Credits modal is single-page in current scope, with close behavior on X/O.
- Captures received and reviewed for: main menu entry state, options base screen, language modal, and credits modal page 1.
- Capture received and reviewed for: game over modal panel.
- Remaining capture target for issue-07 completion: run setup modal (if/when reachable in this build).
- Decision: keep issue-07 open and non-complete for now; run setup parity is deferred as a larger feature scope.

Modal capture locations in current build:
- Language modal: Main Menu -> Options -> Language
- Credits modal: Main Menu -> Options -> Credits
- Game over modal: appears after run loss in gameplay

## Testing Attachments

- Test plan case: TC-07 in docs/plans/ui-comparison-test-plan.md
- Required evidence:
  - test-output/ui-comparison/settings-game-current.png
  - test-output/ui-comparison/settings-video-current.png
  - test-output/ui-comparison/settings-graphics-current.png
  - test-output/ui-comparison/settings-audio-current.png
  - test-output/ui-comparison/language-current.png
  - test-output/ui-comparison/credits-current.png
  - test-output/ui-comparison/run-setup-current.png
  - test-output/ui-comparison/game-over-current.png

## Done Definition

All major modal screens are parity-reviewed with evidence and no major geometry or readability drift.

## Status

Not complete as of 2026-05-16. Partially delivered (settings/language/credits/game over), with run setup deferred to a larger feature pass.
