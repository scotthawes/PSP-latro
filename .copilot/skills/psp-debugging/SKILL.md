---
name: psp-debugging
description: "Debugging techniques for PSP and PPSSPP: logs, screenshots, and emulator-assisted tracing."
---

# PSP Debugging Skill

Use this skill for diagnosing device-only regressions, rendering glitches, crashes, memory corruption, and audio issues that differ between PPSSPP and real hardware.

Techniques
- Reproduce in PPSSPP first; use its built-in logging and frame capture to get a baseline.
- Add runtime logs (file-backed) for critical subsystems: `audio`, `render`, `input`.
- Capture screenshots programmatically to compare frames between runs.
- For hardware-only bugs, add verbose state dumps (ringbuffer levels, thread states) to a persistent log on the memory stick.

Notes
- Avoid shipping verbose logging to `master`; keep logs in `feature/*` branches and remove before merge.
