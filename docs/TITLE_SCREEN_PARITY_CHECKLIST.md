# Title Screen Parity Checklist

Goal: bring the PSP title/menu UI closer to the reference Balatro title screen while using PSP-safe mapped assets.

## Scope and Constraints

- Keep wallpapers directory unused.
- Use mapped PSP assets where available.
- Preserve stable startup and avoid first-frame stalls.

## Current Asset Mapping Baseline

- Title background: assets/backgrounds/balatro_bg_alt.png
- Title logo: assets/balatro-ui-optimized/logo_main_psp.png
- UI icon candidates: assets/balatro-ui-optimized/icons/*
- UI button candidates: assets/balatro-ui-optimized/buttons/*

## Implementation Checklist

- [x] 1. Merge title and main menu into one unified screen layout.
- [x] 2. Replace "Press X to Start" with a bottom action bar.
- [x] 3. Add 4 actions in bar: Play, Options, Quit, Collection.
- [x] 4. Add Collection entry to menu model and input routing.
- [x] 5. Add profile card panel at bottom-left (Profile / P1 style).
- [x] 6. Add top-right utility/social icon chips using PSP icon assets.
- [x] 7. Add bottom-right language selector chip.
- [x] 8. Apply PSP button textures to action bar buttons.
- [x] 9. Adjust logo placement/scale to match reference composition.
- [x] 10. Tune overlay/contrast so background art remains readable.
- [x] 11. Reposition version/build text to top-right style.
- [x] 12. Replace wallpaper variant option with theme/style naming or remove it.
- [x] 13. Finalize control-hint treatment (strip removed for closer reference parity).
- [x] 14. Validate layout on PSP resolution (480x272) for clipping and spacing.
- [x] 15. Runtime validation in PPSSPP: no blank screen, no startup regressions.

## Work Order

1. Structural parity first: items 1 to 4
2. Utility widgets second: items 5 to 7
3. Visual skin pass: items 8 to 11
4. Settings and polish: items 12 to 15

## Progress Tracking

- Status: Completed (visual comparison closed)
- Current focus: Parity pass complete; maintain stability while iterating minor polish only.
- Next: Optional collection-screen implementation and non-regression visual QA on hardware/emulator.
