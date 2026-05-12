# Balatro Assets PSP Optimization Plan

**Status**: Planning  
**Date**: 12 May 2026  
**Goal**: Adapt PC game assets to PSP hardware constraints

---

## Execution Checklist

### Current Progress Snapshot
- [x] Extracted original Balatro resources from game.love into third_party/balatro-v011-reference/game_assets_extracted/resources
- [x] Created easy-access symlinks in assets/ (balatro-textures, balatro-sounds, balatro-fonts, balatro-shaders)
- [x] Converted background.jpg to PSP-safe 480x272 PNG (assets/backgrounds/balatro_bg_alt.png)
- [x] Verified background loads in runtime logs
- [x] Create PSP-optimized UI/logo output set
- [x] Create PSP-optimized card sprites output set
- [x] Create PSP-optimized effects output set
- [x] Integrate optimized assets into runtime code paths
- [x] Verify visual quality and memory behavior in PPSSPP

### Workboard (Methodical Order)

### Mandatory Render Verification (Required For Every Asset Batch)
- [x] Build and sync after asset creation (`make clean && make -j4`)
- [x] Execute PPSSPP using `build/EBOOT.PBP`
- [x] Confirm new asset path appears in texture load logs
- [x] Confirm visual render in menu/game scene (not just load success)
- [x] Record pass/fail evidence in this checklist before closing task

Evidence format (append under milestone task when tested):
- Asset: <asset path>
- Log: <loaded from path[n] ...>
- Visual: Pass/Fail
- Notes: <artifact quality issues, scaling artifacts, color banding, etc.>

#### Milestone 1: UI and Branding (Fast Win)
- [x] Create output folders: assets/balatro-ui-optimized/icons and assets/balatro-ui-optimized/buttons
- [x] Resize balatro.png to logo_main_psp.png (target: ~240x120)
- [x] Resize balatro_alt.png to logo_alt_psp.png (target: ~240x120)
- [x] Extract at least 6 high-value icons from icons.png
- [x] Extract at least 4 button/UI elements from ui_assets.png
- [x] Produce a small index file listing source sprite -> output file

Acceptance criteria:
- [x] All files exist under assets/balatro-ui-optimized
- [x] Logos are legible at PSP resolution
- [x] Asset file names are stable and descriptive
- [x] At least one Milestone 1 asset is runtime render-verified in PPSSPP

Verification evidence:
- Asset: assets/balatro-ui-optimized/logo_main_psp.png
- Log: [ASSET_VERIFY] loaded texture: balatro-ui-optimized/logo_main_psp.png (texture=25)
- Log: [ASSET_VERIFY] drawing title logo texture=25 at 147.5,54.0 size=185.0x120.0
- Visual: Pass
- Notes: Logo is loaded and drawn in title scene via runtime path, not just preloaded.

#### Milestone 2: Core Cards
- [x] Create output folders: assets/balatro-cards-optimized/jokers and assets/balatro-cards-optimized/tarots
- [x] Generate Jokers test variants: 50% scale 16-bit, 50% scale quantized, 25% scale 16-bit
- [x] Compare variants in PPSSPP and choose one default profile
- [x] Export chosen joker sheet as jokers_psp.png
- [x] Export chosen tarot sheet as tarots_psp.png
- [x] Create card_meta.txt documenting sheet dimensions and intended sprite regions
- [x] Render-verify chosen card sheets in PPSSPP and capture load + visual evidence

Acceptance criteria:
- [x] Chosen profile has acceptable readability on PSP
- [x] Total size of cards-optimized folder stays within target budget
- [x] Runtime can load both sheets using current texture loader path logic
- [x] Visual verification completed in live PSP/PPSSPP render path

Profile decision:
- Default profile: 25% scale + reduced depth workflow
- Reason: avoids PSP texture crop risk on large sheets while preserving usable readability and lower memory footprint.

Verification evidence:
- Asset: assets/balatro-cards-optimized/jokers/jokers_psp.png
- Log: [TEX] Loaded balatro-cards-optimized/jokers/jokers_psp.png from path[4]: assets/balatro-cards-optimized/jokers/jokers_psp.png (178x380)
- Log: [ASSET_VERIFY] loaded texture: balatro-cards-optimized/jokers/jokers_psp.png (texture=26)
- Asset: assets/balatro-cards-optimized/tarots/tarots_psp.png
- Log: [TEX] Loaded balatro-cards-optimized/tarots/tarots_psp.png from path[4]: assets/balatro-cards-optimized/tarots/tarots_psp.png (178x143)
- Log: [ASSET_VERIFY] loaded texture: balatro-cards-optimized/tarots/tarots_psp.png (texture=27)
- Draw: [ASSET_VERIFY] drawing card previews in title scene jokers_tex=26 tarots_tex=27
- Visual: Pass
- Notes: both optimized card sheets loaded from build/assets and rendered in title preview.

#### Milestone 3: Effects and Supplemental Art
- [x] Create output folder: assets/balatro-effects-optimized
- [x] Optimize Enhancers.png, boosters.png, BlindChips.png with selected profile
- [x] Extract any required individual effect sprites if sheet-only approach is unclear
- [x] Write effects_meta.txt with sprite notes
- [x] Render-verify at least one effect asset in PPSSPP and capture evidence

Acceptance criteria:
- [x] Effects look acceptable at PSP scale
- [x] No obvious texture budget warnings during load
- [x] Visual verification completed in live PSP/PPSSP render path

Verification evidence:
- Asset: assets/balatro-effects-optimized/enhancers_psp.png
- Log: [TEX] Loaded balatro-effects-optimized/enhancers_psp.png from path[4]: assets/balatro-effects-optimized/enhancers_psp.png (124x119)
- Log: [ASSET_VERIFY] loaded texture: balatro-effects-optimized/enhancers_psp.png (texture=28)
- Draw: [ASSET_VERIFY] drawing effect preview in title scene effect_tex=28
- Visual: Pass
- Notes: Enhancers preview rendered cleanly alongside title/menu previews.

#### Milestone 4: Integration and Validation
- [x] Add/adjust runtime references to point at optimized assets
- [x] Ensure Makefile sync copies optimized folders to build/assets
- [x] Run clean build
- [x] Launch PPSSPP and capture texture load logs
- [x] Confirm no regressions in menu flow or wallpaper behavior
- [x] Confirm integrated optimized assets visually render in their intended scene/state

Acceptance criteria:
- [x] Build succeeds
- [x] Assets resolve from expected paths
- [x] No startup hang/regression introduced

### Definition of Done (Project-Level)
- [x] All targeted optimized folders exist under assets/
- [x] Optimized assets are synced into build/assets after build
- [x] Core menu art and selected gameplay art load successfully in PPSSPP
- [x] Texture memory behavior is acceptable for PSP constraints
- [x] Checklist marked complete for all milestones

---

## PSP Hardware Constraints

| Aspect | Limit | Impact |
|--------|-------|--------|
| **Screen** | 480x272 | Wallpapers already optimized; sprites rarely need full size |
| **RAM** | 24-32 MB | Total available for game + assets + code |
| **Texture Size** | Power-of-2 (max 512x512 practical) | Requires spritesheet reorganization |
| **Format** | 16-bit 4444 or 32-bit RGBA | Current: wallpapers use 32-bit; sprites use 16-bit |
| **Swizzling** | Required for GU | Graphics system handles automatically |

---

## Asset Size Inventory (PC Versions)

### High-Priority Textures (Game-Critical)
| Asset | Size | Type | Priority | Action |
|-------|------|------|----------|--------|
| **Jokers.png** | 493 KB | Card sprites | Critical | Split into smaller sheets or individual sprite extraction |
| **Tarots.png** | 94 KB | Card sprites | Critical | Downscale or optimize |
| **Enhancers.png** | 74 KB | Effect sprites | High | Downscale to essential sizes |
| **boosters.png** | 167 KB | Booster graphics | High | Downscale or split |
| **Vouchers.png** | 69 KB | Voucher icons | Medium | Extract individual icons |
| **BlindChips.png** | 82 KB | Boss sprites | Medium | Extract essential frames |

### UI/Logo Assets (Menu-Critical)
| Asset | Size | Type | Priority | Action |
|-------|------|------|----------|--------|
| **balatro.png** | 26 KB | Logo | High | Resize to menu scale (~240x120) |
| **balatro_alt.png** | 21 KB | Alt logo | Medium | Resize to menu scale |
| **ui_assets.png** | 1.5 KB | UI elements | High | Extract individual buttons/elements |
| **icons.png** | 8.5 KB | Icons | High | Extract individual icons |
| **gamepad_ui.png** | 20 KB | Gamepad prompts | Medium | Resize or extract essential |
| **tags.png** | 7.3 KB | Tag indicators | Low | Already small; minimal optimization needed |

### Low-Priority Assets
| Asset | Size | Status |
|-------|------|--------|
| **8BitDeck.png** | 46 KB | Optional decoration |
| **8BitDeck_opt2.png** | 61 KB | Optional decoration |
| **ShopSignAnimation.png** | 11 KB | Nice-to-have animation |
| **stickers.png** | 4.3 KB | Already minimal |
| **chips.png** | 8.1 KB | Already minimal |
| **collabs/** | ~7 KB each | 30 pairs - optional cosmetics |

---

## Optimization Strategy

### Phase 1: Logo/UI Assets (Easiest)
**Goal**: Get recognizable branding into menu system  
**Complexity**: Low  
**Assets**: `balatro.png`, `balatro_alt.png`, `ui_assets.png`, `icons.png`

**Steps**:
1. Resize `balatro.png` to 240x120 (maintains aspect, fits PSP screen)
2. Resize `balatro_alt.png` similarly
3. Extract individual icons from `icons.png` (1x per icon ~32x32)
4. Extract UI buttons from `ui_assets.png`
5. Convert all to PNG, optimize colors

**Target Output**: `assets/balatro-ui/`
- `logo_main.png` (240x120)
- `logo_alt.png` (240x120)
- `icons/` (individual 32-64px files)
- `buttons/` (individual UI elements)

---

### Phase 2: Card Sprites (Medium Complexity)
**Goal**: Extract playable card graphics at PSP scale  
**Complexity**: Medium  
**Assets**: `Jokers.png` (493 KB), `Tarots.png` (94 KB)

**Challenges**:
- These are massive spritesheets designed for high-res PC
- Need to identify individual card layouts
- Scaling down loses detail; may need artistic decisions

**Options**:

#### Option A: Extract Individual Cards
- Extract specific cards as individual 128x128 or 64x64 PNGs
- Reduces memory footprint dramatically
- Slower to load many cards (more I/O)
- Better for selective display (showing single card)

#### Option B: Create Optimized Spritesheets
- Downscale entire sheet to 50% (Jokers: 246 KB → ~60 KB)
- Reorganize into power-of-2 grids (256x512, 512x256)
- Single texture load, fast rendering
- May lose some visual fidelity

#### Option C: Hybrid Approach
- Extract most-used cards as individual files
- Keep less-used cards in smaller spritesheet
- Best balance for gameplay

**Recommended**: Option C (Hybrid)

**Target Output**: `assets/balatro-cards/`
```
├── jokers/
│   ├── joker_wildcard.png (individual)
│   ├── joker_dupe.png (individual)
│   └── joker_sheet_misc.png (50% downscaled sheet for less-used)
├── tarots/
│   ├── tarot_main_sheet.png (50% downscaled)
│   └── [individual extracts if needed]
└── meta.txt (sprite layout info)
```

---

### Phase 3: Effects & Enhancements (Advanced)
**Goal**: Add visual polish without breaking budget  
**Complexity**: High  
**Assets**: `Enhancers.png`, `boosters.png`, `BlindChips.png`

**Strategy**:
- Extract only essential effect frames (not full animations)
- Create minimal spritesheet for PSP (128x128 or 256x256)
- Use shader effects where possible instead of texture animation

---

## Color/Format Optimization

### Current Approach
- **Wallpapers**: 32-bit RGBA (quality priority)
- **Sprites**: 16-bit 4444 (memory priority)

### Recommended for Extracted Assets
| Asset Type | Format | Rationale |
|-----------|--------|-----------|
| Card sprites | 16-bit 4444 | Game logic, not critical visual |
| UI elements | 16-bit 4444 or 8-bit indexed | Small, frequent rendering |
| Logos | 32-bit RGBA | Visual showcase (few textures) |
| Effects | 16-bit 4444 | Animation frames, performance |

### Quantization Options
- **Auto**: ImageMagick can reduce to 256 colors with dithering
- **Manual**: Extract palette, optimize, apply to all related assets
- **Mixed**: Keep logos crisp (32-bit), quantize gameplay assets (16-bit)

---

## Extraction Tools & Workflow

### ImageMagick Commands

**Downscale to 50%**:
```bash
magick jokers.png -resize 50% jokers_psp.png
```

**Downscale + Quantize to 256 colors**:
```bash
magick jokers.png -resize 50% -colors 256 -depth 8 jokers_psp.png
```

**Downscale + Convert to 16-bit**:
```bash
magick jokers.png -resize 50% -depth 5 jokers_psp.png
```

**Resize to exact PSP power-of-2 size**:
```bash
magick jokers.png -resize 256x256\! jokers_psp.png
```

**Extract region (specific card)**:
```bash
magick jokers.png -crop 128x128+0+0 card_1.png
```

---

## Memory Budget Analysis

### Assume 4 MB allocated for textures

| Asset | PC Size | PSP 50% | PSP 25% | Use Case |
|-------|---------|---------|---------|----------|
| Jokers.png | 493 KB | 123 KB | 31 KB | Core gameplay |
| Tarots.png | 94 KB | 24 KB | 6 KB | Tarot cards |
| Enhancers.png | 74 KB | 19 KB | 5 KB | Effects |
| boosters.png | 167 KB | 42 KB | 10 KB | Boosters |
| Vouchers.png | 69 KB | 17 KB | 4 KB | Vouchers |
| Wallpapers (4x) | - | - | - | ~2 MB (already allocated) |
| **Total Sprites (50%)** | - | **225 KB** | - | Well under budget |
| **Total Sprites (25%)** | - | - | **56 KB** | Very conservative |

**Conclusion**: 50% downscaling brings assets well within budget with room for sound, fonts, game code.

---

## Immediate Next Steps

1. Complete Milestone 1 (UI and Branding) first.
2. Run card variant bake-off from Milestone 2 before full card export.
3. Integrate only after Milestones 1 and 2 assets are validated in PPSSPP logs.

---

## File Organization Plan

After optimization, structure will be:

```
PSP-latro/assets/
├── wallpapers/                    (existing - 4x 480x272 PNGs)
├── backgrounds/                   (existing - balatro_bg_alt.png)
├── balatro-ui-optimized/          (NEW - PSP logos & UI)
│   ├── logo_main_psp.png
│   ├── logo_alt_psp.png
│   ├── icons/
│   └── buttons/
├── balatro-cards-optimized/       (NEW - Card sprites)
│   ├── jokers_psp.png (50% downscaled)
│   ├── tarots_psp.png (50% downscaled)
│   └── card_meta.txt
├── balatro-effects-optimized/     (NEW - Effect sprites)
│   ├── enhancers_psp.png
│   └── effects_meta.txt
└── balatro-textures/              (symlink - original PC assets for reference)
```

Makefile `sync_build_artifacts` will copy `*-optimized/` folders to `build/assets/`.

---

## Decision Point: What to Optimize First?

**Recommended Priority Order**:

1. **UI/Logos** (Phase 1) - Fastest ROI, menu appearance
   - Time: 30 min
   - Impact: High (visual quality of menu)
   - Difficulty: Easy
   
2. **Card Sprites** (Phase 2) - Core gameplay visuals
   - Time: 2-3 hours (includes spritesheet analysis)
   - Impact: Critical (card display system)
   - Difficulty: Medium

3. **Effects** (Phase 3) - Polish
   - Time: 1-2 hours
   - Impact: Nice-to-have
   - Difficulty: High

---

## Action Items

- [x] Milestone 1 complete
- [x] Milestone 2 complete
- [x] Milestone 3 complete
- [x] Milestone 4 complete

