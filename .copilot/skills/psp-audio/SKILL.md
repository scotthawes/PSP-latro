---
name: psp-audio
description: "Audio decoding, ringbuffer patterns, and pspaudiolib usage for PSP homebrew."
---

# PSP Audio Skill

Use when implementing or debugging OGG/Vorbis decoding, audio underruns, audio callbacks, and ringbuffer sizing.

Guidelines
- Decode OGG/Vorbis on a dedicated thread; call `ov_open_callbacks()` and `ov_clear()` on the decode thread.
- Use `ov_test_callbacks()` as a sanity check before opening compressed streams.
- Feed decoded PCM into a ringbuffer consumed by the `pspaudiolib` callback; size the ringbuffer for slack (e.g., 8 chunks of 4096 samples).
- In `audio_callback()` always zero-fill the provided buffer on underrun to avoid silence artifacts or noise.

Diagnostics
- Log `ov_read()` return values and decoded byte counts.
- Track ringbuffer fill level and log underrun events to `logs/audio.log` for device reproductions.

Asset guidelines
- Offline resample to 44.1kHz stereo OGG for the PSP; use `ffmpeg -ar 44100 -ac 2 -c:a libvorbis`.
