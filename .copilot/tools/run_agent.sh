#!/usr/bin/env bash
set -euo pipefail

# Simple deterministic runner for Copilot prompt templates.
# Usage: .copilot/tools/run_agent.sh <template.yml> <output-name> "<task>" "<files>" "<constraints>"

if [ "$#" -lt 3 ]; then
  echo "Usage: $0 <template.yml> <output-name> \"<task>\" [files] [constraints]"
  exit 2
fi

TEMPLATE="$1"
OUTNAME="$2"
TASK="$3"
FILES="${4:-}"
CONSTRAINTS="${5:-}"

OUTDIR=".copilot/artifacts"
mkdir -p "$OUTDIR"

# Render placeholders using awk (safe for most simple templates)
PROMPT=$(awk -v t="$TASK" -v f="$FILES" -v c="$CONSTRAINTS" '{
  gsub("{{task}}", t);
  gsub("{{files}}", f);
  gsub("{{constraints}}", c);
  print
}' "$TEMPLATE")

PROMPT_FILE="$OUTDIR/$OUTNAME.prompt.txt"
META_FILE="$OUTDIR/$OUTNAME.metadata.json"
OUTPUT_FILE="$OUTDIR/$OUTNAME.output.txt"

printf '%s' "$PROMPT" > "$PROMPT_FILE"

cat > "$META_FILE" <<EOF
{
  "prompt_file": "$PROMPT_FILE",
  "prompt_version": "1.0",
  "model": "<MODEL_PLACEHOLDER>",
  "date": "$(date -u +%Y-%m-%dT%H:%M:%SZ)",
  "regenerate_command": "${PWD}/$0 $TEMPLATE $OUTNAME \"$TASK\" \"$FILES\" \"$CONSTRAINTS\""
}
EOF

echo "Rendered prompt -> $PROMPT_FILE"
echo "Wrote metadata -> $META_FILE"

echo "NOTE: This runner does not call an LLM by default. To produce an output using OpenAI or another LLM, run a command like:"
echo
echo "  curl -s -X POST https://api.openai.com/v1/chat/completions \\
    -H \"Authorization: Bearer $OPENAI_API_KEY\" \\
    -H \"Content-Type: application/json\" \\
    -d @request.json > $OUTPUT_FILE"
echo
echo "Where request.json contains your model, messages, and system prompt. Save the model response to $OUTPUT_FILE when done."

exit 0
