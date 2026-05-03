# PSP-latro: Round 2 Improvements

A fresh audit of the codebase following completion of all Round 1 items. Covers three areas: bug fixes, code-quality hygiene, and UI/data correctness.

**Status: §1.1-§2.3 IMPLEMENTED ✅** | Branch: `feature/round2-bug-fixes`

---

## Table of Contents

1. [Bug Fixes](#1-bug-fixes)
   - [x] 1.1 [Audio Underrun Outputs Garbage Samples](#11-audio-underrun-outputs-garbage-samples)
   - [x] 1.2 [`graphics_destroy_texture` Miscounts Bytes for 16-bit Textures](#12-graphics_destroy_texture-miscounts-bytes-for-16-bit-textures)
   - [x] 1.3 [`audio_load_ogg` Does Not Check `fopen` Return Value](#13-audio_load_ogg-does-not-check-fopen-return-value)
   - [x] 1.4 [Buffer Overflow in `strcpy` for `archive_file_name`](#14-buffer-overflow-in-strcpy-for-archive_file_name)
   - [x] 1.5 [Joker Slot Accounting Missing NEGATIVE Joker Bonus](#15-joker-slot-accounting-missing-negative-joker-bonus)

2. [Code Quality / Hygiene](#2-code-quality--hygiene)
   - [x] 2.1 [Remove Commented-Out Interpolation Block in `audio_update`](#21-remove-commented-out-interpolation-block-in-audio_update)
   - [x] 2.2 [`temp_buffer` Lacks `static` Linkage](#22-temp_buffer-lacks-static-linkage)
   - [x] 2.3 [`g_freeze_cards` Lacks `static` Linkage](#23-g_freeze_cards-lacks-static-linkage)
   - [ ] 2.4 [Unimplemented TODO: Spectral Booster Pack Handling](#24-unimplemented-todo-spectral-booster-pack-handling)
   - [ ] 2.5 [Unimplemented TODO: Fallback Joker Selection](#25-unimplemented-todo-fallback-joker-selection)

3. [UI / Data Fixes](#3-ui--data-fixes)
   - [ ] 3.1 ["Troubadour" Misspelled as "Troubador"](#31-troubadour-misspelled-as-troubador)
   - [ ] 3.2 [Wrathful / Gluttonous Joker Hints Missing Suit Color Codes](#32-wrathful--gluttonous-joker-hints-missing-suit-color-codes)
   - [ ] 3.3 [The World Tarot Hint Missing Color Code for Spades](#33-the-world-tarot-hint-missing-color-code-for-spades)
   - [ ] 3.4 [Unimplemented Jokers Show "test" as In-Game Hint Text](#34-unimplemented-jokers-show-test-as-in-game-hint-text)
   - [ ] 3.5 [Polychrome Edition Has No Visual Overlay](#35-polychrome-edition-has-no-visual-overlay)
   - [ ] 3.6 [Card Seals Are Never Drawn](#36-card-seals-are-never-drawn)
   - [ ] 3.7 [Card Hint Does Not Show Seal Information](#37-card-hint-does-not-show-seal-information)
   - [ ] 3.8 [JOKER_TYPE_PARKEO Enum Misspelled (Should Be PERKEO)](#38-joker_type_parkeo-enum-misspelled-should-be-perkeo)
   - [ ] 3.9 [Negative Edition Hint Missing Effect Description](#39-negative-edition-hint-missing-effect-description)

4. [Summary Table](#4-summary-table)

---

## 1. Bug Fixes

### 1.1 Audio Underrun Outputs Garbage Samples

**File:** `src/audio.c`, `audio_callback()`  
**Priority:** High | **Difficulty:** Trivial  

```c
// Current — when the decode thread hasn't filled the next chunk yet:
if (g_audio_buffer.written > 0)
{
    memcpy((void*)buf, ...);
    ...
}
else
{
    g_debug_info.audio_wait_read++;
    // BUG: buf is never written — hardware gets whatever was previously at that address
}
```

When `written == 0` and an OGG is active, `buf` is left with uninitialised / previously-used memory. The PSP audio hardware plays that memory verbatim, producing a loud click or static burst. The `ogg_id == -1` branch correctly calls `memset(buf, 0, ...)` but the underrun branch does not.

**Fix:** Add a `memset` in the underrun path:

```c
else
{
    g_debug_info.audio_wait_read++;
    memset(buf, 0, AUDIO_BUFFER_SIZE);   /* output silence on underrun */
}
```

---

### 1.2 `graphics_destroy_texture` Miscounts Bytes for 16-bit Textures

**File:** `src/graphics.c`, `graphics_destroy_texture()`  
**Priority:** Low | **Difficulty:** Low  

```c
// Always subtracts 4 bytes per pixel regardless of format:
g_allocated_graphic_bytes -= g_textures[texture].width * g_textures[texture].height * 4;
```

All card/joker/tarot textures are loaded as 16-bit (GU_PSM_4444, 2 bytes per pixel) via `graphics_load_texture_from_image_16bit`, but the destruction path always subtracts `w*h*4`. Each destroyed 16-bit texture causes `g_allocated_graphic_bytes` to undercount by `w*h*2`, making the counter go meaninglessly negative over time.

`g_allocated_graphic_bytes` is only used in `DEBUG_PRINTF` calls, so this is a debug-only correctness issue, but it makes the counter untrustworthy during development.

**Fix:** Store bytes-per-pixel in `g_textures` and use it in the destructor:

```c
// In the Texture struct, add:
int bytes_per_pixel;   // 4 for GU_PSM_8888, 2 for GU_PSM_4444

// In graphics_load_texture_from_image():
g_textures[texture_slot].bytes_per_pixel = 4;

// In graphics_load_texture_from_image_16bit():
g_textures[texture_slot].bytes_per_pixel = 2;

// In graphics_destroy_texture():
g_allocated_graphic_bytes -= g_textures[texture].width
                           * g_textures[texture].height
                           * g_textures[texture].bytes_per_pixel;
```

---

### 1.3 `audio_load_ogg` Does Not Check `fopen` Return Value

**File:** `src/audio.c`, `audio_load_ogg()`  
**Priority:** Medium | **Difficulty:** Trivial  

```c
FILE *fp_ogg = fopen(filename, "rb");
fseek(fp_ogg, 0, SEEK_END);   // crash if fp_ogg is NULL
```

If the OGG file is absent from the memory stick (e.g. custom VPK without audio), `fopen` returns `NULL` and the very next `fseek` call dereferences a null pointer, crashing the PSP. The archive-based loader `audio_load_ogg_from_archive` doesn't have this path (it delegates to `archive_load_file_entry`), but the direct-file variant is unguarded.

**Fix:**

```c
FILE *fp_ogg = fopen(filename, "rb");
if (fp_ogg == NULL)
{
    DEBUG_PRINTF("audio_load_ogg: could not open \"%s\"\n", filename);
    return -1;
}
```

Change the return type of `audio_load_ogg` from `int` (currently always returns `0`) to meaningful error codes; the caller should already handle `< 0`.

---

### 1.4 Buffer Overflow in `strcpy` for `archive_file_name`

**File:** `src/game.c`, `game_load_settings()`, line ~1487  
**Priority:** High | **Difficulty:** Trivial  

```c
// Current — no bounds checking:
token_type = ini_read_token(buffer, 128);
if (token_type != INI_TOKEN_VALUE) return false;
strcpy(g_settings.archive_file_name, buffer);  // both are 128 bytes
```

Both `buffer` and `g_settings.archive_file_name` are 128 bytes. If `ini_read_token()` reads exactly 128 bytes, the result buffer will not be null-terminated, and `strcpy` will overflow the destination when it tries to write the null terminator.

**Fix:** Use `strncpy` with length limit to ensure null-termination:

```c
token_type = ini_read_token(buffer, 128);
if (token_type != INI_TOKEN_VALUE) return false;
strncpy(g_settings.archive_file_name, buffer, 127);
g_settings.archive_file_name[127] = '\0';
```

---

### 1.5 Joker Slot Accounting Missing NEGATIVE Joker Bonus

**File:** `src/game_util.c`, `game_util_has_room_in_jokers()` and `game_util_can_tarot_be_used()`, lines ~1035–1047  
**Priority:** Medium | **Difficulty:** Low  

```c
// Current — ignores NEGATIVE joker +1 slot:
bool game_util_has_room_in_jokers()
{
    // TODO: Account for negative jokers
    return g_game_state.jokers.joker_count < g_game_state.joker_slots;
}

bool game_util_can_tarot_be_used(int type)
{
    if (type == TAROT_TYPE_JUDGEMENT && !game_util_has_room_in_jokers()) return false; // TODO: Account for negatives
    ...
}
```

NEGATIVE edition jokers grant an additional +1 joker slot. The slot-checking logic only counts `g_game_state.joker_slots`, which is the base/configurable count. Each NEGATIVE joker in the collection should add +1 to the effective joker slot limit, but this bonus is not factored in.

This means:
- If you have 5 base joker slots and 2 NEGATIVE jokers, you should have room for 7 jokers, but the system thinks you only have 5.
- JUDGEMENT tarot may incorrectly fail to create a joker when you actually have room (because of NEGATIVE slot bonuses).

**Fix:** Update the function to count NEGATIVE jokers:

```c
bool game_util_has_room_in_jokers()
{
    int effective_slots = g_game_state.joker_slots + g_game_state.jokers.negative_count;
    return g_game_state.jokers.joker_count < effective_slots;
}
```

---

## 2. Code Quality / Hygiene

### 2.1 Remove Commented-Out Interpolation Block in `audio_update`

**File:** `src/audio.c`, `audio_update()`, lines ~95–111  
**Priority:** Low | **Difficulty:** Trivial  

A large block of linear-interpolation resampling code is commented out with its own commentary still visible:

```c
// index = floor(i_src);
// index2 = index + 1;
// if (index2 >= size)
// ...
//     float f = i_src - (float)index;
//     dst[i].l = src[index].l * (1.0f - f) + src[index2].l * f;
// ...
```

This was superseded by nearest-neighbour rounding (`roundf(i_src)`). The commented-out block adds ~18 lines of visual noise with no functional purpose. Remove it.

> **Note:** Linear interpolation would give slightly better audio quality (reduced aliasing on pitch-shifted playback). If that is ever desired it should be introduced as its own branch/commit, not left as dead comment. The dedicated audio thread (§1.6 from Round 1) now has CPU headroom to run it with no impact on the main thread.

---

### 2.2 `temp_buffer` Lacks `static` Linkage

**File:** `src/audio.c`, line ~78  
**Priority:** Low | **Difficulty:** Trivial  

```c
char temp_buffer[AUDIO_BUFFER_SIZE];   // file-scope, external linkage
```

`temp_buffer` is only ever used inside `audio_update()`. Declaring it at file scope without `static` exports it into the global namespace, where it could silently shadow or collide with any `temp_buffer` in another translation unit (matters in the unity build where all `.c` files are `#include`-d into `main.c`).

**Fix:**

```c
static char temp_buffer[AUDIO_BUFFER_SIZE];
```

---

### 2.3 `g_freeze_cards` Lacks `static` Linkage

**File:** `src/draw.c`, line ~406  
**Priority:** Low | **Difficulty:** Trivial  

```c
bool g_freeze_cards = false;   // file-scope, external linkage
```

`g_freeze_cards` is set and read exclusively within `draw.c` (toggled before/after animated card scoring, read in `game_draw_get_oscillating_position`). It is not declared in `global.h`, so no other translation unit uses it. The missing `static` exposes it as an unintended export.

**Fix:**

```c
static bool g_freeze_cards = false;
```

---

### 2.4 Unimplemented TODO: Spectral Booster Pack Handling

**File:** `src/game.c`, `game_buy_booster_item()`, line ~1119  
**Priority:** Medium | **Difficulty:** Medium  

```c
case BOOSTER_PACK_TYPE_SPECTRAL:
{
    // TODO:
    break;
}
```

Spectral booster packs are partially defined (enum exists in booster pack type table), but the shop initialization logic for them is completely empty. This means if a spectral pack is encountered, no items will be generated for it — the shop will show 0 booster cards to select from.

**Fix:** Implement spectral card selection logic. In Balatro v0.11, spectral packs offer special cards from the spectral card pool (Ghost cards like Spectral Card, Soul Card, etc.). Implementation requires:
1. Identifying which card types belong to spectral pool
2. Random selection from that pool  
3. Appropriate item count (typically 3–5 cards per pack)

**Reference:** Check `third_party/balatro-v011-reference/` for spectral card definitions.

---

### 2.5 Unimplemented TODO: Fallback Joker Selection

**File:** `src/game_util.c`, `game_util_get_new_joker_type()`, line ~590  
**Priority:** Medium | **Difficulty:** Low  

```c
// Current — returns 0 (JOKER_TYPE_NONE) when no suitable joker available:
// TODO : add repeated jokers if possible_joker_count == 0
if (possible_joker_count == 0) return 0;

return possible_jokers[rand()%possible_joker_count];
```

If no "new" unallocated joker is available (e.g., all eligible jokers have already been offered or are locked), the function returns 0 (JOKER_TYPE_NONE), which likely causes UI/logic errors or undefined behavior in calling code. The TODO comment suggests falling back to allowing repeats when the pool is exhausted.

**Fix:** When no unique joker is available, fall back to selecting from all joker types:

```c
if (possible_joker_count == 0) {
    // Fallback: allow repeated jokers if pool exhausted
    return rand() % JOKER_TYPE_COUNT;
}
return possible_jokers[rand()%possible_joker_count];
```

---

## 3. UI / Data Fixes

### 3.1 "Troubadour" Misspelled as "Troubador"

**File:** `src/game.c`, joker type table, line ~157  
**Priority:** Low | **Difficulty:** Trivial  

```c
{ JOKER_TYPE_TROUBADOUR, "Troubador", ...   // misspelled
```

The enum constant is `JOKER_TYPE_TROUBADOUR` (correct), but the display name used in-game and in hint popups is `"Troubador"` — missing the final `u`. The correct Balatro card name is **Troubadour**.

**Fix:** Change `"Troubador"` → `"Troubadour"`.

---

### 3.2 Wrathful / Gluttonous Joker Hints Missing Suit Color Codes

**File:** `src/game.c`, joker type table, lines ~50–51  
**Priority:** Medium | **Difficulty:** Trivial  

The four suit-Mult jokers should use the text formatting system to highlight the suit name. Greedy and Lusty are correct; Wrathful and Gluttonous are not:

| Joker | Current hint line | Should be |
|---|---|---|
| Greedy | `"Played cards with #5Diamond#-"` | ✅ correct |
| Lusty | `"Played cards with #3Heart#-"` | ✅ correct |
| Wrathful | `"Played cards with Spade"` | `"Played cards with #5Spade#-"` |
| Gluttonous | `"Played cards with Club"` | `"Played cards with #5Club#-"` |

The missing codes mean "Spade" and "Club" render in plain black text instead of the styled colour used for all other suit/card-type references.

---

### 3.3 The World Tarot Hint Missing Color Code for Spades

**File:** `src/game.c`, tarot type table, line ~236  
**Priority:** Low | **Difficulty:** Trivial  

```c
{ TAROT_TYPE_WORLD, "The World", ..., { "Converts up to #53#-", "selected cards", "to #Spades#-", "" } }
//                                                                                   ^ missing digit
```

`"#Spades#-"` is missing the colour index (should be `"#5Spades#-"`). Without the index the formatting parser will likely render it as plain text or produce an artefact character instead of the styled suit name.

**Fix:** Change `"to #Spades#-"` → `"to #5Spades#-"`.

---

### 3.4 Unimplemented Jokers Show "test" as In-Game Hint Text

**File:** `src/game.c`, joker type table  
**Priority:** Medium | **Difficulty:** Low  

Approximately 30 jokers marked `// NOT IMPLEMENTED` still carry `"test"` as all three hint lines, e.g.:

```c
{ JOKER_TYPE_EGG, "Egg", 4, JOKER_RARITY_COMMON, false, 0, 10,
  { "test", "test", "test", "", "", ""}}, // NOT IMPLEMENTED
```

These jokers **do** appear in the shop and can be purchased. A player who buys one sees `"test / test / test"` as the tooltip, which is obviously a placeholder artifact.

**Fix:** Replace the three `"test"` lines with the actual Balatro hint text for each joker. This is a data-entry task (not a code task) — the reference text is available in `third_party/balatro-v011-reference/game_love_extracted/localization/en-us.lua`. Even if the joker effect is not yet implemented, accurate hint text gives players the correct expectation and keeps the UI professional.

Affected jokers (representative sample):

| Joker | Correct hint (from reference) |
|---|---|
| Egg | "Gains $3 of sell value at end of round" |
| Runner | "Gains +15 Chips if played hand has a Straight" |
| Blackboard | "×3 Mult if all cards held in hand are Spades or Clubs" |
| Castle | "This Joker gains +3 Chips each time a discarded card matches the current suit…" |
| Red Card | "This Joker gains +3 Mult when any booster pack is skipped" |
| … | … |

---

### 3.5 Polychrome Edition Has No Visual Overlay

**File:** `src/draw.c`, `g_editions_tex_coords[]`, line ~83  
**Priority:** Medium | **Difficulty:** Low  

```c
g_editions_tex_coords[CARD_EDITION_COUNT] = {
    { 1,   1 },   // CARD_EDITION_BASE        (never drawn)
    { 143, 1 },   // CARD_EDITION_FOIL        ✅
    { 72,  1 },   // CARD_EDITION_HOLOGRAPHIC ✅
    { 1,   1 },   // CARD_EDITION_POLYCHROME  ← same as BASE: nothing visible
    { 1,   1 },   // CARD_EDITION_NEGATIVE    (handled separately via GU_COPY_INVERTED)
};
```

`CARD_EDITION_NEGATIVE` is intentionally excluded from the editions-overlay draw path (the joker draw code uses `GU_COPY_INVERTED` instead). But `CARD_EDITION_POLYCHROME` falls through to the editions draw path and submits coordinates `{1,1}` — which is the top-left corner of the editions texture, likely a blank or near-blank region. Polychrome cards therefore look identical to base cards rather than showing the rainbow shimmer overlay.

**Fix:** Identify the correct UV coordinates for the Polychrome pattern within `editions.png` and set them in the table. If `editions.png` doesn't contain a Polychrome pattern, one should be authored.

---

### 3.6 Card Seals Are Never Drawn

**File:** `src/draw.c`, `game_draw_card()`  
**Priority:** Medium | **Difficulty:** Medium  

The `Card` struct has a `seal` field (`CARD_SEAL_NONE / GOLD / RED / BLUE / PURPLE`) and game logic sets seals during play (e.g. from Tarot cards). However `game_draw_card()` never reads `card->seal` — no seal overlay is ever rendered. Players who acquire sealed cards see no visual distinction on the card face.

In Balatro the seal appears as a small coloured wax-stamp icon at the bottom of the card, sourced from the Enhancers texture sheet.

**Fix:** Identify the seal sprite coordinates within the Enhancers texture (or add them to a `ui_assets.png` slot), then draw a small overlay at the bottom of the card in `game_draw_card()` when `card->seal != CARD_SEAL_NONE`. A colour-tinted version of a single stamp sprite is sufficient.

---

### 3.7 Card Hint Does Not Show Seal Information

**File:** `src/draw.c`, `game_draw_card_hint()`  
**Priority:** Low | **Difficulty:** Low  

`game_draw_card_hint()` already shows enhancement and edition details, but does not account for seals at all:

```c
int line_count = 1;                                         // rank+chips line
if (card->extra_chips > 0)         line_count++;            // extra chips
if (card->enhancement != ...)      line_count++;            // enhancement
if (card->edition != ...)          line_count++;            // edition
// seal: never checked
```

If a card has a Gold/Red/Blue/Purple seal, its effect is invisible to the player unless they already know it from memory. This is particularly confusing for the Gold seal (earn $3 at end of round), where there is no other indicator.

**Fix:** Add a seal line to the hint when `card->seal != CARD_SEAL_NONE`, similarly to how enhancement/edition are handled. Requires adding `g_seal_hint[CARD_SEAL_COUNT]` strings in `game.c` (mirror the pattern of `g_enhancement_hint` and `g_edition_hint`).

---

### 3.8 JOKER_TYPE_PARKEO Enum Misspelled (Should Be PERKEO)

**File:** `src/game.c`, joker type enum and table  
**Priority:** Low | **Difficulty:** Trivial  

```c
{ JOKER_TYPE_PARKEO, "Perkeo", ..., { "Creates a Tarot card", "when Boss Blind is defeated", "" } }
```

The enum constant is `JOKER_TYPE_PARKEO` but the correct spelling (matching the display name) is **Perkeo** (a character from Heidelberg mythology). This is a common misspelling that affects code readability and documentation.

This was noted in `MISSING_FEATURES.md §11` but not yet fixed.

**Fix:** Rename the enum constant: `JOKER_TYPE_PARKEO` → `JOKER_TYPE_PERKEO`.

---

### 3.9 Negative Edition Hint Missing Effect Description

**File:** `src/game.c`, edition hints table  
**Priority:** Low | **Difficulty:** Trivial  

```c
g_edition_hint[CARD_EDITION_NEGATIVE] = "#d#1Negative#-";   // ← no effect description
```

The Negative edition increases the Joker limit by +1 slot. This is a significant gameplay effect, but the hint only shows the name with no description of the bonus. Compare to other editions:

| Edition | Current hint | Should include | 
|---|---|---|
| Foil | `"#4#1Foil#- +2 Chips"` | ✅ effect |
| Holographic | `"#8#1Holographic#- +2 Mult"` | ✅ effect |
| Polychrome | `"#5#1Polychrome#- ×1.5 Mult"` | ✅ effect |
| Negative | `"#d#1Negative#-"` | ❌ missing effect |

**Fix:** Add the effect description to the Negative hint:

```c
g_edition_hint[CARD_EDITION_NEGATIVE] = "#d#1Negative#- +1 Joker Slot";
```

---

## 4. Summary Table

| § | Title | Priority | Difficulty | File(s) |
|---|---|---|---|---|
| 1.1 | Audio underrun outputs garbage | **High** | Trivial | `audio.c` |
| 1.2 | `destroy_texture` byte count wrong for 16-bit | Low | Low | `graphics.c` |
| 1.3 | `audio_load_ogg` unchecked `fopen` | Medium | Trivial | `audio.c` |
| 1.4 | Buffer overflow in `strcpy` for `archive_file_name` | **High** | Trivial | `game.c` |
| 1.5 | Joker slot accounting missing NEGATIVE bonus | Medium | Low | `game_util.c` |
| 2.1 | Remove commented-out interpolation block | Low | Trivial | `audio.c` |
| 2.2 | `temp_buffer` needs `static` | Low | Trivial | `audio.c` |
| 2.3 | `g_freeze_cards` needs `static` | Low | Trivial | `draw.c` |
| 2.4 | Spectral booster pack handling unimplemented | Medium | Medium | `game.c` |
| 2.5 | Fallback joker selection unimplemented | Medium | Low | `game_util.c` |
| 3.1 | "Troubador" → "Troubadour" | Low | Trivial | `game.c` |
| 3.2 | Wrathful/Gluttonous missing suit color codes | Medium | Trivial | `game.c` |
| 3.3 | The World tarot missing `#5` code | Low | Trivial | `game.c` |
| 3.4 | "test" placeholder hints on ~30 jokers | Medium | Low | `game.c` |
| 3.5 | Polychrome edition no visual overlay | Medium | Low | `draw.c`, `editions.png` |
| 3.6 | Card seals never drawn | Medium | Medium | `draw.c` |
| 3.7 | Card hint doesn't show seal | Low | Low | `draw.c`, `game.c` |
| 3.8 | JOKER_TYPE_PARKEO → JOKER_TYPE_PERKEO | Low | Trivial | `game.c` |
| 3.9 | Negative edition hint missing effect | Low | Trivial | `game.c` |
