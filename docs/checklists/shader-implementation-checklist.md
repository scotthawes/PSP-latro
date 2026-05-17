# Shader Implementation Checklist

This checklist tracks the implementation of the shader porting plan from the Balatro reference, adapted for the PSP fixed-function GPU pipeline.

## Documentation References

- [Shader Porting Implementation Plan](../shader_porting_implementation_plan.md)
- PSPSDK Graphics Utility API reference:
	https://pspdev.github.io/pspsdk/group__GU.html
- PSPSDK GU sample index:
	https://github.com/pspdev/pspsdk/tree/master/src/samples/gu

## Key Constraints

1. PSP GPU has no programmable vertex/fragment shaders — all effects must use CPU preprocessing or fixed-function blending.
2. Texture dimension constraints for sceGuTexImage, but we already handle power-of-two backing textures for PBO workflows.
3. Prefer CPU-side preprocessing when possible to keep the render loop lean.
4. Animated effects (holo, background) need time-based variation at draw time or precomputed texture variants.
5. Use fixed-function texture combiners where appropriate (modulate, add, alpha blend).
6. Cache coherency: flush/invalidate data cache after CPU-side pixel writes before GPU reads.
7. Texture state transitions must be explicit when switching between card variants and non-textured quads.

---

## Phase 1: Infrastructure Setup

- [ ] 1. Add shader effect enumeration to `graphics.h` (e.g. `CardEffect_None`, `CardEffect_Foil`, `CardEffect_Holo`, `CardEffect_Negative`, etc.).
- [ ] 2. Create effect processing functions in `graphics.c` — stub signatures matching the plan:
  - `void apply_foil_effect(uint8_t* pixels, int width, int height)`
  - `void apply_holo_effect(uint8_t* pixels, int width, int height, float time)`
  - `void apply_flash_effect(uint8_t* pixels, int width, int height, float intensity)`
  - `void apply_dissolve_effect(...)` placeholder — re-evaluate scope
- [ ] 3. Add CPU texture processing ping-pong buffer: allocate scratch buffer matching max card texture size, use it as intermediate destination for effect-application to avoid overwriting source pixels in-place.
- [ ] 4. Modify the texture loading / card-texture pipeline to accept an effect enum and call the appropriate CPU processing step before uploading to VRAM.
- [ ] 5. Add a per-frame time helper (or reuse existing `time_manager`) so animated effects like holo/background UV spin can drive their variation from a single clock source.

## Phase 2: Simple Effects

### flash.fs equivalent
- [ ] 6. Add a global/Timodal `flash_state` struct (active bool + remaining seconds/absolute time) in `graphics.h` or a shared `effects.h`.
- [ ] 7. Add `trigger_flash(float duration)` public API so gameplay/winner logic can activate the effect.
- [ ] 8. In the render loop, when flash is active, scale the quad alpha or blend white over scene via fixed-function blending before drawing the flash quad.

### background.fs equivalent
- [ ] 9. Add a timed UV rotation for the background quad — rotate texture coordinates each frame based on elapsed time.
- [ ] 10. Apply color modulation (tint) via CPU or fixed-function texture combiner.
- [ ] 11. Confirm background UV spin is visually smooth at 60 Hz before moving on.

## Phase 3: Card Effect Preprocessing (foil / holo / negative / polychrome)

- [ ] 12. Create a card enhancement table keyed by card index/uid — records which effect each card should render with each frame.
- [ ] 13. Implement `apply_foil_effect()`:
  - Boost blue/brightness channel based on local color delta.
  - Apply subtle contrast adjustment.
- [ ] 14. Implement negative effect (simplest win):
  - Invert RGB: `pixel[i] = 255 - pixel[i]` during CPU preprocessing.
  - Toggle between positive/negative texture variant on demand.
- [ ] 15. Implement `apply_holo_effect()`:
  - Hue-shift based on `(time + card_uuid)` for per-card phase offset so all holo cards shimmer independently.
  - Keep CPU cost low — look up hue rotation from a small LUT or rotate HSL on a per-pixel basis with a precomputed sin/cos LUT.
- [ ] 16. Add a `CardEffect` property to the card struct and wire it through the render path so each card caches the correct processed texture variant.
- [ ] 17. For polychrome: cycle palette indices over time, CPU-remap texture palette each frame or use a set of preprocessed variants cycled at draw time.

## Phase 4: Dissolve / Transition Effects (evaluate feasibility)

- [ ] 18. Prototype a noise-dissolve threshold approach for `dissolve.fs`:
  - Use a precomputed noise texture (greyscale) sampled in CPU each frame.
  - Per-pixel alpha = `(noise_sample < threshold)`.
  - Gated behind a config flag — if it causes unacceptable CPU load, fall back to a simple linear/radial wipe.
- [ ] 19. Evaluate `flame.fs` — likely best replaced with sprite-based animation instead of CPU pixel shader. Document the decision and, if replacing, add a placeholder sprite animation and track separately.

## Phase 5: Optimization & Polish

- [ ] 20. Profile CPU time of texture-processing loop; target < 2 ms per frame for all card effects combined.
- [ ] 21. Use lookup tables for sin/cos in holo hue-rotation and for dissolve threshold comparisons.
- [ ] 22. Confirm no texture-state leaks between effect-variant draws and regular card draws.
- [ ] 23. Test on PPSSPP and document any PSP-specific artefacts (UV seams, alpha blending glitches).
- [ ] 24. Consider atlasing effect variants to reduce draw-call count if profiling shows a bottleneck.
- [ ] 25. Update `docs/CONSOLIDATED_STATUS.md` to mark shader ports complete as each phase ships.

---

## Open Questions / Blockers

- [ ] Confirm Balatro reference shader GLSL before implementing each effect in `docs/shader_porting_implementation_plan.md`.
  - Target shader sources: `resources/shaders/` in the Balatro reference.
- [ ] Decide final scope for `dissolve.fs` and `flame.fs` — may be deferred or simplified significantly.
- [ ] Confirm holo shimmer performance budget on PSP hardware (PPSSPP smoke test first).
- [ ] Decide whether effect preprocessing happens once at card spawn or every frame for animated effects.

---

## Status Legend

- `[ ]` Not started
- `[~]` In progress
- `[x]` Complete
- `[-]` Deferred / out of scope
