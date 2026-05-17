# PPSSPP Capture Profile (UI Parity)

Date: 2026-05-15
Scope: Pregame and ingame parity screenshots for PSP-latro.

## Goal

Produce repeatable screenshots that contain only game UI (no debug overlays), with consistent framing and image quality.

## Required Settings

In PPSSPP:
- Disable any on-screen performance/debug overlays before capture.
- Keep windowed mode and stable scale for all captures in the same review batch.
- Use PNG output for screenshot captures.

In PSP-latro settings (`settings.ini`):
- `debug_overlay = false`

Note:
- In this project, FPS/audio text seen in recent captures comes from the game's debug overlay path (`game_draw_debug_info`), not only emulator UI.
- The runtime guard now uses `g_settings.debug_overlay`, so parity captures should keep it `false`.

## Pre-Capture Checklist

1. Launch PPSSPP with the target `EBOOT.PBP`.
2. Confirm PPSSPP overlays are disabled.
3. Confirm PSP-latro `debug_overlay = false`.
4. Enter target scene (pregame blind-select or ingame hand).
5. Verify top-right corner has no FPS/audio text.
6. Capture PNG image.

## Validation Gate

A capture passes profile validation only if:
- No debug/perf overlay text is visible.
- Scene framing is consistent with the parity test plan.
- UI text remains readable.

## Troubleshooting

If overlay text is still visible:
1. Re-check PPSSPP overlay toggles.
2. Re-check `settings.ini` contains `debug_overlay = false` in the loaded settings path.
3. Restart PPSSPP after settings changes.
