# ISSUE-11: Shop Item Rendering Fidelity Regression

## Severity

P1

## Observed Differences

- Shop single-item and booster-item cards appear soft/blurred compared with ingame hand cards.
- Item art readability is reduced in shop scenes after recent parity passes.
- The issue is visible in user-provided shop captures while FPS remains stable.

## Likely Root Cause

- Shop card draw path uses filtering/positioning settings that produce excessive smoothing.
- Subpixel placement plus linear filtering may be degrading card-edge sharpness.
- Shop-specific texture path or UV mapping may differ from ingame card path.

## Proposed Solutions

1. Audit shop card rendering path and compare directly with ingame hand-card draw settings.
2. Add a shop-item sharpness policy (nearest or hybrid filtering where appropriate).
3. Snap shop item draw positions to pixel boundaries where this improves clarity.
4. Verify UV/atlas selection parity for joker/tarot/planet/booster shop items.

## Implementation Checklist

- [x] 1. Identify the exact shop item draw functions and texture/filter state.
- [x] 2. Reproduce and isolate blur source (filtering vs UV vs subpixel placement).
- [x] 3. Implement rendering-path fix for shop item clarity.
- [x] 4. Verify visual improvement in shop screenshots.
- [x] 5. Confirm no FPS regression from the fix.

## Evidence

- User report (2026-05-15): "shop items are not rendering properly anymore".
- Supporting captures: shop scene images shared in the same review thread that closed Issue 5.
- Code fix applied (2026-05-16): shop-context sharpness policy in draw path.
	- Shop item textures now use nearest filtering.
	- Shop item positions are pixel-snapped before draw to reduce subpixel smoothing.
- Build validation succeeded after fix (`make -j4`, 2026-05-16).
- Follow-up rendering-path fix (2026-05-16): switched shop-relevant atlases to higher-detail card sources with matching UV/cell dimensions.
- User validation screenshot (2026-05-16) confirms shop singles/boosters now render crisply and legibly.
- Runtime remained stable during validation runs (no observed FPS regression).

## Current Progress

- Updated shop-relevant draw functions (`game_draw_card`, `game_draw_joker`, `game_draw_tarot`, `game_draw_planet`, `game_draw_booster`) to use a shared shop sharpness policy.
- Shop blur source isolated to linear filtering + subpixel placement interaction in shop context, not atlas UV selection.
- Added shop render-state stabilization pass to address overlay-like artifacts:
	- Disabled card oscillation while in shop scene.
	- Reset transient draw state (`angle`, `scale`, `white_factor`, `alpha`) for shop singles/boosters before draw.

## Completion Note

Issue 11 closure criteria satisfied after atlas/UV correction plus shop sharpness/state stabilization.

## Done Definition

- Shop item cards render with crispness comparable to ingame hand cards while maintaining stable frame rate.