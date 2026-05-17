# Menu Feature Workflow (Start Screen + Main Menu)

Purpose
-------
This document defines the implementation workflow for menu-related features in PSP-latro, with explicit checkpoints for diagnosis, code changes, and verification.

Reference set: [menu-design-resources.md](menu-design-resources.md)

Current task focus
------------------
- Goal: boot to title/start menu reliably, then allow transition to New Run.
- Scope now: menu boot, stage routing, input routing, and startup diagnostics.
- Out of scope for now: complete options UX polish, full save/continue flow.

Task list (active)
------------------
1. Ensure startup reaches menu stage deterministically.
2. Ensure menu draw path is active and visible every frame.
3. Ensure menu input routing is isolated from gameplay automation/event systems.
4. Ensure New Run transition only occurs when resources are initialized.
5. Keep startup resilient when assets/archive fail (degraded menu mode).
6. Verify behavior in deployed paths used by emulator runs.

Workflow: implement + diagnose
------------------------------
Use this sequence for every menu or boot-path change.

1. Define a minimal expected outcome
- Example: "At boot, show title screen with Press X to Start."
- Write one observable signal for success.

2. Instrument first, then modify
- Add one short-lived diagnostic that cannot be missed (solid color frame, stage marker block, explicit warning text).
- Avoid relying only on logs; use on-screen cues first.

3. Isolate systems by stage
- In menu stage, run only menu input/update logic.
- Do not execute gameplay automation/events while in menu stage.

4. Make failures non-silent
- If settings/archive/assets fail, show a visible warning screen or warning line in menu.
- Never fall back to a black/blank loop without user-visible status.

5. Verify deploy path parity
- Copy EBOOT to all active emulator game paths.
- Verify checksums match before testing.

6. Test in this order
- Boot visual self-test (framebuffer path).
- Menu stage marker visible.
- Title prompt visible.
- Input response (X/O, up/down).
- New Run transition.

7. Remove temporary diagnostics
- Keep only useful defensive guards and user-facing warnings.
- Remove noisy per-frame debug prints before finalizing.

Information extraction checklist
--------------------------------
When debugging menu/boot issues, collect these facts before changing logic again:

- Runtime paths
  - Active EBOOT path being launched.
  - Settings path and loaded archive path.
- Stage/input state
  - Current stage/sub-stage and focused input zone.
- Render path health
  - Do full-screen color clears appear?
  - Do texture-free quads appear?
- Resource state
  - Fonts loaded?
  - Archive opened?
  - Audio thread started only after settings are loaded?
- Deploy consistency
  - MD5 of source EBOOT and every deployed copy.

Definition of done (for this feature slice)
-------------------------------------------
- Boot consistently lands on menu title screen.
- "Press X to Start" transitions to main menu.
- New Run transitions to blinds stage.
- Menu stage is not overridden by gameplay automation/events.
- If resources fail, user sees an explicit warning and app remains responsive.
- Build passes with no compile errors.

Regression checklist
--------------------
- App launches from all known emulator paths.
- No blank black screen on init failure.
- Input still works in gameplay stages after menu changes.
- Audio initialization does not start decode thread before settings are loaded.

Notes for future extension
--------------------------
- Add Continue Run once save/progression exists.
- Add deck/stake/seed setup submenus as separate task slices.
- Add persistent option saving for audio/overclock once settings write-back is implemented.
