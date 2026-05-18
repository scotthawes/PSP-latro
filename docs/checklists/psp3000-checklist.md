# PSP-3000 Device Test Checklist

**Target device:** PSP-3000 (333 MHz CPU, 64 MB RAM)  
**Last updated:** 2026-05-18  
**Branch:** `feat/psp3000` (test against the latest deployed build)  
**Reference:** `docs/guides/devguide-psp3000.md`

> Run this checklist on a physical PSP-3000 before accepting any non-EB phone or frame-budget change.

---

## 0 — Pre-flight

| # | Check | Pass |
|---|-------|------|
| 0.1 | `EBOOT.PBP` SHA256 matches the artifact uploaded from CI (use `md5sum` on PSP or compare on host). | |
| 0.2 | `settings.ini` is present on `ms0:/PSP/PSPALATRO/settings.ini` with `debug_overlay = false` for clean captures. | |
| 0.3 | Battery is ≥50 % or PSP is plugged in to avoid power-related clock throttling. | |
| 0.4 | No other large *.iso is in the memory-stick folder that could fragment the filesystem. | |

---

## 1 — Visual & Frame Rate

| # | Check | Pass |
|---|-------|------|
| 1.1 | **Boot time:** Title/start screen appears within 3 seconds of pressing `X`. | |
| 1.2 | **No blank frame:** Title logo, background art, or "Press X to Start" are visible immediately — no black frame. | |
| 1.3 | **Static shot:** Press `Screenshot` on the title screen; save to `ms0:/PSP/PHOTO/` and verify the saved PNG matches PPSSPP emulator screenshot 1:1 (no garbled glyphs). | |
| 1.4 | **Menu navigation:** D-pad / analog stick moves the cursor; `X` confirms, `O` goes back. All four main-menu entries are reachable. | |
| 1.5 | **Menu transition:** "New Run" transitions to blind-select screen without flicker or blank frame. | |
| 1.6 | **Card fan layout:** In-game hand shows 8 cards in correct arc overlap order; no cards clip outside the bottom or side edges at 480×272. | |
| 1.7 | **Blind panel:** The active blind chip is centred and clipped correctly in blind-select mode on PSP resolution. | |
| 1.8 | **Shop:** Shop items render at readable size with icon + label visible; no overflow outside the panel. | |

---

## 2 — Shader & Effects (visual confirmation)

| # | Check | Pass |
|---|-------|------|
| 2.1 | **Foil edition:** A foil-deck playing card shows animated blue-chrome shimmer when held in hand. | |
| 2.2 | **Holographic edition:** A holo-deck card shows rotating hue shift (~4 s cycle) across the card face. | |
| 2.3 | **Polychrome edition:** Card border steps through visible color palette every ~0.67 s (40-frame discrete cycle). | |
| 2.4 | **Negative edition:** Card colours are inverted (dark background becomes lighter, white text becomes dark). | |
| 2.5 | **Flash:** `X` on a winning/score card produces a visible white flash overlay that fades. | |
| 2.6 | **Dissolve:** Mr Bones / card-death dissolve hides the card gradually (no pixel-artifacts or hard edges during transition). | |

> Shader effects are CPU-rasterised per-pixel. If any of 2.1–2.6 shows frame-rate freezing or visual artefacts, record the FPS using the profiling harness (§5) and open a shader-performance ticket.

---

## 3 — Audio

| # | Check | Pass |
|---|-------|------|
| 3.1 | **Music plays on boot:** Background music is audible immediately after the title screen appears. | |
| 3.2 | **Audio fades in:** Track volume is not a hard jump; initial volume is <50 % and ramps to `settings.ini` level. | |
| 3.3 | **No underrun clicks:** Listen for 60 s of continuous music; no pops, clicks, or sudden silence stutters. | |
| 3.4 | **Loop point:** Track repeats cleanly — no audible click or volume bump at the loop boundary. | |
| 3.5 | **New run transition:** Music continues without drop-out when "New Run" is selected. | |
| 3.6 | **SFX:** Earliest card interaction (pick up) produces an audible click/snap. | |
| 3.7 | **OVERRUN log absence:** `logs/audio.log` (if present) contains zero `UNDERRUN` entries in a 5 min play session. | |

---

## 4 — Memory & Stability

| # | Check | Pass |
|---|-------|------|
| 4.1 | **Free RAM at title screen:** ≥ 55 MB free reported in debug overlay (enable by setting `debug_overlay = true` in `settings.ini`). | |
| 4.2 | **AFTER a full run** (8–15 hands): ≥ 50 MB free — heap has not leaked >4 MB during a 20-min run. | |
| 4.3 | **No crash:** Play through 3 consecutive complete runs without the binary freezing or returning to the XMB. | |
| 4.4 | **Log absence:** `| grep -i "panic\|fatal\|abort" /tmp/pspalatro_*.log` returns zero lines. | |
| 4.5 | **Regression: Joker stage-flow freeze (Midas Mask / Devil Tarot):** Spend ≥30 hands (select a joker-heavy run if possible). The score screen must not hang forever on the first or tenth hand — the Midas Mask Gold-tag enable and Devil Tarot Gold-tag select must complete within 10 s each. | |

---

## 5 — Performance Profiling Harness

The profiling harness — added in `src/draw.c` — records per-frame-section CPU time when `settings.ini` sets `debug_overlay = true`.

### Enable profiling

```
ms0:/PSP/PSPALATRO/settings.ini → debug_overlay = true
```

### What the overlay shows (≈ top-left corner)

| Label | Section | Meaning |
|-------|---------|---------|
| `FPS` | (overall) | Global FPS from `sceRtc` interval measurement |
| `f:<ms>` | frame_total | Total `game_draw()` wall-clock (ms per frame) |
| `background` | background | `game_draw_background_depth_layers()` |
| `menu / game_ingame / blind_select / shop` | stage | Stage draw function wall-clock |
| `sub_info` | sub_info | Deck/run info panels |
| `debug` | debug_overlay | `game_draw_debug_info()` itself |

Each row shows `<name> <ms> <pct>` — milliseconds and percentage of total frame time.

### Harness pass thresholds (PSP-3000, 480 × 272):

| Metric | Target | Fail threshold |
|--------|--------|---------------|
| Frame total (`f:`) | ≤ 14 ms (≥ 60 fps budget) | > 16 ms → likely shader hot-path issue |
| Any section overrun | Any single section consuming ≥ 50 % of frame budget | Log section name and open bug |
| Section call count | > 0 for active stage; 0 for inactive stages | > 0 everywhere → PROFILE guard broken |

### Collecting results

1. Set `debug_overlay = true` in `settings.ini`.
2. Launch and hold on the title screen for 30 s.
3. Advance through `New Run → Blind Select → Skip → Hand` to capture all three stage names.
4. Set `debug_overlay = false`, re-pack, and re-test on the packaged EBOOT (profiling overhead must not be present in release builds).

> `profiler_init()` is initialised lazily on the first entry to `game_draw_debug_info()`.  
> Section accumulators are cumulative — the overlay divides by `g_prof.sections[i].call_count` to get average per-section ms.  
> Zero-cost in non-`DEBUG` builds: all `PROF_SECTION` / `PROF_END` macros compile to `(void)0`.

---

## 6 — Packaging & Deploy Parity

| # | Check | Pass |
|---|-------|------|
| 6.1 | **PSP-3000 deploy path:** `ms0:/PSP/GAME/PSPALATRO/EBOOT.PBP` is the exact binary tested. | |
| 6.2 | **UMD fallback test:** The same `EBOOT.PBP` is also loadable from `ef0:/PSP/GAME/PSPALATRO/EBOOT.PBP` (EF01 internal storage on some PSP models). | |
| 6.3 | **MD5 parity:** `md5sum ms0:/PSP/GAME/PSPALATRO/EBOOT.PBP` matches the `${CI_BUILD_HASH}` in the latest workflow run annotation. | |
| 6.4 | **settings.ini loaded from PSP:** Changing `speed = 2` in `settings.ini` on the memory stick produces visibly snappier animation (verify with human eye). | |

---

## 7 — Results & Sign-off

| Field | Value |
|-------|-------|
| Tested by | |
| PSP-3000 serial | (write on device back) |
| Date | |
| iOS/Android version | (FW version under *Settings → System Settings → System Information*) |
| Passed sections | 0 / 1 / 2 / 3 / 4 / 5 / 6 / 8 / 8a |
| Failures / notes | |

---

## 8 — Code-Fix Regression Gate (this release)

> Items here code-verify the four source changes in this session (`src/game.c`, `src/automated_events.c`,  
> `src/game_util.c`, `docs/trackers/consolidated-status.md`).  
> Checklist §8a below captures PSP-3000 device evidence that the fixes operate correctly in the field.

| # | Code change | Build / static confirm | PSP evidence |
|---|-------------|----------------------|--------------|
| 8.1 | `src/game.c` — all 150 `g_joker_types` struct entries initialized `enabled:true`; 3 overrides to `enabled=false` in `game_init_logic()` set CAVENDISH, **STEEL_JOKER** (⚠️ permanently — no `.enabled=true` path), and LUCKY_CAT — CAVENDISH and LUCKY_CAT unlock during gameplay (SCORE_END_ROUND / TAROT_MAGICIAN) → effective **148 enabled jokers** | ✅ `false → true` diff reviewed; 3 runtime locks checked; Steel Joker lock confirmed via exhaustive grep; build green | All 148 enabled jokers appear in shop / booster packs; scoring call-sites fire for those 3 |
| 8.2 | `src/automated_events.c` — scoring call-sites added for **Steel Joker, Square Joker, Blackboard, Runner** in `SCORE_JOKERS` stage | ✅ Cases present at lines 1344 / 1360 / 1368 / 1386 | Mult / Chips counters update immediately when each joker condition is met |
| 8.3 | `src/game_util.c:590` — `game_util_get_new_joker_type()` exhaustion fallback: **any enabled joker of the matching rarity** instead of `rand()%JOKER_TYPE_COUNT` | ✅ Fallback block reviewed; build green | No crash when all same-rarity jokers are owned; new joker is same rarity as requested |
| 8.4 | `src/automated_event_score` — **JOKER_TYPE_MIDAS_MASK** for-loop body closed (broken `for` / `if` nesting swallowed `if (used)` and the `break`) | ✅ Brace nesting confirmed | Midas Mask Gold card enhancement appears visually; score screen advances without freeze (~≤ 10 s) |
| 8.5 | `src/automated_event_score` / `SCORE_PRE_SCORE_JOKER` `default:` **break preserved** — fall-through into `SCORE_CARD_JOKER` eliminated | ✅ `switch` reviewed; build green | Score screen resumes cleanly after any joker type that has no pre-score action |
| 8.6 | `src/automated_event_use_tarot` — **TAROT_TYPE_DEVIL** for-loop body closed (unclosed `if (selected)` swallowed `g_game_state.selected_cards_count = 0;` and stage-exit) | ✅ Brace nesting reviewed; build green | Devil Tarot on selected cards places Gold enhancements; no freeze on card selection |
| 8.7 | **Steel Joker lock status** — `game_init_logic()` disables Steel Joker (line 1798); no `g_joker_types[STEEL_JOKER].enabled = true` exists anywhere. Scoring call-site confirmed at line 1344 but joker NEVER appears in shop. `game_util_get_new_joker_type()` line 579 skips disabled jokers | ✅ `enabled=false` assignment confirmed; exhaustive grep for `STEEL_JOKER.*enabled = true` returned 0 results | Steel Joker has no visible shop presence; drop `STEEL_JOKER` from 8a.2 `enabled=true` count; add unlock path in next sprint |

---

## 8a — PSP-3000 Play Evidence (links §8 changes to checklist §1–§4)

| # | Fix | Section 1 | Section 2 | Section 4 |
|---|-----|-----------|-----------|-----------|
| 8a.1 | Shop jokers (enabled pool): all 150 struct-initialized (`enabled:true`), minus 3 locked in `game_init_logic()` (CAVENDISH, **STEEL_JOKER** [no `.enabled=true` path], LUCKY_CAT), total = 147 available at start; CAVENDISH and LUCKY_CAT unlock during gameplay → **148 after Gros Michel destruction / Magician tarot / Lucky booster** | Shader / edition effects; joker cards visible at shop refresh | No crash / freeze in § 4.3 |
| 8a.2 | Square / Blackboard / Runner scoring call-sites → **§1/2/4**: Mult / Chips counters update when each condition is met; Steel Joker scoring call-site structure confirmed at line 1344 but **joker is PERMANENTLY LOCKED** (no `enabled=true` anywhere in codebase) — cannot verify via shop until Steel Joker unlock path is added | Mult / chip numbers update — § 1.4 / 1.6; § 2 shader effects pass on enabled jokers | § 4.3 no freeze after 15 scoring hands for accessible jokers |
| 8a.3 | Joker-shop fallback → § **4** | N/A — no visual change | § 4.3 no crash when 8 Owned jokers and a new one is drafted |
| 8a.4 | Midas Mask brace fix → § **3/4** | § 1.6 card enhancement visible | § 3.5 / § 4.3 no freeze |
| 8a.5 | SCORE_PRE_SCORE default break → § **4** | N/A — no visual regression expected | § 4.3 no freeze on score-cycle from joker without pre-score action |
| 8a.6 | Devil Tarot brace fix → § **3/4** | § 1.6 Gold card appears | § 3.6 SFX / § 4.3 no freeze on Tarot selection |

---

## Appendix — Profiler API Reference (for source maintenance)

```c
/* Section IDs (global.h) */
PROF_SEC_BACKGROUND     0
PROF_SEC_MENU           1
PROF_SEC_GAME_INGAME    2
PROF_SEC_BLIND_SELECT   3
PROF_SEC_SHOP           4
PROF_SEC_CARD_BASE      5   /* future: individual card draw timing */
PROF_SEC_CARD_EFFECTS   6   /* future: edition-effect pixel pass timing */
PROF_SEC_TEXT           7
PROF_SEC_DEBUG_OVERLAY  8
PROF_SEC_SUB_INFO       9
PROF_SEC_FLASH_OVERLAY 10
PROF_SEC_TOTAL_FRAME   11

/* Macros (global.h) */
PROF_SECTION(&g_prof, PROF_SEC_MENU);   // save t0, set active_section
PROF_END(&g_prof);                       // add delta to total_ticks, inc call_count

/* Display helpers (draw.c) */
float profiler_ms_for_section(&g_prof, sec, tick_res);
float profiler_pct_for_section(&g_prof, sec);
/* g_prof.sections[sec].name — section label string                            */
/* g_prof.sections[sec].call_count — number of frames this section was profiled */
/* g_prof.sections[sec].total_ticks — cumulative RTC ticks                     */
```

All three are `static` file-scope functions in `draw.c`. They compile to zero instructions in non-`DEBUG` builds.
