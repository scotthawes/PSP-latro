---
name: psp-deploy
description: "Packaging and deployment: pack-pbp, PARAM.SFO, and copying to PSP memory stick or FTP."
---

# PSP Deploy Skill

Use when packaging the game and deploying to physical PSP or remote storage.

Packaging
- Create `PARAM.SFO` with correct `TITLE`, `CATEGORY`, and `APP_VER` fields.
- Use `pack-pbp` to create `EBOOT.PBP` from the ELF and assets.

Deploy
- Copy `EBOOT.PBP` to `/Volumes/Untitled/PSP/GAME/PSPALATRO/` for USB deploy.
- Use `scp`/`ftp` scripts for remote deploy where available.

CI
- Add a `pack-eboot` job that runs `make` in a pinned Docker and uploads `EBOOT.PBP` as an artifact for testers.
