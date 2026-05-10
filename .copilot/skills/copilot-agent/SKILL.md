---
name: copilot-agent
description: "Lightweight Copilot agent skill to assist with repository automation, prompt templating, artifact generation, and PR scaffolding for PSP-latro. Use when asking the assistant to perform reproducible prompt runs, prepare artifacts, or generate reviewer-ready PR descriptions."
---

# Copilot Agent Skill

This skill describes a small specialist agent that helps with AI-assisted workflows for this repository.

## When to use this skill

- Generate reproducible artifacts from prompt templates (stored under `.copilot/prompts/`).
- Produce well-formed PR descriptions, commit messages, and change summaries.
- Scaffold or review small code changes, run linters, and create runnable suggestions.
- Create metadata and reproducible run scripts for generated outputs (so outputs can be regenerated in CI).

## Capabilities

- Understand the PSP-latro code layout and common workflows (build, asset pipeline, deploy).
- Produce machine-readable outputs (JSON) with `summary`, `changes` (patches), `tests` (commands).
- Create an artifact bundle with `prompt`, `metadata.json`, and optional `output.txt` saved under `.copilot/artifacts/`.

## Invocation and rules

- Prefer generating patches (unified diff) for code edits rather than directly editing files.
- Always include a `regenerate_command` in artifact metadata showing exactly how to reproduce the output.
- Do not include secrets, credentials, or API keys in prompts, artifacts, or metadata.
- Never push changes directly to `master`/`main` without an explicit user confirmation. Use branches and PRs.

## Artifact metadata (recommended)

Each generated artifact should be accompanied by a `metadata.json` with fields:

```
{
  "prompt_file": "...",
  "prompt_version": "1.0",
  "model": "<model-id>",
  "date": "YYYY-MM-DDThh:mm:ssZ",
  "seed": "<optional>",
  "regenerate_command": "./.copilot/tools/run_agent.sh ..."
}
```

## Examples

- "Create a PR body summarizing the repo-hygiene changes and list any files that need manual review."  
- "Run the `copilot-agent` prompt to produce a tester checklist and a smoke-test command list."  

## Tools

This skill expects the following helpers to exist in the repository:

- `.copilot/prompts/` — versioned prompt templates.
- `.copilot/tools/run_agent.sh` — a deterministic runner that renders a template and writes metadata.
