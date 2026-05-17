# issue-09: Settings and Tabbed Dialog Structure Gap

## Severity

P1

## Observed Differences

- Settings dialogs still need a tighter match to the target frame proportions and tab strip treatment.
- Slider placement, label spacing, and the back-bar treatment should remain consistent across each option screen.

## Reference Attachments

- assets/ui_db/balatro_game_options.jpg
- assets/ui_db/balatro_video_options.jpg
- assets/ui_db/balatro_graphics_options.jpg
- assets/ui_db/balatro_audio_options.jpg

## Likely Root Cause

- The current modal layout is functional, but the dialog shell and tab strip are using a looser PSP-safe layout than the target.

## Implementation Plan

1. Capture each settings tab at the locked comparison profile.
2. Compare modal frame proportions, tab strip spacing, and slider geometry.
3. Patch the shared modal and tabbed-screen layout logic.

## Implementation Checklist

- [ ] 1. Capture settings-current.png for each tab.
- [ ] 2. Validate modal width, height, and border weight.
- [ ] 3. Validate tab strip sizing and active-state emphasis.
- [ ] 4. Validate slider track, thumb, and label spacing.
- [ ] 5. Confirm back-bar placement and readability.

## Testing Attachments

- Test plan case: TC-07 in docs/plans/ui-comparison-test-plan.md
- Required evidence:
  - test-output/ui-comparison/settings-game-current.png
  - test-output/ui-comparison/settings-video-current.png
  - test-output/ui-comparison/settings-graphics-current.png
  - test-output/ui-comparison/settings-audio-current.png

## Done Definition

Tabbed settings dialogs match the target shell, spacing, and slider composition closely enough that the layout reads as the same UI family.

## Status

Open.