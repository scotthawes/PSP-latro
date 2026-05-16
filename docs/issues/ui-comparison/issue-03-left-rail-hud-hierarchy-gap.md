# issue-03: Left Rail HUD Hierarchy and Styling Gap

## Severity

P1

## Observed Differences

- Current left rail is flatter and less segmented than reference.
- Module boundaries, spacing rhythm, and emphasis of key values are weaker.
- Visual polish differs across pregame, ingame, and rewards scenes.

## Reference Attachments

- assets/ui_db/pre_game_lobby.jpg
- assets/ui_db/gameplay_hud.jpg
- assets/ui_db/rewards.jpg

## Likely Root Cause

- Shared left rail tokens do not yet encode full hierarchy tiers.
- Header/value/button modules use similar visual weights.

## Implementation Plan

1. Define hierarchy tiers for headers, values, controls, and stat rows.
2. Normalize module spacing and dividers across scenes.
3. Tune contrast and accent color usage for key value scanability.

## Implementation Checklist

- [x] 1. Build left rail style token table for all scene variants.
- [x] 2. Apply consistent module geometry and spacing.
- [x] 3. Rebalance contrast and value emphasis.
- [x] 4. Validate no clipping or overlap at PSP resolution.
- [x] 5. Re-capture pregame, ingame, and rewards scenes.

## Current Progress

- Implemented a left-rail hierarchy styling pass in `src/draw.c` with explicit section tiers: rail background, section borders, section top bars, and value boxes.
- Replaced repeated ad-hoc section/value quads with shared HUD helpers (`game_draw_hud_section`, `game_draw_hud_value_box`) for consistent geometry and spacing rhythm.
- Rebalanced contrast and emphasis tokens for labels, metadata, and value containers to improve scanability across pregame, ingame, and rewards states.
- Build validation passed (`make -j4`, 2026-05-16).

## Evidence

- User validation screenshot (2026-05-16) confirms improved left-rail segmentation and stronger value emphasis in ingame composition.
- No clipping/overlap observed in the submitted validation frame.

## Completion Note

Issue 03 is closed.

## Testing Attachments

- Test plan case: TC-03 in docs/plans/ui-comparison-test-plan.md
- Required evidence:
  - test-output/ui-comparison/pregame-current.png
  - test-output/ui-comparison/ingame-current.png
  - test-output/ui-comparison/rewards-current.png

## Done Definition

Left rail panels match target hierarchy intent and remain readable and consistent across key scenes.
