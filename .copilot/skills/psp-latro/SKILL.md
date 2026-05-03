---
name: psp-latro
description: "Expert skill for developing PSP-latro, a PSP homebrew port of Balatro written in C. Use when asked to build, modify, debug, or deploy PSP-latro. Covers the PSPSDK toolchain, PSP GU rendering system, unity-build pattern, deploy-to-memory-stick workflow, and project-specific performance guidelines."
---

# PSP-latro Development Skill

PSP-latro is a C homebrew port of Balatro for the PSP-3000. It uses the PSPSDK toolchain (psp-gcc), the PSP hardware graphics unit (GU), and a unity-build compilation model.

## When to Use This Skill

- Building or compiling PSP-latro
- Editing `src/graphics.c`, `src/game.c`, or any source file
- Adding or modifying rendering code (sprites, quads, fonts, textures)
- Working with PSP input, audio, or power management
- Deploying a new EBOOT.PBP to the physical PSP memory stick
- Investigating or improving performance
- Reading or writing `assets/settings.ini`

---

## Project Layout

```
/Users/Scott/PSP-latro/
├── Makefile
├── assets/
│   └── settings.ini          # Runtime config read at startup
├── media/
│   ├── pspalatro_icon.png
│   └── pspalatro_pic.png
└── src/
    ├── main.c                # Entry point; #includes all other .c files (unity build)
    ├── global.h              # Shared types, macros, externs — included everywhere
    ├── graphics.c            # GU renderer, textures, fonts, quad batching
    ├── draw.c                # High-level draw helpers (cards, UI, text)
    ├── audio.c               # OGG decode, ring buffer, pspaudiolib callbacks
    ├── input.c               # Raw PSP button/analog state
    ├── game_input.c          # Game-level input handling
    ├── game.c                # Core game logic
    ├── game_util.c           # Utility helpers used by game.c
    ├── entity_event.c        # Entity/event system
    ├── automated_events.c    # Scripted / AI-driven event sequences
    ├── callbacks.c           # PSP OS callbacks (home button, power)
    ├── archive.c             # ZIP archive reading (game.love assets)
    └── ini.c                 # INI file parser for settings.ini
```

### Unity Build

`main.c` `#include`s every other `.c` file directly. There is **only one translation unit**. Do not add `#include "*.c"` for new files — append the include to `src/main.c`. Do not add include guards to `.c` files.

---

## Build System

### Toolchain

- **Compiler:** `psp-gcc` via PSPSDK at `/Users/Scott/pspdev`
- **Flags:** `-Ofast -Wall -Wno-unused-label -G0`
- **Output:** `EBOOT.PBP` (PSP executable) in the repo root

### Build Commands

```bash
# Normal build
cd /Users/Scott/PSP-latro && make

# Force full rebuild
cd /Users/Scott/PSP-latro && make clean && make

# Check for errors without producing output
cd /Users/Scott/PSP-latro && make 2>&1 | head -40
```

### Linked Libraries

```
-lpspgum -lpspgu -lpspdmac -lpsppower
-lpspaudiolib -lpspaudio
-lvorbisenc -lvorbisfile -lvorbis -logg
-lzip -lz -lbz2 -llzma -lmbedcrypto
```

---

## Deploy to PSP

The PSP memory stick mounts at `/Volumes/Untitled`. Game files live at:

```
/Volumes/Untitled/PSP/GAME/PSPALATRO/
    EBOOT.PBP      ← replace this after every build
    game.love      ← Balatro asset archive (rarely changes)
    settings.ini   ← runtime config (rarely changes)
    editions.png   ← texture atlas (rarely changes)
```

### Deploy Command

```bash
# Copy only the binary after a successful build
cp /Users/Scott/PSP-latro/EBOOT.PBP "/Volumes/Untitled/PSP/GAME/PSPALATRO/EBOOT.PBP"
```

Only copy `game.love` / `settings.ini` / `editions.png` when those files have actually changed.

---

## Screen & Display

| Constant | Value |
|---|---|
| `SCREEN_WIDTH` | 480 |
| `SCREEN_HEIGHT` | 272 |
| `BUFFER_WIDTH` | 512 (GU alignment requirement) |
| `BUFFER_HEIGHT` | 272 |

Origin is top-left. The GU framebuffer uses triple buffering (`g_frame_buffer_0/1/2`).

---

## Graphics System (`src/graphics.c`)

### Vertex Format

```c
struct Vertex {
    int16_t u, v;     // texture coordinates
    uint32_t color;   // ABGR packed colour
    float x, y, z;    // screen position
};
```

Vertex arrays must be `__attribute__((aligned(16)))` for DMA.

### Quad Batching

```c
#define MAX_QUADS 500          // max quads per batch before forced flush
// g_vertex_array[2000 * 2]   // total vertex budget (2000 quads)

GU_START()                    // begin frame, reset vertex pointer
GU_ALLOC_QUAD_ARRAY()         // allocate next batch region
GU_DRAW_ARRAY_QUADS()         // flush batch to GU (range dcache + sceGumDrawArray)
GU_FINISH()                   // sceKernelDcacheWritebackAll + sceGuFinish + sceGuSync
```

`GU_DRAW_ARRAY_QUADS()` performs a **range-scoped** dcache writeback — only the bytes covering the current batch. Do not replace it with `sceKernelDcacheWritebackInvalidateAll()`.

### Textures & Fonts

```c
#define MAX_TEXTURES 32
#define MAX_FONTS    5

struct Texture { bool in_use; int width, height; uint8_t *data; int format; };
struct Font     { bool in_use; int texture; int width, height; int length_x, length_y; };
```

Textures are swizzled into VRAM. Load-time flushes use `sceKernelDcacheWritebackInvalidateAll()` — this is intentional and correct; do not change them.

---

## Performance Guidelines

| Rule | Reason |
|---|---|
| Use `sceKernelDcacheWritebackRange(ptr, size)` in draw hot-paths | Full cache flush stalls ~16 KB per call |
| Never call `sceKernelDcacheWritebackInvalidateAll()` in per-frame code | Extremely expensive; only correct at load time |
| Keep `MAX_QUADS` at 500 | Matches vertex array budget; reduces flush count per frame |
| `overclock = true` in `settings.ini` | 333 MHz vs 222 MHz; ~50% more CPU headroom |
| Do not decode OGG on the render thread in tight loops | Audio ring buffer (`AUDIO_BUFFER_CHUNKS = 8`) provides slack |

### Current `GU_FINISH` macro

```c
#define GU_FINISH()  sceKernelDcacheWritebackAll(); sceGuFinish(); sceGuSync(0,0);
```

`sceKernelDcacheWritebackAll()` (no invalidate) is used here — this is a deliberate improvement over the original `WritebackInvalidateAll`. Future work (§1.5) is to remove it entirely since all vertex batches are already flushed by `GU_DRAW_ARRAY_QUADS()`.

---

## `assets/settings.ini` Reference

| Key | Type | Default | Notes |
|---|---|---|---|
| `archive_file_name` | string | `Balatro.exe` | Asset archive filename inside `game.love` |
| `hand_size` | int | `8` | Cards dealt per hand |
| `hands` | int | `4` | Hands per round |
| `discards` | int | `3` | Discards per round |
| `wealth` | int | `5` | Starting gold |
| `joker_slots` | int | `5` | Max jokers |
| `consumable_slots` | int | `2` | Max consumables |
| `shop_item_slots` | int | `2` | Shop card slots |
| `shop_booster_slots` | int | `2` | Shop booster slots |
| `audio` | bool | `true` | Enable audio |
| `move_cards` | bool | `true` | Card movement animations |
| `overclock` | bool | `true` | 333 MHz CPU (recommended) |
| `speed` | int | `2` | Animation speed (1=slow … 4=fast) |
| `ante_score_scaling` | int | `2` | Score requirement scaling per ante |

Parsed at startup by `ini.c` into the global `struct Settings g_settings`.

---

## PSP API Quick Reference

| API | Header | Purpose |
|---|---|---|
| `sceGuStart/Finish/Sync` | `<pspgu.h>` | GU command list management |
| `sceGumDrawArray` | `<pspgum.h>` | Submit vertex array to GU |
| `sceKernelDcacheWritebackRange` | `<pspkernel.h>` | Flush CPU→RAM for specific buffer |
| `sceKernelDcacheWritebackAll` | `<pspkernel.h>` | Flush entire dcache (no invalidate) |
| `scePowerSetClockFrequency` | `<psppower.h>` | Set CPU/bus clock (called from settings load) |
| `sceCtrlReadBufferPositive` | `<pspctrl.h>` | Read button + analog state |
| `pspAudioLibSetChannelCallback` | `<pspaudiolib.h>` | Audio fill callback |

---

## Common Patterns

### Adding a new texture draw call

1. Load texture via the existing texture API in `graphics.c` — returns an index into `g_textures[]`.
2. In your draw function, call `GU_ALLOC_QUAD_ARRAY()` before filling vertices.
3. Write two `struct Vertex` entries (top-left, bottom-right sprite corners) into `g_quad_vertices[g_current_quad * 2]` and `g_quad_vertices[g_current_quad * 2 + 1]`.
4. Increment `g_current_quad`. If `g_current_quad >= MAX_QUADS`, call `GU_DRAW_ARRAY_QUADS()` then `GU_ALLOC_QUAD_ARRAY()` to flush and start a new batch.

### Reading input

```c
// In game_input.c — buttons are already debounced by input.c
if (g_input.pressed & PSP_CTRL_CROSS)  { /* confirm */ }
if (g_input.held   & PSP_CTRL_RIGHT)   { /* held right */ }
// Analog (currently unused)
int lx = g_pad.Lx;  // 0–255, centre=128
int ly = g_pad.Ly;
```

### Modifying settings at runtime

`g_settings` is a global `struct Settings`. Changes are not written back to `settings.ini` — they are session-only unless you call the INI serialiser explicitly.
