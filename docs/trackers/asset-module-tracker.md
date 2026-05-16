# PSP Asset-to-Module Tracker

**Branch**: `feature/menu-system`  
**Purpose**: Track how original Balatro assets map to PSP-optimized outputs and the game modules that load or draw them.

## Working Model

Use this pipeline for every asset group:

`original Balatro asset` -> `PSP-optimized output in assets/` -> `runtime loader in src/` -> `draw/use in game`

The goal is not only to store the resized files, but to make sure each asset is loaded by the correct module in the game.

## Current Mapping

| Asset Group | Original Source | PSP Output | Runtime Module | Current Status |
|---|---|---|---|---|
| Title background art | `assets/backgrounds/balatro_bg_alt.png` | `assets/backgrounds/balatro_bg_alt.png` | `src/menu.c` (`menu_init_wallpapers`, `menu_draw_title`) | Integrated |
| Main logo art | `assets/balatro-textures/1x/balatro.png` | `assets/balatro-ui-optimized/logo_main_psp.png` | `src/menu.c` (`menu_load_logo_variant`, `menu_draw_title`) | Integrated |
| Alternate logo art | `assets/balatro-textures/1x/balatro_alt.png` | `assets/balatro-ui-optimized/logo_alt_psp.png` | `src/menu.c` | Available, not currently mapped in title |
| UI icons | `assets/balatro-textures/1x/icons.png` | `assets/balatro-ui-optimized/icons/*.png` | future UI use in `src/draw.c` and `src/menu.c` | Pending integration |
| UI buttons | `assets/balatro-textures/1x/ui_assets.png` | `assets/balatro-ui-optimized/buttons/*.png` | future UI use in `src/draw.c` and `src/menu.c` | Pending integration |
| Joker spritesheet | `assets/balatro-textures/1x/Jokers.png` | `assets/balatro-cards-optimized/jokers/jokers_psp.png` | `src/draw.c` | Integrated with PSP UV mapping |
| Tarot spritesheet | `assets/balatro-textures/1x/Tarots.png` | `assets/balatro-cards-optimized/tarots/tarots_psp.png` | `src/draw.c` | Integrated with PSP UV mapping |
| Booster art | `assets/balatro-textures/1x/boosters.png` | `assets/balatro-effects-optimized/boosters_psp.png` | `src/draw.c` | Integrated with PSP UV mapping |
| Enhancer art | `assets/balatro-textures/1x/Enhancers.png` | `assets/balatro-effects-optimized/enhancers_psp.png` | `src/draw.c` | Kept on original atlas for compatibility |
| Blind chip art | `assets/balatro-textures/1x/BlindChips.png` | `assets/balatro-effects-optimized/blindchips_psp.png` | `src/draw.c` | Pending module remap |
| Gamepad prompts | `assets/balatro-textures/1x/gamepad_ui.png` | Current archive asset path | `src/draw.c` | Still using original atlas |
| Deck / editions / shop art | `assets/balatro-textures/1x/8BitDeck.png`, `editions.png`, `ShopSignAnimation.png` | Current archive asset path | `src/draw.c` | Still using original atlas |

## Module Notes

### `src/menu.c`

This module currently owns the title screen, main menu, and menu-facing branding assets. The title page now maps to a non-wallpapers background (`assets/backgrounds/balatro_bg_alt.png`) and `logo_main_psp.png`.

### `src/draw.c`

This module currently loads most gameplay-facing textures. Jokers, tarots, and boosters are now sourced from PSP-optimized sheets with dedicated UV mapping, while enhancer/deck/editions remain on original atlases to preserve base card layout compatibility.

## Recommended Next Steps

1. Keep `src/menu.c` as the menu-facing showcase for the optimized logo and UI assets.
2. Update `src/draw.c` to load the PSP-optimized card and effects outputs.
3. Add or adjust crop/index metadata only where the optimized sheet layout changed.
4. Rebuild and verify in PPSSPP after each module switch.

## Tracking Rule

When you add or replace an asset, record these four things:

- original source file
- PSP output filename
- module that loads or draws it
- verification result in PPSSPP

If those four pieces are not known, the asset is not fully tracked yet.