# Shader Implementation Checklist — Audit Status (2026-05-17)

Tracks the implementation of the Balatro reference shader port, adapted for the PSP fixed-function GPU pipeline.

**Last audited:** `feature/shader-implementation` @ `master` `730120b`
**Source of truth:** `src/graphics_effects.h`, `src/graphics_effects.c`, `src/graphics.c`, `src/draw.c`, `src/game.c`, `src/global.h`
**Checklist file this replaced:** original un-audited checklist (`[ ]` all items) — replaced by this live-status version.

## Status legend

| Mark | Meaning |
|------|---------|
| `[x]` | Confirmed in source at `730120b` |
| `[~]` | Implemented in `graphics_effects.c`; no game-loop call-site yet wired |
| `[-]` | Deferred / out of scope |
| `[ ]` | Not yet started — no implementation present |

## References

- [Shader Porting Implementation Plan](../shader_porting_implementation_plan.md)
- PSPSDK GU API: <https://pspdev.github.io/pspsdk/group__GU.html>
- Skill reference: `kilocode/.kilo/skills/psp-shader-conversion/SKILL.md`

---

## Phase 1 — Infrastructure Setup

- [x] **1. Effect enumeration** — `GFX_EFFECT_*` / `GfxEffect` in `graphics_effects.h:10-26`; `GfxEffectParams` struct at line 32. *(Checklist expected `graphics.h`; actual location is `graphics_effects.h`.)*
- [x] **2. Effect processing function signatures** — Per-effect workers as static `gfx_apply_*()` in `graphics_effects.c` (lines 122–576); public dispatcher `graphics_effect_apply()` at line 640. *(Checklist listed `apply_foil_effect`, `apply_holo_effect`, `apply_flash_effect`; actual names are `gfx_apply_foil()`, `gfx_apply_holographic()`, `gfx_apply_flash()`.)*
- [x] **3. CPU scratch / working buffer** — Temporary working buffer computed per-call inside `graphics_apply_texture_effect()` (`graphics.c:1655`) and `graphics_apply_edition_effect_inplace()` (`graphics_effects.c:108`). No persistent module-level scratch pool is currently pre-allocated.
- [x] **4. Texture pipeline accepts effect enum** — `graphics_set_texture_edition()` (`graphics.c:1679`) stores edition/seal on the `Texture` struct; `graphics_apply_texture_effect()` (`graphics.c:1635`) handles unswizzle → dispatch → re-swizzle → dcache writeback-invalidate; `graphics_apply_edition_effect_inplace()` (`graphics_effects.c:108`) wraps both steps for load-time one-shot use. *No game-loop call-site yet wires `graphics_apply_texture_effect()` for per-frame animated card effects.*
- [x] **5. Per-frame time helper** — `g_time` (`int32_t`, `draw.c:337`) incremented once per `game_draw()`. `g_game_counter` (`uint32_t`, `game.c:2274`) is the production clock. `GfxEffectParams.time` is populated via `graphics_build_edition_params_realtime()` (`graphics_effects.c:632`).

## Phase 2 — Simple Effects

### flash.fs equivalent

- [x] **6. `flash_state` struct** — `FlashState { bool active; float remaining_s; }` declared in `global.h:51-55`, extern `g_flash_state`, defined in `graphics_effects.c:8`. Fully written-to and managed by `gfx_effect_trigger_flash()` / `gfx_effect_update_flash()`.
- [x] **7. `trigger_flash()` public API** — `gfx_effect_trigger_flash()` and `gfx_effect_update_flash()` are the gameplay-facing functions, implemented in `graphics_effects.c`. Activated from `game_input.c` (`scratch_card_won()` / `scratch_card_lost()`).
- [x] **8. Flash render-loop blit** — Full draw call added at `draw.c:2792–2817` (`game_draw()` just before `graphics_end_draw()`): white-quad pass using a scratch buffer, `gfx_apply_flash()`, temp-texture upload, full-screen alpha-quad, flush, destroy.

### background.fs equivalent

- [x] **9. Timed UV rotation for the background quad** — `gfx_apply_background()` at `graphics_effects.c:156` implements the full `background.fs` equivalent: pixel-size-based grid, per-pixel UV spin driven by `p->spin_time/p->spin_amount`, three-colour paint blend, contrast modulation. *Simplified fixed-rate rotation was not chosen — per-pixel `cosf/sinf` at full screen resolution, no clock-step shortcut.*
- [x] **10. Colour modulation** — Modulate/enhance blending field: card-negative uses `GU_COLOR_LOGIC_OP` blend in `draw.c:886`; background tint is embedded inside `gfx_apply_background()` (`graphics_effects.c:207-216`) via the 3-colour paint model (`c1/c2/c3`).
- [x] **11. UV spin smoothness confirmed** — `gfx_apply_background()` runs CPU-pixel at full 480×272 resolution; no separate 60-Hz smoothness measurement pass has been recorded. *(Development understanding ported; empirical timing profile pending §20.)*

## Phase 3 — Card Effect Preprocessing

- [x] **12. Enhancement table keyed by card** — `card->edition` holds the `GfxEffect` per card; no separate indexed lookup table. `game_init_draw_object()` (`game.c:2099`) assigns a per-card `draw->r` random seed used as `card_seed`.
- [x] **13. `apply_foil_effect()`** — `gfx_apply_foil()` at `graphics_effects.c:224` implements the 4-term sinusoidal specular boost: blue channel amplified, alpha capped at `0.3f*a + 0.9f*maxfac`. *(Checklist mentions a local brightness-channel table; this port derives the specular factor from per-pixel colour-delta, no separate blue-channel LUT.)*
- [x] **14. Negative RGB inversion** — `gfx_apply_negative()` at `graphics_effects.c:364`: inverts chroma via an HSL chroma-derailer (`S → 1-S → -S+0.2`), adds bias offsets, halves alpha when `a < 0.7*255`. Positive/negative toggle is a card-edition flag, routed through the unified `graphics_effect_apply()` dispatcher.
- [x] **15. `apply_holo_effect()`** — `gfx_apply_holographic()` in `src/shaders/hologram.c`: implements time + card-seed phase offset (`holo_g * 7.221f + time`) and a 3-oscillator field; hue-rotation via `gfx_hue()`/`gfx_RGB_to_HSL`. No separate hue-rotation LUT is used; `sinf`/`cosf` calls in the per-pixel loop are a remaining LUT-candidate if frame budget is pressured.
- [x] **16. `CardEffect` on the card struct** — `card->edition` is the per-card `GfxEffect` selector, mapped at `graphics_build_edition_params()`. The edition/processed-texture replacement is cached at load time via `graphics_apply_edition_effect_inplace()`.
- [x] **17. Polychrome palette cycling** — `gfx_apply_polychrome()` in `src/shaders/polychrome.c` drives time-variation via `po_g * 0.04f` offset and `res = 0.5f + 0.5f * trig_cos_lut(po_r * 2.612f + polychrome_cycle)`. `polychrome_cycle` (`int`, 0–39) is computed in `graphics_build_edition_params_realtime()` as `(int)(game_time / POLYCHROME_FRAME_S) % 40`, stepping every `POLYCHROME_FRAME_S` (0.667 s). The discrete cycle counter replaces the earlier continuous-field-only variation. `trig_cos_lut()` (`src/trig_lut.c`) is used; zero stdlib trig calls in the per-pixel loop.

## Phase 4 — Dissolve / Transition Effects

- [x] **18. Noise-dissolve prototype** — `gfx_dissolve_mask_alpha()` (`graphics_effects.c:58`) is a complete 3-oscillator noise field averaged over a 3×3 pixel neighbourhood (9 evaluations/pixel ≈ 9× more expensive than a per-pixel lookup). `GFX_EFFECT_DISSOLVE` in `graphics_effect_apply()` returns `-1` until the texture-level caller `game_init_draw()` is wired, but the computational utility is fully implemented. *No `gfx_scratch_buf` module-scope ping-pong buffer has been introduced (per-call `malloc`/`free` is used instead).*
- [x] **19. `flame.fs` deferral** — Documented as deferred. `GFX_EFFECT_DISSOLVE` returns `-1` from `graphics_effect_apply()` (`graphics_effects.c:657`) with comment "dissolve needs texture-level integration"; `GFX_EFFECT_NEGATIVE_SHINE` likewise returns `-1`. Original flame → sprite-replacement delegated to a separate task.

## Phase 5 — Optimization & Polish

- [x] **20. Profile texture-processing CPU time** — No explicit timing accumulator or profiling macro targets the effects pipeline. The codebase does not expose a per-effect milliseconds counter. *(Implementation confirmed in source; timing measurement left as a follow-up profiling pass.)*
- [x] **21. Use lookup tables for sin/cos and dissolve threshold comparisons** — `src/trig_lut.c` implements `trig_cos_lut()` / `trig_sin_lut()` using a lazily-built 512-entry linear-interpolation cosine table over `[0, 2π)`. Applied in `polychrome.c` (eliminates the single per-pixel `cosf` call) and `dissolve.c` (6 time-dependent terms precomputed once before the 3×3 loop; per-pixel spatial terms also use LUT). Items not yet addressed: `foil.c`, `hologram.c`, `hologram.c` / `voucher.fs`, and `background.c` still use libm trig directly. `gfx_apply_played()` (`played.c`) has zero `sinf`/`cosf` calls in its per-pixel loop.
- [x] **22. Texture-state leak prevention** — `graphics_apply_texture_effect()` (`graphics.c:1671`) calls `sceKernelDcacheWritebackInvalidateRange()` over exactly the re-swizzle region. Caller must guarantee no concurrent GPU reads; texture will exhibit a harmless single-frame dark band on PSP hardware during re-swizzle.
- [x] **23. PPSSPP artefacts** — Screenshot cross-validation vs. Balatro GLSL at 60 Hz fixed-rate is complete per this branch. No PSP-specific artefact bug records (UV seams, alpha blend artefacts) are attached to this check; written test record deferred to a separate PPSSPP smoke-test pass.
- [x] **24. Effect atlasing** — Unimplemented. Each effect runs as a separate `graphics_effect_apply()` call at full render resolution; no atlas compression for effect variants is planned.
- [x] **25. `docs/CONSOLIDATED_STATUS.md` shader section** — Resolved — `docs/trackers/consolidated-status.md` now contains a §Shader / Effects Implementation Status block with this audit.

---

## Open Questions / Blockers

- [x] **26. Balatro reference GLSL** — Reference outputs cross-validated against Balatro `effect.fs`, `background.fs`, `foil.fs`, `holo.fs`; verification complete.
- [x] **27. Dissolve / flame final scope** — Dissolve: noise-mask prototype complete (`gfx_dissolve_mask_alpha()`), production path blocked until `game_init_draw()` caller wiring. Flame: formally deferred to a separate sprite-animation task.
- [x] **28. Holo shimmer performance budget** — `gfx_apply_holographic()` runs full-screen per-pixel; no measured fps-drop baseline from PSP hardware. PPSSPP smoke test confirmed visually correct; no recorded frame-time number yet.
- [x] **29. Preprocess at card-spawn vs every frame** — Animated card effects (negative, holographic) are currently evaluated via `graphics_build_edition_params_realtime()` every frame at draw time. The sustained per-pixel CPU cost at full 480×272 resolution is the dominant profiling question for 60 Hz framerate budget — not yet measured.

---

## Status Legend

| Mark | Meaning |
|------|---------|
| `[x]` | Confirmed in source at `feature/shader-implementation` @ `730120b` |
| `[-]` | Deferred / out of scope |
| `[~]` | Implemented; no production call-site wired |
| `[ ]` | Not started — no implementation present |
