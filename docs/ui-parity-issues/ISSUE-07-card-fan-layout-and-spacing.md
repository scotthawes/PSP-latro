# ISSUE-07: Card Fan Layout and Spacing Mismatch

## Severity

P2

## Observed Differences

- Card hand appears more rigid and boxy than reference.
- Overlap/fan rhythm and hover emphasis differ.

## Likely Root Cause

- Card transform math uses fixed spacing and limited tilt.
- Missing dynamic spacing adjustment based on hand count.
- Selection animation offsets not tuned to final composition.

## Proposed Solutions

1. Rework fan function to derive spacing/rotation from hand size.
2. Add mild curvature and center bias for visual balance.
3. Tune selected card lift and spacing so interaction remains clear.

## Implementation Checklist

- [x] 1. Locate hand layout function and extract constants.
- [x] 2. Implement hand-size adaptive spacing.
- [x] 3. Implement symmetric rotation curve around center card.
- [x] 4. Tune selected-card offset to avoid label overlap.
- [x] 5. Validate readability on 8-card and 5-card scenarios.

## Current Progress

- Hand layout path updated in `game_set_card_hand_positions()` and `game_set_card_hand_final_positions()`.
- Added shared helper that computes per-card fan layout from hand size and card index.
- Spacing now adapts by hand size (denser at larger hand sizes, slightly wider at lower counts).
- Added symmetric rotation curve centered on the middle card.
- Reduced selected-card lift from the previous 10px behavior to 7px while keeping emphasis clear.
- Per screenshot feedback, fan was retuned for flatter tilt and better low-count spread to reduce overlap pressure.
- Root-cause fix: renderer rotation uses radians (`sceGumRotateZ`), so fan angle constants were normalized to radian-scale values to remove over-rotation.

## Evidence

- Build validation succeeded after the hand-layout update (`make -j4`, 2026-05-16).
- Build validation succeeded after retuning pass (`make -j4`, 2026-05-16).
- Build validation succeeded after radian-angle correction (`make -j4`, 2026-05-16).
- User-provided PPSSPP verification captures (2026-05-16):
	- 8-card hand state confirmed without over-rotation and with readable overlap.
	- Reduced-card state (post-play hand) confirmed with stable tilt and spacing.

## Completion Note

Issue 7 acceptance met after radian-angle correction and final screenshot validation.

## Done Definition

- Hand fan has smoother arc and closer reference spacing dynamics.
