---
name: psp-shader-conversion
description: "Converting Balatro GLSL fragment shaders (.fs) to equivalent C implementations for the PSP fixed-function GU pipeline."
---

# PSP Shader Conversion Skill

Use this skill when porting a `.fs` shader from
`third_party/balatro-v011-reference/game_assets_extracted/resources/shaders/`
into a C equivalent in `src/draw.c` or `src/graphics.c`.

The PSP has **no programmable fragment shader pipeline**. Every `.fs` must be
reimplemented using the fixed-function GU: vertex transforms, color-combiner
units, blend state, and alpha-test. There is no per-pixel callback.

---

## Shader categories

Reference shaders fall into four categories; use the matching strategy.

### 1 – Dissolve-mask shader (per-quad alpha blit with branching dissolve curve)

**Files:** `dissolve.fs`, `foil.fs`, `negative.fs`, `hologram.fs`,
`negative_shine.fs`, `voucher.fs`, `booster.fs`, `played.fs`, `holo.fs`,
`debuff.fs`, `gold_seal.fs` (partial)

**Shared GLSL `dissolve_mask()` body** (near-identical in 10 files).

GLSL pattern:

```glsl
float adjusted_dissolve = (dissolve*dissolve*(3.-2.*dissolve))*1.02 - 0.01;
float t = time * 10.0 + 2003.;
vec2 floored_uv = (floor((uv*texture_details.ba)))/max(texture_details.b, texture_details.a);
vec2 uv_scaled_centered = (floored_uv - 0.5) * 2.3 * max(texture_details.b, texture_details.a);
// 3 warped-field noise samples -> float field
float res = (.5 + .5*cos((adjusted_dissolve)/82.612 + (field + -.5)*3.14))
          - border_clamp(floored_uv, adjusted_dissolve);
return res > adjusted_dissolve ? tex.a : 0.0;
```

C translation strategy:

- The PSP cannot branch per pixel. The dissolve threshold `adjusted_dissolve`
  determines **whether the entire quad is visible or fully clipped**.
- Compute `res` identically in C, then gate with:
  ```c
  if (res > adjusted_dissolve) {
      // draw quad (normal alpha)
  } else {
      // skip quad entirely, or draw with alpha=0
  }
  ```
- The three-field noise uses `sinf`/`cosf` on `uv` and `time`
  variables passed in from the game loop each frame (see §Variables).
- Burn-edge colour overlay: when `res` falls in the burn band and
  `burn_colour_1.a > 0.01`, replace `tex.rgb` with `burn_colour_1.rgb`
  (or `burn_colour_2`) before uploading pixels.

### 2 – Per-texel colour effect (permanent tint, no branching dissolve)

**Files:** `negative.fs` (effect body), `negative_shine.fs`, `voucher.fs`,
`booster.fs`

These modify `tex.rgb` permanently (RGB shift / HSL remap), then call
`dissolve_mask`.

C translation:

- HSL/RGB conversion functions: port `hue()`, `RGB()`, `HSL()` from GLSL to C
  (`float` not `double`; `modf()` / `fmod()` for GLSL `mod()`).
- Color remap is applied to all pixel data **before upload** or per-vertex as a
  vertex color tint if the effect has no per-pixel branch.
- For negative-style per-pixel inversion, apply a `0.5 - value` transform on
  the R/G/B channels before writing the sprite region.

### 3 – Vertex-shader transforms (screen-space vertex displacement)

**Files:** `vortex.fs`, `skew.fs` (shared hover-pop `position()` block),
`foil.fs`, `polychrome.fs`, `negative.fs`, `hologram.fs`, `negative_shine.fs`,
`holo.fs`, `voucher.fs`, `debuff.fs` (shared hover-pop)

**Shared hover-pop GLSL** (present in 10 files; identical body):

```glsl
vec4 position(mat4 transform_projection, vec4 vertex_position) {
    if (hovering <= 0.) return transform_projection * vertex_position;
    float mid_dist = length(vertex_position.xy - 0.5*love_ScreenSize.xy)/length(love_ScreenSize.xy);
    vec2 mouse_offset = (vertex_position.xy - mouse_screen_pos.xy)/screen_scale;
    float scale = 0.2*(-0.03 - 0.3*max(0.,0.3-mid_dist))
                  *hovering*(length(mouse_offset)*length(mouse_offset))/(2.-mid_dist);
    return transform_projection * vertex_position + vec4(0,0,0,scale);
}
```

C translation: pre-displace the four quad corner positions in `draw.c` before
submitting to GU. The displacement is purely a w-component scale (screen-space
bulge toward/away from view), no position change — so it can also be applied as
a vertex-alpha or scale modulation on the quad object.

**`vortex.fs`** – screen-centre angular twist:

```glsl
vec2 uv = (vertex_position.xy - 0.5*love_ScreenSize.xy)/length(love_ScreenSize.xy);
float len  = length(uv * vec2(love_ScreenSize.x/love_ScreenSize.y, 1.));
float angle = atan(uv.y,uv.x) + effectAngle * smoothstep(effectRadius,0.,len);
vertex_position.x = (radius*cos(angle) + center.x) * length(love_ScreenSize.xy);
vertex_position.y = (radius*sin(angle) + center.y) * length(love_ScreenSize.xy);
```

C translation: modify the quad corner vertices in a temporary array before the
`sceGumVertexArray` upload. Use `atan2f`, `cosf`, `sinf` from `<math.h>`
(already included via `_USE_MATH_DEFINES` in `global.h`).

### 4 – Full-screen post-processing pass

**Files:** `flash.fs`, `splash.fs`, `CRT.fs`, `background.fs`

These read `screen_coords` (pixel position) and write the final colour directly,
bypassing the texture. There is **no texture input**.

C translation:

- Render to an off-screen render target via `graphics_set_offscreen_render_target()`.
- Blit the result back to the framebuffer each frame.
- `flash.fs` is the simplest: white-tint modulated by `time` and `mid_flash`.
  Multiply a full-screen quad alpha by the computed white factor.
- `splash.fs` combines pixelated screen-space coords, a swirl UV rotation, and
  a procedural smoke field (`5-iteration chaos loop`). Port the smoke loop
  verbatim into C; render as a full-screen textured quad with the computed
  colour.
- `CRT.fs` is a no-op stub (`return vec4(0.0)`), skip on PSP.
- `background.fs` applies a spin distortion (`spin_time`, `spin_amount`) and
  a 3-colour paint blend (`colour_1/2/3`). Port the pixel-size quantisation and
  spin-angle math to C; render to the texture-atlas region used for the blind
  background.

---

## Variable mapping (GLSL extern → C global)

| GLSL type | GLSL `extern` name | C equivalent | Notes |
|-----------|--------------------|--------------|-------|
| `vec2`    | `foil` / `polychrome` / `negative` / `hologram` / `holo` / `negative_shine` / `voucher` / `booster` / `played` / `debuff` | `float g_effect_<name>[2]` | Game loop seeds `.r`/`.g` each frame |
| `vec2`    | `mouse_screen_pos` | `float g_mouse_screen_pos[2]` | Canvas pixel coords |
| `vec2`    | `image_details` | `float g_texture_image_details[2]` or struct field | `{w, h}` UV scaling; derive per-sprite from sprite region in `graphics.c` |
| `vec4`    | `texture_details` | `float g_texture_details[4]` | `{x, y, w, h}` — same as LÖVE `Quad` |
| `vec4`    | `burn_colour_1/2` | `float g_burn_colour_1[4]` | RGBA, updated from game state |
| `vec4`    | `colour_1/2/3` | `float g_colour_1[4]` etc. | Full-screen passes |
| `float`   | `dissolve` | `float g_dissolve` | 0→1 progress |
| `float`   | `time` | `float g_time` | Seconds since start |
| `float`   | `vortex_amt` | `float g_vortex_amt` | 0→1 |
| `float`   | `screen_scale` | `float g_screen_scale` | Hover-vertex uniform |
| `float`   | `hovering` | `float g_hovering` | 0 or 1 |
| `bool`    | `shadow` | `int g_is_shadow` | 0 or 1 |

A GLSL `extern vec2 X;` reads from the game loop's `vec2 X = {r, g}` that is
updated every frame in `src/`. Those variables are transparently injected by
Balatro's shader compiler; on the PSP side the game loop writes `g_effect_X[0]`
and `g_effect_X[1]` before each draw call (starting at `g_time = 0` and
incrementing by delta each frame).

`love_ScreenSize` → `SCREEN_WIDTH` (480) and `SCREEN_HEIGHT` (272) from
`global.h`.

---

## Math helpers (GLSL → C)

All shaders use only `sin`, `cos`, `length`, `max`, `min`, `mod`, `floor`,
`atan`, `smoothstep`, and `dot`. `<math.h>` provides all of them.

| GLSL | C equivalent |
|------|-------------|
| `mod(a,1.)` | `fmod(a, 1.0f)` |
| `floor(x)` | `floorf(x)` |
| `length(v)` | `sqrtf(v[0]*v[0] + v[1]*v[1])` |
| `smoothstep(edge0,edge1,x)` | inline: `t = CLAMP((x-edge0)/(edge1-edge0),0,1); return t*t*(3-2*t);` |
| `atan(y,x)` | `atan2f(y, x)` |
| `dot(a,b)` | inline: `a[0]*b[0]+a[1]*b[1]` |
| `vec2(sin(x),cos(x))` | `(float[2]){sinf(x), cosf(x)}` or manual struct |

---

## Conversion checklist

1. Identify shader category (dissolve-mask / colour-effect / vertex-transform /
   fullscreen-pass).
2. Port the shared `dissolve_mask` if the file calls it — copy it verbatim into
   a static C function in the same `.c` file. The body is pure float arithmetic.
3. Port any HSL/RGB helpers unchanged (same signatures, replace GLSL builtins
   with C stdlib / inline).
4. Port the `effect` body: replace `Texel()` with the RGBA8888 pixel unpack or
   the 16-bit PSP texture sample, replace `colour` with the vertex colour
   argument passed to the draw call.
5. For per-quad dissolve gating, add the `if (res > adjusted_dissolve)` check
   in the C caller so the quad either draws or is skipped.
6. For vertex transforms, pre-displace the corner array in `draw.c` before the
   `sceGumVertexArray` or `sceGuVertexArray` call.
7. For fullscreen passes, render to offscreen then blit.

---

## Conversion process (step-by-step)

### Phase 1 — Read the GLSL

Open a `.fs` file and identify in order:

1. **`extern` declarations** — uniform/input variables. Map each to a C global
   using the Variable mapping table above.
2. **`effect()` function** — the main per-pixel body where color math happens.
3. **`dissolve_mask()` call** — if present, it wraps alpha gating. Copy its
   shared body into a C helper (see Phase 2A).
4. **`#ifdef VERTEX / position()` block** — if present, this is a
   vertex-transform. Skip to Phase 3.

---

### Phase 2 — Fragment-only conversion (no vertex shader)

**Step A — Port `dissolve_mask()`**

Copy the shared `dissolve_mask()` body into a `static float shader_dissolve_mask_c(...)`
helper in `src/draw.c`. It takes `tex_r,tex_g,tex_b,tex_a, uv_x,uv_y, texture_details[4], image_details[2], burn_colour_1[4], burning_colour_2[4], shadow, dissolve, time` and returns a single float: the dissolve threshold. It must **not** modify texture memory.

Replace every GLSL builtin:

| GLSL | C |
|------|---|
| `vec2` | `float v[2]` or `struct {float x,y;}` |
| `Texel(tex, tc)` | caller passes pixel as individual `float r,g,b,a` args |
| `floor(...)` | `floorf(...)` |
| `mod(...)` | `fmodf(...)` |
| `length(v)` | `sqrtf(v[0]*v[0]+v[1]*v[1])` |
| `dot(a,b)` | `a[0]*b[0]+a[1]*b[1]` |
| `vec4(r,g,b,a)` | pass as individual float args / struct |
| `smoothstep(e0,e1,x)` | inline polynomial (see Math helpers above) |

**Step B — Port `effect()` body**

Replace `Texel()` with the 16-bit PSP texture sample (or RGBA8888 pixel load if
using a software raster path). Replace `colour * tex` with the vertex-color
multiplication already done upstream by the pipeline. Any per-pixel branch
(e.g. `if (tex.a > 0.01 && ...)`) becomes a scalar computation whose result is
fed into the vertex-color tint or the dissolve threshold returned by the helper.

**Step C — Gate the quad in `game_draw_card()`**

In `draw.c`, after computing the dissolve threshold:

```c
float dissolve_threshold;
shader_dissolve_mask_c(tex_r, tex_g, tex_b, tex_a,
                       uv[0], uv[1],
                       g_texture_details, g_texture_image_details,
                       g_burn_colour_1, g_burn_colour_2,
                       is_shadow, g_dissolve, g_time,
                       &burn_edge_colour);

if (dissolve_threshold < g_dissolve) {
    /* quad is dissolved away — skip drawing entirely */
    return;
}

if (burn_edge_colour[3] > 0.0f) {
    /* tint vertex colour with the burn edge */
    card_color = burn_edge_colour;
}
```

---

### Phase 3 — Vertex-transform conversion

**Step A — Shared hover-pop**

The shared hover-pop block (present in 10 files) is a w-component bulge, not a
position change. Don't add a vertex shader. Compute `hover_scale` from the
game's hover state and apply it as a `DrawObject.scale` multiplier in
`game_draw_card()` before drawing, or pre-multiply the quad corner positions
in draw-space.

**Step B — Vortex / twist**

`vortex.fs` has unique vertex displacement. Pre-compute a
`vortex_corners[4][2]` array in C, applying the same UV-distortion math to
each corner position before passing the array to `sceGumVertexArray`.

---

### Phase 4 — Fullscreen passes (flash, splash, background)

These have no sprite texture; they compute screen-space color from scratch.

1. Call `graphics_set_offscreen_render_target()` at the start of the frame
   (or the correct sub-stage).
2. Port the `effect()` math into a C function that fills a
   `uint32_t pixel_buf[BUFFER_WIDTH * BUFFER_HEIGHT]`.
3. Call `graphics_unset_offscreen_render_target()`.
4. Upload `pixel_buf` as a texture and draw a full-screen textured quad to the
   framebuffer.

| Shader | Shortcut |
|--------|---------|
| `flash.fs` | Skip the pixel buffer; draw a full-screen blended quad with alpha = computed white factor × `mid_flash`. |
| `splash.fs` | Port the 5-iteration smoke loop to C; render as a full-screen blended textured quad. |
| `CRT.fs` | No-op stub (`return vec4(0.0)`); skip entirely on PSP. |
| `background.fs` | Port pixel-size quantisation and spin-angle math; render into the texture-atlas region used for blind backgrounds. |

---

### Phase 5 — Wire up game-loop variables

In the game loop (`src/main.c` or equivalent), seed each `g_effect_*` variable
before the draw call that uses it:

```c
/* Example: before drawing a foil card */
g_effect_foil[0] = card_params.foil_r;
g_effect_foil[1] = card_params.foil_g;
g_dissolve       = card_params.dissolve;
g_time           = (float)time_in_seconds;
```

These variables are what Balatro's LÖVE runtime injects as `extern vec2` /
`extern float` uniforms. On the PSP the game loop owns and updates them.

---

### Phase 6 — Verify

1. Build: `make` (or the Docker build script in `scripts/pspbuilder/`).
2. Run in PPSSPP.
3. Compare each card/background frame against the reference `.fs` render at the
   same `time` / `dissolve` value.
4. Small numeric differences between LÖVE's Lua JIT float precision and the
   PSP's C `float` are normal. Clamp the burn-edge alpha band (normally the
   `0.01` threshold in `burn_colour_1.a > 0.01`) if you see flickering at the
   dissolve boundary.
