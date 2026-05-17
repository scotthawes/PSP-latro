/*
 * foil.fs equivalent
 * Ported from original Balatro:
 *   third_party/balatro-v011-reference/game_love_extracted/resources/shaders/foil.fs
 */
#include "../graphics_effects.h"
#include <math.h>

static int gfx_apply_foil(uint8_t *pixels, int w, int h, const GfxEffectParams *p)
{
    if (pixels == NULL) return -1;

    float foil_r = p->foil_param_r;
    float foil_g = p->foil_param_g;

    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            int idx = (j * w + i) * 4;
            uint8_t r = pixels[idx];
            uint8_t g = pixels[idx + 1];
            uint8_t b = pixels[idx + 2];
            uint8_t a = pixels[idx + 3];

            float low  = MIN(r, MIN(g, b)) / 255.0f;
            float high = MAX(r, MAX(g, b)) / 255.0f;
            float delta = MIN(high, MAX(0.5f, 1.0f - low));
            float cx = ((float)i / (float)w) - 0.5f;
            float cy = ((float)j / (float)h) - 0.5f;
            float adjusted_uv_len = sqrtf(cx * cx + cy * cy) * (float)MAX(w, h);

            float fac = MAX(MIN(
                2.0f * sinf(adjusted_uv_len * 90.0f + foil_r * 2.0f + 3.0f * (1.0f + 0.8f * cosf(adjusted_uv_len * 113.1121f - foil_r * 3.121f))) - 1.0f
                - MAX(5.0f - adjusted_uv_len * 90.0f, 0.0f), 1.0f), 0.0f);

            float rot_c = cosf(foil_r * 0.1221f);
            float rot_s = sinf(foil_r * 0.3512f);
            float dotp = rot_c * cx + rot_s * cy;
            float len  = sqrtf(rot_c * rot_c + rot_s * rot_s) * sqrtf(cx * cx + cy * cy);
            float angle = (len > 0.0f) ? dotp / len : 0.0f;
            float fac2 = MAX(MIN(
                5.0f * cosf(foil_g * 0.3f + angle * 3.14f * (2.2f + 0.9f * sinf(foil_r * 1.65f + 0.2f * foil_g)))
                - 4.0f - MAX(2.0f - adjusted_uv_len * 20.0f, 0.0f), 1.0f), 0.0f);

            float fac3 = 0.3f * MAX(MIN(
                2.0f * sinf(foil_r * 5.0f + (float)i / (float)w * 3.0f + 3.0f * (1.0f + 0.5f * cosf(foil_r * 7.0f))) - 1.0f, 1.0f), -1.0f);
            float fac4 = 0.3f * MAX(MIN(
                2.0f * sinf(foil_r * 6.66f + (float)j / (float)h * 3.8f + 3.0f * (1.0f + 0.5f * cosf(foil_r * 3.414f))) - 1.0f, 1.0f), -1.0f);

            float maxfac = fac + fac2 + fac3 + fac4 + 2.2f * MIN(MAX(fac + fac2 + fac3 + fac4, 0.0f), 1.0f);
            maxfac = MAX(0.0f, maxfac) * 0.3f;

            float nr = r/255.0f - delta + delta * maxfac;
            float ng = g/255.0f - delta + delta * maxfac;
            float nb = b/255.0f + delta * maxfac * 1.9f;
            float na = MIN(a/255.0f, 0.3f * a/255.0f + 0.9f * MIN(0.5f, maxfac * 0.1f));

            pixels[idx]     = (uint8_t)(nr * 255.0f);
            pixels[idx + 1] = (uint8_t)(ng * 255.0f);
            pixels[idx + 2] = (uint8_t)(nb * 255.0f);
            pixels[idx + 3] = (uint8_t)(na * 255.0f);
        }
    }
    return 0;
}
