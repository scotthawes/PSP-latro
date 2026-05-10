# PSP-latro — Developer Guide (PSP-3000 Target)

This document collects recommended workflows, agent/skill ideas, asset pipeline commands, build/test/deploy steps, and PSP-3000 specifics to support development and automation for PSP-latro.

## Purpose & Scope
- Target device: PSP-3000 (333 MHz CPU, 64 MB main RAM). Build and test for this baseline.
- Audience: contributors and CI systems that produce artifacts (`EBOOT.PBP`, `game.love`) and PSP-ready assets.
- Goals: reproducible builds, automated asset pipeline (textures/audio), device deployment, and fast feedback loop.

## Quick links
- Source root: [./](./)
- Improvements tracker: [IMPROVEMENTS2.md](IMPROVEMENTS2.md)

---

## PSP-3000 constraints (practical)
- CPU: 333 MHz (ARM9) — expensive CPU work, keep heavy processing offline (asset pipeline).
- RAM: 64 MB main; plan textures/audio budget carefully.
- Resolutions: PSP native 480x272; avoid very large framebuffers.
- GPU: GU supports 16-bit formats (GU_PSM_4444 / GU_PSM_565) — prefer 16-bit textures.
- Audio: expect 44.1 kHz stereo Ogg Vorbis playback; re-encode audio assets to 44.1kHz stereo.

Budgeting rules:
- Texture memory: pack small tiles into atlases; reuse textures across screens.
- Avoid per-frame malloc/free; pre-allocate audio buffers and chunked queues.
- Keep the heap/stack usage visible during development (`DEBUG_PRINTF` overlays).

---

## Recommended repo layout
- `assets/src/` — source assets (PSD, WAV, AI)
- `assets/out/` — generated PSP-ready assets (16-bit PNG, atlases, .ogg files)
- `src/` — game C source
- `scripts/` — small helpers (asset conversion, pack, deploy)
- `tools/` — utility programs if any
- `Makefile` — top-level build targets (assets, build, pack, deploy)

---

## Asset pipeline (commands and examples)
Goals: generate consistent 16-bit textures and OGG audio at 44.1kHz stereo.

Install useful tools (macOS/homebrew example):

```bash
brew install ffmpeg imagemagick pngquant
```

Audio (re-encode to Vorbis 44.1kHz stereo):

```bash
# Quality scale 4..6 is fine for PSP music
ffmpeg -i assets/src/music/source.wav -ar 44100 -ac 2 -c:a libvorbis -q:a 5 assets/out/music1.ogg
```

SFX (mono/stereo):

```bash
ffmpeg -i sfx.wav -ar 44100 -ac 2 -c:a libvorbis -q:a 5 assets/out/sfx.ogg
```

Textures: reduce to 16-bit-looking PNGs and quantize alpha to reduce size

```bash
# convert to 16-bit palette/limited colors (preserve alpha)
magick convert assets/src/sprite.png -colors 65536 -depth 8 assets/out/sprite_16.png
# optional: optimize PNGs
pngquant --quality=60-90 --force --output assets/out/sprite_16_q.png assets/out/sprite_16.png
```

Atlases (pack many small PNGs into a sheet):
- Use `TexturePacker` or a free CLI packer (eg. `spritesheet` or `glue`).

Example (TexturePacker CLI):

```bash
TexturePacker --sheet assets/out/atlas.png --data assets/out/atlas.json assets/out/*.png
```

Notes:
- Keep atlas tile sizes aligned to the `TEXTURE_CARD_WIDTH` / `TEXTURE_CARD_HEIGHT` used by the renderer (69x93 in this project).
- For PSP use 16-bit textures and `graphics_load_texture_from_image_16bit` workflow.

---

## Build & Pack
- `make` should build `pspalatro.elf` and pack `EBOOT.PBP` using `pack-pbp` (see repository `Makefile`).
- Useful Makefile targets:
  - `make assets` — regenerate `assets/out/`
  - `make` — compile
  - `make pack` — create `EBOOT.PBP`
  - `make deploy` — copy to mounted PSP (user-specific, see deploy section)

Example: build from scratch

```bash
make clean
make
```

---

## Device deployment (manual & scripted)
- If PSP is mounted (USB), copy the new `EBOOT.PBP` directly:

```bash
cp build/EBOOT.PBP /Volumes/PSP/GAME/PSPALATRO/EBOOT.PBP
```

- Use `rsync` when doing iterative deploys to avoid long copy time:

```bash
rsync -av --progress build/EBOOT.PBP /Volumes/PSP/GAME/PSPALATRO/EBOOT.PBP
```

- Script (scripts/deploy.sh):

```bash
#!/usr/bin/env bash
MOUNT=/Volumes/LOGIK/PSP/GAME/PSPALATRO
if [ ! -d "$MOUNT" ]; then
  echo "PSP mount not found: $MOUNT"
  exit 1
fi
cp build/EBOOT.PBP "$MOUNT/EBOOT.PBP"
```

---

## Emulator testing (PPSSPP)
- Use PPSSPP for quick iteration. Manually load `EBOOT.PBP` or the `game.love` file.
- For automated screenshots/logging you can script the emulator or run headless builds and run smoke checks (platform-specific).

Manual quick test:
- Open PPSSPP → `File` → `Load EBOOT.PBP`.
- Watch debug text output; if you enabled `DEBUG_PRINTF` messages they appear in terminal for emulator builds.

---

## CI recommendation (GitHub Actions example)
- Use a container with `pspsdk` to produce reproducible builds.
- Typical workflow steps:
  - Checkout
  - Run `scripts/ci/install-pspsdk.sh` (or use Docker image)
  - Run `make assets` (optional)
  - Run `make` and `make pack`
  - Upload `EBOOT.PBP` and `pspalatro.elf` as artifacts

Example snippet (authoring required):

```yaml
name: Build PSP
on: [push, pull_request]
jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Build with Docker PSP SDK
        run: |
          docker run --rm -v "${{ github.workspace }}:/src" -w /src pspdev/psp-sdk:latest make clean all
      - name: Upload artifact
        uses: actions/upload-artifact@v4
        with:
          name: eboot
          path: EBOOT.PBP
```

Note: pick/maintain a PSP SDK Docker image you trust or create a custom runner that has `pspsdk` installed.

---

## Agents & Skills (concepts and triggers)
The goal: automate routine tasks and provide consistent checks.

1. AssetBuilder (Skill/Agent)
- Trigger: commits affecting `assets/src/**` or manual run.
- Actions: re-encode audio, convert textures, pack atlases, produce `assets/out/` and manifest (`assets/out/manifest.json`).
- Output: artifacts (atlas, .ogg, metadata).
- Implementation: scripts in `scripts/asset_builder/` or GitHub action.

2. PSPBuilder
- Trigger: push to main / PR or manual.
- Actions: run `make` in pspsdk environment, run linter/static checks, pack `EBOOT.PBP`.
- Output: `EBOOT.PBP` artifact and build logs.

3. TestRunner
- Trigger: PR or push; optionally periodic.
- Actions: run unit tests (if present), run emulator smoke test (PPSSPP), take screenshots and log crashes.
- Output: pass/fail + log artifact + screenshots.

4. PerfProfiler
- Trigger: nightly or on-demand.
- Actions: run instrumented build, collect frame time and memory usage, produce report.

5. PR Reviewer Skill
- Run static analysis: `cppcheck`, `clang-format` check, and ensure assets pass size limits.

6. LocalizationSync
- Sync strings from `third_party` references into `src/game.c` or an external localization file.

---

## PSP-3000 test checklist (developer)
- Audio: verify music loads and `ov_read` yields samples — re-encode music to 44.1kHz stereo if needed.
- Visual: confirm polychrome/foil overlays and seal icons render on device.
- Memory: open debug overlay and confirm memory usage remains < 64 MB (account for OS/app).
- Performance: check FPS is stable; probe the hotpath with `DEBUG_PRINTF` timing macros.
- Packaging: `EBOOT.PBP` runs on real PSP-3000 (tested via device).

---

## Debugging tips
- Add `DEBUG_PRINTF(...)` lines near suspect subsystems (audio, graphics, resource loading).
- Expose small diagnostic overlay (FPS, audio buffer state) to see runtime health.
- When audio is silent: confirm `settings.ini` `audio = true`, confirm archive contains OGGs and that files were encoded to 44.1kHz.

## Audio Best Practices

- Use libvorbisfile for Ogg Vorbis decoding (vorbisfile).
  - Prefer `ov_test_callbacks()` when you only need a fast validity check.
  - Call `ov_open_callbacks()` from the decode/playback thread (not the main control thread). `ov_open_callbacks()` can be relatively slow because it determines the full logical structure of the stream.
  - Always check the return value of `ov_open_callbacks()`; 0 means success, negative values indicate errors (e.g. `OV_EREAD`, `OV_ENOTVORBIS`, `OV_EVERSION`, `OV_EBADHEADER`, `OV_EFAULT`). Log the code and map it to a readable message when debugging.
  - When using custom `ov_callbacks`, implement `read`, `seek`, and `close` to operate on your in-memory archive file slices (the `game.love` archive). If `ov_open_callbacks()` fails, be responsible for closing the datasource if required by the callback semantics.

- Threading and lifetime rules:
  - Do heavy setup (open) in the decode thread. Perform `ov_read()` and other libvorbisfile calls from the same thread unless protected by synchronization.
  - Keep the `OggVorbis_File` structure alive for the whole decode lifetime and call `ov_clear()` on the same thread when finished.

- PSP audio format and sample handling:
  - Convert `ov_read()` output to the sample format expected by the PSP audio output (commonly interleaved 16-bit signed PCM). Confirm channel count and sample rate (44.1 kHz stereo recommended) match the audio hardware/DMA configuration.
  - If you must resample, do so in the offline asset pipeline (preferred) or in a dedicated resampling thread before the ringbuffer to avoid runtime jitter.

- Buffering and underrun guidance:
  - Use a pre-allocated ring buffer sized to tolerate decode jitter. The repo uses `AUDIO_BUFFER_CHUNKS=8` and `AUDIO_BUFFER_SIZE=4096` as a starting point; tune these values if underruns occur.
  - As an emergency fallback, the audio callback can repeat the last frame or zero-fill on underrun, but this hides the root cause. Prefer surfacing the error (log and overlay) and fixing producer timing.

- Diagnostics and reproducible checks:
  - Log `ov_open_callbacks()` return codes and include a short mapping to human-friendly messages in logs.
  - Add a lightweight `ov_test_callbacks()` precheck when loading audio from the archive to reject invalid files early.
  - Surface ring-buffer fill percentage and `ov_read()` bytes on the in-game debug overlay to distinguish decode vs. DMA issues.
  - If audio is silent only on hardware (not PPSSPP), inspect static/global changes and thread init order (ensure `g_audio_buffer.ogg_id` and other flags are set before the decoder thread reads them).

- Minimal pseudocode pattern (decoder thread):

```c
// in decode thread
if (ov_test_callbacks(datasource, &vf, NULL, 0, callbacks) < 0) {
  log("not vorbis");
}
int r = ov_open_callbacks(datasource, &vf, NULL, 0, callbacks);
if (r < 0) {
  log_error_ov(r);
  close_datasource_if_needed(datasource);
  return;
}
while (!stop) {
  long bytes = ov_read(&vf, temp_buf, temp_buf_size, 0, 2, 1, &bitstream);
  if (bytes > 0) ringbuffer_push(temp_buf, bytes);
  else if (bytes == 0) break; // end of stream
  else { log_error_ov(bytes); break; }
}
ov_clear(&vf);
```

- References:
  - PSPSDK documentation and examples: https://pspdev.github.io/pspsdk/
  - libvorbisfile (Vorbisfile) reference: https://xiph.org/vorbis/doc/vorbisfile/
  - PPSSPP wiki (emulator debugging tips): https://github.com/hrydgard/ppsspp/wiki
  - Wololo (homebrew community articles): https://wololo.net/

---
  ## Third-Party Homebrew Resources

  - **PSPDev / PSPSDK**: GitHub repo and docs for the official community toolchain and GU/audio examples — https://github.com/pspdev/pspsdk / https://pspdev.github.io/pspsdk/
  - **PPSSPP (emulator)**: extensive docs and debugging workflows useful for reproducing/device vs emulator differences — https://www.ppsspp.org/docs/intro/ and https://github.com/hrydgard/ppsspp/wiki
  - **Wololo (homebrew news & tutorials)**: community articles, releases, and PSP-specific how‑tos — https://wololo.net/
  - **ConsoleMods (firmware index)**: community wiki indexing PS3 firmwares (HFW/CFW/Dual-boot/Downgraders) with PUP MD5s and mirror links (Mega/MediaFire/Archive.org/Dropbox). Useful when PSX-Place threads are inaccessible — https://consolemods.org/wiki/PS3:Firmwares
  - **PSX-Place (forums)**: active community for console homebrew, porting threads, and tool discussions — https://www.psx-place.com/
  - **Xiph Vorbis docs**: libvorbisfile reference and examples for `ov_open_callbacks`/`ov_read` — https://xiph.org/vorbis/doc/vorbisfile/

  Notes:
  - Use PSPSDK for build examples and `pack-pbp`/`psp-config` workflows.
  - Use PPSSPP+forum threads to reproduce rendering/audio issues and gather emulator-specific debugging steps.
  - If you want, I can fetch specific PSX-Place threads or any "pspunk" posts and summarize them here.

  ### Resource Summaries & How to Use Them
  - **Hitmen / Yet Another PSP Documentation**: Comprehensive offline PSP hardware & software reference (motherboards, CPU/FPU/VFPU, module exports, cache, common errata). Use for low-level register behavior, known hardware bugs (eg. `lvl.q`/`lvr.q` FPU corruption on PSP-1000), and test examples. Link: http://hitmen.c02.at/files/yapspd/psp_doc/frames.html
  - **uOFW**: Community-collected hardware and software docs (TA-085/TA-088, cache, ID-Storage, module/export tutorials). Good for module patching, IPL/motherboard notes, and archived posts. Link: https://uofw.github.io/upspd/docs/
  - **pspdev VFPU docs**: VFPU instruction set, encodings, performance numbers and errata; essential when optimizing math/transform code or diagnosing register alignment and pipeline hazards. Link: https://pspdev.github.io/vfpu-docs/
  - **ConsoleMods (PS3:Firmwares)**: Community wiki indexing PS3 firmwares (HFW/CFW/Dual-boot/Downgraders). Lists PUP MD5s and multiple mirror links (Mega/MediaFire/Archive.org/Dropbox), and includes cautionary notes about installing HFW/OFW on consoles with CFW. Content licensed CC BY; last updated 10 May 2026. Link: https://consolemods.org/wiki/PS3:Firmwares
  - **PSX-Place (forums)**: Active threads for porting, PSPSDK tips, and community patches. Note: automated fetch returned HTTP 403 (site likely blocks crawlers). Open manually in a browser for targeted threads (search terms: `PSP audio libvorbis`, `ov_open_callbacks`, `PSP GU`, `PPSSPP debug`).
  - **pspunk**: No canonical site found via automated fetch (404). If you have a specific URL or thread, provide it and I'll fetch and summarize.

  Practical uses:
  - Search Hitmen for hardware errata and low-level examples; consult VFPU docs for math optimizations and known instruction bugs; use uOFW for module and IPL/motherboard reference; use PSX-Place for community patches and troubleshooting threads.
  - Recommended search terms for forums: `libvorbis ov_open_callbacks`, `audio underrun PSP`, `VFPU errata lvl.q`, `pspsdk pack-pbp issues`, `PPSSPP device differences`.
  - If PSX-Place is blocked by automated fetch, copy links from a browser session to this guide or run a local cURL script I can generate to fetch pages from your machine.


## Example `scripts/` to scaffold (you can add these quickly)
- `scripts/convert_audio.sh` — batch re-encode audio files to 44.1kHz OGG.
- `scripts/convert_textures.sh` — batch quantize PNGs and pack atlas.
- `scripts/ci_build.sh` — wrapper to run `make` inside Docker/pspsdk.
- `scripts/deploy.sh` — deploy to mounted PSP via `rsync`.

---

## Next steps I can take for you
- Scaffold `scripts/convert_audio.sh` and `scripts/convert_textures.sh` and test locally.
- Create a GitHub Actions workflow that runs `AssetBuilder` and `PSPBuilder`.
- Implement a minimal `TestRunner` job that runs PPSSPP smoke tests.

If you want me to scaffold the `AssetBuilder` scripts and a GitHub Actions workflow now, say "Scaffold AssetBuilder and CI" and I will create the scripts and a starter workflow in `.github/workflows/ci.yml`.

---

## Credits
- This guide was generated to match the current PSP-latro repository and PSG-3000 constraints.


