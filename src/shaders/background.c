/*
 * background.fs equivalent
 * Ported from original Balatro:
 *   third_party/balatro-v011-reference/game_love_extracted/resources/shaders/background.fs
 */
#include "../graphics_effects.h"
#include <math.h>

typedef struct { float x, y; } vec2;

static int gfx_apply_background(uint8_t *pixels, int w, int h, const GfxEffectParams *p)
{
    if (pixels == NULL) return -1;

    const float PIXEL_SIZE_FAC  = 700.0f;
    const float SPIN_EASE      = 0.5f;

    /* screen_len = sqrt(480^2 + 272^2) */
    float screen_len = sqrtf(480.0f * 480.0f + 272.0f * 272.0f);

    float pixel_size = screen_len / PIXEL_SIZE_FAC;

    /* Background colours (colour_1/2/3 from background.fs GLSL) */
    const float c1[4] = {0.38f, 0.50f, 0.0f, 1.0f};
    const float c2[4] = {0.50f, 0.50f, 0.0f, 1.0f};
    const float c3[4] = {0.50f, 0.30f, 0.50f, 1.0f};

    float speed       = (p->spin_time * SPIN_EASE * 0.2f) + 302.2f;
    float spin_amount = CLAMP(p->spin_amount, 0.0f, 1.0f);
    float contrast_v  = (p->contrast < 0.01f) ? 0.01f : p->contrast;

    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            int idx = (j * w + i) * 4;

            /* uv = (floor(screen_coords/pixel_size)*pixel_size - 0.5*screenLen)/screenLen - vec2(0.12,0) */
            vec2 uv = {
                (floorf((float)i / pixel_size) * pixel_size - 0.5f * screen_len) / screen_len - 0.12f,
                (floorf((float)j / pixel_size) * pixel_size - 0.5f * screen_len) / screen_len
            };
            float uv_len = sqrtf(uv.x * uv.x + uv.y * uv.y);

            float new_pixel_angle = atan2f(uv.y, uv.x) + speed
                                  - SPIN_EASE * 20.0f * (spin_amount * uv_len + (1.0f - spin_amount));

            /* mid = (screenLen/(2*screenLen)) */
            vec2 mid = { screen_len / (2.0f * screen_len), screen_len / (2.0f * screen_len) };

            uv.x = uv_len * cosf(new_pixel_angle) + mid.x - mid.x;
            uv.y = uv_len * sinf(new_pixel_angle) + mid.y - mid.y;

            uv.x -= cosf(uv.x + uv.y + p->time) - sinf(uv.x * 0.711f - uv.y + p->time);

            float contrast_mod = 0.25f * p->contrast + 0.5f * spin_amount + 1.2f;
            float paint_res    = MIN(2.0f, MAX(0.0f, sqrtf(uv.x * uv.x + uv.y * uv.y) * 0.035f * contrast_mod));
            float c1p = MAX(0.0f, 1.0f - contrast_mod * fabsf(1.0f - paint_res));
            float c2p = MAX(0.0f, 1.0f - contrast_mod * fabsf(paint_res));
            float c3p = 1.0f - MIN(1.0f, c1p + c2p);

            float r = (0.3f / contrast_v) * c1[0]
                    + (1.0f - 0.3f / contrast_v) * (c1[0] * c1p + c2[0] * c2p + c3[0] * c3p);
            float g = (0.3f / contrast_v) * c1[1]
                    + (1.0f - 0.3f / contrast_v) * (c1[1] * c1p + c2[1] * c2p + c3[1] * c3p);
            float b = (0.3f / contrast_v) * c1[2]
                    + (1.0f - 0.3f / contrast_v) * (c1[2] * c1p + c2[2] * c2p + c3[2] * c3p);

            pixels[idx]     = (uint8_t)(CLAMP(r * 255.0f, 0.0f, 255.0f));
            pixels[idx + 1] = (uint8_t)(CLAMP(g * 255.0f, 0.0f, 255.0f));
            pixels[idx + 2] = (uint8_t)(CLAMP(b * 255.0f, 0.0f, 255.0f));
            /* preserve alpha */
        }
    }
    return 0;
}
