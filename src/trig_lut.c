/*
 * trig_lut.c — cosine/sine lookup table for GPU-cell CPU shader port
 * Uses linear interpolation over TRIG_LUT_SIZE samples covering [0, 2π].
 */
#include "trig_lut.h"
#include <math.h>

#ifndef TRIG_LUT_SIZE
#define TRIG_LUT_SIZE 512
#endif

static float g_cos_lut[TRIG_LUT_SIZE];
static int   g_lut_ready = 0;
#define LUT_SCALE ((float)TRIG_LUT_SIZE / (2.0f * (float)M_PI))

static void trig_lut_build(void)
{
    if (g_lut_ready) return;
    for (int i = 0; i < TRIG_LUT_SIZE; i++)
    {
        float angle = (2.0f * (float)M_PI * (float)i) / (float)TRIG_LUT_SIZE;
        g_cos_lut[i] = cosf(angle);
    }
    g_lut_ready = 1;
}

float trig_cos_lut(float angle)
{
    trig_lut_build();

    /* normalize into [0, 2π) */
    float twopi  = 2.0f * (float)M_PI;
    float a      = fmodf(fabsf(angle), twopi);
    if (a < 0.0f) a += twopi;

    /* fractional index */
    float idx_f  = a * LUT_SCALE;
    int   idx    = (int)idx_f;
    float frac   = idx_f - (float)idx;

    int   i0     = idx;
    int   i1     = idx + 1;
    if (i1 >= TRIG_LUT_SIZE) i1 -= TRIG_LUT_SIZE;

    return g_cos_lut[i0] + frac * (g_cos_lut[i1] - g_cos_lut[i0]);
}

float trig_sin_lut(float angle)
{
    return trig_cos_lut(angle - (float)M_PI_2);
}
