# issue-10: Language and Credits Panel Parity Gap

## Severity

P2

## Observed Differences

- The language selector still needs a more exact button grid and beta-tag placement pass.
- Credits and thanks panels should better preserve the target typography hierarchy and centered modal balance.

## Reference Attachments

- assets/ui_db/balatro_language_options.jpg
- assets/ui_db/balatro_credits.jpg
- assets/ui_db/balatro_credits_2.jpg

## Likely Root Cause

- These screens reuse generic modal components, but the target uses denser grid rules and a more deliberate text hierarchy.

## Implementation Plan

1. Capture language and credits screens at the locked comparison profile.
2. Compare button grid density, text sizes, and modal centering.
3. Update the shared language and credits layout rules.

## Implementation Checklist

- [ ] 1. Capture language-current.png and credits-current.png.
- [ ] 2. Validate language button grid spacing and wrapping.
- [ ] 3. Validate beta-tag placement and visibility.
- [ ] 4. Validate credits card sizing and centered composition.
- [ ] 5. Recheck typography hierarchy on both credits variants.

## Testing Attachments

- Test plan case: TC-07 in docs/plans/ui-comparison-test-plan.md
- Required evidence:
  - test-output/ui-comparison/language-current.png
  - test-output/ui-comparison/credits-current.png

## Done Definition

Language and credits screens preserve the target's modal balance, readable hierarchy, and compact button layout.

## Status

Open.