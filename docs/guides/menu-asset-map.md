# Menu Asset Map

This file maps menu-facing assets to the menu runtime so logo/background changes are predictable.

## Variant Mapping

| Variant | Title Background | Main Background | Title Logo |
|---|---|---|---|
| A (`wallpaper_variant = 0`) | `backgrounds/balatro_bg_alt.png` | solid fallback color | `balatro-ui-optimized/logo_main_psp.png` |
| B (`wallpaper_variant = 1`) | `backgrounds/balatro_bg_alt.png` | solid fallback color | `balatro-ui-optimized/logo_main_psp.png` |

## Runtime Ownership

- Module: `src/menu.c`
- Mapping table: `s_menu_variant_assets`
- Active variant switch: `s_wallpaper_variant` and `menu_apply_active_variant_assets()`

## Behavior

1. The title page background is loaded from `backgrounds/balatro_bg_alt.png`.
2. The title logo is mapped to `balatro-ui-optimized/logo_main_psp.png`.
3. Main menu background uses a solid fallback color (no wallpapers directory usage).
4. Variant `0` acts as fallback if asset loading fails.

## Notes

- Gameplay card/effects asset mapping is tracked separately in `docs/ASSET_MODULE_TRACKER.md`.
- This file only covers menu-facing assets (logo/background and related menu visuals).