# issue-02: Pregame Blind Card Composition Gap

## Severity

P1

## Observed Differences

- Current blind cards read as flat color panels with reduced internal detail.
- Action grouping tied to blind selection is less explicit than in reference.
- Title, reward, and penalty hierarchy is weaker than reference.

## Reference Attachments

- assets/ui_db/pre_game_lobby.jpg

## Likely Root Cause

- Blind panel renderer prioritizes simplified PSP-safe blocks over layered internals.
- Select and skip action composition is not aligned to the reference structure.

## Implementation Plan

1. Rebuild blind card internals as layered components with strict spacing tokens.
2. Restore explicit action grouping for select and skip interactions.
3. Tune icon and text hierarchy for blind type readability.

## Implementation Checklist

- [x] 1. Define blind card component geometry table.
- [x] 2. Implement layered internal sections and badges.
- [x] 3. Align select/skip interaction block to reference intent.
- [x] 4. Tune text and icon emphasis on all three blind types.
- [x] 5. Validate readability and clipping at 480x272.

## Current Progress

- Updated `game_draw_blind_select` in `src/draw.c` with stronger panel layering (inner card body, separators, reward row) to reduce flat placeholder appearance.
- Added explicit selected-blind action cluster at card bottom with `Select` and `Skip Blind` visual grouping to better mirror reference structure.
- Added unavailable-state footer badge (`Locked`) for non-selected blinds to improve hierarchy clarity.
- Build validation passed (`make -j4`, 2026-05-16).

## Evidence

- User validation screenshot (2026-05-16) confirms blind card composition refresh is readable, unclipped, and closer to reference hierarchy.
- Selected blind now presents explicit action grouping, and non-selected blinds retain clear locked-state communication.

## Completion Note

Issue 02 is closed.

## Testing Attachments

- Test plan case: TC-02 in docs/plans/ui-comparison-test-plan.md
- Required evidence:
  - test-output/ui-comparison/pregame-current.png

## Done Definition

Pregame blind cards communicate identity, actions, and values with reference-like hierarchy and clarity.
