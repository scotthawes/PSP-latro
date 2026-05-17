#pragma once

#include "global.h"
#include <stdint.h>

/* ------------------------------------------------------------------ */
/*  Shader effect identifiers                                          */
/* ------------------------------------------------------------------ */

typedef enum
{
    GFX_EFFECT_NONE          = 0,
    GFX_EFFECT_FLASH         = 1,
    GFX_EFFECT_BACKGROUND    = 2,
    GFX_EFFECT_FOIL          = 3,
    GFX_EFFECT_HOLOGRAPHIC   = 4,
    GFX_EFFECT_NEGATIVE      = 5,
    GFX_EFFECT_POLYCHROME    = 6,
    GFX_EFFECT_HOLOGRAM      = 7,
    GFX_EFFECT_PLAYED        = 8,
    GFX_EFFECT_GOLD_SEAL     = 9,
    GFX_EFFECT_VOUCHER       = 10,
    GFX_EFFECT_DISSOLVE      = 11,
    GFX_EFFECT_NEGATIVE_SHINE = 12,
    GFX_EFFECT_COUNT
} GfxEffect;

/* ------------------------------------------------------------------ */
/*  Effect parameter mirroring the Love2D uniforms                     */
/* ------------------------------------------------------------------ */

typedef struct
{
    GfxEffect         effect;
    float             time;          /* global time counter */
    float             mid_flash;     /* flash.fs */
    float             vort_speed;    /* splash.fs */
    float             vort_offset;   /* splash.fs */
    float             spin_time;     /* background.fs */
    float             spin_amount;   /* background.fs */
    float             contrast;      /* background.fs */
    float             dissolve;      /* many shaders */
    float             foil_param_r;  /* foil.fs / holo.fs / … */
    float             foil_param_g;
    float             holo_param_r;
    float             holo_param_g;
    float             negative_param_r;
    float             polychrome_param_r;
    float             polychrome_param_g;
    float             gold_seal_param_r;
    float             voucher_param_r;
    float             hologram_param_r;
    float             hologram_param_g;
    float             negative_shine_param_r;
} GfxEffectParams;

/**
 * Build a GfxEffectParams populated for a static / time-independent effect.
 * foil_r / foil_g / holo_r / holo_g / polychrome_r / polychrome_g / gold_seal_r
 * are seed floats in [0,1].
 *
 * @param edition    CARD_EDITION_* from global.h
 * @param card_seed  use the card type or first-refresh call result as the seed
 * @return zero-initialised GfxEffectParams ready for a load-time apply or 0 at draw time
 */
GfxEffectParams graphics_build_edition_params(int edition, float card_seed);

/**
 * Build a GfxEffectParams populated for drawing / evaluation time.
 * time-dependent parameters are refreshed each frame via this helper.
 *
 * @param edition    CARD_EDITION_* from global.h
 * @param card_seed  per-card random seed in [0,1]
 * @param game_time  g_game_counter expressed in seconds
 * @return GfxEffectParams for a real-time effect
 */
GfxEffectParams graphics_build_edition_params_realtime(
    int edition, float card_seed, float game_time);

/**
 * Apply an edition/seal effect to an already-loaded RGBA8888 texture in-place.
 * The texture data is temporarily unswizzled in software, processed, and
 * re-swizzled — acceptable for a one-time load-time cost.
 *
 * The Texture struct @p tex must be valid (in_use == true, data != NULL).
 *
 * @return 0 on success, -1 on error
 */
int  graphics_apply_texture_effect(int texture_index, const GfxEffectParams *params);

/* ------------------------------------------------------------------ */
/*  Public API                                                         */
/* ------------------------------------------------------------------ */

/**
 * Apply @p effect to @p pixels in-place.
 * @param  pixels   RGBA8 pixel buffer (width * height pixels in a contiguous
 *                  array; stride = width * 4)
 * @param  width    texture width in pixels
 * @param  height   texture height in pixels
 * @param  params   tuning knobs
 * @return 0 on success, -1 if the pixel buffer is NULL
 */
int  graphics_effect_apply(uint8_t *pixels, int width, int height,
                           const GfxEffectParams *params);

/**
 * Return true if @p effect needs regular re-evaluation (i.e. it is
 * time-dependent or otherwise animated).  Called once per frame.
 */
bool graphics_effect_is_animated(GfxEffect effect);