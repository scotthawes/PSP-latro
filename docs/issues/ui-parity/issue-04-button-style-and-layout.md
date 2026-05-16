# issue-04: Action Button Style and Alignment Drift

## Severity

P1

## Observed Differences

- Button proportions, spacing, and color contrast differ from reference.
- Interaction controls look functional but not visually aligned with target design.

## Likely Root Cause

- Inconsistent button sizing rules across screens.
- Mixed typography metrics due to font/render variance.
- Missing final state colors and border tokens.

## Proposed Solutions

1. Introduce shared button style tokens (height, radius, border, padding, text offset).
2. Use one layout helper for horizontal action bars.
3. Normalize selected/hover/disabled state palette.

## Implementation Checklist

- [x] 1. Audit button draw calls in pregame and ingame.
- [x] 2. Refactor to shared button style constants.
- [x] 3. Apply consistent spacing and baseline alignment.
- [x] 4. Validate state colors against readability.
- [x] 5. Re-capture screenshots for final comparison.

## Current Progress

- Added shared button tokens and a common draw helper (`game_draw_action_button`) in `src/draw.c`.
- Refactored hand action buttons (`Play Hand`, `Discard`) to use the shared helper with unified border/focus/text/icon layout.
- Refactored shop action buttons (`Next Round`, `Reroll`) to use the shared helper, including two-line label handling for reroll cost.
- Refactored card-side action buttons (`Buy`, `Sell`, `Use`, `Open`, `Select`) through the shared path via `game_draw_item_button`.

## Evidence

- Build verification: `make -j4` succeeded after the button style/layout refactor.
- Visual verification (2026-05-15 user-provided captures):
	- Ingame action bar: shared style and alignment visible for `Play Hand` and `Discard` controls.
	- Post-game cashout scene: action/control style consistency preserved with updated baseline and border rhythm.
	- Shop scene: `Next Round` and `Reroll` buttons match shared spacing, sizing, and state-contrast treatment.

## Remaining For Completion

- None. Issue 4 completion criteria are satisfied.

## Done Definition

- Buttons match target sizing rhythm and state clarity.
