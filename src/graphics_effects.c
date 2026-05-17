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


/**
 * Apply an edition effect to @p pixels buffer in-place.
 * Convenience wrapper: builds params from edition+seed and dispatches.
 *
 * @return 0 on success, -1 on error
 */
int graphics_apply_edition_effect_inplace(uint8_t *pixels, int width, int height,
                                          int edition, float card_seed, float time)
{
    if (pixels == NULL || width <= 0 || height <= 0) return -1;

    GfxEffectParams params = graphics_build_edition_params_realtime(edition, card_seed, time);
    return graphics_effect_apply(pixels, width, height, &params);
}

/* ================================================================ */
/*  Per-effect implementations                                      */
/* ================================================================ */

/* ── FLASH ─────────────────────────────────────────────────────── */
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

/* ── BACKGROUND ────────────────────────────────────────────────── */
static int gfx_apply_background(uint8_t *pixels, int w, int h, const GfxEffectParams *p)
{
    if (pixels == NULL) return -1;

    const float PIXEL_SIZE_FAC  = 700.0f;
    const float SPIN_EASE      = 0.5f;

    /* screen_len = length(love_ScreenSize.xy) = sqrt(480^2 + 272^2) */
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

            /* mid = (screenLen/(2*screenLen)) → effectively 0.5/screen_len in each axis */
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

/* ── FOIL ──────────────────────────────────────────────────────── */
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

/* ── HOLOGRAPHIC ───────────────────────────────────────────────── */
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

/* ── NEGATIVE (without dissolve) ───────────────────────────────── */
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

/* ── POLYCHROME ────────────────────────────────────────────────── */
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

/* ── PLAYED ────────────────────────────────────────────────────── */
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

/* ── GOLD_SEAL ─────────────────────────────────────────────────── */
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

/* ── VOUCHER ───────────────────────────────────────────────────── */
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

/* ================================================================ */
/*  Public API                                                       */
/* ================================================================ */

/* ××× low-level pixel dispatch — already defined above ××× */

GfxEffectParams graphics_build_edition_params(int edition, float card_seed)
{
    GfxEffectParams p = {0};
    p.time = 0.0f;

    switch (edition)
    {
        case GFX_EFFECT_FOIL:
            p.effect        = GFX_EFFECT_FOIL;
            p.foil_param_r  = card_seed;
            p.foil_param_g  = card_seed * 0.73f;
            break;

        case GFX_EFFECT_HOLOGRAPHIC:
            p.effect        = GFX_EFFECT_HOLOGRAPHIC;
            p.holo_param_r  = card_seed;
            p.holo_param_g  = card_seed * 0.8f;
            break;

        case GFX_EFFECT_POLYCHROME:
            p.effect        = GFX_EFFECT_POLYCHROME;
            p.polychrome_param_r = card_seed;
            p.polychrome_param_g = card_seed * 0.7f;
            break;

        case GFX_EFFECT_NEGATIVE:
            p.effect            = GFX_EFFECT_NEGATIVE;
            p.negative_param_r  = card_seed; /* sign flip: card_seed > 0 → invert G */
            break;

        case GFX_EFFECT_GOLD_SEAL:
            p.effect           = GFX_EFFECT_GOLD_SEAL;
            p.gold_seal_param_r = card_seed;
            break;

        case GFX_EFFECT_VOUCHER:
            p.effect         = GFX_EFFECT_VOUCHER;
            p.voucher_param_r = card_seed;
            break;

        default:
            p.effect = GFX_EFFECT_NONE;
            break;
    }

    return p;
}

GfxEffectParams graphics_build_edition_params_realtime(int edition, float card_seed, float game_time)
{
    GfxEffectParams p = graphics_build_edition_params(edition, card_seed);
    if (p.effect != GFX_EFFECT_NONE)
        p.time = game_time;
    return p;
}

int graphics_effect_apply(uint8_t *pixels, int width, int height,
                          const GfxEffectParams *params)
{
    if (pixels == NULL || params == NULL) return -1;

    switch (params->effect)
    {
        case GFX_EFFECT_FLASH:         return gfx_apply_flash(pixels, width, height, params);
        case GFX_EFFECT_BACKGROUND:    return gfx_apply_background(pixels, width, height, params);
        case GFX_EFFECT_FOIL:          return gfx_apply_foil(pixels, width, height, params);
        case GFX_EFFECT_HOLOGRAPHIC:   return gfx_apply_holographic(pixels, width, height, params);
        case GFX_EFFECT_NEGATIVE:      return gfx_apply_negative(pixels, width, height, params);
        case GFX_EFFECT_POLYCHROME:    return gfx_apply_polychrome(pixels, width, height, params);
        case GFX_EFFECT_HOLOGRAM:      return gfx_apply_negative(pixels, width, height, params); /* simplified */
        case GFX_EFFECT_PLAYED:        return gfx_apply_played(pixels, width, height, params);
        case GFX_EFFECT_GOLD_SEAL:     return gfx_apply_gold_seal(pixels, width, height, params);
        case GFX_EFFECT_VOUCHER:       return gfx_apply_voucher(pixels, width, height, params);
        case GFX_EFFECT_DISSOLVE:      return -1; /* dissolve needs texture-level integration */
        case GFX_EFFECT_NEGATIVE_SHINE: return -1;
        case GFX_EFFECT_NONE:
        default:                       return 0;
    }
}

bool graphics_effect_is_animated(GfxEffect effect)
{
    switch (effect)
    {
        case GFX_EFFECT_FLASH:
        case GFX_EFFECT_BACKGROUND:
        case GFX_EFFECT_FOIL:
        case GFX_EFFECT_HOLOGRAPHIC:
        case GFX_EFFECT_POLYCHROME:
        case GFX_EFFECT_HOLOGRAM:
        case GFX_EFFECT_VOUCHER:
        case GFX_EFFECT_DISSOLVE:
            return true;
        default:
            return false;
    }
}
