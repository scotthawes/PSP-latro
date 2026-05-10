---
name: psp-performance
description: "Performance diagnosis and tuning for PSP: frame timing, cache, memory, and overclocking guidance."
---

# PSP Performance Skill

Use when profiling FPS, memory usage, and CPU hotspots on PSP-3000 hardware.

Guidance
- Instrument the main loop with frame timestamps and log min/avg/max frame time.
- Minimize cache flushes in the render hot-path; use range-based writebacks.
- Keep per-frame allocations to zero; pre-allocate pools for sprites and temporary buffers.
- Consider `overclock=true` in `settings.ini` for testing, but require user consent for shipping.

Profiling
- Collect trace logs on-device and compare to PPSSPP traces to isolate platform-specific costs.
