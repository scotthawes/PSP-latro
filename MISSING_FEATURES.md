# PSP-latro: Missing Features vs Original Balatro v0.11

This document tracks features present in the original Balatro v0.11 (PSVita port, `game.love`) that are not yet implemented in PSP-latro.

---

## 1. Deck Selection (Backs)

The original has 15 unlockable decks, each with unique rule modifiers. PSP-latro uses only the default Red Deck with no selection screen.

| Deck | Effect | Unlock Condition |
|---|---|---|
| Red Deck | +1 Discard per round | Default |
| Blue Deck | +1 Hand per round | Discover 20 cards |
| Yellow Deck | +$10 at start | Discover 50 cards |
| Green Deck | +$2/hand played, +$1/discard, no interest | Discover 75 cards |
| Black Deck | +1 Joker slot, -1 Hand per round | Discover 100 cards |
| Magic Deck | Start with Crystal Ball voucher + 2 Tarot cards | Win with Red Deck |
| Nebula Deck | Start with Telescope voucher, -1 consumable slot | Win with Blue Deck |
| Ghost Deck | Spectral cards appear in shop (2x rate), start with The Hex | Win with Yellow Deck |
| Abandoned Deck | No face cards in deck | Win with Green Deck |
| Checkered Deck | 26 Spades + 26 Hearts | Win with Black Deck |
| Zodiac Deck | Start with Tarot Merchant, Planet Merchant, Overstock | Win Stake 2 |
| Painted Deck | +2 Hand size, -1 Joker slot | Win Stake 3 |
| Anaglyph Deck | After each Boss Blind, get a Double Tag | Win Stake 4 |
| Plasma Deck | Balances Chips and Mult, 2x Ante scaling | Win Stake 5 |
| Erratic Deck | All card ranks and suits are randomised | Win Stake 7 |

---

## 2. Vouchers

Vouchers are permanent upgrades purchased in the shop. Not implemented at all in PSP-latro.

| Voucher | Effect |
|---|---|
| Overstock | +1 card slot in shop |
| Overstock Plus | +1 additional card slot in shop |
| Clearance Sale | All cards and packs cost 25% less |
| Liquidation | All cards and packs cost 50% less |
| Hone | Foil, Holographic, Polychrome cards appear 2x more often |
| Glow Up | Foil, Holographic, Polychrome cards appear 4x more often |
| Reroll Surplus | Rerolls cost $1 less |
| Reroll Glut | Rerolls cost $2 less |
| Crystal Ball | +1 consumable slot |
| Omen Globe | Any standard pack can contain Spectral cards |
| Telescope | Celestial packs always contain the most used planet card |
| Observatory | Each planet card in collection gives X1.5 Mult to related hand |
| Grabber | +1 hand per round |
| Nacho Tong | +1 discard per round |
| Wasteful | +1 discard per round (stacks) |
| Tarot Merchant | Tarot cards appear 2x more often in shop |
| Tarot Tycoon | Tarot cards appear 4x more often in shop |
| Planet Merchant | Planet cards appear 2x more often in shop |
| Planet Tycoon | Planet cards appear 4x more often in shop |
| Seed Money | Max interest raised to $10 per round |
| Money Tree | Max interest raised to $20 per round |
| Blank | *(No effect — collector item)* |
| Magic Trick | Playing cards can appear in shop |
| Illusion | Playing cards in shop may have edition |
| Hieroglyph | -1 Ante, -1 Hand per round |
| Petroglyph | -1 Ante, -1 Discard per round |
| Directors Cut | One free reroll per round |
| Retcon | Reroll shop anytime (unlimited) |
| Paint Brush | +1 hand size |
| Palette | +1 hand size (stacks) |
| Antimatter | +1 Joker slot |

---

## 3. Spectral Cards

Spectral cards are a third consumable type (alongside Tarot and Planet). Not implemented in PSP-latro (the booster pack type exists but no individual cards are defined).

| Card | Effect |
|---|---|
| Familiar | Destroy 1 random card in hand, add 3 random Enhanced face cards |
| Grim | Destroy 1 random card in hand, add 2 random Enhanced Aces |
| Incantation | Destroy 1 random card in hand, add 4 random Enhanced numbered cards |
| Talisman | Add Gold Seal to 1 selected card |
| Aura | Add Foil/Holo/Polychrome edition to 1 selected Joker |
| Wraith | Creates a random Rare Joker, sets money to $0 |
| Sigil | Converts all cards in hand to a single random suit |
| Ouija | Converts all cards in hand to a single random rank, -1 Hand size |
| Ectoplasm | Adds Negative edition to a random Joker, -1 Joker slot |
| Immolate | Destroys 5 random cards in hand, gain $20 |
| Ankh | Copies a random Joker, destroys all other Jokers |
| Deja Vu | Add Red Seal to 1 selected card |
| Hex | Adds Polychrome to a random Joker, destroys all other Jokers |
| Trance | Add Blue Seal to 1 selected card |
| Medium | Add Purple Seal to 1 selected card |
| Cryptid | Creates 2 copies of 1 selected card |
| The Soul | Creates a Legendary Joker |
| Black Hole | Upgrades every poker hand by 1 level |

---

## 4. Challenges Mode

A separate game mode with 20 pre-set runs using fixed rules and starting conditions. Not implemented in PSP-latro.

| # | Challenge Name |
|---|---|
| 1 | The Omelette |
| 2 | 15 Minute City |
| 3 | Rich get Richer |
| 4 | On a Knife's Edge |
| 5 | X-ray Vision |
| 6 | Mad World |
| 7 | Luxury Tax |
| 8 | Non-Perishable |
| 9 | Medusa |
| 10 | Double or Nothing |
| 11 | Typecast |
| 12 | Inflation |
| 13 | Bram Poker |
| 14 | Fragile |
| 15 | Monolith |
| 16 | Blast Off |
| 17 | Five-Card Draw |
| 18 | Golden Needle |
| 19 | Cruelty |
| 20 | Jokerless |

---

## 5. Boss Blind Variety & Debuff System

The original has 30 distinct boss blinds, each with a unique visual and a special debuff rule that restricts gameplay (e.g. certain suits are debuffed, hands are face-down, only one hand type scores, etc.). PSP-latro has a single generic `GAME_BLIND_BOSS` type with no individual identities or debuff logic.

### Missing Boss Blinds (original identifiers)

| Boss Blind | Debuff / Effect |
|---|---|
| bl_small | Blind is worth only 1.5x |
| bl_big | Blind is worth 2x |
| bl_hook | Discards 2 random cards per hand played |
| bl_ox | Playing most-played hand sets $0 |
| bl_house | All face cards are debuffed |
| bl_wall | Extra-large blind |
| bl_wheel | 1 in 7 chance for any played card to be debuffed |
| bl_arm | Decreases level of played poker hand |
| bl_club | All Club cards are debuffed |
| bl_fish | Cards are drawn face-down |
| bl_psychic | Must play 5 cards |
| bl_goad | All Spade cards are debuffed |
| bl_water | Start with 0 discards |
| bl_window | All Diamond cards are debuffed |
| bl_manacle | -1 Hand size |
| bl_eye | No repeat hand types in the round |
| bl_mouth | Only 1 hand type can score |
| bl_plant | All face cards are debuffed |
| bl_serpent | After playing a hand, always draw 3 new cards (replaces draw logic) |
| bl_pillar | Cards played previously this ante are debuffed |
| bl_needle | Only 1 hand to play |
| bl_head | All Heart cards are debuffed |
| bl_tooth | Lose $1 per card played |
| bl_flint | Base Chips and Mult are halved |
| bl_mark | All face cards drawn face-down |
| bl_final_acorn | Debuffs all Jokers and flips them face-down |
| bl_final_leaf | All Jokers are debuffed |
| bl_final_vessel | All Jokers are debuffed |
| bl_final_heart | All active discards are disabled |
| bl_final_bell | -1 Hand size |

### Debuff System Absent

No card-debuffing mechanism exists in the PSP port. The boss blind debuff rules (suit restrictions, forced play counts, face-down draws, etc.) are fully absent. Implementing boss blind variety requires first building a per-card `debuffed` flag and checking it throughout the scoring pipeline.

---

## 6. Tags System

In the original, skipping a Small or Large blind awards a **Tag** — a one-time bonus applied in the shop or next run. PSP-latro has no tag system at all: no skip-to-earn mechanic and no tag types defined.

### All 24 Tags

| Tag | Effect |
|---|---|
| tag_uncommon | Free Uncommon Joker |
| tag_rare | Free Rare Joker |
| tag_negative | Add Negative edition to a random Joker |
| tag_foil | Add Foil edition to next shop Joker |
| tag_holo | Add Holographic edition to next shop Joker |
| tag_polychrome | Add Polychrome edition to next shop Joker |
| tag_investment | After defeating Boss Blind, earn $25 |
| tag_voucher | Gain a free Voucher this shop |
| tag_boss | Immediately re-roll the current Boss Blind |
| tag_standard | Gain a free Standard Pack |
| tag_charm | Gain a free Arcana Pack |
| tag_meteor | Gain a free Celestial Pack |
| tag_buffoon | Gain a free Buffoon Pack |
| tag_handy | Earn $1 per hand played this run so far |
| tag_garbage | Earn $1 per unused discard this run so far |
| tag_ethereal | Gain a free Spectral Pack |
| tag_coupon | Free reroll in the next shop |
| tag_double | Duplicate next Tag |
| tag_juggle | +3 Hand size for next round only |
| tag_d_six | Gain a free D6 (randomiser die) |
| tag_top_up | Fill empty Joker slots with Commons |
| tag_skip | Gain $1 per card in deck after winning this blind |
| tag_orbital | Upgrade one random poker hand by 3 levels |
| tag_economy | Cash out everything you currently own |

---

## 7. Stake / Difficulty System

The original has 8 Stakes (White through Gold) that increase difficulty with modifiers such as eternal jokers, perishable jokers, reduced shop discounts, and ante scaling. PSP-latro has no stake system — every run plays at the same fixed difficulty.

| Stake | Key Modifiers |
|---|---|
| White Stake | Baseline |
| Red Stake | Eternal Jokers can appear |
| Green Stake | Perishable Jokers can appear |
| Black Stake | Rental Jokers can appear |
| Blue Stake | Debuffed Jokers at game over |
| Purple Stake | Reduced shop discounts |
| Orange Stake | Increased Ante score requirements |
| Gold Stake | All modifiers combined |

---

## 8. Save & Unlock Progression

The original tracks a persistent profile across runs, storing:
- Joker/card discovery progress
- Deck unlock status
- Stake unlock status per deck
- Run history / win count

PSP-latro has no save system. `settings.ini` only stores gameplay configuration; nothing is persisted between runs. As a result, all unlock conditions (decks, challenges) cannot function even if those features were added.

---

## 9. Partially Implemented Jokers

Approximately 55 jokers are defined in `src/game.c` but marked `// NOT IMPLEMENTED` — they appear in the pool with placeholder names (`"name"`) and descriptions (`"test"`), and their scoring logic in the callbacks is absent or stubbed. These jokers can appear in shops but have no effect.

| Joker | Notes |
|---|---|
| Four Fingers | Flushes/Straights with 4 cards |
| Loyalty Card | x4 Mult every 6 hands |
| 8 Ball | 1-in-4 chance to create Tarot from 8s |
| Steel Joker | xMult per Steel Card in deck |
| Delayed Gratification | $2 per discard if none used |
| Blackboard | xMult if all held cards are Spades/Clubs |
| Runner | Chips per consecutive Straight |
| Sixth Sense | Destroy a 6 to create a Spectral |
| Constellation | xMult each time a Planet is used |
| Card Sharp | xMult for repeated poker hands |
| Red Card | +3 Mult when skipping a pack |
| Madness | xMult when Blind selected, destroys a Joker |
| Square Joker | +4 Chips when hand is exactly 4 cards |
| Seance | Creates a Spectral if hand is a Straight Flush |
| Vampire | -0.1 per scoring enhanced card; +xMult |
| Shortcut | Straights with gaps allowed |
| Hologram | +0.25 xMult per card added to deck |
| Cloud 9 | $1 per 9 in full deck at end of round |
| Obelisk | xMult per consecutive non-most-played hand |
| Luchador | Sell to disable Boss Blind effect |
| Gift Card | +$1 sell value to random Joker/Consumable |
| Turtle Bean | +5 hand size, shrinks each round |
| Erosion | +4 Mult per missing card vs starting deck |
| Reserved Parking | 1 in 2 chance of $1 per face card held |
| Mail In Rebate | Earn $3 for discarding the listed rank |
| Hallucination | 1 in 2 chance to create a Tarot when opening a pack |
| Fortune Teller | +1 Mult per Tarot used this run |
| Stone Joker | +25 Chips per Stone Card in full deck |
| Baseball Card | Uncommon Jokers each give x1.5 Mult |
| Diet Cola | Sell to create a free Buffoon Pack |
| Trading Card | Sell to earn $3 if first discard of round |
| Flash Card | +2 Mult per reroll in current shop |
| Campfire | xMult per card sold; resets after Boss Blind |
| Mr. Bones | Prevents death if score >= 25% of blind |
| Seltzer | Retrigger all cards for next 10 hands |
| Castle | +3 Chips per discarded suit (accumulates) |
| Ramen | +2 Mult; loses 0.01 per discarded card |
| Glass Joker | +xMult per Glass Card destroyed |
| Showman | Joker, Tarot, Planet, Spectral may repeat in shop |
| Flowerpot | x3 Mult if all 4 suits in hand |
| Wee Joker | +8 Chips when a 2 is scored |
| The Idol | x2 Mult if specific card is played |
| Seeing Double | x2 Mult if hand has Club + another suit |
| Matador | $8 if hand triggers Boss Blind ability |
| Hit the Road | x0.5 Mult per Jack discarded this round |
| Invisible Joker | After 2 rounds, duplicate a random Joker |
| Satellite | $1 per unique Planet used this run |
| Shoot the Moon | +13 Mult per Queen held in hand |
| Driver's License | x3 Mult if 16+ enhanced cards in deck |
| Cartomancer | Creates a Tarot when Blind is selected |
| Astronomer | Celestial Packs and Planet cards are free |
| Burnt Joker | Upgrade most-played hand; costs a discard |
| Bootstraps | +2 Mult per $5 you have |
| Canio | xMult each time a card is destroyed |
| Triboulet | x2 Mult per scored King or Queen |
| Yorick | xMult every 23 cards discarded |
| Chicot | Disables Boss Blind effect |
| Perkeo | Creates a Negative copy of every Consumable at end of shop |
| Egg | Gains $3 sell value at end of round |

---

## 10. Joker Name Typo

`JOKER_TYPE_PARKEO` in `src/global.h` should be `JOKER_TYPE_PERKEO` to match the original joker name "Perkeo" (`j_perkeo`). This is a cosmetic issue and does not affect gameplay logic, but should be corrected for consistency.

---

## Summary

| Feature | Status |
|---|---|
| Jokers (150 defined) | ⚠️ ~95 implemented, ~55 stubbed (`// NOT IMPLEMENTED`) |
| Tarot cards (22) | ✅ Complete |
| Planet cards (11) | ✅ Complete |
| Card editions (Foil/Holo/Poly/Negative) | ✅ Complete |
| Card enhancements | ✅ Complete |
| Card seals | ✅ Complete |
| Booster packs | ✅ Complete |
| Spectral cards (18) | ❌ Not implemented (pack type only) |
| Vouchers (31) | ❌ Not implemented |
| Deck selection (15 decks) | ❌ Not implemented |
| Challenges mode (20 challenges) | ❌ Not implemented |
| Boss blind variety (30 blinds) | ❌ Not implemented (generic boss only) |
| Boss blind debuff system | ❌ Not implemented |
| Tags system (24 tags) | ❌ Not implemented |
| Stake / difficulty system (8 stakes) | ❌ Not implemented |
| Save & unlock progression | ❌ Not implemented |
| Joker name typo (PARKEO → PERKEO) | ⚠️ Cosmetic fix needed |
