AssetBuilder scripts

Usage: run `scripts/asset_builder/build_assets.sh` to convert source assets in `assets/src/` into PSP-ready outputs under `assets/out/`.

Requirements (local):
- ffmpeg
- ImageMagick (`magick` or `convert`)
- pngquant (optional)
- python3 (for manifest fallback)

GitHub Actions: a minimal workflow is available at `.github/workflows/assetbuilder.yml` and uploads `assets/out` as an artifact.
