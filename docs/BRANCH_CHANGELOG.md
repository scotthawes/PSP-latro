# PSP-latro Branch Change Log

**Branch**: `feature/menu-system`  
**Base**: `master`  
**Date**: 12 May 2026  
**Status**: Handoff ready

## Summary

This branch turns the project into a more complete menu-driven PSP build with a structured asset pipeline and PSP-optimized content. The work focused on making Balatro assets accessible, validating them in PPSSPP, and then using the optimized logo, card, effects, and UI assets in the permanent main-menu presentation after the verification-only hooks were removed.

## Main Changes

- Added a menu system with wallpaper handling, title/menu presentation, and asset-backed UI rendering.
- Expanded the graphics loader so it can resolve assets from local, build, archive, and fallback paths.
- Added PSP-optimized asset sets for UI/logo art, core card sheets, and effects art.
- Added build-time asset sync so optimized folders are copied into `build/assets/` automatically.
- Added documentation for the menu workflow, wallpaper setup, and PSP asset optimization plan.
- Added a permanent main-menu asset showcase that displays the optimized logo, card sheet, effects art, and UI thumbnail assets.
- Removed temporary runtime verification hooks after the milestone validation pass, leaving only the permanent asset integrations.

## Milestone Log

### Milestone 1: UI and Branding

- Created PSP-sized logo outputs.
- Extracted representative icon and button assets for the menu layer.
- Verified the title logo path in PPSSPP.

### Milestone 2: Core Cards

- Generated optimized Joker and Tarot sheets.
- Added metadata to document sheet layout and intent.
- Verified card textures loaded and rendered in the runtime preview path.

### Milestone 3: Effects and Supplemental Art

- Created optimized effects textures for enhancers, boosters, and blind chips.
- Added effects metadata for source/output tracking.
- Verified the optimized effect sheet in PPSSPP.

### Milestone 4: Integration and Validation

- Wired the optimized assets into runtime code paths.
- Confirmed the build sync workflow includes the optimized asset folders.
- Ran a clean build and PPSSPP validation.
- Removed the temporary verification overlay/logging code after confirmation.

## Verification

- Clean build completed successfully.
- PPSSPP load logs confirmed the optimized asset paths were resolving.
- The final runtime code no longer contains the temporary `ASSET_VERIFY` hooks.

## Notes for Future Work

- Keep new asset additions under the existing `assets/*-optimized/` pattern so the Makefile sync continues to work.
- If more visual polish is added later, validate it in PPSSPP before keeping any runtime instrumentation.