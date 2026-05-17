/*
 * played.fs + gold_seal.fs + voucher.fs equivalents
 * Ported from original Balatro:
 *   third_party/balatro-v011-reference/game_love_extracted/resources/shaders/played.fs
 *   third_party/balatro-v011-reference/game_love_extracted/resources/shaders/gold_seal.fs
 *   third_party/balatro-v011-reference/game_love_extracted/resources/shaders/voucher.fs
 */
#include "../graphics_effects.h"
#include <math.h>

/* ── played.fs ──────────────────────────────────────────────────── */
static int gfx_apply_played(uint8_t *pixels, int w, int h, const GfxEffectParams *p)
{
    if (pixels == NULL) return -1;

    float played_r = p->negative_param_r; /* reuse slot */

    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            int idx = (j * w + i) * 4;
            uint8_t r = pixels[idx];
            uint8_t g = pixels[idx + 1];
            uint8_t b = pixels[idx + 2];
            uint8_t a = pixels[idx + 3];

            float H, S, L;
            gfx_RGB_to_HSL(r, g, b, &H, &S, &L);
            S = S * 0.5f + 0.000001f * played_r;
            L = L * 0.8f;

            float t2 = (L < 0.5f) ? S * L + L : -S * L + (S + L);
            float s2 = 2.0f * L - t2;
            pixels[idx]     = (uint8_t)(CLAMP(gfx_hue(s2, t2, H + 1.0f/3.0f) * 255.0f, 0, 255));
            pixels[idx + 1] = (uint8_t)(CLAMP(gfx_hue(s2, t2, H)         * 255.0f, 0, 255));
            pixels[idx + 2] = (uint8_t)(CLAMP(gfx_hue(s2, t2, H - 1.0f/3.0f) * 255.0f, 0, 255));
            pixels[idx + 3] = (uint8_t)(a * 0.5f);
        }
    }
    return 0;
}

/* ── gold_seal.fs ───────────────────────────────────────────────── */
static int gfx_apply_gold_seal(uint8_t *pixels, int w, int h, const GfxEffectParams *p)
{
    if (pixels == NULL) return -1;

    float gold_r = p->gold_seal_param_r; /* timing */

    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            int idx = (j * w + i) * 4;
            uint8_t r = pixels[idx];
            uint8_t g = pixels[idx + 1];
            uint8_t b = pixels[idx + 2];

            float high = MAX(r, MAX(g, b)) / 255.0f;
            float delta = high * 0.5f;

            float u = (float)i / (float)w;
            float v = (float)j / (float)h;
            float fac = 0.3f + sinf(u * 450.0f + sinf(gold_r * 6.0f) * 180.0f - 700.0f * gold_r)
                             - sinf(u * 190.0f + v * 30.0f + 1080.3f * gold_r);

            float nr = MAX(r / 255.0f, (1.0f - r / 255.0f) * delta * fac + r / 255.0f);
            float ng = MAX(g / 255.0f, (1.0f - g / 255.0f) * delta * fac + g / 255.0f);
            float nb = MAX(b / 255.0f, (1.0f - b / 255.0f) * delta * fac + b / 255.0f);

            pixels[idx]     = (uint8_t)(CLAMP(nr * 255.0f, 0, 255));
            pixels[idx + 1] = (uint8_t)(CLAMP(ng * 255.0f, 0, 255));
            pixels[idx + 2] = (uint8_t)(CLAMP(nb * 255.0f, 0, 255));
            /* alpha unchanged */
        }
    }
    return 0;
}

/* ── voucher.fs ─────────────────────────────────────────────────── */
static int gfx_apply_voucher(uint8_t *pixels, int w, int h, const GfxEffectParams *p)
{
    if (pixels == NULL) return -1;

    float vr   = p->voucher_param_r;

    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            int idx = (j * w + i) * 4;
            uint8_t r = pixels[idx];
            uint8_t g = pixels[idx + 1];
            uint8_t b = pixels[idx + 2];
            uint8_t a = pixels[idx + 3];

            float nx = (float)i / (float)w;
            float ny = (float)j / (float)h;

            float lowv  = MIN(r, MIN(g, b)) / 255.0f;
            float highv = MAX(r, MAX(g, b)) / 255.0f;
            float delta = highv - lowv;

            float fac  = 0.8f + 0.9f * sinf(13.0f * nx + 5.32f * ny + vr * 12.0f + cosf(vr * 5.3f  + ny * 4.2f  - nx * 4.0f));
            float fac2 = 0.5f + 0.5f * sinf(10.0f * nx + 2.32f * ny + vr * 5.0f  - cosf(vr * 2.3f  + nx * 8.2f));
            float fac3 = 0.5f + 0.5f * sinf(12.0f * nx + 6.32f * ny + vr * 6.111f + sinf(vr * 5.3f  + ny * 3.2f));
            float fac4 = 0.5f + 0.5f * sinf( 4.0f * nx + 2.32f * ny + vr * 8.111f + sinf(vr * 1.3f  + ny * 13.2f));
            float fac5 = sinf    (16.0f * nx + 5.32f * ny + vr * 12.0f + cosf(vr * 5.3f + ny * 4.2f - nx * 4.0f));

            float maxfac = 0.6f * MAX(MAX(fac, MAX(fac2, MAX(fac3, 0.0f))) + (fac + fac2 + fac3 * fac4), 0.0f);

            float rn = r/255.0f * 0.5f + 0.4f
                         - delta + delta * maxfac * (0.7f + fac5 * 0.07f) - 0.1f;
            float gn = g/255.0f * 0.5f + 0.4f
                         - delta + delta * maxfac * (0.7f - fac5 * 0.17f) - 0.1f;
            float bn = b/255.0f * 0.5f + 0.4f
                         - delta + delta * maxfac * 0.7f - 0.1f;
            float an = a/255.0f * (0.8f * MAX(MIN(1.0f, MAX(0.0f, 0.3f * MAX(lowv * 0.2f, delta) + MIN(MAX(maxfac * 0.1f, 0.0f), 0.4f))), 0.0f)
                                   + 0.15f * maxfac * (0.1f + delta));

            pixels[idx]     = (uint8_t)(CLAMP(rn * 255.0f, 0, 255));
            pixels[idx + 1] = (uint8_t)(CLAMP(gn * 255.0f, 0, 255));
            pixels[idx + 2] = (uint8_t)(CLAMP(bn * 255.0f, 0, 255));
            pixels[idx + 3] = (uint8_t)(CLAMP(an * 255.0f, 0, 255));
        }
    }
    return 0;
}
