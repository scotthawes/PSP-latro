# Target vs Replica Improvement Tracker

Date: 2026-05-16
Branch context: feature/menu-system
Reference target: Balatro v0.11 visual behavior and composition

## Purpose

Track the concrete visual and layout improvements still needed to move the PSP-latro replica closer to the target screenshots in `assets/ui_db/`.

This document is intended as a working checklist for the comparison wave shown in the attached reference images. It complements:

- `docs/TARGET_VS_PSP_COMPARISON_2026-05-16.md`
- `docs/UI_COMPARISON_ISSUE_COLLATION.md`
- `docs/UI_COMPARISON_TEST_PLAN.md`

## Scope

Reference scenes observed in the target set:

- Title screen and main menu
- Settings menu and tabbed option dialogs
- Language selector
- Credits / thanks panels
- New run / custom rules / game over modals
- Pregame blind select scene
- Ingame table, HUD, and hand layout
- Rewards and cash-out overlays
- Shop presentation

## Identified Improvements

### 1. Title and Main Menu Composition

- Recenter and rescale the Balatro logo so it dominates the title screen the way the reference does.
- Match the main action row sizing, spacing, and color hierarchy for `PLAY`, `OPTIONS`, `QUIT`, and `COLLECTION`.
- Align the profile chip and language control with the reference anchors near the lower corners.
- Preserve the strong red cloth background treatment and keep the logo/background contrast high.

### 2. Settings and Tabbed Dialog Structure

- Match the modal frame proportions, border weight, and corner treatment used by the reference option dialogs.
- Standardize the tab strip styling across `Game`, `Video`, `Graphics`, `Audio`, `Music`, `Misc`, `Production`, `Sounds`, and `Imagery`.
- Use the same slider placement, track length, thumb scale, and label spacing seen in the target.
- Keep the orange back bar visually dominant and consistently aligned across every settings screen.

### 3. Language and Credits Panels

- Recreate the compact language grid with the same button sizing, wrapping, and beta-tag placement.
- Preserve the reference typography hierarchy in the language feedback and credits/thanks panels.
- Match the centered credits card sizing and vertical spacing so the modal feels balanced within the frame.

### 4. Run Setup and Modal Navigation

- Bring the new run / custom rules panel into closer alignment with the target card spacing, deck preview placement, and mode selection treatment.
- Match the game-over modal width, button hierarchy, and score/stat block proportions.
- Keep modal transitions and overlays visually clean so the target composition remains readable during scene changes.

### 5. Pregame Blind Select Scene

- Tighten the blind cards’ alignment, spacing, and panel proportions to better match the target’s modular layout.
- Improve the left-side score rail hierarchy so blind name, score target, and round state read like the reference.
- Match the action cluster at the bottom, including button size, label placement, and safe-area padding.

### 6. Ingame HUD and Hand Presentation

- Rebalance the left HUD so the score, multiplier, hands, discards, and round info follow the target hierarchy.
- Refine the action row so `Play Hand`, `Sort Hand`, and `Discard` feel anchored and evenly spaced.
- Adjust hand fan spacing, overlap, and tilt to match the reference presentation more closely.
- Keep the deck widget tucked into the same lower-right safe-area position as the target.

### 7. Table Depth, Background, and Atmosphere

- Increase the sense of depth in the playfield background without reducing text or card legibility.
- Match the subtle texture and compositional balance visible in the reference gameplay captures.
- Keep transitions such as cash-out and blind selection visually grounded instead of flattening the scene.

### 8. Shop and Rewards Fidelity

- Maintain crisp card and booster art in the shop so higher-detail sources do not soften at PSP scale.
- Align reward and cash-out overlays to the same centered, readable framing used by the target.
- Preserve the reference emphasis on the active card/item while keeping surrounding chrome secondary.

### 9. Capture Cleanliness and Comparison Hygiene

- Keep debug overlays and emulator-specific artifacts out of comparison screenshots.
- Use a consistent capture profile so future diffs reflect UI changes rather than tooling drift.
- Record any intentional PSP trade-offs separately so they do not get mistaken for regressions.

## Suggested Tracking Order

1. Title and main menu composition.
2. Settings and modal structure.
3. Pregame blind select layout.
4. Ingame HUD and hand presentation.
5. Shop and reward fidelity.
6. Final capture cleanup and signoff.

## Working Checklist

- [ ] Confirm which menu and modal differences are visual polish versus structural mismatch.
- [ ] Capture fresh side-by-side title/menu screenshots after the next pass.
- [ ] Recheck settings tabs and sliders against the target reference images.
- [ ] Verify blind select, HUD, and hand presentation in the same capture session.
- [ ] Validate shop and reward screens after layout and asset changes.
- [ ] Mark any intentional PSP-only differences explicitly before closing the tracker.

## Notes

- If a difference is already covered by an issue doc in `docs/ui-comparison-issues/`, link it there instead of duplicating the full investigation.
- If a new discrepancy appears during manual gameplay comparison, add it here first and then split it into a dedicated issue document if it needs implementation work.