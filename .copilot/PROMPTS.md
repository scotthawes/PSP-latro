# Prompt templates for Copilot / agents

Use these short templates when asking the assistant to perform repository tasks.

Run AssetBuilder (convert/pack assets):

"Run AssetBuilder: execute `scripts/asset_builder/build_assets.sh`. Upload artifacts and report any errors. Return the manifest path and number of generated files. If a step fails, show the failing command and last 20 lines of output."

Build project (PSPBuilder):

"Run PSPBuilder: execute `scripts/pspbuilder/build_with_docker.sh $REPO_PATH all`. Use `pspdev/psp-sdk:latest` container. Upload `build/EBOOT.PBP` or `EBOOT.PBP` and build logs. If build fails, include compiler errors."

Run smoke tests (TestRunner):

"Run TestRunner: execute `scripts/testrunner/run_smoke_test.sh true`. Capture screenshot, compare to baseline, and upload `test-output/diff.png`. Return PASS/FAIL and normalized RMS."

Static checks (PR Reviewer):

"Run PR checks: run `cppcheck` and `clang-format --dry-run` on changed files and report issues. Also validate asset sizes in `assets/out` do not exceed configured budgets."

Notes for the assistant:
- When running scripts, always run them from repository root.
- If artifacts are produced, attach or list their relative paths.
- Use `--dry-run` or `--force` flags as appropriate only when explicitly requested.
