# issue-08: HUD Hierarchy and Legibility Regression

## Severity

P1

## Observed Differences

- HUD counters and labels are harder to parse quickly.
- Visual grouping and emphasis hierarchy differ from reference.
- Some panels blend into background due to low contrast.

## Likely Root Cause

- Incomplete typography scale system.
- Inconsistent panel contrast and spacing.
- Missing icon-led hierarchy cues in certain blocks.

## Proposed Solutions

1. Define type scale tiers: title, section, value, metadata.
2. Increase contrast for value rows and critical counters.
3. Re-group HUD blocks with consistent padding and separators.
4. Add icon anchors where reference relies on quick recognition.

## Implementation Checklist

- [x] 1. Create HUD typography and spacing token sheet.
- [x] 2. Apply panel contrast adjustments.
- [x] 3. Standardize numeric value emphasis.
- [x] 4. Re-test readability at native PSP scale.
- [x] 5. Confirm parity with side-by-side screenshot check.

## Current Progress

- Added left-HUD style tokens in `draw.c` for background, section, value-row, and label/meta contrast tiers.
- Updated `game_draw_left_info()` to use consistent section/value row treatment across score, hand summary, hands/discards, wealth, and ante/round blocks.
- Increased value-row contrast by adding accent+inner rows for numeric fields to improve glance readability.

## Evidence

- Build validation succeeded after HUD hierarchy pass (`make -j4`, 2026-05-16).
- User-provided native-scale PPSSPP captures (2026-05-16) validated both:
	- Pregame HUD readability and block contrast.
	- Ingame HUD readability for score/counters/value rows.

## Completion Note

Issue 8 closure criteria satisfied for HUD hierarchy/contrast readability at native PSP scale.

## Done Definition

- HUD data can be read at a glance with clear visual hierarchy.
