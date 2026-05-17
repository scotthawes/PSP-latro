# issue-04: Gameplay Action Row and Control Affordance Gap

## Severity

P1

## Observed Differences

- Current action row appears denser and less legible than reference.
- Primary and secondary button groups are not as clearly distinguished.
- Label clarity and control affordance are reduced.

## Reference Attachments

- assets/ui_db/gameplay_hud.jpg

## Likely Root Cause

- Action row dimensions and text scaling are not tuned to the same hierarchy as reference.
- Color and border treatments do not clearly separate button roles.

## Implementation Plan

1. Define role-based button tokens for primary, secondary, and utility controls.
2. Update action row spacing and alignment with card fan and deck region.
3. Tune text scale and contrast for control labels.

## Implementation Checklist

- [x] 1. Create action row layout token set.
- [x] 2. Apply role-based styling for each control type.
- [x] 3. Align action row anchors to gameplay composition grid.
- [x] 4. Verify readability at native PSP resolution.
- [x] 5. Capture and compare updated ingame screenshot.

## Current Progress

- Updated `game_draw_ingame_hand` in `src/draw.c` with role-based action-row layout tokens for primary actions and utility sort controls.
- Increased separation between `Play Hand`, sort controls, and `Discard` to reduce density and improve visual scanability.
- Refined sort control styling with clearer utility-cluster framing and focused sub-button emphasis.
- Build validation passed (`make -j4`, 2026-05-16).

## Evidence

- User validation screenshot (2026-05-16) confirms improved action-row readability and clearer role separation between primary and utility controls.
- Updated control strip shows better spacing and button affordance alignment with the comparison target.

## Completion Note

Issue 04 is closed.

## Testing Attachments

- Test plan case: TC-04 in docs/plans/ui-comparison-test-plan.md
- Required evidence:
  - test-output/ui-comparison/ingame-current.png

## Done Definition

Gameplay action controls are immediately scannable and role-distinct, matching target interaction clarity.
