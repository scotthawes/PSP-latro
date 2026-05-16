# issue-08: Title and Main Menu Layout Parity Gap

## Severity

P1

## Observed Differences

- The title composition still needs a direct side-by-side pass against the latest target capture.
- Logo scale, main button row geometry, and utility chip placement should match the target more closely.

## Further Differences Checklist

- [ ] Title logo is still slightly too large relative to the target composition.
- [ ] Title logo still sits a little too high and dominates the upper half of the frame.
- [ ] The Ace card overlay remains visually prominent enough to compete with the wordmark.
- [ ] The lower-right utility cluster still reads less polished than the target icons.
- [ ] The left utility chip still leans warmer than the reference blue-toned chip.
- [ ] The right utility chip still reads too neutral and square compared with the reference button treatment.
- [ ] Utility chip icon scale still needs final tuning for crisp readability without looking oversized.
- [ ] Utility chip padding and border weight still need a last pass to match the target chrome.
- [ ] The main action row is close, but the final spacing/weight should be frozen to prevent future drift.
- [ ] Profile chip needs to stay on the same safe-area anchor if the bottom bar is adjusted again.
- [ ] Language chip needs to preserve its right-side alignment if icon or text assets change.
- [ ] Version text should remain subtle and consistently anchored in the top-right corner.
- [ ] Title/menu layout should not regress when texture budgets or fallback assets change.

## Reference Attachments

- assets/ui_db/Balatro_title.jpg
- assets/ui_db/balatro_menu.jpg

## Likely Root Cause

- Title and menu tuning is close, but the current layout likely drifted from the reference during later menu-system work.

## Implementation Plan

1. Capture current title and menu screens at the locked comparison profile.
2. Compare logo placement, action row spacing, and corner chip alignment.
3. Patch the title/menu geometry and styling deltas.

## Proposed Fixes

- Reduce the logo scale slightly and lock its vertical anchor so it stays centered without overpowering the frame.
- Keep the Ace card overlay proportional to the logo bounds so it reads as part of the mark, not a floating card.
- Normalize the utility chips with a shared helper that enforces fill, border, padding, icon scale, and border radius together.
- Use explicit color tokens for the left and right utility chips so the blue/dark pairing remains stable across future menu changes.
- Freeze the main action row geometry in a single layout block instead of letting later visual tweaks drift individual buttons.
- Keep profile and language chips tied to the same safe-area math as the action row so bottom chrome stays coherent.

## Long-Term Reliability And Usability

- Promote repeated chip and button geometry values into named constants to avoid magic-number drift.
- Route all title utility icons through one validated asset-loading path so missing or wrong-cropped textures fail visibly.
- Keep fallback behavior deterministic: if a texture is missing, draw a controlled placeholder and log the asset key.
- Preserve the current layout as a shared title-screen baseline so future menu features can inherit it instead of reimplementing it.
- Reuse the same capture profile for every future title/menu comparison so usability regressions are visible and reproducible.
- Document any intentional PSP-specific simplifications separately so they do not get mistaken for parity defects later.

## Implementation Checklist

- [ ] 1. Capture title-current.png and menu-current.png.
- [ ] 2. Compare the title logo against the reference for final scale and vertical placement.
- [ ] 3. Check the Ace overlay so it sits inside the logo composition instead of competing with it.
- [ ] 4. Validate the action row sizing, spacing, and color hierarchy.
- [ ] 5. Verify profile chip, language chip, and version label safe-area anchors.
- [ ] 6. Verify utility chip icon scale, fill color, border weight, and padding.
- [ ] 7. Confirm the utility chip pair reads as polished icon buttons, not generic squares.
- [ ] 8. Re-capture for signoff and freeze the final title/menu constants.

## Testing Attachments

- Test plan case: TC-06 in docs/plans/ui-comparison-test-plan.md
- Required evidence:
  - test-output/ui-comparison/title-current.png
  - test-output/ui-comparison/menu-current.png

## Done Definition

Title and main menu composition match the target layout intent with no notable placement or hierarchy drift.

## Status

Open.

## Current Progress

- Latest capture confirms the main action row, profile chip, and language chip are close to the target composition.
- Remaining title-screen gap is concentrated in the logo scale/placement and the lower-right utility chip presentation.
- Lower-right utility chips now use the correct icon art family and a blue/dark contrast pair; the remaining gap should be final alignment/weight tuning rather than source discovery.
- Checklist progress: title logo scale/placement is being tightened again before moving on to the Ace overlay and utility chip polish.
- Checklist progress: the title logo has now been split into a wordmark plus a separately controlled Ace overlay, and the overlay has been shrunk and lowered to reduce visual competition with the wordmark.
- Checklist progress: the lower-right utility chips now use a shared icon-chip helper so their fill, border, padding, and icon scale stay consistent.