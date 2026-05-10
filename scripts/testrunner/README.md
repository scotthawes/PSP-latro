TestRunner

Purpose: run quick PPSSPP smoke tests and compare screenshots to a golden baseline.

Quick local steps:

1. Ensure build artifacts exist (EBOOT.PBP). Build locally or with Docker:

```bash
# build (uses pspsdk docker image if available)
scripts/pspbuilder/build_with_docker.sh $(pwd) all
```

2. Install local dependencies (for headless capture and image diff):

```bash
# Debian/Ubuntu
sudo apt-get update
sudo apt-get install -y xvfb imagemagick x11-apps xwd
python3 -m pip install --user pillow
```

3. Capture baseline (run once):

```bash
scripts/testrunner/run_smoke_test.sh false test-output/actual.png test-output/expected.png
# This will create test-output/expected.png if missing
```

4. Run smoke test and diff:

```bash
# Optionally build first by passing 'true'
scripts/testrunner/run_smoke_test.sh true
```

Notes:
- The headless capture uses `xvfb-run` and ImageMagick's `import`/`convert`. Adjust `scripts/testrunner/run_ppsspp_headless.sh` for your environment if necessary.
- CI: a workflow `.github/workflows/testrunner.yml` is provided as a manual dispatch; it will only run if you set the `run_ppsspp` input to `true` when dispatching (PPSSPP installation in CI is best-effort and may require custom runners).
