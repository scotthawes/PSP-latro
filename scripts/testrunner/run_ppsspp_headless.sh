#!/usr/bin/env bash
set -euo pipefail

EBOOT="${1:-build/EBOOT.PBP}"
OUT="${2:-test-output/actual.png}"
DURATION="${3:-6}"
WIDTH="${4:-480}"
HEIGHT="${5:-272}"

if [ ! -f "$EBOOT" ]; then
  echo "EBOOT not found: $EBOOT" >&2
  exit 1
fi

PPSSPP_BIN=""
if command -v ppsspp >/dev/null 2>&1; then
  PPSSPP_BIN=$(command -v ppsspp)
elif command -v PPSSPPSDL >/dev/null 2>&1; then
  PPSSPP_BIN=$(command -v PPSSPPSDL)
fi

# macOS app-bundle fallbacks (Homebrew cask installs PPSSPPSDL.app)
if [ -z "$PPSSPP_BIN" ]; then
  if [ -x "/Applications/PPSSPPSDL.app/Contents/MacOS/PPSSPPSDL" ]; then
    PPSSPP_BIN="/Applications/PPSSPPSDL.app/Contents/MacOS/PPSSPPSDL"
  elif [ -x "/Applications/PPSSPP.app/Contents/MacOS/PPSSPP" ]; then
    PPSSPP_BIN="/Applications/PPSSPP.app/Contents/MacOS/PPSSPP"
  fi
fi

if [ -z "$PPSSPP_BIN" ]; then
  echo "PPSSPP binary not found. Install PPSSPP and ensure 'ppsspp' or 'PPSSPPSDL' is in PATH." >&2
  exit 2
fi

mkdir -p "$(dirname "$OUT")"

TMP_SCRIPT=$(mktemp -t ppspp-run.XXXX.sh)
cat > "$TMP_SCRIPT" <<EOF
#!/usr/bin/env bash
set -eo pipefail
"$PPSSPP_BIN" "$EBOOT" &
EMUPID=\$!
sleep $DURATION
if command -v import >/dev/null 2>&1; then
  import -window root "$OUT" || true
else
  xwd -root -silent > /tmp/ppsspp.xwd || true
  if [ -f /tmp/ppsspp.xwd ]; then
    if command -v convert >/dev/null 2>&1; then
      convert /tmp/ppsspp.xwd "$OUT" || true
    else
      mv /tmp/ppsspp.xwd "$OUT" || true
    fi
  fi
fi
if [ -n "\${EMUPID:-}" ]; then
  kill "\$EMUPID" || true
fi
EOF

chmod +x "$TMP_SCRIPT"

if command -v xvfb-run >/dev/null 2>&1; then
  xvfb-run -s "-screen 0 ${WIDTH}x${HEIGHT}x24" "$TMP_SCRIPT"
else
  # try to run on current display
  "$TMP_SCRIPT"
fi

rm -f "$TMP_SCRIPT"
echo "Screenshot saved to $OUT (if capture succeeded)"