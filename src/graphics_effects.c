#include "global.h"
#include "graphics_effects.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* Local 2-D float vector mirroring GLSL vec2 */
typedef struct { float x, y; } vec2;

/* ---------------------------------------------------------------- */
/*  Math helpers (port of the GLSL utility functions found in many   */
/*  Balatro shaders)                                                */
/* ---------------------------------------------------------------- */

static inline float gfx_hue(float s, float t, float h)
{
    float hs = fmodf(h, 1.0f) * 6.0f;
    if (hs < 1.0f) return (t - s) * hs + s;
    if (hs < 3.0f) return t;
    if (hs < 4.0f) return (t - s) * (4.0f - hs) + s;
    return s;
}

/* c expects r=H, g=S, b=L in [0,1] */
static inline void gfx_HSL_to_RGB(const float *c, float *r, float *g, float *b)
{
    float t = (c[2] < 0.5f) ? c[1] * c[2] + c[2] : -c[1] * c[2] + (c[1] + c[2]);
    float s = 2.0f * c[2] - t;
    *r = gfx_hue(s, t, c[0] + 1.0f / 3.0f);
    *g = gfx_hue(s, t, c[0]);
    *b = gfx_hue(s, t, c[0] - 1.0f / 3.0f);
    (void)c;  /* suppress unused-param warning for READMEM */
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

/* dissolvemask port. Matches the cross-shader body.                   */
static inline uint8_t gfx_dissolve_mask_alpha(
        uint8_t pixel_alpha, float dissolve_01,
        int pixel_x, int pixel_y,
        int texture_w, int texture_h,
        float time, bool shadow)
{
    if (dissolve_01 < 0.001f) return shadow ? (uint8_t)(pixel_alpha * 0.3f) : pixel_alpha;

    float adjusted_dissolve = (dissolve_01 * dissolve_01 * (3.0f - 2.0f * dissolve_01)) * 1.02f - 0.01f;

    float t = time * 10.0f + 2003.0f;

    /* Minimal acceptable approximation of the dissolve field for PSP budget:
     * evaluate the 3-term noise over a 3x3 pixel neighbourhood and average. */
    float noise = 0.0f;
    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            int fx = pixel_x + dx;
            int fy = pixel_y + dy;

            float d1 = (float)fy + 50.0f * sinf(-t / 143.6340f);
            float d2 = (float)fx + 50.0f * cosf(-t / 99.4324f);
            float len1 = sqrtf(d1*d1 + d2*d2);
            float d3 = (float)fy + 50.0f * cosf(t / 53.1532f);
            float d4 = (float)fx + 50.0f * cosf(t / 61.4532f);
            float len2 = sqrtf(d3*d3 + d4*d4);
            float d5 = (float)fy + 50.0f * sinf(-t / 87.53218f);
            float d6 = (float)fx + 50.0f * sinf(-t / 49.0000f);
            float len3 = sqrtf(d5*d5 + d6*d6);

            noise += (1.0f + (cosf(len1 / 19.483f)
                            + sinf(len2 / 33.155f) * cosf(d4 / 15.73f)
                            + cosf(len3 / 27.193f) * sinf(d6 / 21.92f))) / 2.0f;
        }
    }
    noise /= 9.0f;  /* 3x3 average */

    float res = (0.5f + 0.5f * cosf(adjusted_dissolve / 82.612f + (noise - 0.5f) * 3.14f));
    return (res > adjusted_dissolve) ? (shadow ? (uint8_t)(pixel_alpha * 0.3f) : pixel_alpha) : 0;
}
