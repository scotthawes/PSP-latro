# PSP-latro: Performance & UI Improvement Opportunities

This document catalogues targeted improvements to the existing codebase, focused on rendering performance and UI polish. Large missing features are tracked separately in `MISSING_FEATURES.md`.

---

## Table of Contents

1. [Performance Improvements](#1-performance-improvements)
   - ✅ 1.1 [Replace Full Dcache Flush in Hot-Path Draw Macro](#11-replace-full-dcache-flush-in-hot-path-draw-macro)  
   - ✅ 1.2 [Increase MAX_QUADS to Reduce Flush Frequency](#12-increase-max_quads-to-reduce-flush-frequency)  
   - ✅ 1.3 [Enable 333 MHz Overclock by Default](#13-enable-333-mhz-overclock-by-default)  
   - ✅ 1.4 [Scoped Dcache Flush for Rotated Quads](#14-scoped-dcache-flush-for-rotated-quads)  
   - 1.5 [Redundant Dcache Flush at End of Frame](#15-redundant-dcache-flush-at-end-of-frame)  
   - 1.6 [Audio Decoding Runs on the Main Thread](#16-audio-decoding-runs-on-the-main-thread)
2. [UI Improvements](#2-ui-improvements)
   - 2.1 [Analog Stick Unused Despite Being Configured](#21-analog-stick-unused-despite-being-configured)  
   - 2.2 [Large Numbers Have No Thousands Separator](#22-large-numbers-have-no-thousands-separator)  
   - 2.3 [Default Animation Speed is the Slowest Setting](#23-default-animation-speed-is-the-slowest-setting)  
   - 2.4 [Placeholder Joker Names in Stub Entries](#24-placeholder-joker-names-in-stub-entries)  
   - 2.5 [Some UI Labels Use Small Font Where Big Font Fits](#25-some-ui-labels-use-small-font-where-big-font-fits)  
   - 2.6 [Formatted-Text Centering Traverses String Twice](#26-formatted-text-centering-traverses-string-twice)
3. [Code Quality / Hygiene](#3-code-quality--hygiene)
   - 3.1 [MAX_QUADS Inconsistency with Vertex Array Size](#31-max_quads-inconsistency-with-vertex-array-size)
4. [Summary Table](#4-summary-table)

---

## 1. Performance Improvements

### ✅ 1.1 Replace Full Dcache Flush in Hot-Path Draw Macro

**File:** `src/graphics.c`, line 58  
**Priority:** High | **Difficulty:** Low  

```c
// Current
#define GU_DRAW_ARRAY_QUADS()   sceKernelDcacheWritebackInvalidateAll(); sceGumDrawArray(...)

// Problem: flushes and invalidates the *entire* CPU data cache every time
// a batch of quads is submitted to the GU. This can happen 5–20+ times per
// frame (once per texture switch, plus the end-of-frame flush).
```

`sceKernelDcacheWritebackInvalidateAll()` is one of the most expensive kernel calls available on PSP — it stalls the CPU until the entire 16 KB data cache has been written back to RAM and then invalidated. Calling it on every quad batch compounds to significant per-frame overhead.

**Fix:** Replace with a range-scoped writeback covering only the vertex data about to be submitted:

```c
#define GU_DRAW_ARRAY_QUADS() \
    sceKernelDcacheWritebackRange(g_quad_vertices, sizeof(struct Vertex) * 2 * g_current_quad); \
    sceGumDrawArray(GU_SPRITES, GU_COLOR_8888|GU_TEXTURE_16BIT|GU_VERTEX_32BITF|GU_TRANSFORM_3D, \
        2 * g_current_quad, NULL, g_quad_vertices);
```

`sceKernelDcacheWritebackRange()` (no invalidate) only flushes the modified lines covering the vertex buffer, which is a small, bounded region. Because the GU is a DMA-capable unit that does *not* write back to the same vertex memory, a full invalidate is unnecessary.

---

### ✅ 1.2 Increase MAX_QUADS to Reduce Flush Frequency

**File:** `src/graphics.c`, line 60  
**Priority:** Medium | **Difficulty:** Low  

```c
// Current
#define MAX_QUADS 100
```

The vertex array is allocated for **2,000 quads** (`g_vertex_array[2000 * 2]`), but the batch is flushed every 100 quads. This means the hardware submits up to 20 small draw calls per frame where it could submit 1–2 large ones. Each flush also triggers the expensive dcache writeback described in §1.1.

**Fix:** Raise `MAX_QUADS` to match actual usage. A value of `500` or `1000` would allow the GU to process larger continuous batches and reduce flush overhead:

```c
#define MAX_QUADS 500
```

Pair this with the range-scoped flush fix in §1.1 so that each flush of 500 quads only cleans that portion of the vertex buffer.

---

### ✅ 1.3 Enable 333 MHz Overclock by Default

**File:** `assets/settings.ini`  
**Priority:** High | **Difficulty:** Trivial  

The PSP-3000 ships with the CPU running at 222 MHz. The port supports 333 MHz via `settings.ini`:

```ini
overclock=false  ; current default
```

Raising to `true` increases available CPU time by ~50%, which directly benefits:

- OGG decode in `audio_update()` (fewer stalls before the audio callback buffer empties)
- Game-logic calculations during scoring animations
- Rendering overhead (GU command processing, matrix operations)

The PSP-3000 supports 333 MHz reliably. Battery life impact is minimal for a portable session of a card game. Recommending or defaulting this to `true` is the single highest-impact, zero-code change available.

**Fix:**

```ini
overclock=true
```

---

### ✅ 1.4 Scoped Dcache Flush for Rotated Quads

**File:** `src/graphics.c` (`graphics_draw_rotated_quad`)  
**Priority:** Medium | **Difficulty:** Low  

`graphics_draw_rotated_quad()` issues a full `sceKernelDcacheWritebackInvalidateAll()` for the 4-vertex triangle fan used to draw each rotated card. This is the same problem as §1.1 but for a different code path.

**Fix:** Replace with a range flush covering the 4 vertices at the start of `g_quad_vertices`:

```c
sceKernelDcacheWritebackRange(g_quad_vertices, sizeof(struct Vertex) * 4);
```

---

### 1.5 Redundant Dcache Flush at End of Frame

**File:** `src/graphics.c`, line 57  
**Priority:** Low | **Difficulty:** Low  

```c
#define GU_FINISH()  sceKernelDcacheWritebackInvalidateAll(); sceGuFinish(); sceGuSync(0,0);
```

`GU_FINISH()` performs a full cache flush *before* `sceGuFinish()`. By this point all quad batches have already been flushed (each via `GU_DRAW_ARRAY_QUADS()`), so the GU draw list in `g_draw_list` is the only remaining dirty region. The flush here is either already covered by the draw-array flushes, or can be replaced with a targeted range flush of `g_draw_list`.

**Fix:** Remove the `sceKernelDcacheWritebackInvalidateAll()` from `GU_FINISH` entirely, as `sceGuFinish()` + `sceGuSync()` waits for the GU to process the already-submitted command list. If the display list must be flushed, use a range flush scoped to the actual list size:

```c
#define GU_FINISH()  sceGuFinish(); sceGuSync(0,0);
```

> Note: Validate against the PSPSDK documentation for `GU_DIRECT` mode; if the display list itself must be cache-coherent before `sceGuFinish()`, a targeted range writeback on `g_draw_list` covering only the bytes written is sufficient.

---

### 1.6 Audio Decoding Runs on the Main Thread

**File:** `src/main.c`, `src/audio.c`  
**Priority:** Medium | **Difficulty:** Medium  

```c
// main game loop
while(running()) {
    audio_update();   // OGG decode here, on the main/render thread
    input_update();
    game_update();
    game_draw();
}
```

`audio_update()` calls `ov_read()` (libvorbis OGG decode) synchronously each frame to fill the pre-buffer. Vorbis decode is computationally non-trivial and adds CPU time to every game loop iteration.

The audio system already has the right architecture — a ring buffer (`AUDIO_BUFFER_CHUNKS = 8`) with a hardware callback — but the *producer* (OGG decode) runs on the same thread as rendering. The ring buffer provides enough slack that starvation is unlikely unless a frame is very slow, but it does add a variable-length CPU cost to every frame.

**Fix (optional):** Spawn a dedicated low-priority kernel thread (`sceKernelCreateThread`) that runs only `audio_update()`, sleeping when the ring buffer is full. This completely decouples audio decode from frame timing. With `overclock=true` at 333 MHz the headroom is sufficient to do this safely.

---

## 2. UI Improvements

### 2.1 Analog Stick Unused Despite Being Configured

**File:** `src/input.c`, `src/game_input.c`  
**Priority:** Medium | **Difficulty:** Low–Medium  

The input system correctly sets `PSP_CTRL_MODE_ANALOG`, which samples the left analog stick (`g_pad.Lx`, `g_pad.Ly`, range 0–255, centre 128). However, no code in `game_input.c` reads `Lx` or `Ly`. The analog stick is entirely unused in gameplay.

**Suggested uses:**

| Action | Analog mapping |
|---|---|
| Scroll through joker/card list horizontally | Lx deflection > threshold → repeat left/right |
| Quick-scroll through shop items | Combined with a held trigger |
| Navigate menus faster | Large deflection = faster repeat rate |

Even a simple threshold-based button emulation (analog stick displacement > 64 → treat as D-pad press with a 100ms repeat) would noticeably improve navigation feel without requiring a large code change.

---

### 2.2 Large Numbers Have No Thousands Separator

**File:** `src/draw.c` (score display, chip/mult display)  
**Priority:** Medium | **Difficulty:** Low  

Chip counts and scores are rendered with `sprintf` into plain integer strings (e.g. `1234567`). At higher antes, scores routinely exceed the hundreds of thousands, making numbers harder to read at a glance.

**Fix:** Add a small helper that formats integers with commas:

```c
void format_number(char *buf, int n) {
    char tmp[32];
    sprintf(tmp, "%d", n);
    int len = strlen(tmp), out = 0, commas = (len - 1) / 3;
    int start = len % 3 == 0 ? 3 : len % 3;
    int i = 0;
    for (; i < start; i++) buf[out++] = tmp[i];
    for (; i < len; i++) {
        if ((len - i) % 3 == 0) buf[out++] = ',';
        buf[out++] = tmp[i];
    }
    buf[out] = '\0';
}
```

Replace direct `sprintf(str, "%d", chips)` calls in the score/chip display functions with `format_number`.

---

### 2.3 Default Animation Speed is the Slowest Setting

**File:** `src/automated_events.c`, `assets/settings.ini`  
**Priority:** Medium | **Difficulty:** Trivial  

```c
// Speed 1 (default) → no speedup (g_current_speedup = 1.0)
// Speed 5 → SPEEDUP_STEP^4 = 0.8^4 ≈ 0.41 (animations run at 41% of normal time)
g_current_speedup = powf(SPEEDUP_STEP, (float)(g_settings.speed - 1));
```

`settings.ini` defaults to `speed=1`, which is the slowest animation speed. New players may not discover the speed setting. A default of `speed=2` or `speed=3` would make the scoring animations noticeably snappier without being disorienting, and matches how experienced Balatro players typically prefer faster animations.

**Fix:**

```ini
speed=2
```

---

### 2.4 Placeholder Joker Names in Stub Entries

**File:** `src/game.c` (joker type table)  
**Priority:** Low | **Difficulty:** Trivial  

Several not-yet-implemented joker entries use `"name"` as their display name:

```c
{ JOKER_TYPE_RESERVED_PARKING, "name", 5, ... },  // NOT IMPLEMENTED
{ JOKER_TYPE_MAIL_IN_REBATE,   "name", 5, ... },  // NOT IMPLEMENTED
{ JOKER_TYPE_STONE_JOKER,      "name", 5, ... },  // NOT IMPLEMENTED
{ JOKER_TYPE_BASEBALL_CARD,    "name", 5, ... },  // NOT IMPLEMENTED
// ... and others
```

If a player somehow encounters one of these (e.g. via a stub shop slot), the tooltip reads `"name"` with hint lines of `"test"`. While these jokers are flagged `in_use = false` and should not appear in shops, defensive text is still good practice.

**Fix:** Replace `"name"` with the correct Balatro joker name in each stub entry. The correct names can be sourced from the original game or `MISSING_FEATURES.md`. This is a data-only change with no logic impact.

---

### 2.5 Some UI Labels Use Small Font Where Big Font Fits

**File:** `src/draw.c`  
**Priority:** Low | **Difficulty:** Low  

The small font is 6×8 px and the big font is 8×8 px. Both are crisp bitmap fonts rendered at integer scale. Several prominent labels — such as the blind name, the round score target, and the current score — are drawn with `font_small` at `size=1.0`. On the 480×272 PSP screen viewed at arm's length, these can be difficult to read clearly.

**Assessment:** Review the following draw calls and evaluate swapping `font_small` for `font_big` (or `font_small` at `size=2.0`) for primary information displays:

- Blind name display
- Score target (required chips)
- Current chip total during scoring
- Round number / ante indicator

Hint text and secondary labels are appropriately sized at `font_small` size 1.

---

### 2.6 Formatted-Text Centering Traverses String Twice

**File:** `src/graphics.c` (`graphics_draw_text_formatted_center`)  
**Priority:** Low | **Difficulty:** Low  

```c
void graphics_draw_text_formatted_center(...) {
    int length = graphics_get_formatted_text_length(text, item); // first pass
    graphics_draw_text_formatted(...);                           // second pass
}
```

`graphics_draw_text_formatted_center` first calls `graphics_get_formatted_text_length()` (which walks the entire format string including resolving `#j` and `#t` tokens), then calls `graphics_draw_text_formatted()` which walks the string again. For hint tooltip text (drawn every frame while hovering over a card), this is a double traversal per label per frame.

**Fix:** Cache the computed length, or merge the measure-and-draw into a single pass that resolves the x-offset before emitting quads. Given that tooltip text is bounded and short, the practical impact is small but the fix is straightforward.

---

## 3. Code Quality / Hygiene

### 3.1 MAX_QUADS Inconsistency with Vertex Array Size

**File:** `src/graphics.c`, lines 60 and ~40  
**Priority:** Low | **Difficulty:** Trivial  

```c
#define MAX_QUADS 100                          // flush threshold
struct Vertex g_vertex_array[2000 * 2];        // capacity: 2000 quads
```

`MAX_QUADS` (100) is the flush threshold but the vertex array holds 20× more. This is harmless — the array never overflows — but the constant name implies it is the array capacity, which is misleading. Either:

- Rename it to `QUAD_FLUSH_THRESHOLD` and add a `QUAD_ARRAY_CAPACITY 2000` constant, or
- Increase `MAX_QUADS` to match the array capacity (see §1.2)

---

## 4. Summary Table

| # | Area | Item | Priority | Difficulty | Code Change? |
|---|---|---|---|---|---|
| 1.1 | Perf | Replace full dcache flush in `GU_DRAW_ARRAY_QUADS` | **High** | Low | Yes |
| 1.2 | Perf | Increase `MAX_QUADS` from 100 → 500+ | **Medium** | Low | Yes |
| 1.3 | Perf | Default `overclock=true` in `settings.ini` | **High** | Trivial | Settings only |
| 1.4 | Perf | Scoped dcache flush in `graphics_draw_rotated_quad` | Medium | Low | Yes |
| 1.5 | Perf | Remove redundant full flush from `GU_FINISH` | Low | Low | Yes |
| 1.6 | Perf | Move OGG decode to a dedicated kernel thread | Medium | Medium | Yes |
| 2.1 | UI | Map analog stick to card/menu navigation | Medium | Low–Medium | Yes |
| 2.2 | UI | Thousands separator for large chip counts | Medium | Low | Yes |
| 2.3 | UI | Default animation speed to `speed=2` | Medium | Trivial | Settings only |
| 2.4 | UI | Fix placeholder joker names (`"name"` → real names) | Low | Trivial | Data only |
| 2.5 | UI | Use `font_big` for primary score/blind displays | Low | Low | Yes |
| 2.6 | Perf/UI | Single-pass measure-and-draw for centered formatted text | Low | Low | Yes |
| 3.1 | Hygiene | Rename `MAX_QUADS` or align it with array capacity | Low | Trivial | Yes |

**Quick wins (settings-only, zero rebuild required):**
- Set `overclock=true` → biggest single performance gain
- Set `speed=2` → immediately snappier feel

**Highest-impact code changes:**
- §1.1 (range dcache flush) → eliminates the most expensive per-frame overhead
- §1.2 (increase MAX_QUADS) → reduces draw call count and compounds the §1.1 benefit
- §2.1 (analog stick navigation) → most noticeable UX upgrade
