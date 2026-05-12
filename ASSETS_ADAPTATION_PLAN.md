# PSP-Latro Asset Adaptation Plan

**Status**: In Progress  
**Last Updated**: 12 May 2026  
**Target Platform**: PSP (480x272 resolution, ~24-32 MB RAM)

## Overview

This document tracks the extraction and optimization of assets from the original Balatro PC game (`game.love`) for PSP-Latro. Assets must be downscaled, optimized for memory constraints, and converted to PSP-compatible formats.

## Asset Source Inventory

**Source**: `third_party/balatro-v011-reference/game.love`  
**Total images in archive**: 53 files

### High-Priority Assets (Extraction Candidates)

| Asset | Location | Size | Purpose | PSP Adaptation |
|-------|----------|------|---------|-----------------|
| **background.jpg** | `resources/background/` | 77 KB | Menu background | Resize to 480x272, convert to PNG |
| **balatro.png** | `resources/textures/1x/balatro.png` | 27 KB | Main logo | Resize to fit PSP UI (est. 240x120) |
| **balatro_alt.png** | `resources/textures/1x/balatro_alt.png` | 21 KB | Alt logo variant | Resize for PSP UI |
| **ui_assets.png** | `resources/textures/1x/ui_assets.png` | 1.5 KB | UI icons/elements | Already small; evaluate spritesheet |
| **icons.png** | `resources/textures/1x/icons.png` | 8.5 KB | Small UI icons | Evaluate for PSP UI |

### Lower-Priority Assets (Not Recommended for Direct Import)

| Asset | Location | Size | Reason |
|-------|----------|------|--------|
| **Jokers.png** | `resources/textures/1x/` | 504 KB | Far too large for PSP; requires complete redesign |
| **Tarots.png** | `resources/textures/1x/` | 96 KB | PC scale; needs downscaling & optimization |
| **Enhancers.png** | `resources/textures/1x/` | 76 KB | PC scale; needs significant optimization |
| **boosters.png** | `resources/textures/1x/` | 171 KB | PC scale; large for PSP memory budget |
| **collabs/\*.png** | `resources/textures/1x/collabs/` | ~7 KB each | Collaborator art; lower priority for MVP |
| **Fonts/** | `resources/fonts/` | ~60 MB total | Already using system fonts; skip bulk import |

## PSP Constraints

- **Screen Resolution**: 480x272 (16:9 aspect ratio)
- **Memory**: ~24-32 MB total (shared with OS + game logic)
- **Texture Format**: 32-bit RGBA (8 bytes/pixel) or 16-bit 4444 (4 bytes/pixel)
- **Max Recommended Texture**: ~256x256 to 512x512 depending on usage
- **Path Resolution**: Multi-candidate probing via `graphics_load_image()` in `src/graphics.c`

## Adaptation Strategy

### Step 1: Extraction
- Extract from `game.love` using `unzip` to `third_party/balatro-v011-reference/extracted_assets/`
- Organize by asset type: `backgrounds/`, `logos/`, `ui_icons/`, etc.

### Step 2: Optimization
- Resize to PSP constraints using ImageMagick or similar
- Convert to PNG (RGB or RGBA as appropriate)
- Target formats:
  - **Backgrounds/Wallpapers**: 480x272 PNG, 32-bit RGBA (quality)
  - **Logos/UI Elements**: 256x256 or smaller, 32-bit RGBA
  - **Small Icons**: 64x64 or smaller, 16-bit 4444 (memory efficient)

### Step 3: Deployment
- Place optimized assets in `assets/` subfolder (e.g., `assets/backgrounds/`, `assets/logos/`)
- Makefile `sync_build_artifacts` automatically copies to `build/assets/` for launch
- Reference in code using relative keys: `graphics_load_image("backgrounds/bg_alt.png")`

### Step 4: Testing
- Build and verify runtime logs: `[TEX] Loaded ... from path[X]`
- Check memory usage and texture format warnings
- Validate visual appearance in PPSSPP

## Extraction Workflow (Examples)

### ✅ COMPLETED: Extract background.jpg → Convert to 480x272 PNG

**Status**: ✅ Extraction & Deployment Complete  
**Asset**: `assets/backgrounds/balatro_bg_alt.png`  
**Dimensions**: 480x272 PNG  
**File Size**: 226 KB  
**Runtime Test**: ✅ PASSED  

**Test Output**:
```
[TEX] Loaded backgrounds/balatro_bg_alt.png from path[4]: assets/backgrounds/balatro_bg_alt.png (480x272)
[ASSET_TEST] ✓ Successfully loaded backgrounds/balatro_bg_alt.png (texture=23)
```

**Workflow Used**:
```bash
# 1. Extract from game.love
unzip -j game.love "resources/background/background.jpg" -d /tmp/

# 2. Resize & convert to PNG with ImageMagick
magick background.jpg -resize 480x272\! -quality 90 balatro_bg_alt.png

# 3. Place in assets
cp balatro_bg_alt.png assets/backgrounds/

# 4. Build and sync
make clean && make -j4

# 5. Runtime loads successfully via graphics_load_wallpaper("backgrounds/balatro_bg_alt.png")
```

**Path Resolution Verified**: Asset found via path[4] = `assets/backgrounds/balatro_bg_alt.png`

---

### Extract & adapt balatro.png → UI logo (240x120)
```bash
# 1. Extract
unzip -j /path/to/game.love "resources/textures/1x/balatro.png" -d /tmp/extracted/

# 2. Resize to fit PSP screen
magick /tmp/extracted/balatro.png -resize 240x120 -quality 95 assets/logos/balatro_logo.png

# 3. Deploy & test
make clean && make -j4
```

## Current Asset Structure in PSP-Latro

```
assets/
├── wallpapers/              ← Menu backgrounds (current)
│   ├── title_wallpaper.png
│   ├── title_wallpaper_b.png
│   ├── main_menu_wallpaper.png
│   └── main_menu_wallpaper_b.png
├── backgrounds/             ← [PLANNED] Additional background variants
├── logos/                   ← [PLANNED] Balatro logo & variants
├── ui_icons/                ← [PLANNED] UI elements extracted from balatro
└── [other]/                 ← Custom or future asset categories
```

## Next Steps

**Choose one of the following tasks:**

### Option A: Wallpaper Variant from Background
- Extract `resources/background/background.jpg` from game.love
- Resize to 480x272 PNG
- Add as `assets/backgrounds/menu_bg_alternate.png`
- Update menu code to use as secondary wallpaper variant

### Option B: Balatro Logo for PSP UI
- Extract `resources/textures/1x/balatro.png` and `balatro_alt.png`
- Resize to ~240x120 for PSP screen proportions
- Place in `assets/logos/`
- Integrate into menu or title screen display

### Option C: UI Icons/Elements
- Extract `resources/textures/1x/ui_assets.png` and `icons.png`
- Analyze spritesheet layout and identify usable elements
- Create cropped/optimized versions for PSP UI
- Add to `assets/ui_icons/`

### Option D: Custom Design Instead
- Skip asset extraction; design custom PSP-native art
- Keep current approach of minimalist PSP-optimized visuals

---

## Reference: Path Resolution in Code

When you reference an asset with `graphics_load_image("backgrounds/menu_bg.png")`, the graphics system tries these paths (in order):

1. `backgrounds/menu_bg.png` (direct)
2. `./backgrounds/menu_bg.png` (relative)
3. `../backgrounds/menu_bg.png` (parent dirs)
4. `assets/backgrounds/menu_bg.png` ← **Matches in dev build**
5. `ms0:/PSP/GAME/PSPALATRO/assets/backgrounds/menu_bg.png` ← **Matches on actual PSP**
6. Archive fallback (game.love ZIP)

**Result**: Single relative key works across dev, build, and PSP runtime contexts automatically.

---

## Memory Budget Example

For reference, current wallpaper memory usage:

- Wallpaper (480x272 RGBA): ~524 KB per texture
- 2 title variants + 2 menu variants = ~2 MB total
- Remaining budget: ~20+ MB for game logic, sprites, sounds

**Guideline**: Keep individual textures under 512x512 unless absolutely necessary.

---

## Documentation Status

- ✅ Asset inventory compiled
- ✅ PSP constraints documented
- ✅ Extraction workflow outlined
- ⏳ Awaiting user selection of priority assets
- ⏳ Extraction & optimization to follow
