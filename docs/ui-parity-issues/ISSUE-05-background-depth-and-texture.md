# ISSUE-05: Background Depth and Texture Gap

## Severity

P1

## Observed Differences

- Current screens look flatter than reference.
- Atmospheric layering and subtle texture detail are reduced.

## Likely Root Cause

- Background composed from single fill or low-detail layer.
- Missing overlay passes (noise, vignette, gradient modulation).
- Conservative contrast settings to avoid UI clash.

## Proposed Solutions

1. Use layered background rendering: base texture + low-alpha overlays.
2. Add subtle vignette and soft pattern pass with PSP-safe quads.
3. Re-tune saturation and luminance to keep UI legible.

## Implementation Checklist

- [x] 1. Identify current background render path for both scenes.
- [x] 2. Add two lightweight overlay layers.
- [x] 3. Tune alpha values for readability and depth.
- [x] 4. Verify no measurable frame-time regression.
- [x] 5. Lock final values and document in render style notes.

## Current Progress

- Current gameplay background path identified in `src/draw.c` (`game_draw`), where scenes previously relied on `graphics_clear(COLOR_BACKGROUND_2)` and UI-layer solids.
- Added `game_draw_background_depth_layers()` and invoked it for blind/ingame/shop stages before scene widgets render.
- Overlay pass now uses only GU quads (no extra texture binds) to keep PSP cost low.

## Render Style Notes (Locked Values)

- Layer A (atmosphere):
	- top cool cast: `0x10253A44`, height `44px`
	- bottom weight: `0x13000000`, height `62px`
	- center lift: `0x06243F50`, rect `(50, 32, SCREEN_WIDTH-100, SCREEN_HEIGHT-90)`
- Layer B (depth texture):
	- vignette ring count: `9`
	- edge alpha ramp: `16 -> 8` (computed as `16 - i`)
	- scanline stride: `14px`
	- scanline alpha alternation: `0x05000000` / `0x03000000`

## Evidence

- Build verification: `make -j4` succeeded after depth-layer changes.
- Runtime verification (2026-05-15 user-provided screenshots): FPS remained stable in the sampled scenes (`~59.66` to `~59.94`) across blind-select, ingame, post-game, and shop.

## Remaining For Completion

- None. Issue 5 completion criteria are satisfied.

## Done Definition

- Table/menu backgrounds feel layered and closer to reference atmosphere.
