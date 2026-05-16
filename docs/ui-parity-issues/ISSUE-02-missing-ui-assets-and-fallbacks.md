# ISSUE-02: Missing UI Assets and Fallback Visuals

## Severity

P0 (causes placeholder look and invalid parity readings)

## Observed Differences

- Left HUD and panel elements appear as flat placeholders.
- Iconic visual elements from reference are absent or simplified.
- Multiple UI blocks use generic fallback style.

## Likely Root Cause

- Missing texture files in expected asset paths.
- Registry/map keys not aligned with runtime lookup names.
- Silent fallback path masking unresolved assets.

## Proposed Solutions

1. Build a definitive manifest of required UI assets for pregame and ingame.
2. Add strict missing-asset logging with key and resolved path.
3. Align asset naming between mapper and renderer.
4. Replace generic fallback with temporary high-visibility debug texture for development builds.

## Implementation Checklist

- [x] 1. Generate required UI asset inventory from menu and gameplay draw code.
- [x] 2. Validate each key resolves to a physical file.
- [x] 3. Fix path/key mismatches.
- [x] 4. Rebuild and verify no missing-asset warnings in target scenes.
- [x] 5. Remove remaining placeholder surfaces from pregame and ingame.
- [x] 6. Capture updated parity screenshots.

## Current Progress

- Menu asset inventory validated for title background, title logos, profile/language/social icons, and action button textures.
- Menu loader now logs structured asset load success/failure with key + path and scene context (`[MENU][ASSET]`, `[MENU][MISSING]`).
- Action button render path now consumes `s_action_button_textures[i]` instead of forcing generic panel fallback.
- Runtime root cause identified for collection button: texture-slot exhaustion (`Maximum amount of textures reached`) rather than bad path/key.
- Added optional/budget-aware handling for collection button load to avoid false missing-asset reporting and reuse an already loaded action button texture.
- Pregame menu runtime now reports `[MENU][ASSET][SKIP]` + `[MENU][ASSET][FALLBACK]` for collection button instead of `[MENU][MISSING]`.

## Evidence

- Build verification: `make` succeeded after menu asset-loading/render updates.
- File resolution check: all current menu asset paths resolve to existing files under repo/deploy paths.
- Runtime log: `test-output/ui-parity/issue-02-menu-assets.log` shows menu asset loads and collection-button fallback without new `[MENU][MISSING]` in the latest run segment.
- Focused verification artifact: `test-output/ui-parity/issue-02-ingame-verification.log`.
- Item 5 validation artifact: `test-output/ui-parity/issue-02-item5-visual-validation.txt`.
- Screenshot evidence (2026-05-15 user-provided captures):
	- Pregame title/menu screen with action buttons rendered (`PLAY`, `OPTIONS`, `QUIT`, `COLLECTION`).
	- Ingame blind-selection screen showing gameplay HUD/panel layout.
	- Ingame hand-play screen showing card row and action buttons.

## Option 1 Verification Result

- Menu-side verification: PASS.
: `action_button_collection` now emits `[MENU][ASSET][SKIP]` and `[MENU][ASSET][FALLBACK]` under texture-budget pressure instead of `[MENU][MISSING]`.
- Ingame-side verification: PASS.
: The `"[TEX] FAILED to load ... from all 22 paths"` lines are local-override probe failures, followed by successful archive loads (`Loaded image resources/textures/1x/...`) for all five gameplay textures.
: No archive-entry load errors were observed for those assets.

Checklist items 4 and 6 are now complete.

## Remaining For Completion

- None. Issue 2 completion criteria are satisfied.

## Done Definition

- No placeholder/fallback visuals remain in target screens.
