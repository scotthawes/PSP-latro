# UI Parity Issue Collation

Date: 2026-05-16
Branch context: feature/menu-system

## Purpose

This document collates all issues raised during screenshot comparison between:

- Reference Balatro pregame/ingame screenshots
- Current PSP-latro pregame/ingame screenshots

Each issue has its own detailed document with:

- Symptoms (pregame and ingame)
- Likely root cause
- Proposed solutions
- Implementation checklist

Primary validation guide:

- docs/UI_PARITY_TEST_PLAN.md

## Issue Index

1. Garbled Text Rendering
   - File: docs/ui-parity-issues/ISSUE-01-font-rendering-garbled-text.md
   - Priority: P0
   - Affects: Pregame, Ingame
2. Missing UI Assets and Fallback Visuals
   - File: docs/ui-parity-issues/ISSUE-02-missing-ui-assets-and-fallbacks.md
   - Priority: P0
   - Affects: Pregame, Ingame
3. Blind Panel Style Mismatch
   - File: docs/ui-parity-issues/ISSUE-03-blind-panel-visual-parity.md
   - Priority: P1
   - Affects: Pregame
4. Action Button Style and Alignment Drift
   - File: docs/ui-parity-issues/ISSUE-04-button-style-and-layout.md
   - Priority: P1
   - Affects: Pregame, Ingame
5. Background Depth and Atmosphere Gap
   - File: docs/ui-parity-issues/ISSUE-05-background-depth-and-texture.md
   - Priority: P1
   - Affects: Pregame, Ingame
6. Debug Overlay Visible in Gameplay Captures
   - File: docs/ui-parity-issues/ISSUE-06-debug-overlay-in-captures.md
   - Priority: P1
   - Affects: Pregame, Ingame
7. Card Fan Layout and Spacing Mismatch
   - File: docs/ui-parity-issues/ISSUE-07-card-fan-layout-and-spacing.md
   - Priority: P2
   - Affects: Ingame
8. HUD Hierarchy and Legibility Regression
   - File: docs/ui-parity-issues/ISSUE-08-hud-hierarchy-and-legibility.md
   - Priority: P1
   - Affects: Pregame, Ingame
9. Deck Widget Scale and Placement Difference
   - File: docs/ui-parity-issues/ISSUE-09-deck-widget-scale-placement.md
   - Priority: P2
   - Affects: Ingame
10. Global Composition and Density Drift
   - File: docs/ui-parity-issues/ISSUE-10-global-spacing-composition.md
   - Priority: P1
   - Affects: Pregame, Ingame

## Recommended Delivery Order

1. Resolve all P0 issues first.
2. Resolve P1 issues in this order: 03, 08, 04, 05, 10, 06.
3. Resolve P2 issues: 07, 09.
4. Run full screenshot parity pass.

## Cross-Issue Dependencies

- ISSUE-01 is foundational because text metrics affect button and panel layout.
- ISSUE-02 gates multiple style tasks because fallback visuals hide true parity state.
- ISSUE-08 and ISSUE-10 should be finalized after ISSUE-01 and ISSUE-02.
- ISSUE-06 should be enforced in capture workflow before final comparison.

## Testing Plan

- End-to-end test plan: docs/UI_PARITY_TEST_PLAN.md
- Run the test plan after each completed issue group (P0, then P1, then P2).
- Do not close any issue until its related test cases pass.

## Global Implementation Checklist

- [x] 1. Confirm font atlas load, glyph map, and UTF-8/encoding path are correct.
- [x] 2. Verify all required pregame/ingame UI textures are present and mapped.
- [x] 3. Add missing asset logging for unresolved texture keys.
- [x] 4. Rebuild blind panels with color and hierarchy parity.
- [x] 5. Normalize action button dimensions, spacing, and state colors.
- [x] 6. Apply background layering pass (base texture + subtle overlays).
- [x] 7. Update card fan transform math to target spacing and overlap.
- [x] 8. Rebalance HUD typography, contrast, and grouping.
- [x] 9. Tune deck widget scale, padding, and counter placement.
- [x] 10. Enforce screenshot capture profile with debug overlays off.
- [x] 11. Capture updated pregame/ingame screenshots in PPSSPP.
- [x] 12. Perform side-by-side visual signoff and close issues.

## Final Signoff

Date: 2026-05-16

- Side-by-side validation completed across pregame, ingame, cash-out overlay, and shop scenes.
- Final screenshot pass confirmed no debug overlay contamination and improved parity composition.
- Issue trackers ISSUE-01 through ISSUE-10 are marked complete.

## Acceptance Criteria

- All critical text is readable and stable in both scenes.
- No placeholder/fallback visuals remain in targeted UI components.
- Blind cards, HUD, action controls, and deck area visually match intended reference style.
- Capture screenshots are free of emulator debug overlays.
- Remaining differences are intentional PSP constraints, documented explicitly.
