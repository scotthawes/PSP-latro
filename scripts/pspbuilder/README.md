PSPBuilder

Purpose: build and pack the PSP-latro project using PSPSDK (local or Docker).

Local usage (requires pspsdk in PATH):

```bash
# build locally
scripts/pspbuilder/build_with_docker.sh
# pack EBOOT
scripts/pspbuilder/pack_eboot.sh
```

Docker usage (recommended if you don't have pspsdk installed):

```bash
# will use image pspdev/psp-sdk:latest by default
scripts/pspbuilder/build_with_docker.sh /path/to/repo all
```

Notes:
- The GitHub Actions workflow `.github/workflows/pspbuilder.yml` runs the build inside `pspdev/psp-sdk:latest`.
- If `make pack` is not available, `pack_eboot.sh` will try `pack-pbp` on `pspalatro.elf`.
