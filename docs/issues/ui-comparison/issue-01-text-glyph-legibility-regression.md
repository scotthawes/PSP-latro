# issue-01: Text and Glyph Legibility Regression

## Severity

P0

## Observed Differences

- Current pregame and ingame captures show garbled labels and incorrect glyph substitutions.
- Reference captures show fully readable labels and consistent symbol rendering.

## Reference Attachments

- assets/ui_db/pre_game_lobby.jpg
- assets/ui_db/gameplay_hud.jpg
- assets/ui_db/rewards.jpg

## Likely Root Cause

- Font atlas mapping and glyph decode path have regressed in the active build path.
- String normalization and fallback handling may differ between scene code paths.

## Implementation Plan

1. Re-validate font atlas load and character map initialization order.
2. Audit text decode and formatting path used by pregame and ingame panels.
3. Add targeted logging for unresolved codepoints and fallback substitutions.
4. Verify scene-level text render calls use the same font and scale assumptions.

## Implementation Checklist

- [x] 1. Capture baseline examples of each garbled label location.
- [x] 2. Confirm atlas and glyph map integrity at runtime.
- [x] 3. Patch decode/mapping path and fallback behavior.
- [x] 4. Re-capture pregame and ingame screenshots.
- [x] 5. Confirm all critical labels are readable.

## Current Progress

- Root cause identified in src/graphics.c: lowercase glyphs were forcibly remapped one slot backward (`'a'..'z' -> c - 1`), producing systematic label corruption.
- Applied fix by restoring direct ASCII mapping in `graphics_font_map_char`.
- Follow-up root cause from validation captures: big-font lowercase glyphs use a different atlas offset than small font.
- Applied font-specific mapping in `src/graphics.c` so lowercase remap is only used for the big font atlas while small font keeps direct ASCII mapping.
- Build validation passed (`make -j4`, 2026-05-16).

## Evidence

- User validation (2026-05-16): recaptured pregame/ingame/shop screenshots confirm text labels now render correctly.
- No remaining garbled labels in the previously affected left-rail and blind title text paths.

## Completion Note

Issue 01 is closed.

## Testing Attachments

- Test plan case: TC-01 in docs/plans/ui-comparison-test-plan.md
- Required evidence:
  - test-output/ui-comparison/pregame-current.png
  - test-output/ui-comparison/ingame-current.png

## Done Definition

No garbled UI labels remain in pregame, ingame, or rewards scenes at PSP resolution.
