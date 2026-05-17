/*
 * polychrome.fs equivalent
 * Ported from original Balatro:
 *   third_party/balatro-v011-reference/game_love_extracted/resources/shaders/polychrome.fs
 */
#include "../graphics_effects.h"
#include "../trig_lut.h"
#include <math.h>

static int gfx_apply_polychrome(uint8_t *pixels, int w, int h, const GfxEffectParams *p)
{
    if (pixels == NULL) return -1;

    float po_r = p->polychrome_param_r;
    float po_g = p->polychrome_param_g;

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
            float delta = high - low;
            float sat_fac = 1.0f - MAX(0.0f, 0.05f * (1.1f - delta));

            float H, S, L;
            gfx_RGB_to_HSL(r, g, b, &H, &S, &L);
            S = S * sat_fac;

            /* Palette stepping: discrete 0-39 cycle counter, stepping every
             * POLYCHROME_FRAME_S (0.6667 s, i.e. ~40 frames at 60 Hz).
             * The oscillating field above stays continuous; only the res palette
             * offset is quantised to match the Balatro 40-frame colour cycle. */
            float cycle_t = (float)p->polychrome_cycle;

            float res = 0.5f + 0.5f * trig_cos_lut(po_r * 2.612f + cycle_t);
            H += res + po_g * 0.04f;
            S = MIN(0.6f, S + 0.5f);

            float t2 = (L < 0.5f) ? S * L + L : -S * L + (S + L);
            float s2 = 2.0f * L - t2;
            pixels[idx]     = (uint8_t)(CLAMP(gfx_hue(s2, t2, H + 1.0f/3.0f) * 255.0f, 0, 255));
            pixels[idx + 1] = (uint8_t)(CLAMP(gfx_hue(s2, t2, H)         * 255.0f, 0, 255));
            pixels[idx + 2] = (uint8_t)(CLAMP(gfx_hue(s2, t2, H - 1.0f/3.0f) * 255.0f, 0, 255));
            if (a < 0.7f * 255.0f)
                pixels[idx + 3] = a / 3;
        }
    }
    return 0;
}
