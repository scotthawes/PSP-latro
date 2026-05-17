/*
 * holo.fs equivalent
 * Ported from original Balatro:
 *   third_party/balatro-v011-reference/game_love_extracted/resources/shaders/holo.fs
 */
#include "../graphics_effects.h"
#include <math.h>

static int gfx_apply_holographic(uint8_t *pixels, int w, int h, const GfxEffectParams *p)
{
    if (pixels == NULL) return -1;

    float time   = p->time;
    float holo_r = p->holo_param_r;
    float holo_g = p->holo_param_g;

    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            int idx = (j * w + i) * 4;
            uint8_t r0 = pixels[idx];
            uint8_t g0 = pixels[idx + 1];
            uint8_t b0 = pixels[idx + 2];
            uint8_t a0 = pixels[idx + 3];

            /* half blue tint → HSL */
            float tr = r0 / 255.0f * 0.5f + 0.5f;
            float tg = g0 / 255.0f * 0.5f;
            float tb = b0 / 255.0f * 0.5f + 0.5f;

            float H, S, L;
            gfx_RGB_to_HSL((uint8_t)(tr * 255), (uint8_t)(tg * 255), (uint8_t)(tb * 255), &H, &S, &L);

            float t = holo_g * 7.221f + time;

            float nx = ((float)i / (float)w) - 0.5f;
            float ny = ((float)j / (float)h) - 0.5f;
            float scaled = sqrtf(nx * nx + ny * ny) * 250.0f;

            float f1 = scaled + 50.0f * sinf(-t / 143.6340f)
                             + 50.0f * cosf(-t / 99.4324f);
            float f2 = scaled + 50.0f * cosf( t / 53.1532f)
                             + 50.0f * cosf( t / 61.4532f);
            float f3 = scaled + 50.0f * sinf(-t / 87.53218f)
                             + 50.0f * sinf(-t / 49.0000f);

            float field = (1.0f + (cosf(sqrtf(f1*f1 + ny*ny) / 19.483f)
                                  + sinf(sqrtf(f2*f2 + ny*ny) / 33.155f) * cosf(ny / 15.73f)
                                  + cosf(sqrtf(f3*f3 + ny*ny) / 27.193f) * sinf(scaled    / 21.92f))) / 2.0f;

            float res   = 0.5f + 0.5f * cosf(holo_r * 2.612f + (field - 0.5f) * 3.14f);
            float low   = MIN(r0, MIN(g0, b0)) / 255.0f;
            float high  = MAX(r0, MAX(g0, b0)) / 255.0f;
            float delta = 0.2f + 0.3f * (high - low) + 0.1f * high;

            H += res;
            S *= 1.3f;
            L  = L * 0.6f + 0.4f;

            /* RGB(H,S,L) back */
            float t2  = (L < 0.5f) ? S * L + L : -S * L + (S + L);
            float s2  = 2.0f * L - t2;
            float R_h = gfx_hue(s2, t2, H + 1.0f/3.0f);
            float G_h = gfx_hue(s2, t2, H);
            float B_h = gfx_hue(s2, t2, H - 1.0f/3.0f);

            /* tex = (1-delta)*tex + delta*RGB(hsl)*vec4(0.9,0.8,1.2,tex.a) */
            float rf = r0 / 255.0f;
            float gf = g0 / 255.0f;
            float bf = b0 / 255.0f;
            float af = a0 / 255.0f;

            float nr = (1.0f - delta) * rf + delta * R_h * 0.9f;
            float ng = (1.0f - delta) * gf + delta * G_h * 0.8f;
            float nb = (1.0f - delta) * bf + delta * B_h * 1.2f;

            pixels[idx]   = (uint8_t)(CLAMP(nr * 255.0f, 0.0f, 255.0f));
            pixels[idx+1] = (uint8_t)(CLAMP(ng * 255.0f, 0.0f, 255.0f));
            pixels[idx+2] = (uint8_t)(CLAMP(nb * 255.0f, 0.0f, 255.0f));
            if (af < 0.7f)
                pixels[idx+3] = (uint8_t)(CLAMP(af / 3.0f * 255.0f, 0.0f, 255.0f));
        }
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/*  gfx_apply_hologram — dispatcher entry point for GFX_EFFECT_HOLOGRAM */
/* ------------------------------------------------------------------ */

static int gfx_apply_hologram(uint8_t *pixels, int w, int h, const GfxEffectParams *p)
{
    return gfx_apply_holographic(pixels, w, h, p);
}
