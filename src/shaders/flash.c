/*
 * flash.fs equivalent
 * Ported from original Balatro:
 *   third_party/balatro-v011-reference/game_love_extracted/resources/shaders/flash.fs
 */
#include "../graphics_effects.h"
#include <math.h>

static int gfx_apply_flash(uint8_t *pixels, int w, int h, const GfxEffectParams *p)
{
    if (pixels == NULL) return -1;

    float max_screen = sqrtf((float)(480 * 480 + 272 * 272));

    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            int idx = (j * w + i) * 4;
            float u = ((float)i - 0.5f * 480.0f) / max_screen;
            float v = ((float)j - 0.5f * 272.0f) / max_screen;
            float uv_len = sqrtf(u * u + v * v);

            float mid_white = 0.0f;
            if (p->time > 2.5f)
                mid_white += MAX(0.0f, sqrtf(p->time - 2.5f) - 60.0f * uv_len);
            if (p->time > 11.0f)
                mid_white = MIN(1.0f, mid_white + MAX(0.0f, (p->time - 11.0f) * (p->time - 11.0f) - 5.0f * uv_len));

            if (mid_white > 0.0f && p->mid_flash > 0.0f)
            {
                float a = mid_white * p->mid_flash;
                pixels[idx]     = (uint8_t)(255.0f * a + pixels[idx]     * (1.0f - a));
                pixels[idx + 1] = (uint8_t)(255.0f * a + pixels[idx + 1] * (1.0f - a));
                pixels[idx + 2] = (uint8_t)(255.0f * a + pixels[idx + 2] * (1.0f - a));
            }
        }
    }
    return 0;
}
