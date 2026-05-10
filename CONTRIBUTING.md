# Contributing to PSP-latro

Thank you for contributing. This document explains where files belong, what to commit,
and quick commands to keep generated PSP assets out of history.

## Where files go

- `assets/src/` — source assets you edit (PSD, WAV, AI, .blend, source art/audio). Commit these.
- `assets/out/` — generated PSP-ready assets (16-bit PNGs, atlases, `.ogg`, `manifest.json`). Do NOT commit binary outputs; this directory is ignored and tracked with Git LFS when necessary.
- `src/` — C source and headers. Commit code changes here.
- `scripts/` — helper scripts (asset conversion, build wrappers, deploy helpers). Commit these and mark executable.
- `tools/` — developer tools and Dockerfiles (small text-based helpers). Prefer a separate repo for very large tools.
- `build/` — build artifacts and `EBOOT.PBP`. Do NOT commit; produced by CI or local builds.
- `.github/workflows/` — CI workflows (YAML). Commit.
- `.copilot/` — playbooks, prompts, agent manifests. Commit small text files only.

## Git rules & best practices

- Commit only source files, scripts, and docs. Do not commit generated binaries (assets/out, build).
- Use `.gitattributes` and Git LFS for large generated assets you must keep in the repo (example pattern: `assets/out/*.{ogg,png}`).
- Keep `assets/out/` listed in `.gitignore`.
- CI should reproduce `assets/out/` and `build/` from `assets/src/` and `src/` (no binary artifacts in master).

## Quick commands

Run this from the repository root to locate tracked binary assets and move them into `assets/out/` or `build/`:

```bash
EXCLUDE_PAT='^(assets/src/|src/|media/|scripts/|docs/|\.copilot/|assets/out/|build/|\.github/)'
git ls-files -z | tr '\0' '\n' \
  | egrep -i '\.(ogg|wav|mp3|flac|png|jpg|jpeg|dds|pbp|elf)$' \
  | egrep -v "$EXCLUDE_PAT" > /tmp/psp_tracked_binaries.txt || true

if [ -s /tmp/psp_tracked_binaries.txt ]; then
  mkdir -p assets/out build
  while IFS= read -r f; do
    [ -z "$f" ] && continue
    if [ ! -e "$f" ]; then
      echo "Skipping missing: $f"; continue
    fi
    ext="${f##*.}"
    if [[ "$ext" =~ ^(pbp|elf)$ ]]; then dest="build/$f"; else dest="assets/out/$f"; fi
    mkdir -p "$(dirname "$dest")"
    git rm --cached -q -- "$f" || true
    mv -- "$f" "$dest" || echo "Failed to move: $f"
  done < /tmp/psp_tracked_binaries.txt
  echo "Moved tracked binaries to assets/out/ and build/"
else
  echo "No tracked generated binary assets found outside source dirs."
fi
```

## Notes

- Do not include copyrighted commercial binaries in the repository. If you need to preserve extracted files for development, move them into `assets/out/` and document license/ownership in `docs/`.
- When in doubt, open a PR and request review for large binary moves.

Thank you — keep commits small and focused.