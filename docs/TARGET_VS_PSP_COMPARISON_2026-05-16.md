# Target vs PSP Comparison Review

Date: 2026-05-16
Branch context: feature/menu-system
Reference target: Balatro v0.11 visual behavior and composition

## Purpose

Provide a single post-parity comparison review between the target presentation and the current PSP-latro build, consolidating issue-by-issue fixes into scene-level signoff.

## Comparison Method

1. Use the capture guardrails from docs/PPSSPP_CAPTURE_PROFILE.md.
2. Reuse pass criteria from docs/UI_PARITY_TEST_PLAN.md.
3. Map each scene result to the resolved issue documents in docs/ui-parity-issues/.
4. Record any remaining differences that are intentional PSP constraints.

## Scene-by-Scene Comparison

1. Pregame blind-select scene
   - Target expectation: readable hierarchy, distinct blind identities, stable spacing, no overlays.
   - PSP-latro status: matched after fixes in ISSUE-01, ISSUE-02, ISSUE-03, ISSUE-04, ISSUE-05, ISSUE-06, ISSUE-08, ISSUE-10.
   - Result: PASS.

2. Ingame hand scene
   - Target expectation: clear HUD hierarchy, natural card fan, aligned action controls, correct deck/widget placement.
   - PSP-latro status: matched after fixes in ISSUE-07, ISSUE-08, ISSUE-09, ISSUE-10.
   - Result: PASS.

3. Shop scene
   - Target expectation: shop cards/boosters remain crisp and legible with stable composition.
   - PSP-latro status: restored via ISSUE-11 (shop sharpness/state controls plus atlas/UV correction to higher-detail card sources).
   - Result: PASS.

4. Cash-out/transition overlays
   - Target expectation: balanced composition and text readability across overlay states.
   - PSP-latro status: validated during final parity signoff sweep in collation.
   - Result: PASS.

5. Capture cleanliness
   - Target expectation: no emulator stats or game debug text in comparison captures.
   - PSP-latro status: enforced via debug_overlay setting and capture profile.
   - Result: PASS.

## Consolidated Verdict

- Overall parity verdict for pregame, ingame, cash-out, and shop scenes: PASS.
- UI parity issue tracker status: ISSUE-01 through ISSUE-11 closed.
- Remaining differences: constrained to intentional PSP platform trade-offs unless new screenshot regressions are discovered.

## Known Out-of-Scope / Separate Track

- Title/menu parity has its own checklist in docs/TITLE_SCREEN_PARITY_CHECKLIST.md and should continue to be validated there.
- This review focuses on gameplay-facing parity scenes covered by the UI issue collation set.

## Regression Watchlist for Future Changes

1. Shop atlas source or UV math changes can reintroduce clarity loss.
2. Draw-state/filter changes outside shop paths can spill into gameplay renders.
3. Global spacing refactors can regress composition density and HUD legibility.
4. Capture workflow drift (overlay toggles) can invalidate screenshot comparisons.

## Evidence Links

- Collation signoff: docs/UI_PARITY_ISSUE_COLLATION.md
- Test plan: docs/UI_PARITY_TEST_PLAN.md
- Capture profile: docs/PPSSPP_CAPTURE_PROFILE.md
- Per-issue evidence and checklists: docs/ui-parity-issues/