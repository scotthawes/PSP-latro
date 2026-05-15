# ISSUE-01: Font Rendering Garbled Text

## Severity

P0 (blocks parity and readability)

## Observed Differences

- Pregame headers and labels render as corrupted characters.
- Ingame blind/title text is unreadable.
- Some labels appear to use fallback glyphs or wrong codepage mapping.

## Likely Root Cause

- Font atlas/glyph index mismatch.
- Incorrect encoding assumptions in text rendering path.
- Missing glyph range in baked atlas for UI strings.

## Proposed Solutions

1. Validate font load pipeline and atlas dimensions at runtime.
2. Confirm string encoding path end-to-end (source literals, localization layer, renderer).
3. Add explicit glyph-range bake for all required UI characters.
4. Add fallback font only for missing glyphs, not as primary draw path.
5. Add startup self-check for critical labels with known expected glyph IDs.

## Implementation Checklist

- [x] 1. Identify the renderer function used by blind/title/left HUD labels.
- [x] 2. Log first 32 codepoints for affected strings before draw.
- [x] 3. Verify atlas includes all glyphs needed for menu and gameplay HUD.
- [x] 4. Fix any UTF-8 to codepoint conversion issues.
- [x] 5. Fix glyph lookup/index offset issues.
- [x] 6. Re-test pregame title, blind labels, and HUD labels.
- [x] 7. Capture before/after screenshot pair and archive in test-output.

## Current Progress

- Implemented lowercase glyph remap in [src/graphics.c](src/graphics.c#L1152), [src/graphics.c](src/graphics.c#L1167), and [src/graphics.c](src/graphics.c#L1260).
- Added DEBUG runtime codepoint logging in [src/graphics.c](src/graphics.c#L66), [src/graphics.c](src/graphics.c#L1172), and [src/graphics.c](src/graphics.c#L1266).
- Added DEBUG ASCII atlas coverage validation during font loads in [src/graphics.c](src/graphics.c#L523) and [src/graphics.c](src/graphics.c#L958).
- Added UTF-8 compatible decoding with safe legacy-byte fallback in [src/graphics.c](src/graphics.c#L1168), [src/graphics.c](src/graphics.c#L1274), and [src/graphics.c](src/graphics.c#L1406).
- Added codepoint-aware text measurement for centering and formatted strings in [src/graphics.c](src/graphics.c#L1235), [src/graphics.c](src/graphics.c#L1256), and [src/graphics.c](src/graphics.c#L1321).
- Build validated successfully after renderer changes.

## Evidence Artifacts

- Runtime diagnostics log: [test-output/ui-parity/issue-01-font-debug.log](test-output/ui-parity/issue-01-font-debug.log)
- Visual re-test captures:
  - [test-output/ui-parity/issue-01-after-pregame.jpg](test-output/ui-parity/issue-01-after-pregame.jpg)
  - [test-output/ui-parity/issue-01-after-ingame.jpg](test-output/ui-parity/issue-01-after-ingame.jpg)
- Confirmed in log:
  - FONT slot=0 ASCII coverage OK (0x20-0x7E), glyph_count=256
  - FONT slot=1 ASCII coverage OK (0x20-0x7E), glyph_count=256
  - Multiple FONT draw_text entries with raw-to-mapped byte traces for first 32 bytes
- Confirmed visually in captures:
  - Pregame blind selection labels are readable and stable.
  - Ingame blind/HUD labels are readable and stable.

## Done Definition

- All text shown in screenshots is readable and stable across scene transitions.

## Testing Reference

- Use TC-01 and TC-02 from docs/UI_PARITY_TEST_PLAN.md
- Capture validation images after fixes and attach to test-output/ui-parity/
