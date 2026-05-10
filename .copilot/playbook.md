# Playbook — Troubleshooting flows & agent commands

How to use
- Follow the symptom-specific flow below. Each flow lists quick repro steps, triage checks, which agent scripts to run, and artifacts to collect for issues/PRs.

Agents (quick)
- AssetBuilder: `scripts/asset_builder/build_assets.sh` — converts audio/textures, packs atlases, writes manifests.
- PSPBuilder: `scripts/pspbuilder/build_with_docker.sh $(pwd) all` — builds the project (Docker) and produces `EBOOT.PBP`.
- TestRunner: `scripts/testrunner/run_smoke_test.sh` / `scripts/testrunner/run_ppsspp_headless.sh` — runs PPSSPP smoke tests and produces screenshots/diffs.

Common flows

1) Background music silent on PSP hardware (high priority)
- Symptom: Music plays in PPSSPP but is silent on real PSP; SFX may still work.
- Quick repro:
  1. Rebuild assets: `scripts/asset_builder/build_assets.sh`
  2. Build the game: `scripts/pspbuilder/build_with_docker.sh $(pwd) all`
  3. Run emulator smoke test: `scripts/testrunner/run_smoke_test.sh` (captures emulator screenshots)
  4. Pack and deploy `EBOOT.PBP` to device (use `scripts/pspbuilder/pack_eboot.sh` or manual copy).
- Triage checklist:
  - Verify `ov_test_callbacks()` and check return before `ov_open_callbacks()` in the decode thread.
  - Ensure `ov_open_callbacks()` is executed on the decode thread (not main/audio callback thread).
  - Log `ov_open_callbacks()` return codes (map against OV_EREAD, OV_ENOTVORBIS, etc.).
  - Instrument the decode loop to log `ov_read()` byte counts and errors.
  - Check ring buffer fill/underflow (add small overlay or write `ms0:/PSP/GAME/Balatro/logs/audio.log`).
  - Temporarily revert recent `static`/buffer-layout changes (e.g., `temp_buffer` static toggle) to test for timing/layout regressions.
  - If silence appears only on device: collect device run logs + sample Ogg used.
- Agent commands:
  - Rebuild assets: `scripts/asset_builder/build_assets.sh`
  - Rebuild/package: `scripts/pspbuilder/build_with_docker.sh $(pwd) all` then `scripts/pspbuilder/pack_eboot.sh`
  - Run emulator smoke tests: `scripts/testrunner/run_smoke_test.sh`
- Files to inspect:
  - `src/audio.c`, `src/game.c`, `src/draw.c`
  - `assets/out/` (resampled Oggs)
- Artifacts to collect when filing an issue:
  - `git rev-parse --short HEAD`
  - `build/EBOOT.PBP` (the built package)
  - `assets/out/` (tarball of the produced assets)
  - `logs/audio.log` (runtime diagnostic logs)
  - PPSSPP logs and screenshots showing working vs failing states

2) Docker build fails (exit code 125)
- Symptom: `scripts/pspbuilder/build_with_docker.sh $(pwd) all` returns 125.
- Triage steps:
  - Verify Docker daemon: `docker info`
  - Try interactive container: `docker run --rm -it pspdev/psp-sdk:latest bash`
  - If Docker unavailable, try host build: `make` (requires local pspsdk setup).
- Agent: `scripts/pspbuilder/build_with_docker.sh $(pwd) all`
- Artifacts: full build log, `docker info` output.

3) PPSSPP smoke-test diffs (visual regression)
- Steps:
  - Run smoke tests: `scripts/testrunner/run_smoke_test.sh`
  - Diff screenshots: `python3 scripts/testrunner/diff_screenshots.py baseline current --threshold 5`
  - If diffs only on device, re-run `scripts/asset_builder/build_assets.sh` and re-test.
- Agent: `scripts/testrunner/run_smoke_test.sh`

4) Texture or atlas visual artifacts
- Steps:
  - Rebuild textures: `scripts/asset_builder/convert_textures.sh <src> <out>` or `scripts/asset_builder/build_assets.sh`
  - Repack atlas: `scripts/asset_builder/pack_atlas.sh`
  - Verify `assets/out/` formats (16-bit PNGs, expected sizes)
- Agent: `scripts/asset_builder/build_assets.sh`

5) Localization / UI layout regressions
- Steps:
  - Rebuild UI assets and launch smoke test covering localized screens.
  - Check localization source files in `assets/src/locales` (if present) and compare string lengths.

Quick commands (copy/paste)
```
# rebuild assets
scripts/asset_builder/build_assets.sh

# build for PSP (Docker)
scripts/pspbuilder/build_with_docker.sh $(pwd) all

# run smoke tests
scripts/testrunner/run_smoke_test.sh

# diff screenshots
python3 scripts/testrunner/diff_screenshots.py baseline current
```

Device deployment (copying build/EBOOT.PBP to your PSP memory stick / FTP)

Methods

- USB Mass-Storage (recommended)
- On the PSP: enable "USB Connection" (Settings → USB Connection) or run your USB plugin/homebrew. Connect the PSP to your Mac; the Memory Stick mounts under `/Volumes/<MemoryStickName>`.
- Create the game folder (if absent) and copy the built package:
  - mkdir -p "/Volumes/<MemoryStickName>/PSP/GAME/Balatro"
  - rsync -av --progress build/EBOOT.PBP "/Volumes/<MemoryStickName>/PSP/GAME/Balatro/EBOOT.PBP"
- Notes: unmount/eject the volume in Finder before unplugging the PSP.

- FTP (when PSP runs an FTP server)
- Start an FTP server on the PSP (homebrew). From host:
  - curl -T build/EBOOT.PBP "ftp://<psp-ip>/PSP/GAME/Balatro/EBOOT.PBP"
  - or lftp -e "put build/EBOOT.PBP -o /PSP/GAME/Balatro/EBOOT.PBP; bye" -u user,pass ftp://<psp-ip>

Automated helper scripts

- `scripts/pspbuilder/pack_eboot.sh` — packs the EBOOT and places it in `build/` (or similar output dir); extend it locally to copy to a mounted Memory Stick.
- Example helper (save as `scripts/pspbuilder/deploy_to_usb.sh` and run with the mounted volume as first arg):

```bash
#!/usr/bin/env bash
set -euo pipefail
MEMVOL="${1:-/Volumes/PSP_MS}"
mkdir -p "${MEMVOL}/PSP/GAME/Balatro"
rsync -av --progress build/EBOOT.PBP "${MEMVOL}/PSP/GAME/Balatro/EBOOT.PBP"
echo "Deployed EBOOT.PBP to ${MEMVOL}/PSP/GAME/Balatro"
```

Troubleshooting
- If the Memory Stick doesn't appear on macOS: ensure the PSP is in USB mode and not running a plugin that prevents mounting.
- If FTP fails: verify PSP IP, credentials, and firewall/port (default 21). Try passive mode or use an external FTP client.

What to include when opening an issue
- Short description, device model (PSP-3000), PPSSPP version (if applicable), commit hash, reproduction steps, attached `EBOOT.PBP`, `assets/out/` sample, and logs (`audio.log`, build output, PPSSPP logs, screenshots).

Where this file lives
- `.copilot/playbook.md` (this file) — use it as a first-stop triage checklist and expand flows as we find more device-specific quirks.
