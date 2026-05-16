# ISSUE-06: Title and Main Menu Layout Parity Gap

## Severity

P1

## Observed Differences

- Current title and main menu need a fresh parity pass against latest reference captures.
- Logo scale, action bar geometry, and utility chip placement need strict side-by-side validation.

## Reference Attachments

- assets/ui_db/Balatro_title.jpg
- assets/ui_db/balatro_menu.jpg

## Likely Root Cause

- Existing title/menu tuning was done in a prior pass and may drift from latest target references.

## Implementation Plan

1. Capture current title and menu at locked profile settings.
2. Compare geometry and spacing against references.
3. Patch logo, action bar, profile block, and utility chips as needed.

## Implementation Checklist

- [x] 1. Capture title-current.png and menu-current.png.
- [x] 2. Validate logo scale and vertical placement.
- [x] 3. Validate action bar sizing and spacing.
- [x] 4. Validate profile/language/social chip alignment.
- [x] 5. Apply deltas and re-capture for signoff.

## Testing Attachments

- Test plan case: TC-06 in docs/UI_COMPARISON_TEST_PLAN.md
- Required evidence:
  - test-output/ui-comparison/title-current.png
  - test-output/ui-comparison/menu-current.png

## Done Definition

Title and main menu composition match target layout intent with no notable placement or hierarchy drift.

## Status

Closed on 2026-05-16 after title-screen split and main-menu geometry/color alignment pass validated by updated user captures.
