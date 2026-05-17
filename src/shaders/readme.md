# Shader implementations (`src/` → C)

Each file in this directory is a 1-to-1 C port of a `.fs` fragment shader from
the Balatro v0.11 reference (`third_party/balatro-v011-reference/…/resources/shaders/`).

The GLSL sources are the authoritative reference; the C bodies are the adapted
PSP fixed-function equivalent.

## Naming

| File | GLSL reference | Public symbol |
|---|---|---|
| `flash.c` | `flash.fs` | `gfx_apply_flash()` |
| `background.c` | `background.fs` | `gfx_apply_background()` |
| `foil.c` | `foil.fs` | `gfx_apply_foil()` |
| `hologram.c` | `holo.fs` / `hologram.fs` | `gfx_apply_holographic()` |
| `negative.c` | `negative.fs` | `gfx_apply_negative()` |
| `polychrome.c` | `polychrome.fs` | `gfx_apply_polychrome()` |
| `played.c` | `played.fs`, `gold_seal.fs`, `voucher.fs` | `gfx_apply_played()`, `gfx_apply_gold_seal()`, `gfx_apply_voucher()` |
| `dissolve.c` | `dissolve.fs` | `gfx_dissolve_mask_alpha()` |

## Compilation

All files are compiled into the main binary via the unity build in
`src/main.c` → `src/graphics_effects.c`:

```c
/* graphics_effects.c */
#include "shaders/flash.c"
#include "shaders/background.c"
#include "shaders/foil.c"
/* … */
```

Building with `make` recompiles `graphics_effects.c` whenever any file in
`src/shaders/` changes.

## Architecture notes

- All effects are CPU-side pixel loops — the PSP has no programmable fragment
  shader stage.
- Output format is RGBA8888; the caller swizzles to/from the GPU texture buffer
  via `graphics_apply_texture_effect()` in `graphics.c`.
- Math helpers (`gfx_hue`, `gfx_RGB_to_HSL`, `gfx_HSL_to_RGB`) and the `vec2`
  typedef live in `graphics_effects.c` as `static inline` — shared by all
  shader files without exporting symbols.
- `gfx_dissolve_mask_alpha()` is both a `static inline` in `graphics_effects.c`
  and a full definition in `dissolve.c` for call-site readability.
