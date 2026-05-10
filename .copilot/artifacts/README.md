Copilot artifacts
=================

Purpose
-------
This folder is intended to host Copilot / AI-generated artifacts when working on the
`feature/copilot-artifacts` branch. Keep generated outputs, prompt-run logs, model demo
outputs, and temporary artifacts here to avoid polluting `master`/`main`.

Policy
------
- Store minimal metadata alongside each artifact: `prompt_file`, `prompt_version`,
  `model`, `date`, `seed`, and `regenerate_command`.
- Large binaries (images, audio, builds) must use Git LFS and preferably live under
  `assets/out/` or `build/` rather than this folder.
- Do not merge this folder into `master` without an explicit review and acceptance of
  the generated content.
- Never include secrets or API keys in committed prompts or logs.

Regeneration
------------
Always provide a reproducible command (script) to re-run generation. Prefer a
containerized runner (Docker / GitHub Actions) and check that the script exits
deterministically.
