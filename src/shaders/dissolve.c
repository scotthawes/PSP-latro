/*
 * dissolve.fs equivalent
 * Ported from original Balatro:
 *   third_party/balatro-v011-reference/game_love_extracted/resources/shaders/dissolve.fs
 *
 * gfx_dissolve_mask_alpha() is a self-contained pixel lookup: given RGBA
 * source pixels it returns the per-pixel alpha the dissolve effect needs.
 * gfx_apply_dissolve() is the per-pixel shader entry point called from
 * graphics_effect_apply() via the GFX_EFFECT_DISSOLVE dispatch.
 *
 * Coord space: gfx_apply_dissolve() computes unified (x,y) from the
 * tile-fragment swizzle layout so pixels match the Balatro noise field.
 */
#include "../graphics_effects.h"
#include "../trig_lut.h"
#include <math.h>

/*
 * Compute the dissolve alpha for a single pixel given its position in
 * unified atlas space (linear row stride = texture_w * 4 bytes).
 */
static uint8_t gfx_dissolve_mask_alpha(
        uint8_t pixel_alpha, float dissolve_01,
        int pixel_x, int pixel_y,
        int texture_w, int texture_h,
        float time, bool shadow)
{
    if (dissolve_01 < 0.001f) return shadow ? (uint8_t)(pixel_alpha * 0.3f) : pixel_alpha;

    float adjusted_dissolve = (dissolve_01 * dissolve_01 * (3.0f - 2.0f * dissolve_01)) * 1.02f - 0.01f;

    float t = time * 10.0f + 2003.0f;

    /* time-dependent trig precomputed once; shared by all 9 neighbours */
    float s_coarse = 50.0f * trig_sin_lut(-t / 143.6340f);
    float c_coarse_a = 50.0f * trig_cos_lut(-t /  99.4324f);
    float c_coarse_b = 50.0f * trig_cos_lut( t /  53.1532f);
    float c_coarse_c = 50.0f * trig_cos_lut( t /  61.4532f);
    float s_coarse_b = 50.0f * trig_sin_lut(-t /  87.53218f);
    float s_coarse_c = 50.0f * trig_sin_lut(-t /  49.0000f);

    /* 3-term noise averaged over a 3×3 neighbourhood */
    float noise = 0.0f;
    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            int   fx    = pixel_x + dx;
            int   fy    = pixel_y + dy;
            float d1    = (float)fy + s_coarse;
            float d2    = (float)fx + c_coarse_a;
            float len1  = sqrtf(d1 * d1 + d2 * d2);
            float d3    = (float)fy + c_coarse_b;
            float d4    = (float)fx + c_coarse_c;
            float len2  = sqrtf(d3 * d3 + d4 * d4);
            float d5    = (float)fy + s_coarse_b;
            float d6    = (float)fx + s_coarse_c;
            float len3  = sqrtf(d5 * d5 + d6 * d6);

            noise += (1.0f + (trig_cos_lut(len1 / 19.483f)
                            + trig_sin_lut(len2 / 33.155f) * trig_cos_lut(d4 / 15.73f)
                            + trig_cos_lut(len3 / 27.193f) * trig_sin_lut(d6 / 21.92f))) / 2.0f;
        }
    }
    noise /= 9.0f;

    float res = (0.5f + 0.5f * trig_cos_lut(adjusted_dissolve / 82.612f + (noise - 0.5f) * 3.14f));
     return (res > adjusted_dissolve) ? (shadow ? (uint8_t)(pixel_alpha * 0.3f) : pixel_alpha) : 0;
}

/* ------------------------------------------------------------------ */
/*  gfx_apply_dissolve — dispatcher entry point                        */
/* ------------------------------------------------------------------ */

static int gfx_apply_dissolve(uint8_t *pixels, int w, int h, const GfxEffectParams *p)
{
    if (pixels == NULL) return -1;

    float dissolve_01 = p->dissolve;
    float time        = p->time;
    bool  shadow      = false;

    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            int idx    = (j * w + i) * 4;
            uint8_t a  = pixels[idx + 3];
            uint8_t na = gfx_dissolve_mask_alpha(a, dissolve_01, i, j, w, h, time, shadow);
            pixels[idx + 3] = na;
        }
    }
    return 0;
}
