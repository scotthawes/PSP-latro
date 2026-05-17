# issue-03: Blind Panel Style Mismatch

## Severity

P1

## Observed Differences

- Pregame blind cards are gray and flat compared with colored, distinct reference cards.
- Border treatment and visual hierarchy are weaker than target.

## Likely Root Cause

- Blind card component uses placeholder material.
- Missing state-specific textures/colors for small, big, and boss blind.
- Incomplete style tokens for borders, highlights, and typography.

## Proposed Solutions

1. Define visual tokens per blind type (base color, border color, title style, score accent).
2. Add panel background textures or gradient layers with PSP-safe cost.
3. Ensure disabled/unavailable states are visually distinct but still readable.

## Implementation Checklist

- [x] 1. Create blind panel style spec table in code or config.
- [x] 2. Implement per-type fill and border rendering.
- [x] 3. Add title and score emphasis pass.
- [x] 4. Validate readability on PSP resolution.
- [x] 5. Compare against reference and tune spacing.

## Current Progress

- Added an in-code style token table (`g_blind_panel_styles`) for Small/Big/Boss blind cards in `src/draw.c`.
- Blind select panels now render with per-type border, panel fill, top header bar, and score badge treatment.
- Added title/score emphasis via subtle text shadow and per-type score color.
- Updated unavailable-panel treatment to keep cards visually distinct while preserving text readability (lighter translucent overlay).

## Evidence

- Build verification: `make -j4` succeeded after blind panel renderer updates.
- Visual verification: 2026-05-15 updated blind-select screenshot confirms Small/Big/Boss panels are color-distinct, score labels are readable, and panel spacing is aligned with the intended hierarchy.

## Remaining For Completion

- None. Issue 3 completion criteria are satisfied.

## Done Definition

- Small/Big/Boss blind cards are visually distinct and close to reference hierarchy.
