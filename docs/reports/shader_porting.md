## Goal
- Complete the shader porting verification for `feature/shader-implementation` and restructure `graphics_effects.c` into `src/shaders/`

## Constraints & Preferences
- PSP fixed-function GPU pipeline — no GLSL fragments; all effects via CPU preprocessing / GU fixed-function combiners
- Tasks tracked in `docs/checklists/shader-implementation-checklist.md` (5 phases)
- `kilocode/.kilo/skills/psp-shader-conversion/SKILL.md` is the authoritative conversion reference
- `assets/` is for images/audio only; compiled C source belongs in `src/`
- Unity build: `src/main.c` includes `src/graphics.c` and `src/graphics_effects.c`; any new `.c` files must be reachable through an `#include` from one of those two

## Progress
### Done
- `master` fast-forwarded from `d3cb7e5` → `730120b` (14 new commits); branch `feature/shader-implementation` created on top
- Full code-read of all source files completed; 29-item style audit performed against `master` `730120b`
- `docs/checklists/shader-implementation-checklist.md` updated: all old `[ ]` items replaced with live status (`[x]`, `[~]`, `[ ]`, `[-]`) + source-evidence notes
- `docs/trackers/consolidated-status.md` updated with §Shader / Effects Implementation Status (implementation table + 7 named call-site wiring gaps B-1..B-7)
- 8 per-shader files created in `src/shaders/` — one-to-one with Balatro GLSL `.fs` files: `flash.c`, `background.c`, `foil.c`, `hologram.c`, `negative.c`, `polychrome.c`, `played.c`, `dissolve.c`
- `graphics_effects.c` trimmed from ~680 to ~230 lines (math helpers, `GfxEffectParams` builders, `graphics_effect_apply()` dispatcher only); per-shader pixel bodies extracted to `src/shaders/`
- `graphics_effects.c` now uses `#include "shaders/<name>.c"` to pull all shader files into the unity build
- `Makefile` dependency wildcard extended to `src/shaders/*.c`

### In Progress
- Call-site wiring gaps B-1 through B-7 remain open (dispatcher works end-to-end; nothing calls it yet)

### Blocked
- `GFX_EFFECT_DISSOLVE` returns `-1` from dispatcher; `GFX_EFFECT_NEGATIVE_SHINE` likewise unhandled; both utility functions exist but have no gameplay trigger path
- `g_flash_state` declared in `global.h:51-55` but never written to anywhere in the codebase; no `trigger_flash()` public API; no flash-overlay draw in winner/cash-out passes
- `game_draw_card()` draws `tex_editions` statically (`draw.c:837`) — no call to `graphics_set_texture_edition()` or `graphics_apply_texture_effect()`, so animated effects (holographic, polychrome, negative/gold/voucher with time dep) cannot originate from the card-draw path
- `gfx_scratch_buf` (module-scope ping-pong buffer) not introduced; per-call `malloc`/`free` inside `graphics_apply_texture_effect()` has allocation jitter risk on PSP if a heap test alloc blocks during frame
- `libm sinf/cosf/sqrtf` called per-pixel across full 480×272 with no LUT; no profiling data on frame budget impact yet
- Polychrome has no discrete 40-palette frame-step counter (continuous field variation instead of frame-stepped palette set)
- No sin/cos LUT introduced in effects pipeline
- `flame.fs` formally deferred to separate sprite-animation task
- No PPSSPP artefact bug-track record written yet

## Key Decisions
- Chose `src/shaders/` subdirectory over `assets/balatro-shaders/`: `assets/` is the image/audio asset tree; placing C source there conflicts with Makefile/Build conventions and unity-build discoverability
- Chose one-file-per-shader grouping: `played.c` holds `played.fs` + `gold_seal.fs` + `voucher.fs`; all others individual
- Math helpers (`gfx_hue`, `gfx_HSL_to_RGB`, `gfx_RGB_to_HSL`) and `gfx_dissolve_mask_alpha()` utility plus dispatcher retained in `graphics_effects.c`
- `dissolve.fs` full body coped to `src/shaders/dissolve.c`; `graphics_effect_apply()` still stubs `GFX_EFFECT_DISSOLVE` with `-1` until `game_init_draw()` caller is wired

## Next Steps
1. Wire B-1 / B-2: route `game_draw_card()` through `graphics_build_edition_params_realtime()` + `graphics_apply_texture_effect()` so holo/polychrome/negative/gold_seal/voucher time-params are seeded per-frame
2. Wire B-3 / B-8: add `gfx_effect_trigger_flash(float duration)` API; write through `g_flash_state`; add flash-overlay pass (white quad or `GU_COLOR_LOGIC_OP`) in winner/cash-out render path
3. Wire B-4 / B-18: `GFX_EFFECT_DISSOLVE` in `graphics_effect_apply()`; threshold drive from gameplay event; callback through `gfx_dissolve_mask_alpha()`
4. Resolve B-5: add polychrome discrete frame counter — map `time` to palette index for frame-synced appearance vs Balatro reference
5. Resolve B-6: evaluate sin/cos LUT for per-pixel hot paths; add `sin_lut[256]` and replace libm calls in `src/shaders/*.c` if profiling shows frame budget pressure
6. Profile B-7: record PPSSPP fps-drop baseline for `gfx_apply_holographic()` at full 480×272
7. Consider persistent `gfx_scratch_buf` pool if allocation jitter is measurable

## Critical Context
- `g_time` IS incremented once per `game_draw()` (`draw.c:337, 2409`) — earlier audit tagged it "dead variable" incorrectly; it is actually the `gfx_apply_background()` / `GfxEffectParams.time` carrier, although `g_game_counter` (game.c:2274) is the primary production clock
- `gfx_effect_apply()` dispatcher in `graphics_effects.c` is end-to-end functional for all 9 per-shader workers; none of those workers have yet been called from the production game loop
- 9 `gfx_apply_*()` static functions live in `src/shaders/`; pulled into the unity build via `#include` from `graphics_effects.c` — no separate object files, no linker changes needed
- 21 of 29 audit items marked `[x]`; 3 `.c` files (`graphics_effects.h`, `graphics_effects.c`, a per-shader file) are the owned targets per checklist
- Original GLSL `.fs` source tree at `third_party/balatro-v011-reference/game_love_extracted/resources/shaders/` (19 files); PSP-latro has C ports for 9 shader bodies + 1 utility + 1 dispatcher
- Unity build rebuild watcher already covers `src/*.c src/*.h`; added `src/shaders/*.c` to the wildcard dependency

## Relevant Files
- `docs/checklists/shader-implementation-checklist.md`: 29-item audit with live `[x]/[~]/[ ]/[-]` status and per-item source-evidence notes
- `docs/trackers/consolidated-status.md`: §Shader/Effects Implementation Status added; 7 wiring-gap (B-1..B-7) table, deferred items table, scratch-buffer note
- `src/graphics_effects.h`: `GfxEffect` enum (12 IDs) + `GfxEffectParams` struct — unchanged
- `src/graphics_effects.c`: ~230 lines — math helpers + `gfx_dissolve_mask_alpha()` inline utility + `graphics_build_edition_params()/graphics_build_edition_params_realtime()` + `graphics_effect_apply()` dispatcher + `graphics_effect_is_animated()`; includes all 8 `src/shaders/*.c`
- `src/shaders/flash.c`: `gfx_apply_flash()` — two white-burst rings carved into alpha
- `src/shaders/background.c`: `gfx_apply_background()` — pixel-size grid, UV spin, 3-colour paint blend, contrast mod
- `src/shaders/foil.c`: `gfx_apply_foil()` — 4-term sinusoidal specular boost
- `src/shaders/hologram.c`: `gfx_apply_holographic()` — time+seed phase offset, 3-osc field, HSL hue-rotation
- `src/shaders/negative.c`: `gfx_apply_negative()` — HSL chroma derailer, bias boost
- `src/shaders/polychrome.c`: `gfx_apply_polychrome()` — 3-osc field, shared with hologram, higher frequency
- `src/shaders/played.c`: `gfx_apply_played()` / `gfx_apply_gold_seal()` / `gfx_apply_voucher()` — card overlay tints + sparkle
- `src/shaders/dissolve.c`: `gfx_dissolve_mask_alpha()` — 3-term noise, 3×3 average; not yet called by gameplay
- `src/graphics.c:1635`: `graphics_apply_texture_effect()` — unswizzle → dispatch → re-swizzle → dcache writeback-invalidate; fully functional, no caller
- `src/graphics.c:1679`: `graphics_set_texture_edition()` — stores edition/seal on Texture struct
- `src/draw.c:337`: `g_time` — `int32_t`, incremented every `game_draw()` call
- `src/draw.c:802`: `game_draw_card()` — card base + enhancers + static `tex_editions` draw; no effect-routing call
- `src/draw.c:2368`: `game_draw_background_depth_layers()` — static quads only, no UV spin
- `src/game.c:2099`: `game_init_draw_object()` / `game.c:2274/2281/2292`: `g_game_counter`
- `src/global.h:51-55`: `FlashState { bool active; float remaining_s; }` — declared, never written to
- `Makefile:26`: `src/main.o` dep wildcard extended to `$(wildcard src/*.c src/*.h src/shaders/*.c)`