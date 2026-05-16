# ISSUE-09: Deck Widget Scale and Placement Difference

## Severity

P2

## Observed Differences

- Deck card stack appears differently sized/positioned than reference.
- Counter placement and edge padding feel misaligned.

## Likely Root Cause

- Deck widget uses inherited defaults not tuned for final layout.
- Right-side safe-area and margin logic differs between scenes.

## Proposed Solutions

1. Set explicit deck widget anchor, width, and bottom offset per scene.
2. Normalize counter text alignment relative to deck sprite.
3. Apply consistent safe-area padding constants.

## Implementation Checklist

- [x] 1. Trace deck widget placement code path.
- [x] 2. Replace magic numbers with constants.
- [x] 3. Tune anchor and offsets against reference screenshot.
- [x] 4. Validate with multiple hand sizes and states.

## Current Progress

- Refactored deck widget layout into explicit constants and helper-based anchor computation in `game_draw_deck()`.
- Added scene-aware anchor behavior (shop offset) and safe-area-based right/bottom placement.
- Applied tuned deck sprite scale and normalized deck counter alignment in a consistent value-row style.

## Evidence

- Build validation succeeded after deck widget refactor (`make -j4`, 2026-05-16).
- User-provided PPSSPP validation captures (2026-05-16):
	- Ingame hand state confirms right-side composition and counter legibility.
	- Cash-out state confirms deck anchor consistency with overlay/panel state.
	- Shop state confirms scene offset behavior and safe-area margin.

## Completion Note

Issue 9 closure criteria met after multi-scene screenshot validation.

## Done Definition

- Deck widget location and scale match expected right-side composition.
