# Title Screen Parity Checklist

Goal: bring the PSP title/menu UI closer to the reference Balatro title screen while using PSP-safe mapped assets.

## Scope and Constraints

- Keep wallpapers directory unused.
- Use mapped PSP assets where available.
- Preserve stable startup and avoid first-frame stalls.

## Current Asset Mapping Baseline

- Title background: assets/backgrounds/balatro_bg_alt.png
- Title logo: assets/balatro-ui-optimized/logo_main_psp_ace.png
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
- [x] 8. Use PSP-styled action buttons with solid fill-first rendering for clear color blocks.
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

- Status: In progress (layout parity pass applied in runtime code)
- Current focus: Verify latest code-tuned layout in PPSSPP screenshot diff.
- Next: Run screenshot-by-screenshot parity check with overlays disabled.

## Required Changes From Latest Side-By-Side

- [x] 1. Ensure title logo never falls back to placeholder text when mapped logo assets exist.
- [x] 2. Fine-tune logo scale/position to match reference vertical balance.
- [x] 3. Tighten bottom action bar thickness/spacing to reference compactness.
- [x] 4. Reduce button border heaviness and texture noise for cleaner blocks.
- [x] 5. Align profile, social, and language chips to the tighter right-lower cluster pattern.
- [x] 6. Confirm version text size/offset matches reference subtle top-right placement.
- [x] 7. Apply shader-inspired post-processing (vignette/grade/sheen) using GU-safe layered quads.
- [x] 8. Upgrade menu label treatment (weight/outline/shadow) for closer title-screen typography.
- [ ] 9. Capture new PPSSPP screenshot and perform another side-by-side diff pass.
- [ ] 10. Verify emulator debug overlays are disabled for comparison captures (PPSSPP setting, not game UI).
