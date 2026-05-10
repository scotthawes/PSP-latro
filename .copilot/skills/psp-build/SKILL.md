---
name: psp-build
description: "Build and toolchain guidance for PSP homebrew: PSPSDK, Makefile, Docker, and reproducible builds."
---

# PSP Build Skill

Use this skill when you need to build, cross-compile, or package PSP-latro, diagnose linker errors, or make builds reproducible in CI.

When to use
- Cross-compilation problems (missing headers, linker errors)
- Packaging `EBOOT.PBP` and `PARAM.SFO`
- Creating deterministic builds (Docker, pinned SDK)

Key guidance
- Use the official PSPSDK toolchain (`psp-gcc`) and `psp-config` to find SDK paths.
- Prefer containerized builds for CI: the `pspdev/psp-sdk` image provides a reproducible environment.
- Keep build artifacts out of `master`; use `build/` and `assets/out/` (Git LFS for large files).

Docker example (reproducible build):

```bash
docker run --rm -v "$PWD":/work -w /work pspdev/psp-sdk:latest \
  bash -lc "make clean && make"
```

Common fixes
- Linker: add missing `-l<lib>` to `LDFLAGS` (e.g., `-lvorbisfile`, `-lpspaudiolib`).
- Headers: ensure `PSPDEV`/`PSPSDK` env vars are set; run `psp-config --cflags` to verify.
- Packaging: use `pack-pbp` to create `EBOOT.PBP` and verify `PARAM.SFO` fields.

Outputs
- `pspalatro.elf` — intermediate ELF
- `EBOOT.PBP` — packaged executable
- `PARAM.SFO` — metadata file for PSP
