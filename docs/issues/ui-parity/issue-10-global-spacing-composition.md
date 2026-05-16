# issue-10: Global Composition and Density Drift

## Severity

P1

## Observed Differences

- Current layouts feel sparser with larger empty regions.
- Major blocks do not align to the same density rhythm as reference.

## Likely Root Cause

- Incremental layout changes without shared composition grid.
- Mixed use of absolute positions and ad-hoc offsets.

## Proposed Solutions

1. Define a simple composition grid for 480x272 scenes.
2. Convert key anchors to grid-relative positions.
3. Rebalance whitespace and block widths using target density goals.

## Implementation Checklist

- [x] 1. Create 480x272 layout grid constants.
- [x] 2. Map HUD, center area, and right rail to grid columns.
- [x] 3. Normalize vertical rhythm for top/mid/bottom clusters.
- [x] 4. Remove ad-hoc offsets that fight grid alignment.
- [x] 5. Run final parity screenshot pass and signoff.

## Current Progress

- Added composition-grid anchors in `draw.c` for center area, right rail, mid cluster baseline, and inter-panel spacing.
- Updated blind-select layout to use grid-derived panel width and spacing instead of fixed per-panel magic numbers.
- Moved shop main block anchor to grid-based center/mid constants.
- Updated background depth modulation block to follow center-grid width, reducing mismatch against gameplay composition.

## Evidence

- Build validation succeeded after composition-grid pass (`make -j4`, 2026-05-16).
- User-provided validation captures (2026-05-16):
	- Pregame blind-select scene confirms tighter center-grid alignment and reduced dead-space drift.
	- Ingame hand scene confirms mid/bottom cluster rhythm is consistent with right-rail deck composition.
	- Cash-out overlay state confirms composition remains stable with center modal content.
	- Shop scene confirms center cluster anchoring remains aligned under alternate HUD header state.

## Completion Note

Issue 10 closure criteria satisfied after composition-grid implementation and multi-scene screenshot signoff.

## Done Definition

- Overall scene composition feels intentional and closer to target density.
