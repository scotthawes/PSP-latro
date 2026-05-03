extern float time;
extern float spin_time;
extern vec4 colour_1;
extern vec4 colour_2;
extern vec4 colour_3;
extern float contrast;
extern float spin_amount;

#define PIXEL_SIZE_FAC 700.0
#define SPIN_EASE 0.5

vec4 effect(vec4 colour, Image texture, vec2 texture_coords, vec2 screen_coords) {
    float pixel_size = length(love_ScreenSize.xy) / PIXEL_SIZE_FAC;
    vec2 uv = (floor(screen_coords.xy * (1.0 / pixel_size)) * pixel_size - 0.5 * love_ScreenSize.xy) / length(love_ScreenSize.xy) - vec2(0.12, 0.0);
    float uv_len = length(uv);

    float speed = (spin_time * SPIN_EASE * 0.2) + 302.2;
    float new_pixel_angle = atan(uv.y, uv.x) + speed - SPIN_EASE * 20.0 * (spin_amount * uv_len + (1.0 - spin_amount));
    vec2 mid = (love_ScreenSize.xy / length(love_ScreenSize.xy)) / 2.0;
    uv = vec2(uv_len * cos(new_pixel_angle) + mid.x, uv_len * sin(new_pixel_angle) + mid.y) - mid;

    uv -= cos(uv.x + uv.y + time) - sin(uv.x * 0.711 - uv.y + time);
    float contrast_mod = 0.25 * contrast + 0.5 * spin_amount + 1.2;
    float paint_res = min(2.0, max(0.0, length(uv) * 0.035 * contrast_mod));
    float c1p = max(0.0, 1.0 - contrast_mod * abs(1.0 - paint_res));
    float c2p = max(0.0, 1.0 - contrast_mod * abs(paint_res));
    float c3p = 1.0 - min(1.0, c1p + c2p);

    vec4 ret_col = (0.3 / contrast) * colour_1 + (1.0 - 0.3 / contrast) * (colour_1 * c1p + colour_2 * c2p + vec4(c3p * colour_3.rgb, c3p * colour_1.a));
    return ret_col;
}