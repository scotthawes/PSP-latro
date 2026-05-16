# UI Comparison Test Plan (Post-Parity)

Date: 2026-05-16
Scope: Compare current PSP-latro UI against actual game references in assets/ui_db/.

## Objectives

- Quantify remaining UI differences after the previous parity closure.
- Provide repeatable pass and fail criteria for each new comparison issue.
- Attach evidence per scene and per modal screen.

## Capture Profile

Follow docs/PPSSPP_CAPTURE_PROFILE.md exactly.

Required:

- PPSSPP overlays off
- debug_overlay = false
- PNG captures
- Stable window scale during a single run

## Required Current Captures

Store under test-output/ui-comparison/:

1. pregame-current.png
2. ingame-current.png
3. rewards-current.png
4. scoring-current.png
5. title-current.png
6. menu-current.png
7. settings-game-current.png
8. settings-video-current.png
9. settings-graphics-current.png
10. settings-audio-current.png
11. language-current.png
12. credits-current.png
13. run-setup-current.png
14. game-over-current.png

## Reference Set

Use assets/ui_db/:

1. pre_game_lobby.jpg
2. gameplay_hud.jpg
3. rewards.jpg
4. gameplay_huds_2.jpg
5. Balatro_title.jpg
6. balatro_menu.jpg
7. balatro_game_options.jpg
8. balatro_video_options.jpg
9. balatro_graphics_options.jpg
10. balatro_audio_options.jpg
11. balatro_language_options.jpg
12. balatro_credits.jpg
13. custom_game_rules.jpg
14. failure_game_over.jpg

## Test Cases

1. TC-01 Text legibility
   - Method: side-by-side visual inspection
   - Pass: no garbled labels in all captured screens

2. TC-02 Pregame blind card composition
   - Method: compare pregame-current.png vs pre_game_lobby.jpg
   - Pass: blind card internals, action grouping, and hierarchy are aligned

3. TC-03 Left rail hierarchy
   - Method: compare pregame and ingame HUD columns
   - Pass: module boundaries, spacing, and value emphasis match target intent

4. TC-04 Gameplay action row
   - Method: compare ingame-current.png vs gameplay_hud.jpg
   - Pass: primary and secondary controls match sizing and readability intent

5. TC-05 Table atmosphere and composition
   - Method: compare ingame and scoring captures
   - Pass: background depth and overlay balance are within target style

6. TC-06 Title and main menu parity
   - Method: compare title-current.png and menu-current.png
   - Pass: logo scale, action bar layout, chips, and panel spacing are aligned

7. TC-07 Modal screen parity
   - Method: compare settings, language, credits, run setup, game over captures
   - Pass: tab bars, panel geometry, control spacing, and button treatment are aligned

## Exit Criteria

- All P0 and P1 issues pass.
- Any P2 differences are documented with explicit deferral.
- Evidence captures are attached in each issue doc.
- Collation checklist is fully checked.
