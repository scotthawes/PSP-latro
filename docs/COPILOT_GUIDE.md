Copilot / AI Usage Guide
========================

Purpose
-------
Guidelines and practical steps to make safe, reproducible, and cost-effective use
of Copilot/LLM tooling in this repository.

Principles
----------
- Keep prompts and templates under `.copilot/prompts/` with a `version` field.
- Keep generated outputs on a dedicated branch (`feature/copilot-artifacts`).
- Record metadata (model, prompt, prompt_version, seed, timestamp) for every run.
- Prefer reproducible scripts to regenerate outputs; commit the script, not the
  output, to the default branch unless outputs are reviewed.

Repository layout (suggested)
----------------------------
- `.copilot/prompts/` — canonical prompt templates (YAML or JSON), versioned.
- `.copilot/tools/` — helper scripts to run generation and normalization.
- `.copilot/artifacts/` — generated outputs (branch-only).
- `docs/COPILOT_GUIDE.md` — this file.

Workflow
--------
1. Write or update a prompt template in `.copilot/prompts/` with clear variable
   placeholders and examples.
2. Add/modify a small script under `.copilot/tools/` that runs the template with
   deterministic settings (fixed seed, model version, token limits).
3. Run locally or in CI container to generate outputs. Store outputs in
   `feature/copilot-artifacts` branch and include a JSON metadata file per artifact.
4. Open a PR from `feature/copilot-artifacts` to a review branch; reviewers must
   verify content and tests before merge to the artifacts branch (do not merge
   artifacts into `master` automatically).

CI and reproducibility
----------------------
- Provide a GitHub Actions job that can regenerate artifacts in a pinned container
  (example: `pspdev/psp-sdk:latest` for build steps or a minimal Python container
  for prompt runs).
- Tag the workflow to run only when a PR has the `regenerate-artifacts` label or
  when maintainers explicitly request regeneration.
- Archive outputs as CI artifacts and attach the metadata JSON for traceability.

Security & governance
---------------------
- Never commit API keys, credentials, or private data inside prompts or logs.
- Audit prompt outputs for PHI/PII or license problems before publishing.

Cost & efficiency
-----------------
- Limit context size and token budget in templates.
- Cache deterministic outputs and reuse them where possible.

Quality & testing
-----------------
- Add automated checks: snapshot diffs (images/audio), schema validation for
  generated JSON, and smoke tests to verify expected outputs.

Quick start
-----------
1. Create or update a prompt: `.copilot/prompts/example.yml`
2. Run: `./.copilot/tools/run_prompt.sh example.yml > .copilot/artifacts/example.json`
3. Commit to `feature/copilot-artifacts`, open PR, request review.
