# Improvements Summary

Consolidated open and high-priority items merged from `IMPROVEMENTS.md` and `IMPROVEMENTS2.md`.

## High priority

- Background music silent after startup (IMPROVEMENTS2 §1.6)
  - Affected: `src/audio.c` (`audio_load_ogg`, `audio_update`, `audio_callback`)
  - Proposed actions:
    1. Validate `fopen` return paths and return errors when files are missing.
    2. Use `ov_test_callbacks()` as a precheck; call `ov_open_callbacks()` on the decode thread.
    3. Log `ov_open_callbacks()` return codes and `ov_read()` errors for diagnostics.
    4. Keep `OggVorbis_File` lifetime on the decode thread and call `ov_clear()` there.
    5. Zero-fill audio callback underrun paths; temporarily revert static `temp_buffer` changes while debugging.
  - Verification: build, run in PPSSPP, test on device, capture `logs/audio.log` and ring-buffer overlay.

## Medium priority

- PSP-3000 compatibility checklist — create `docs/PSP3000_CHECKLIST.md` (expand `DEVGUIDE_PSP3000.md` as needed).
- Performance profiling harness — instrumented build and nightly profiling job.
- PPSSPP smoke-test reliability — provide a container image or self-hosted runner for consistent emulator tests.

## Low priority / housekeeping

- Fetch and archive PSX-Place threads and `pspunk` posts (manual fetch due to site WAF); store summaries in `docs/psxplace/`.
- Repo hygiene tasks completed: moved generated assets to `assets/out/`, added `.gitattributes`, added CI workflows and scripts.

## Recommended next steps

1. Implement and test the `src/audio.c` fixes; add audio diagnostics overlay and `logs/audio.log` collection.
2. Create `docs/PSP3000_CHECKLIST.md` and surface it from `DEVGUIDE_PSP3000.md`.
3. Add a profiling harness and CI job to run it periodically.
4. Archive key forum threads in `docs/psxplace/` with short summaries and links.

--
This file was generated to centralize open items and short next actions for triage.
