# issue-11: Run Setup and Game Over Modal Parity Gap

## Severity

P1

## Observed Differences

- The run setup flow still needs a more faithful comparison pass against the target custom-rules presentation.
- The game-over modal should better match the target width, score block proportions, and button hierarchy.

## Reference Attachments

- assets/ui_db/custom_game_rules.jpg
- assets/ui_db/failure_game_over.jpg

## Likely Root Cause

- The current modal flow is functional, but the setup and end-state overlays use layout rules that do not yet mirror the reference.

## Implementation Plan

1. Capture run setup and game over screens at the locked comparison profile.
2. Compare card spacing, modal width, score block proportions, and button hierarchy.
3. Patch the shared modal flow and end-state overlay layout.

## Implementation Checklist

- [ ] 1. Capture run-setup-current.png and game-over-current.png.
- [ ] 2. Validate deck preview placement and rule-card spacing.
- [ ] 3. Validate game-over modal width and stat block proportions.
- [ ] 4. Validate button hierarchy and footer spacing.
- [ ] 5. Re-capture for signoff.

## Testing Attachments

- Test plan case: TC-07 in docs/plans/ui-comparison-test-plan.md
- Required evidence:
  - test-output/ui-comparison/run-setup-current.png
  - test-output/ui-comparison/game-over-current.png

## Done Definition

Run setup and game-over modals visually match the target's framing, hierarchy, and overall composition.

## Status

Open.