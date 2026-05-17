# Menu Design Resources

Purpose
-------
This document centralizes the best available references for implementing and validating the PSP-latro menu feature.

Use this with [menu-feature-workflow.md](menu-feature-workflow.md).

1. Source game references (highest fidelity)
-------------------------------------------
These files come from the extracted source game archive and are the best reference for state-driven UI behavior and menu structure.

- [third_party/balatro-v011-reference/game_love_extracted/main.lua](../third_party/balatro-v011-reference/game_love_extracted/main.lua)
  - Main update/draw loop architecture.
  - Input event routing shape.
- [third_party/balatro-v011-reference/game_love_extracted/functions/state_events.lua](../third_party/balatro-v011-reference/game_love_extracted/functions/state_events.lua)
  - Canonical state transition patterns.
  - Separation between state logic and event queues.
- [third_party/balatro-v011-reference/game_love_extracted/functions/UI_definitions.lua](../third_party/balatro-v011-reference/game_love_extracted/functions/UI_definitions.lua)
  - UI composition patterns.
  - Main-menu button conventions and overlay interactions.

Observed menu-relevant cues from source game
--------------------------------------------
- Menu behavior is state-first, UI-second.
- Overlay menus and explicit state transitions are used heavily.
- Visual structure is defined declaratively in UI definition helpers.

2. PSP-native references (platform behavior)
--------------------------------------------
These references are valuable for reliable PSP button handling and menu navigation patterns.

- [pspdev sample: gu/common/menu.c](../../../pspdev/psp/sdk/samples/gu/common/menu.c)
  - Menu context model (`MenuContext`, `MenuItem` tree).
  - Input edge handling using previous button state.
  - Input capture while menu is open.
- [pspdev sample: gu/common/menu.h](../../../pspdev/psp/sdk/samples/gu/common/menu.h)
  - Minimal abstractions that map cleanly to PSP-latro's C codebase.
- [pspdev sample: gu/vertex/vertex.c](../../../pspdev/psp/sdk/samples/gu/vertex/vertex.c)
  - Practical example of integrating menu handling into a render loop.
- [pspdev sample: kernel/fileio/main.c](../../../pspdev/psp/sdk/samples/kernel/fileio/main.c)
  - Straightforward controller polling loop pattern and callback setup.

Platform implementation patterns to copy
----------------------------------------
- Maintain `last_buttons` and detect press edges (`pressed = current & ~last`).
- Keep menu input handling centralized and consume input when menu is active.
- Keep render and input deterministic in the same loop cadence.

3. Emulator/runtime references (deployment correctness)
------------------------------------------------------
These references help with path and startup assumptions in PPSSPP.

- PPSSPP codebase references (for path/savestate behavior):
  - `Core/PSPLoaders.cpp`
  - `Core/Util/GameManager.cpp`
  - `UI/GameBrowser.cpp`

Practical implication for PSP-latro
-----------------------------------
- Always verify deployed EBOOT path and hash parity across all launch locations.
- Treat `ms0:/PSP/GAME/PSPALATRO/` as the primary runtime path assumption.

4. Current PSP-latro files to align with these references
---------------------------------------------------------
- [src/menu.c](../src/menu.c)
- [src/game.c](../src/game.c)
- [src/game_input.c](../src/game_input.c)
- [src/draw.c](../src/draw.c)
- [src/main.c](../src/main.c)
- [src/global.h](../src/global.h)

5. Design decisions guided by resources
---------------------------------------
When making menu changes, enforce these rules:

1. State machine controls flow.
2. Menu stage must bypass gameplay automation/event systems.
3. Input edges, not level-triggered repeats, drive menu selection changes.
4. Menu render path must include texture-free fallback cues for diagnosis.
5. Deploy-path parity must be validated before concluding behavior.

6. Quick extraction commands
---------------------------
Use these commands to refresh context quickly:

1. List source-game archive entries:
   `unzip -l third_party/balatro-v011-reference/game.love | head -120`
2. Find menu/state usage in extracted Lua:
   `grep -Rni "MENU\|state\|title\|main_menu" third_party/balatro-v011-reference/game_love_extracted | head -120`
3. Find PSP menu sample patterns:
   `grep -Rni "handleMenu\|sceCtrlReadBufferPositive\|PSP_CTRL_UP\|PSP_CTRL_DOWN" /Users/Scott/pspdev/psp/sdk/samples | head -120`
