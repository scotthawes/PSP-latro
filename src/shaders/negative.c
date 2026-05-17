/*
 * negative.fs equivalent
 * Ported from original Balatro:
 *   third_party/balatro-v011-reference/game_love_extracted/resources/shaders/negative.fs
 */
#include "../graphics_effects.h"
#include <math.h>

static int gfx_apply_negative(uint8_t *pixels, int w, int h, const GfxEffectParams *p)
{
    if (pixels == NULL) return -1;

    bool invert_g = (p->negative_param_r < 0.0f) || (p->negative_param_r > 0.0f);

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

            if (invert_g) S = 1.0f - S;
            S = -S + 0.2f;

            float t = (L < 0.5f) ? S * L + L : -S * L + (S + L);
            float s = 2.0f * L - t;
            float nr = gfx_hue(s, t, H + 1.0f/3.0f);
            float ng = gfx_hue(s, t, H);
            float nb = gfx_hue(s, t, H - 1.0f/3.0f);

            /* colour_boost from original: + 0.8*vec4(79/255,…) */
            pixels[idx]     = (uint8_t)CLAMP((nr + 0.312f) * 255.0f, 0.0f, 255.0f);
            pixels[idx + 1] = (uint8_t)CLAMP((ng + 0.388f) * 255.0f, 0.0f, 255.0f);
            pixels[idx + 2] = (uint8_t)CLAMP((nb + 0.403f) * 255.0f, 0.0f, 255.0f);
            if (a < (uint8_t)(0.7f * 255.0f))
                pixels[idx + 3] = a / 3;
        }
    }
    return 0;
}
