# UI Parity Testing Plan

Date: 2026-05-15
Scope: Pregame and ingame UI parity against approved reference captures.

## Objectives

- Verify each parity issue is fixed without regressions.
- Standardize screenshot capture for reliable side-by-side comparison.
- Provide clear pass/fail gates before issue closure.

## Test Environments

1. PPSSPP on macOS (primary parity capture environment).
2. Optional device-targeted PSP settings profile (secondary validation).

## Capture Profile (Required)

- Disable emulator debug overlays and runtime stats.
- Ensure PSP-latro runtime debug overlay is disabled (`debug_overlay = false` in `settings.ini`).
- Use consistent window scale and aspect behavior.
- Use same scene entry path for every run.
- Capture PNG screenshots to preserve UI detail.

Reference profile:

- docs/PPSSPP_CAPTURE_PROFILE.md

## Test Data and Scene Setup

Pregame setup:
- Enter blind selection view from clean boot.
- Stop on screen with all blind cards visible.

Ingame setup:
- Enter a hand with cards visible, action buttons visible, and deck visible.
- Ensure left HUD and score/combo area are present in frame.

## Execution Phases

### Phase 1: P0 Functional Parity Blockers

Targets:
- issue-01 (font rendering)
- issue-02 (missing assets/fallbacks)

Checks:
- All key text strings are readable and stable.
- No placeholder/fallback surfaces in target regions.

### Phase 2: P1 Visual Hierarchy and Layout

Targets:
- issue-03, issue-04, issue-05, issue-06, issue-08, issue-10

Checks:
- Blind cards have distinct visual identities.
- Buttons match expected spacing and state clarity.
- Background has depth without harming legibility.
- HUD hierarchy is readable at glance.
- Composition density is close to reference.
- Debug overlay absent from captures.

### Phase 3: P2 Polish and Fit

Targets:
- issue-07, issue-09

Checks:
- Card fan spacing/rotation is natural and readable.
- Deck widget scale and placement align with reference composition.

## Test Case Matrix

1. TC-01 Pregame text readability
   - Method: Visual inspect + screenshot
   - Pass: No garbled labels in blind/header/sidebar text
2. TC-02 Ingame text readability
   - Method: Visual inspect + screenshot
   - Pass: No garbled labels in blind/header/HUD text
3. TC-03 Asset resolution
   - Method: Runtime log scan + visual inspect
   - Pass: No missing-asset warnings for active scene assets
4. TC-04 Blind panel parity
   - Method: Side-by-side pregame compare
   - Pass: Clear per-blind color/style differentiation
5. TC-05 Button alignment and style
   - Method: Side-by-side compare
   - Pass: Consistent spacing, sizing, and state colors
6. TC-06 Background depth
   - Method: Side-by-side compare
   - Pass: Layered depth visible; text remains readable
7. TC-07 Debug-free capture
   - Method: Screenshot inspection
   - Pass: No emulator overlays in final captures
8. TC-08 HUD hierarchy
   - Method: Side-by-side compare
   - Pass: Values and labels scan cleanly at PSP resolution
9. TC-09 Card fan behavior
   - Method: Ingame interaction check
   - Pass: Fan arc and selected-card offsets are readable
10. TC-10 Deck widget placement
   - Method: Side-by-side compare
   - Pass: Deck anchor and counter placement align with target

## Regression Suite (Minimum)

- Boot to pregame screen and verify no startup regressions.
- Transition pregame -> ingame -> pregame three times.
- Confirm no persistent rendering artifacts after transitions.
- Confirm action inputs remain correct after layout/style changes.

## Artifacts to Store

- Pregame final capture: test-output/ui-parity/pregame-final.png
- Ingame final capture: test-output/ui-parity/ingame-final.png
- Optional before/after diffs per issue.

## Exit Criteria

- All P0 test cases pass.
- No open P1 failures.
- P2 failures (if any) are documented with explicit deferral.
- Final captures are approved and linked from issue docs.
