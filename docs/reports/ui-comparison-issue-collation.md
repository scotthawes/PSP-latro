# UI Comparison Issue Collation (Post-Parity)

Date: 2026-05-16
Branch context: feature/menu-system

## Purpose

Track the next comparison wave between:

- Current PSP-latro screenshots
- Actual game references in assets/ui_db/

This is a separate track from the previously closed parity set (issue-01 through issue-11).

## Issue Index

1. Text and Glyph Legibility Regression
   - File: docs/issues/ui-comparison/issue-01-text-glyph-legibility-regression.md
   - Priority: P0
   - Affects: Pregame, Ingame

2. Pregame Blind Card Composition Gap
   - File: docs/issues/ui-comparison/issue-02-pregame-blind-card-composition-gap.md
   - Priority: P1
   - Affects: Pregame

3. Left Rail HUD Hierarchy and Styling Gap
   - File: docs/issues/ui-comparison/issue-03-left-rail-hud-hierarchy-gap.md
   - Priority: P1
   - Affects: Pregame, Ingame, Rewards

4. Gameplay Action Row and Control Affordance Gap
   - File: docs/issues/ui-comparison/issue-04-gameplay-action-row-affordance-gap.md
   - Priority: P1
   - Affects: Ingame

5. Gameplay Table Depth and Composition Atmosphere Gap
   - File: docs/issues/ui-comparison/issue-05-gameplay-table-depth-composition-gap.md
   - Priority: P2
   - Affects: Ingame, Scoring
   - Current implementation: static `galaxy_15` background image for small and big blinds; animation removed

6. Title and Main Menu Layout Parity Gap
   - File: docs/issues/ui-comparison/issue-06-title-main-menu-parity-gap.md
   - Priority: P1
   - Affects: Title, Menu

7. Modal Screens Parity Gap (Settings, Language, Credits, Run Setup, Game Over)
   - File: docs/issues/ui-comparison/issue-07-modal-screens-parity-gap.md
   - Priority: P1
   - Affects: Menu Modals
   - Current status: partial complete; run setup modal parity deferred to larger feature work

8. Title and Main Menu Layout Parity Gap
   - File: docs/issues/ui-comparison/issue-08-title-main-menu-layout-parity-gap.md
   - Priority: P1
   - Affects: Title, Menu

9. Settings and Tabbed Dialog Structure Gap
   - File: docs/issues/ui-comparison/issue-09-settings-and-tabbed-dialog-structure-gap.md
   - Priority: P1
   - Affects: Menu Modals

10. Language and Credits Panel Parity Gap
   - File: docs/issues/ui-comparison/issue-10-language-and-credits-panel-parity-gap.md
   - Priority: P2
   - Affects: Menu Modals

11. Run Setup and Game Over Modal Parity Gap
   - File: docs/issues/ui-comparison/issue-11-run-setup-and-game-over-modal-parity-gap.md
   - Priority: P1
   - Affects: Menu Modals

## Recommended Delivery Order

1. P0 first: issue-01
2. Pregame/ingame foundations: issue-03, issue-02, issue-04
3. Composition polish: issue-05
5. Menu and modal parity: issue-06, issue-07, issue-08, issue-09, issue-10, issue-11
6. Full comparison rerun and signoff

## Cross-Issue Dependencies

- issue-01 gates all UI readability checks.
- issue-03 influences perceived quality of issue-02 and issue-04.
- issue-06 and issue-07 should be validated with the same capture profile to avoid false diffs.
- issue-07 through issue-11 should be validated with the same capture profile to avoid false diffs.

## Validation Source Set

Reference images and clips are under assets/ui_db/:

- pre_game_lobby.jpg
- gameplay_hud.jpg
- gameplay_huds_2.jpg
- rewards.jpg
- Balatro_title.jpg
- balatro_menu.jpg
- balatro_game_options.jpg
- balatro_video_options.jpg
- balatro_graphics_options.jpg
- balatro_audio_options.jpg
- balatro_language_options.jpg
- balatro_credits.jpg
- balatro_credits_2.jpg
- custom_game_rules.jpg
- failure_game_over.jpg

## Global Checklist

- [x] 1. Confirm text and glyph legibility parity in pregame and ingame.
- [x] 2. Align blind card composition and action cluster presentation with reference.
- [x] 3. Match left rail hierarchy, spacing, and emphasis treatment.
- [x] 4. Refine gameplay action row control sizing, color coding, and labels.
- [x] 5. Improve table/background depth while preserving readability.
- [x] 6. Validate title and main menu composition against references.
- [ ] 7. Validate settings/language/credits/run setup/game over modal parity.
- [ ] 8. Validate title and main menu parity in the same capture pass.
- [ ] 9. Capture fresh rerun screenshots and attach evidence.
- [ ] 10. Perform side-by-side signoff and close all open items.

## Primary Test Plan

- docs/plans/ui-comparison-test-plan.md
