# Menu & Title Screen — Implementation Progress

**Last updated:** 2026-05-18  
**Branch:** `feat/psp3000`
**Session:** post-bugfix pass — static declarations, `hint_y`, forward decl, duplicate `#define`s

---

## Phase A — Structural data & persistence (completed)

| # | Item | Status | Files / Lines |
|---|------|--------|---------------|
| A.1 | `int stake` added to `struct Settings` | ✅ | `src/global.h:78` |
| A.2 | `STAKE_TYPE_WHITE/RED/BLUE/BLACK` enum + `STAKE_TYPE_COUNT` | ✅ | `src/global.h:622–631` |
| A.3 | `g_stake_type_names[4]` static table | ✅ | `src/game.c:66–71` |
| A.4 | `g_deck_type_names[15]` static table | ✅ | `src/game.c` |
| A.5 | `GAME_SUBSTAGE_MENU_SETUP` + `INPUT_FOCUSED_ZONE_MENU_SETUP` | ✅ | `src/global.h:643, 769` |
| A.6 | `seed[9]` + `deck_type` already existed in `struct Settings` | ✅ | `src/global.h:76–77` |
| A.7 | INI save: `deck_type`, `seed`, `stake` fields | ✅ | `src/game.c:1822–1824` |
| A.8 | INI load: `deck_type`, `seed`, `stake` branches | ✅ | `src/game.c` |
| A.9 | Seeded RNG — `srand(hash)` when `seed[0] != '\0'` | ✅ | `src/game.c:1843–1851` |
| A.10 | Remove unused `util_hash_seed()` stub | ✅ | `src/game_util.c` |

---

## Phase B — Options + Run-Setup screens (completed)

| # | Item | Status | Files / Lines |
|---|------|--------|---------------|
| B.1 | `MENU_OPTIONS_*` widened 6 → 10 rows; CREDITS → START_RUN | ✅ | `src/menu.c:116–126` |
| B.2 | `MENU_SETUP_ITEM_*` defines (card_type, seed, stake, start) | ✅ | `src/menu.c:129–132` |
| B.3 | Options draw: 10-row layout with right-aligned values; START_RUN green | ✅ | `src/menu.c:~725–789` |
| B.4 | Options input: 4 gameplay toggle rows + START_RUN → setup transition | ✅ | `src/menu.c:~940–1014` |
| B.5 | `menu_draw_setup()` — 4-row layout: Card Type / Seed / Stake / START RUN | ✅ | `src/menu.c:1020–1100` |
| B.6 | `menu_input_setup()` — UP/DOWN rows, L/R cycle, CROSS commit, CIRCLE back | ✅ | `src/menu.c:1105–1222` |
| B.7 | `menu_draw()` dispatcher: new case + bounds-check | ✅ | `src/menu.c:~834–847` |
| B.8 | `game_input.c`: dispatch `INPUT_FOCUSED_ZONE_MENU_SETUP` | ✅ | `src/game_input.c:1153–1155` |
| B.9 | Forward declarations of `menu_draw_setup()` + `menu_draw_collection()` before `menu_draw()` | ✅ | `src/menu.c:855–856` |
| B.10 | Setup card-type: L/R pages through 15-deck list, CROSS steps one-card | ✅ | `src/menu.c:1180–1194` |

---

## Phase C — Stake scaling & seed display (completed)

| # | Feature | Status | Files / Lines |
|---|---------|--------|---------------|
| C.1 | Seed alphabet fixed to `[A-N, P-Z, 1-9]` — 34 chars, no O / no 0 | ✅ | `src/global.h:638–641` |
| C.2 | `g_stake_price_mults[]` const table — White=1.0, Red=1.25, Blue=1.5, Black=2.0 | ✅ | `src/game_util.c:470` |
| C.3 | `g_stake_blind_score_mults[]` const table — same values | ✅ | `src/game_util.c:469` |
| C.4 | Accessor functions `game_util_get_stake_blind_mult()` / `game_util_get_stake_price_mult()` | ✅ | `src/game_util.c:472–480` |
| C.5 | `game_get_current_blind_score()` multiplies by stake blind mult | ✅ | `src/game.c:500–508` |
| C.6 | All 6 price getters stake-aware: joker/booster/planet/tarot buy+sell | ✅ | `src/game_util.c:491–525` |
| C.7 | Seed/briefcase panel drawn in `menu_draw_title()` | ✅ | `src/menu.c:582–599` |
| C.8 | `game_init_logic()` clamps `deck_type` / `stake` at run start | ✅ | `src/game.c:1836–1838` |
| C.9 | `game_init_logic()` auto-generates 8-char seed when `seed[0] == '\0'` | ✅ | `src/game.c:1853–1860` |
| C.10 | `g_stake_type_names` string table + `STAKE_BLIND_SCORE_MULT`/`STAKE_PRICE_MULT` macros removed (replaced by const arrays) | ✅ | `src/game.c:66–71`, `src/global.h:632–634` |

---

## Phase D — Bugfix pass (completed)

| # | Fix | Status | Files / Lines |
|---|-----|--------|---------------|
| D.1 | Move all static state varis into fixed block at lines 28–34 so `menu_activate_main_item()` (line 36) sees `s_setup_selected_item`, `s_setup_card_type`, `s_setup_stake_type`, `s_setup_seed_char`, `s_setup_deck_page`, `s_collection_selected_item` | ✅ | `src/menu.c:28–34` |
| D.2 | Restore `float hint_y` local + `menu_draw_separator()` call in `menu_draw_options()` (was removed by refactor, leaving `hint_y` dangling on line 813) | ✅ | `src/menu.c:810–812` |
| D.3 | Restore same `hint_y` + separator in `menu_draw_setup()` | ✅ | `src/menu.c:1143–1144` |
| D.4 | Add `void menu_draw_collection(void)` forward declaration before `menu_draw()` dispatcher | ✅ | `src/menu.c:855–856` |
| D.5 | Remove one copy of the duplicated `MENU_COLLECTION_ITEM_*` `#define` block (two identical copies existed after earlier merge) | ✅ | `src/menu.c:87–92` |

---

## Phase E — Collection screen (completed)

| # | Feature | Status | Files / Lines |
|---|---------|--------|---------------|
| E.1 | `menu_draw_collection()` — 2×2 category grid + Back button on dark backdrop | ✅ | `src/menu.c:1267–1326` |
| E.2 | `menu_input_collection()` — Circle → back to main menu; L/R arrows cycle Jokers/Tarots/Planets/Boosters/Back | ✅ | `src/menu.c:1328–1352` |
| E.3 | `INPUT_FOCUSED_ZONE_MENU_COLLECTION` switch arm added to `game_input_update()` | ✅ | `src/game_input.c:1159` |
| E.4 | Menu-draw dispatcher: new `GAME_SUBSTAGE_MENU_COLLECTION` case with bounds guard | ✅ | `src/menu.c:896–901` |

---

## Key decisions locked

| Decision | Choice |
|----------|--------|
| Seed format | 8-char uppercase `[A-N,P-Z,1-9]` — matches `random_string(8, …)` in Lua reference |
| Seed → PRNG | djb2-style hash → `srand()`. Empty → `time(0)` + auto-generate displayable seed |
| Deck list | 15 Balatro decks — Red through Erratic |
| Stake tiers | 4 tiers — White / Red / Blue / Black |
| Blind-score mults | 1.0 / 1.25 / 1.50 / 2.00 |
| Price mults | Same: 1.0 / 1.25 / 1.50 / 2.00 — applied to joker, planet, tarot, booster buy/sell prices |
| Options → setup | PLAY button opens Run Setup screen; START RUN commits → game |
| Collection screen | 2×2 grid (Jokers / Tarots / Planets / Boosters) + Back; full hover navigation via L/R/Circle |
| Setup deck L/R | L/R scrolls 8-card page through 15-deck list; CROSS steps one card at a time on current page |
| Constant type | `STAKE_BLIND_SCORE_MULT` / `STAKE_PRICE_MULT` preprocessor macros removed in favour of `const float []` tables with accessor functions |

---

## Code-location index

| Goal | File | What to do |
|------|------|------------|
| Seed alphabet | `src/global.h:638–641` | 34-char table — `A–N, P–Z, 1–9` |
| Seed on title | `src/menu.c:582–599` | Briefcase panel: PSP-LATRO / deck name / seed display |
| Price arrays | `src/game_util.c:469–470` | `const float g_stake_blind_score_mults[4]`, `g_stake_price_mults[4]` |
| Price accessors | `src/game_util.c:472–480` | `game_util_get_stake_blind_mult()`, `game_util_get_stake_price_mult()` |
| Blind-score scaling | `src/game.c:500–508` | `game_get_current_blind_score()` × `stake_blind_mult` |
| Prices stake-aware | `src/game_util.c:491–525` | All 6 price helpers multiply through `_stake_price()` |
| Auto-seed on run start | `src/game.c:1853–1860` | When `seed[0] == '\0'`, `srand(time)` + fill from `g_seed_alphabet` |
| Stake clamp at run start | `src/game.c:1836–1838` | `CLAMP(g_settings.stake, 0, STAKE_TYPE_COUNT-1)` |
| Setup state vars | `src/menu.c:28–34` | `s_setup_selected_item`, `s_setup_card_type`, `s_setup_stake_type`, `s_setup_seed_char`, `s_setup_deck_page`, `s_collection_selected_item` |
| Setup L/R page nav | `src/menu.c:1180–1194` | L/R dec/inc `s_setup_deck_page`; CROSS steps one card, syncs page |
| Options L/R page nav | `src/menu.c:752–770` | `s_options_page` + page dots in `menu_draw_options()` |
| `hint_y` in options | `src/menu.c:810–814` | Declared + `menu_draw_separator()` + control-hint text |
| `hint_y` in setup | `src/menu.c:1143–1147` | Same pattern for setup screen |
| Collection draw | `src/menu.c:1267–1326` | `menu_draw_collection()` — grid + Back button |
| Collection input | `src/menu.c:1328–1352` | `menu_input_collection()` — Circle back, arrows nudge selection |
| Fwd declarations | `src/menu.c:855–856` | `menu_draw_setup(void)` + `menu_draw_collection(void)` |
| Collection dispatch | `src/game_input.c:1159` | `INPUT_FOCUSED_ZONE_MENU_COLLECTION` arm in `game_input_update()` |

---

## Open questions / next work

| # | Item | Owner |
|---|------|-------|
| 1. | **Collection screen is a nav stub** — selecting "Jokers" / "Tarots" etc. currently does Nothing (no detail panels) | — |
| 2. | **Sprite/icon assets for joker/tarot/planet/booster grid cells** — 4 placeholder buttons until art assets exist | — |
| 3. | **Options modal: Theme / Overclock / Hand Size / Discards / Slots** are marked `ON`/`OFF` but the toggle IDs in `game_input.c` still hit stub functions (1384–1416): the actual settings writes were never landed | Need toggle write-back, else no option is persistent |
| 4. | **Language modal** — language select grid + write to `g_settings.language` not yet implemented | — |
