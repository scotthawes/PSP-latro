---
name: psp-assets
description: "Asset pipeline guidance: image/audio transforms, atlases, LFS, and tooling for PSP builds."
---

# PSP Assets Skill

Use for converting, optimizing, and packaging game assets for PSP targets.

Key rules
- Convert audio offline to 44.1kHz stereo OGG. Example:
  `ffmpeg -i input.wav -ar 44100 -ac 2 -c:a libvorbis -q:a 6 output.ogg`
- Convert images to PSP-ready formats (16-bit) and run `pngquant` or `optipng` to reduce size.
- Use a texture packer (TexturePacker or custom atlas tool) to create atlases matching GU alignment requirements.
- Large generated assets belong in `assets/out/` and should be tracked with Git LFS.

Automation
- Provide reproducible scripts under `scripts/asset_builder/` and use CI to generate artifacts on demand.
