#include "global.h"
#include "graphics_effects.h"
#include <math.h>

/* Flash overlay state: lives in graphics_effects.c so gfx_effect_trigger_flash()
 * and gfx_effect_update_flash() have a single definition point in the unity
 * build, and global.h holds only the extern declaration. */
struct FlashState g_flash_state = { false, 0.0f };

/* Unity build: shared math helpers must appear before shader includes
 * so every static helper is defined before any shader file calls it. */
static inline float gfx_hue(float s, float t, float h)
{
    float hs = fmodf(h, 1.0f) * 6.0f;
    if (hs < 1.0f) return (t - s) * hs + s;
    if (hs < 3.0f) return t;
    if (hs < 4.0f) return (t - s) * (4.0f - hs) + s;
    return s;
}

static inline void gfx_HSL_to_RGB(const float *c, float *r, float *g, float *b)
{
    float t = (c[2] < 0.5f) ? c[1] * c[2] + c[2] : -c[1] * c[2] + (c[1] + c[2]);
    float s  = 2.0f * c[2] - t;
    *r = gfx_hue(s, t, c[0] + 1.0f / 3.0f);
    *g = gfx_hue(s, t, c[0]);
    *b = gfx_hue(s, t, c[0] - 1.0f / 3.0f);
    (void)c;
}

static inline void gfx_RGB_to_HSL(uint8_t r8, uint8_t g8, uint8_t b8,
                                    float *H, float *S, float *L)
{
    float rf = r8 / 255.0f;
    float gf = g8 / 255.0f;
    float bf = b8 / 255.0f;
    float low  = MIN(rf, MIN(gf, bf));
    float high = MAX(rf, MAX(gf, bf));
    float delta = high - low;
    float sum   = high + low;

    *L = 0.5f * sum;
    if (delta < 0.0001f) { *H = 0.0f; *S = 0.0f; return; }

    *S = (*L < 0.5f) ? (delta / sum) : (delta / (2.0f - sum));
    if      (high == rf) *H = (gf - bf) / delta;
    else if (high == gf) *H = (bf - rf) / delta + 2.0f;
    else                  *H = (rf - gf) / delta + 4.0f;
    *H = fmodf(*H / 6.0f, 1.0f);
    if (*H < 0.0f) *H += 1.0f;
}

/* Unity build: include each per-shader translation unit so the linker
 * finds all gfx_apply_*() symbols from a single main.c compilation. */
#include "shaders/flash.c"
#include "shaders/background.c"
#include "shaders/foil.c"
#include "shaders/hologram.c"
#include "shaders/negative.c"
#include "shaders/polychrome.c"
#include "shaders/played.c"
#include "shaders/dissolve.c"

/**
 * Apply an edition effect to @p pixels buffer in-place.
 * Builds GfxEffectParams from edition+seed+time, dispatches via
 * graphics_effect_apply() to the per-shader functions in src/shaders/.
 * @return 0 on success, -1 on error
 */
int graphics_apply_edition_effect_inplace(uint8_t *pixels, int width, int height,
                                          int edition, float card_seed, float time)
{
    if (pixels == NULL || width <= 0 || height <= 0) return -1;

    GfxEffectParams params = graphics_build_edition_params_realtime(edition, card_seed, time);
    return graphics_effect_apply(pixels, width, height, &params);
}

/* ------------------------------------------------------------------ */
/*  GfxEffectParams builder                                            */
/* ------------------------------------------------------------------ */

GfxEffectParams graphics_build_edition_params(int edition, float card_seed)
{
    GfxEffectParams p = {0};
    p.time = 0.0f;

    switch (edition)
    {
        case GFX_EFFECT_FOIL:
            p.effect        = GFX_EFFECT_FOIL;
            p.foil_param_r  = card_seed;
            p.foil_param_g  = card_seed * 0.73f;
            break;

        case GFX_EFFECT_HOLOGRAPHIC:
            p.effect        = GFX_EFFECT_HOLOGRAPHIC;
            p.holo_param_r  = card_seed;
            p.holo_param_g  = card_seed * 0.8f;
            break;

        case GFX_EFFECT_POLYCHROME:
            p.effect        = GFX_EFFECT_POLYCHROME;
            p.polychrome_param_r = card_seed;
            p.polychrome_param_g = card_seed * 0.7f;
            break;

        case GFX_EFFECT_NEGATIVE:
            p.effect            = GFX_EFFECT_NEGATIVE;
            p.negative_param_r  = card_seed; /* sign flip: card_seed > 0 → invert G */
            break;

        case GFX_EFFECT_GOLD_SEAL:
            p.effect           = GFX_EFFECT_GOLD_SEAL;
            p.gold_seal_param_r = card_seed;
            break;

        case GFX_EFFECT_VOUCHER:
            p.effect         = GFX_EFFECT_VOUCHER;
            p.voucher_param_r = card_seed;
            break;

        default:
            p.effect = GFX_EFFECT_NONE;
            break;
    }

    return p;
}

GfxEffectParams graphics_build_edition_params_realtime(int edition, float card_seed, float game_time)
{
    GfxEffectParams p = graphics_build_edition_params(edition, card_seed);
    if (p.effect != GFX_EFFECT_NONE)
        p.time = game_time;
    return p;
}

/* ------------------------------------------------------------------ */
/*  Dispatch table — implementation functions live in src/shaders/     */
/* ------------------------------------------------------------------ */

int graphics_effect_apply(uint8_t *pixels, int width, int height,
                          const GfxEffectParams *params)
{
    if (pixels == NULL || params == NULL) return -1;

    switch (params->effect)
    {
        case GFX_EFFECT_FLASH:         return gfx_apply_flash(pixels, width, height, params);
        case GFX_EFFECT_BACKGROUND:    return gfx_apply_background(pixels, width, height, params);
        case GFX_EFFECT_FOIL:          return gfx_apply_foil(pixels, width, height, params);
        case GFX_EFFECT_HOLOGRAPHIC:   return gfx_apply_holographic(pixels, width, height, params);
        case GFX_EFFECT_NEGATIVE:      return gfx_apply_negative(pixels, width, height, params);
        case GFX_EFFECT_POLYCHROME:    return gfx_apply_polychrome(pixels, width, height, params);
        case GFX_EFFECT_HOLOGRAM:      return gfx_apply_hologram(pixels, width, height, params);
        case GFX_EFFECT_PLAYED:        return gfx_apply_played(pixels, width, height, params);
        case GFX_EFFECT_GOLD_SEAL:     return gfx_apply_gold_seal(pixels, width, height, params);
        case GFX_EFFECT_VOUCHER:       return gfx_apply_voucher(pixels, width, height, params);
        case GFX_EFFECT_DISSOLVE:      return gfx_apply_dissolve(pixels, width, height, params);
        case GFX_EFFECT_NEGATIVE_SHINE: return -1;
        case GFX_EFFECT_NONE:
        default:                       return 0;
    }
}

bool graphics_effect_is_animated(GfxEffect effect)
{
    switch (effect)
    {
        case GFX_EFFECT_FLASH:
        case GFX_EFFECT_BACKGROUND:
        case GFX_EFFECT_FOIL:
        case GFX_EFFECT_HOLOGRAPHIC:
        case GFX_EFFECT_POLYCHROME:
        case GFX_EFFECT_HOLOGRAM:
        case GFX_EFFECT_VOUCHER:
        case GFX_EFFECT_DISSOLVE:
            return true;
        default:
            return false;
    }
}

/* ------------------------------------------------------------------ */
/*  Flash overlay state & API                                          */
/* ------------------------------------------------------------------ */

void gfx_effect_trigger_flash(float duration_s)
{
    if (duration_s <= 0.0f) return;
    if (g_flash_state.remaining_s < duration_s)
        g_flash_state.remaining_s = duration_s;
}

bool gfx_effect_update_flash(float dt_s)
{
    if (dt_s <= 0.0f) return g_flash_state.active;
    if (g_flash_state.remaining_s > 0.0f)
    {
        g_flash_state.remaining_s -= dt_s;
        if (g_flash_state.remaining_s <= 0.0f)
        {
            g_flash_state.remaining_s = 0.0f;
            g_flash_state.active = false;
        }
    }
    return g_flash_state.active;
}
