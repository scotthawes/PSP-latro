/*
 * polychrome.fs equivalent
 * Ported from original Balatro:
 *   third_party/balatro-v011-reference/game_love_extracted/resources/shaders/polychrome.fs
 */
#include "../graphics_effects.h"
#include <math.h>

static int gfx_apply_polychrome(uint8_t *pixels, int w, int h, const GfxEffectParams *p)
{
    if (pixels == NULL) return -1;

    float time = p->time;
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

            float t = po_g * 2.221f + time;
            float nx = ((float)i / (float)w) - 0.5f;
            float ny = ((float)j / (float)h) - 0.5f;
            float scaled = sqrtf(nx*nx + ny*ny) * 50.0f;

            float f1 = scaled + 50.0f * sinf(-t / 143.6340f);
            float f2 = scaled + 50.0f * cosf( t / 53.1532f);
            float field = (1.0f + (cosf(sqrtf(f1*f1 + ny*ny) / 19.483f)
                                  + sinf(sqrtf(f2*f2 + ny*ny) / 33.155f) * cosf(ny / 15.73f)
                                  + cosf(sqrtf(scaled*scaled + ny*ny) / 27.193f) * sinf(scaled / 21.92f))) / 2.0f;

            float res = 0.5f + 0.5f * cosf(po_r * 2.612f + (field - 0.5f) * 3.14f);
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
