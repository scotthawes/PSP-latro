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
-
- **B-1 RESOLVED** — `graphics_apply_texture_effect()` has a production call-site: `draw.c:1048–1056` inside `game_draw_card()` via `game_draw_card_edition_effect()` → scratch-buffer → `graphics_effect_apply()` per-frame
- **B-2 RESOLVED** — `game_draw_card()` no longer draws `tex_editions` statically; per-frame scratch-buffer pipeline (`game_draw_card_edition_effect()` / `game_draw_joker_edition_effect()`) fully replaces it
- **B-3 RESOLVED** — `g_flash_state` lifecycle fully wired: `gfx_effect_trigger_flash()` / `gfx_effect_update_flash()` in `graphics_effects.c`; flash-overlay full-quad pass at `draw.c:2792–2817`; gameplay call-sites in `game_input.c`
- **B-4 RESOLVED** — `GFX_EFFECT_DISSOLVE` production pipeline wired at `draw.c:1063–1103`: card base + enhancer composited into scratch, `graphics_effect_apply()` run, result uploaded to temp texture and drawn
- **B-5 RESOLVED** — `polychrome_cycle` (`int`, 0–39) computed in `graphics_build_edition_params_realtime()` as `(int)(game_time / POLYCHROME_FRAME_S) % 40`; discrete 40-frame palette stepping replaces continuous field-only variation
- **B-6 RESOLVED** — `src/trig_lut.c` adds a lazily-initialised 512-entry cosine LUT with linear interpolation over `[0, 2π)`; `trig_cos_lut()` / `trig_sin_lut()` used in `polychrome.c` (zero per-pixel stdlib trig) and `dissolve.c` (6 time-dependent terms precomputed once per frame; per-pixel spatial terms also use LUT). `trig_lut.c` included from `src/main.c`
- **B-7 RESOLVED** — `docs/CONSOLIDATED_STATUS.md` now includes §Shader / Effects Implementation Status

### Deferred / Out of Scope
- `flame.fs` — formally deferred to separate sprite-animation task
- `GFX_EFFECT_NEGATIVE_SHINE` — dispatcher stub (`return -1`); no Balatro reference resolved yet
- Persistent `gfx_scratch_buf` ping-pool pool — per-call `malloc`/`free` currently used; monitor for allocation jitter on device
- PPSSPP smoke-test written record — visually verified; formal bug-track pass pending

### Not Yet Profiled
- Full-scene `graphics_apply_background()` at 480×272 CPU cost (per-pixel `sinf`/`cosf` still in `background.c`)
- `hologram.c` / `foil.c` / `flash.c` still use libm trig in hot loops; LUT migration is candidate work if frame budget is pressured

## Key Decisions
- Chose `src/shaders/` subdirectory over `assets/balatro-shaders/`: `assets/` is the image/audio asset tree; placing C source there conflicts with Makefile/Build conventions and unity-build discoverability
- Chose one-file-per-shader grouping: `played.c` holds `played.fs` + `gold_seal.fs` + `voucher.fs`; all others individual
- Math helpers (`gfx_hue`, `gfx_HSL_to_RGB`, `gfx_RGB_to_HSL`) and `gfx_dissolve_mask_alpha()` utility plus dispatcher retained in `graphics_effects.c`
- LUT uses lazy Initialisation (built on first call, occupying ~2 KB in BSS) rather than static init to avoid startup cost in init-only paths
- `trig_lut.c` has no interdependency on `graphics_effects.c`; it is included from `main.c` before `graphics_effects.c` so shader files can `#include "../trig_lut.h"` without circular-declaration issues

## Outstanding Items (non-shader)
- Audio BGM silent on PSP device only (PPSSPP OK) — highest product impact; `audio_callback` instrumentation with `logs/audio.log` pending
- ~55 Jokers remain `// NOT IMPLEMENTED` in `game.c`
- Deck selection, vouchers, spectral cards, challenges mode — out of scope for shader pass

## Relevant Files
- `docs/checklists/shader-implementation-checklist.md`: 29-item audit with live `[x]/[~]/[ ]/[-]` status and per-item source-evidence notes
- `docs/trackers/consolidated-status.md`: §Shader/Effects Implementation Status; B-1..B-7 all resolved; dcache and LUT changes recorded
- `src/trig_lut.h` / `src/trig_lut.c`: cosine LUT module
- `src/graphics_effects.h`: `GfxEffect` enum (12 IDs) + `GfxEffectParams` struct
- `src/graphics_effects.c`: math helpers + `gfx_dissolve_mask_alpha()` + `graphics_build_edition_params()` / `graphics_build_edition_params_realtime()` + `graphics_effect_apply()` dispatcher; includes all `src/shaders/*.c`
- `src/shaders/polychrome.c`: discrete 40-frame `trig_cos_lut` palette stepping
- `src/shaders/dissolve.c`: LUT applied; 6 time-dependent trig terms precomputed once per pixel invocation
- `src/shaders/foil.c`, `hologram.c`, `background.c`, `flash.c`: still use libm `sinf`/`cosf`/`sqrtf`; LUT migration candidate
- `src/graphics.c`: `graphics_apply_texture_effect()` — unswizzle → dispatch → re-swizzle → dcache writeback-invalidate
- `src/draw.c`: `game_draw_card_edition_effect()` (card scratch pipeline), `game_draw_joker_edition_effect()` (joker scratch pipeline), flash overlay at `draw.c:2792–2817`, dissolve pass at `draw.c:1063–1103`
