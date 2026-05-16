# ISSUE-05: Gameplay Table Depth and Composition Atmosphere Gap

## Severity

P2

## Observed Differences

- Current table and background layers feel flatter than reference.
- Midground and overlay balance does not fully match target atmosphere.
- Scoring and combo overlays have less contextual depth.

## Current Implementation

- Small and big blinds now use a static `assets/wallpapers/galaxy_15_psp.png` background image, with fallback to `galaxy_15.webp` and `galaxy_15.jpeg`.
- Animated background motion has been removed for this issue.
- Boss-blind and overlay handling remain separate so readability stays stable.

## Reference Attachments

- assets/ui_db/gameplay_hud.jpg
- assets/ui_db/gameplay_huds_2.jpg

## Likely Root Cause

- Background and board layers use simplified blending and reduced texture variation.
- Overlay strength and board tint are not dynamically balanced per scene.

## Implementation Plan

1. Keep the static small/big blind background image readable under the existing depth stack.
2. Verify scoring overlays remain clear above the updated background pass.
3. Capture gameplay and scoring screenshots for comparison evidence.

## Implementation Checklist

- [x] 1. Audit current background and board depth layer stack.
- [x] 2. Add static `galaxy_15` background pass for small and big blinds.
- [x] 3. Validate readability under scoring/combo overlays.
- [x] 4. Compare side-by-side with gameplay and scoring references.
- [x] 5. Capture updated ingame and scoring screenshots.

## Testing Attachments

- Test plan case: TC-05 in docs/UI_COMPARISON_TEST_PLAN.md
- Required evidence:
  - test-output/ui-comparison/ingame-current.png
  - test-output/ui-comparison/scoring-current.png

## Done Definition

Gameplay scene depth feels reference-aligned while preserving HUD and control readability.

## Status

Closed on 2026-05-16 after user screenshot validation of static small/big blind background behavior and readability.

Known limitation carried forward: boss blind currently keeps the default gameplay background and does not yet render a galaxy variant in this issue scope.
