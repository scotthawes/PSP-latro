# Target vs PSP Comparison Review

Date: 2026-05-16
Branch context: feature/menu-system
Reference target: Balatro v0.11 visual behavior and composition

## Purpose

Provide a single post-parity comparison review between the target presentation and the current PSP-latro build, consolidating issue-by-issue fixes into scene-level signoff.

## Comparison Method

1. Use the capture guardrails from docs/PPSSPP_CAPTURE_PROFILE.md.
2. Reuse pass criteria from docs/plans/ui-parity-test-plan.md.
3. Map each scene result to the resolved issue documents in docs/issues/ui-parity/.
4. Record any remaining differences that are intentional PSP constraints.

## Scene-by-Scene Comparison

1. Pregame blind-select scene
   - Target expectation: readable hierarchy, distinct blind identities, stable spacing, no overlays.
   - PSP-latro status: matched after fixes in issue-01, issue-02, issue-03, issue-04, issue-05, issue-06, issue-08, issue-10.
   - Result: PASS.

2. Ingame hand scene
   - Target expectation: clear HUD hierarchy, natural card fan, aligned action controls, correct deck/widget placement.
   - PSP-latro status: matched after fixes in issue-07, issue-08, issue-09, issue-10.
   - Result: PASS.

3. Shop scene
   - Target expectation: shop cards/boosters remain crisp and legible with stable composition.
   - PSP-latro status: restored via issue-11 (shop sharpness/state controls plus atlas/UV correction to higher-detail card sources).
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
- UI parity issue tracker status: issue-01 through issue-11 closed.
- Remaining differences: constrained to intentional PSP platform trade-offs unless new screenshot regressions are discovered.

## Known Out-of-Scope / Separate Track

- Title/menu parity has its own checklist in docs/title-screen-parity-checklist.md and should continue to be validated there.
- This review focuses on gameplay-facing parity scenes covered by the UI issue collation set.

## Regression Watchlist for Future Changes

1. Shop atlas source or UV math changes can reintroduce clarity loss.
2. Draw-state/filter changes outside shop paths can spill into gameplay renders.
3. Global spacing refactors can regress composition density and HUD legibility.
4. Capture workflow drift (overlay toggles) can invalidate screenshot comparisons.

## Evidence Links

- Collation signoff: docs/reports/ui-parity-issue-collation.md
- Test plan: docs/plans/ui-parity-test-plan.md
- Capture profile: docs/PPSSPP_CAPTURE_PROFILE.md
- Per-issue evidence and checklists: docs/issues/ui-parity/