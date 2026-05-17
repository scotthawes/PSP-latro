#include "global.h"

/* Format a non-negative double as an integer with thousands commas.
 * Falls back to %g for values >= 1e15 (scores that need scientific notation). */
static void fmt_num(char *buf, double n)
{
    if (n < 0.0 || n >= 1e15) { sprintf(buf, "%g", n); return; }
    long long v = (long long)n;
    char tmp[24];
    char *p = tmp + sizeof(tmp) - 1;
    *p = '\0';
    int group = 0;
    long long rem = v;
    if (rem == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
    while (rem > 0) {
        if (group > 0 && group % 3 == 0) *--p = ',';
        *--p = '0' + (int)(rem % 10);
        rem /= 10;
        group++;
    }
    int i = 0;
    while (*p) buf[i++] = *p++;
    buf[i] = '\0';
}

#define TEXTURE_CARD_WIDTH  69
#define TEXTURE_CARD_HEIGHT 93

#define PSP_TEX_CARD_WIDTH   17
#define PSP_TEX_CARD_HEIGHT  23

int tex_enhancers, tex_deck, tex_deck2, tex_gamepad_ui, tex_editions, tex_shop, tex_ui_assets, tex_blind_chips, tex_title_bg_alt;
int tex_blind_background;
int tex_jokers[2][4];
int tex_tarots[2][2];
int tex_boosters[2];

int font_big, font_small;

#define GAMEPAD_UI_CROSS            0
#define GAMEPAD_UI_CIRCLE           1
#define GAMEPAD_UI_SQUARE           2
#define GAMEPAD_UI_TRIANGLE         3
#define GAMEPAD_UI_LEFT_TRIGGER     4
#define GAMEPAD_UI_RIGHT_TRIGGER    5

#define GAMEPAD_UI_COUNT            6

#define UI_ASSETS_CHIP              0
#define UI_ASSETS_ACE               1
#define UI_ASSETS_FACE              2
#define UI_ASSETS_NUMBER            3
#define UI_ASSETS_HEARTS            4
#define UI_ASSETS_DIAMONDS          5
#define UI_ASSETS_CLUBS             6
#define UI_ASSETS_SPADES            7

#define UI_ASSETS_COUNT             8
#define SEAL_TEXTURE_WIDTH          32
#define SEAL_TEXTURE_HEIGHT         32

struct TexCoords
{
    int x, y;
};

static const struct TexCoords g_enhancement_tex_coords[CARD_ENHANCEMENT_COUNT] = {
    { 71, 1 },  // CARD_ENHANCEMENT_NONE
    { 71, 95 },   // CARD_ENHANCEMENT_BONUS
    { 142, 95 },   // CARD_ENHANCEMENT_MULT
    { 213, 95 },   // CARD_ENHANCEMENT_WILD
    { 355, 95 },   // CARD_ENHANCEMENT_GLASS
    { 426, 95 },   // CARD_ENHANCEMENT_STEEL
    { 355, 1 },   // CARD_ENHANCEMENT_STONE
    { 426, 1 },   // CARD_ENHANCEMENT_GOLD
    { 284, 95 }    // CARD_ENHANCEMENT_LUCKY
};

static const struct TexCoords g_gamepad_ui_text_coords[GAMEPAD_UI_COUNT] = {
    { 0, 32 }, // GAMEPAD_UI_CROSS
    { 32, 32 }, // GAMEPAD_UI_CIRCLE        
    { 64, 32 }, // GAMEPAD_UI_SQUARE        
    { 96, 32 }, // GAMEPAD_UI_TRIANGLE      
    { 128, 64 }, // GAMEPAD_UI_LEFT_TRIGGER  
    { 160, 64 }  // GAMEPAD_UI_RIGHT_TRIGGER 
};

static const struct TexCoords g_editions_tex_coords[CARD_EDITION_COUNT] = {
    // Base and Polychrome intentionally share the first editions tile.
    // Base never draws this overlay path; Polychrome uses it.
    { 1, 1 },   // CARD_EDITION_BASE
    { 143, 1 }, // CARD_EDITION_FOIL        
    { 72, 1 }, // CARD_EDITION_HOLOGRAPHIC        
    { 1, 1 }, // CARD_EDITION_POLYCHROME      
    { 1, 1 },   // CARD_EDITION_NEGATIVE  
};

static const struct TexCoords g_seal_tex_coords[CARD_SEAL_COUNT] = {
    { 0, 0 },       // CARD_SEAL_NONE
    { 154, 16 },    // CARD_SEAL_GOLD
    { 359, 414 },   // CARD_SEAL_RED
    { 431, 414 },   // CARD_SEAL_BLUE
    { 287, 414 }    // CARD_SEAL_PURPLE
};

static const struct TexCoords g_ui_assets_tex_coords[UI_ASSETS_COUNT] = {
    { 0, 0 }, // UI_ASSETS_CHIP
    { 18, 0 }, // UI_ASSETS_ACE
    { 36, 0 }, // UI_ASSETS_FACE
    { 54, 0 }, // UI_ASSETS_NUMBER
    { 0, 18 }, // UI_ASSETS_HEARTS
    { 18, 18 }, // UI_ASSETS_DIAMONDS
    { 36, 18 }, // UI_ASSETS_CLUBS
    { 54, 18 }  // UI_ASSETS_SPADES
};

struct BlindPanelStyle
{
    uint32_t panel_fill;
    uint32_t header_fill;
    uint32_t border_color;
    uint32_t score_color;
    uint32_t badge_color;
};

static const struct BlindPanelStyle g_blind_panel_styles[3] = {
    { 0xFF2D5B3A, 0xFF3C704D, 0xFF79B07A, 0xFF97E49A, 0xFF315A2D }, // Small
    { 0xFF3F476C, 0xFF4E5D87, 0xFF9BA9D6, 0xFFD3E1FF, 0xFF4A5170 }, // Big
    { 0xFF5E3550, 0xFF7A486A, 0xFFCAA1C0, 0xFFD8B8FF, 0xFF5A2E55 }  // Boss
};

static const struct BlindPanelStyle *game_get_blind_panel_style(int blind)
{
    if (blind < GAME_BLIND_SMALL || blind > GAME_BLIND_BOSS)
    {
        return &g_blind_panel_styles[GAME_BLIND_SMALL];
    }

    return &g_blind_panel_styles[blind];
}

#define ACTION_BUTTON_FOCUS_PAD        2.0f
#define ACTION_BUTTON_BORDER_PAD       1.0f
#define ACTION_BUTTON_ICON_SIZE        16.0f

#define HUD_BG_COLOR            0xFF42474F
#define HUD_RAIL_INNER_COLOR    0xFF3A3F47
#define HUD_SECTION_BORDER      0xFF2F343C
#define HUD_SECTION_BG_COLOR    0xFF4A4F58
#define HUD_SECTION_TOPBAR      0xFF5A616B
#define HUD_VALUE_BG_COLOR      0xFF343943
#define HUD_VALUE_ACCENT_COLOR  0xFF6B7686
#define HUD_LABEL_COLOR         0xFFE1E3E6
#define HUD_META_COLOR          0xFFC9CFD8

static void game_draw_pill(float x, float y, float w, float h, uint32_t color)
{
    // Procedural rounded rectangle (pill) using solid quads.
    // This avoids texture tinting issues and matches text box colors perfectly.
    float b = 2.0f; // Bevel size for corner rounding
    if (h < 10.0f || w < 10.0f) b = 1.0f;

    graphics_set_no_texture();

    // Main horizontal body section
    graphics_draw_solid_quad(x, y + b, w, h - b * 2.0f, color);
    
    // Top and Bottom "cap" segments
    graphics_draw_solid_quad(x + b, y, w - b * 2.0f, b, color);
    graphics_draw_solid_quad(x + b, y + h - b, w - b * 2.0f, b, color);
    
    if (b > 1.0f) {
        // Soften the inner corner transitions for a smoother round
        graphics_draw_solid_quad(x + 1.0f, y + 1.0f, 1.0f, 1.0f, color);
        graphics_draw_solid_quad(x + w - 2.0f, y + 1.0f, 1.0f, 1.0f, color);
        graphics_draw_solid_quad(x + 1.0f, y + h - 2.0f, 1.0f, 1.0f, color);
        graphics_draw_solid_quad(x + w - 2.0f, y + h - 2.0f, 1.0f, 1.0f, color);
    }
}

static void game_draw_hud_section(float x, float y, float w, float h)
{
    game_draw_pill(x, y, w, h, HUD_SECTION_BORDER);
    game_draw_pill(x + 1.0f, y + 1.0f, w - 2.0f, h - 2.0f, HUD_SECTION_BG_COLOR);
    
    // Decorative top accent pill
    float accent_w = w * 0.8f;
    game_draw_pill(x + (w - accent_w) / 2.0f, y + 1.0f, accent_w, 3.0f, HUD_SECTION_TOPBAR);
}

static void game_draw_hud_value_box(float x, float y, float w, float h)
{
    game_draw_pill(x, y, w, h, HUD_VALUE_ACCENT_COLOR);
    game_draw_pill(x + 1.0f, y + 1.0f, w - 2.0f, h - 2.0f, HUD_VALUE_BG_COLOR);
}

#define HAND_ACTION_PRIMARY_WIDTH      (UI_HAND_ACTIONS_CONTAINER_WIDTH * 0.30f)
#define HAND_ACTION_PRIMARY_HEIGHT     (UI_HAND_ACTIONS_CONTAINER_HEIGHT * 0.90f)
#define HAND_ACTION_SORT_WIDTH         (UI_HAND_ACTIONS_CONTAINER_WIDTH * 0.33f)
#define HAND_ACTION_SORT_HEIGHT        (UI_HAND_ACTIONS_CONTAINER_HEIGHT)
#define HAND_ACTION_ROW_GAP            (UI_HAND_ACTIONS_CONTAINER_WIDTH * 0.04f)
#define HAND_ACTION_ROW_Y_OFFSET       (SCREEN_HEIGHT * 0.01f)
#define HAND_ACTION_INNER_PAD          (UI_HAND_ACTIONS_CONTAINER_HEIGHT * 0.04f)
#define HAND_ACTION_SEGMENT_W          (HAND_ACTION_SORT_WIDTH * 0.40f)
#define HAND_ACTION_SEGMENT_H          (HAND_ACTION_SORT_HEIGHT * 0.42f)
#define HAND_ACTION_SEGMENT_GAP        (HAND_ACTION_SORT_WIDTH * 0.05f)

static void game_draw_action_button(float x, float y, float width, float height,
    const char *text, const char *text2,
    int gamepad_ui, bool icon_on_right,
    uint32_t button_color, bool focused)
{
    if (focused)
    {
        game_draw_pill(x - ACTION_BUTTON_FOCUS_PAD, y - ACTION_BUTTON_FOCUS_PAD,
            width + ACTION_BUTTON_FOCUS_PAD * 2.0f, height + ACTION_BUTTON_FOCUS_PAD * 2.0f,
            COLOR_WHITE);
    }

    game_draw_pill(x - ACTION_BUTTON_BORDER_PAD, y - ACTION_BUTTON_BORDER_PAD,
        width + ACTION_BUTTON_BORDER_PAD * 2.0f, height + ACTION_BUTTON_BORDER_PAD * 2.0f,
        0xAA202020);
    game_draw_pill(x, y, width, height, button_color);

    float text_center_x = x + width / 2.0f;
    float text_max_w = width * 0.85f; // Margin for button inner padding
    if (gamepad_ui >= 0)
    {
        int icon_u = 0;
        int icon_v = 32;
        switch (gamepad_ui)
        {
            case 0: icon_u = 0;   icon_v = 32; break;  // CROSS
            case 1: icon_u = 32;  icon_v = 32; break;  // CIRCLE
            case 2: icon_u = 64;  icon_v = 32; break;  // SQUARE
            case 3: icon_u = 96;  icon_v = 32; break;  // TRIANGLE
            case 4: icon_u = 128; icon_v = 64; break;  // L TRIGGER
            case 5: icon_u = 160; icon_v = 64; break;  // R TRIGGER
            default: break;
        }

        float icon_x = icon_on_right ? (x + width - ACTION_BUTTON_ICON_SIZE - 2.0f) : (x + 2.0f);
        float icon_y = y + (height - ACTION_BUTTON_ICON_SIZE) / 2.0f;
        graphics_set_texture(tex_gamepad_ui, GRAPHICS_TEXTURE_FILTER_LINEAR);
        graphics_draw_quad(icon_x, icon_y, ACTION_BUTTON_ICON_SIZE, ACTION_BUTTON_ICON_SIZE,
            icon_u,
            icon_v,
            32, 32, COLOR_WHITE);
        text_center_x += icon_on_right ? -8.0f : 8.0f;
        text_max_w -= (ACTION_BUTTON_ICON_SIZE + 4.0f);
    }

    if (text2 == NULL)
    {
        float scale = graphics_get_text_scale_to_fit(font_small, text, text_max_w, 1.0f);
        graphics_draw_text_center(font_small, text, text_center_x, y + (height / 2.0f), scale, COLOR_WHITE);
    }
    else
    {
        float s1 = graphics_get_text_scale_to_fit(font_small, text, text_max_w, 1.0f);
        float s2 = graphics_get_text_scale_to_fit(font_small, text2, text_max_w, 1.0f);
        graphics_draw_text_center(font_small, text, text_center_x, y + (height / 2.0f) - 4.0f, s1, COLOR_WHITE);
        graphics_draw_text_center(font_small, text2, text_center_x, y + (height / 2.0f) + 5.0f, s2, COLOR_WHITE);
    }
}

static int game_load_texture_16bit_with_fallback(const char *asset_key, const char *primary_path, const char *fallback_path)
{
    int texture = graphics_load_texture_from_archive_16bit(primary_path, 0, 0);
    if (texture >= 0)
    {
        return texture;
    }

    DEBUG_PRINTF("[DRAW][ASSET][FALLBACK] key=%s primary=%s fallback=%s\n", asset_key, primary_path, fallback_path);
    texture = graphics_load_texture_16bit(fallback_path, 0, 0);
    if (texture < 0)
    {
        DEBUG_PRINTF("[DRAW][ASSET][MISSING] key=%s primary=%s fallback=%s\n", asset_key, primary_path, fallback_path);
    }
    return texture;
}

static void game_draw_static_background_texture(int texture)
{
    int tex_width = 0;
    int tex_height = 0;
    if (!graphics_get_texture_content_size(texture, &tex_width, &tex_height) || tex_width <= 0 || tex_height <= 0)
    {
        return;
    }

    // Background is now downsampled to 512px during load, so a single
    // draw call captures 100% of the image without hardware truncation.
    graphics_set_texture(texture, GRAPHICS_TEXTURE_FILTER_LINEAR);
    graphics_draw_quad(0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT,
        0, 0, (int16_t)tex_width, (int16_t)tex_height,
        COLOR_WHITE);

    graphics_set_no_texture();
}

static struct Image game_load_image_with_fallback(const char *asset_key, const char *primary_path, const char *fallback_path)
{
    char logbuf[256];
    
    // Always try archive first, then local override
    DEBUG_PRINTF("[DRAW][LOAD] Trying primary from archive: %s\n", primary_path);
    struct Image image = graphics_load_image_from_archive(primary_path);
    
    if (image.data != NULL)
    {
        DEBUG_PRINTF("[DRAW][LOAD] Primary loaded from archive successfully\n");
        return image;
    }
    
    DEBUG_PRINTF("[DRAW][LOAD] Primary not in archive, trying fallback: %s\n", fallback_path);
    snprintf(logbuf, sizeof(logbuf), "[DRAW] Fallback loading: %s", asset_key);
    boot_log(logbuf);
    
    image = graphics_load_image(fallback_path);
    
    if (image.data == NULL)
    {
        snprintf(logbuf, sizeof(logbuf), "[DRAW][MISSING] key=%s primary=%s fallback=%s", asset_key, primary_path, fallback_path);
        boot_log(logbuf);
        DEBUG_PRINTF("[DRAW][ASSET][MISSING] key=%s primary=%s fallback=%s\n", asset_key, primary_path, fallback_path);
    }
    else
    {
        snprintf(logbuf, sizeof(logbuf), "[DRAW] Fallback success: %s", asset_key);
        boot_log(logbuf);
    }

    return image;
}

int32_t g_time = 0;

void game_init_draw_load_fonts()
{
    // font_big = graphics_load_font("font_big.png", 8, 8, 16, 16);
    // font_small = graphics_load_font("font_small.png", 6, 8, 16, 16);
    font_big = graphics_load_big_font();
    font_small = graphics_load_small_font();
}

bool game_init_draw()
{
    char str[64];
    
    DEBUG_PRINTF("[DRAW] game_init_draw: start\n");
    
    tex_enhancers = game_load_texture_16bit_with_fallback("enhancers", "resources/textures/1x/Enhancers.png", "balatro-textures/1x/Enhancers.png");
    DEBUG_PRINTF("[DRAW] enhancers result=%d\n", tex_enhancers);
    if (tex_enhancers < 0) { DEBUG_PRINTF("[DRAW] enhancers failed\n"); return false; }

    DEBUG_PRINTF("[DRAW] Loading deck\n");
    struct Image deck_image = game_load_image_with_fallback("deck", "resources/textures/1x/8BitDeck.png", "balatro-textures/1x/8BitDeck.png");
    DEBUG_PRINTF("[DRAW] deck_data=%p\n", (void*)deck_image.data);
    if (deck_image.data == NULL) { DEBUG_PRINTF("[DRAW] deck failed\n"); return false; }
    game_draw_loading_text("Creating deck texture 0", COLOR_WHITE, COLOR_BLACK);
    tex_deck = graphics_load_texture_from_image_16bit(&deck_image, 0, 0);
    game_draw_loading_text("Creating deck texture 1", COLOR_WHITE, COLOR_BLACK);
    tex_deck2 = graphics_load_texture_from_image_16bit(&deck_image, 71*7, 0);
    graphics_destroy_image(&deck_image);

    struct Image joker_image = game_load_image_with_fallback("jokers_atlas", "resources/textures/1x/Jokers.png", "balatro-cards-optimized/jokers/jokers_50_16bit.png");
    if (joker_image.data == NULL) return false;
    for(int i = 0; i < 2; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            DEBUG_PRINTF("Loading Jokers (%d; %d)\n", i, j);
            sprintf(str, "Creating joker texture %d", i*4+j);
            game_draw_loading_text(str, COLOR_WHITE, COLOR_BLACK);
            tex_jokers[i][j] = graphics_load_texture_from_image_16bit(&joker_image, i * (TEXTURE_CARD_WIDTH + 2) * 7, j * (TEXTURE_CARD_HEIGHT + 2) * 5);
        }
    }
    graphics_destroy_image(&joker_image);

    tex_gamepad_ui = game_load_texture_16bit_with_fallback("gamepad_ui", "resources/textures/1x/gamepad_ui.png", "balatro-textures/1x/gamepad_ui.png");
    if (tex_gamepad_ui < 0) return false;

    game_draw_loading_text("Loading Tarots.png", COLOR_WHITE, COLOR_BLACK);
    struct Image tarots_image = game_load_image_with_fallback("tarots_atlas", "resources/textures/1x/Tarots.png", "balatro-cards-optimized/tarots/tarots_psp.png");
    if (tarots_image.data == NULL) return false;
    for(int i = 0; i < 2; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            DEBUG_PRINTF("Loading Tarots (%d; %d)\n", i, j);
            sprintf(str, "Creating tarots texture %d", i*2+j);
            game_draw_loading_text(str, COLOR_WHITE, COLOR_BLACK);
            tex_tarots[i][j] = graphics_load_texture_from_image_16bit(&tarots_image, i * (TEXTURE_CARD_WIDTH + 2) * 7, j * (TEXTURE_CARD_HEIGHT + 2) * 5);
        }
    }
    graphics_destroy_image(&tarots_image);

    tex_editions = graphics_load_texture_16bit("editions.png", 0, 0);
    sprintf(str, "[DRAW] editions loaded=%d", tex_editions);
    boot_log(str);
    if (tex_editions < 0) { boot_log("[DRAW] editions failed"); return false; }

    boot_log("[DRAW] Loading boosters");
    struct Image boosters_image = game_load_image_with_fallback("boosters_atlas", "resources/textures/1x/boosters.png", "balatro-effects-optimized/boosters_psp.png");
    sprintf(str, "[DRAW] boosters_data=%p", (void*)boosters_image.data);
    boot_log(str);
    if (boosters_image.data == NULL) { boot_log("[DRAW] boosters failed"); return false; }
    for(int j = 0; j < 2; j++)
    {
        DEBUG_PRINTF("Loading boosters (%d)\n", j);
        sprintf(str, "Creating boosters texture %d", j);
        game_draw_loading_text(str, COLOR_WHITE, COLOR_BLACK);
        tex_boosters[j] = graphics_load_texture_from_image_16bit(&boosters_image, 0, j * (TEXTURE_CARD_HEIGHT + 2) * 5);
    }
    graphics_destroy_image(&boosters_image);

    tex_shop = game_load_texture_16bit_with_fallback("shop_sign", "resources/textures/1x/ShopSignAnimation.png", "balatro-textures/1x/ShopSignAnimation.png");
    if (tex_shop < 0) return false;

    tex_ui_assets = game_load_texture_16bit_with_fallback("ui_assets", "resources/textures/1x/ui_assets.png", "balatro-textures/1x/ui_assets.png");
    if (tex_ui_assets < 0) return false;

    tex_blind_chips = game_load_texture_16bit_with_fallback("blind_chips", "resources/textures/1x/BlindChips.png", "balatro-textures/1x/BlindChips.png");
    if (tex_blind_chips < 0) return false;

    tex_title_bg_alt = graphics_load_texture("assets/backgrounds/background.png", 0, 0);

    tex_blind_background = graphics_load_wallpaper("assets/wallpapers/galaxy_15_psp.png");
    if (tex_blind_background < 0)
    {
        tex_blind_background = graphics_load_wallpaper("assets/wallpapers/galaxy_15.webp");
    }
    if (tex_blind_background < 0)
    {
        tex_blind_background = graphics_load_wallpaper("assets/wallpapers/galaxy_15.jpeg");
    }
    if (tex_blind_background < 0)
    {
        DEBUG_PRINTF("[DRAW][ASSET][MISSING] key=blind_background paths=galaxy_15_psp.png|galaxy_15.webp|galaxy_15.jpeg\n");
    }

    sceKernelDcacheWritebackInvalidateAll();

    return true;
}

float g_loading_text_y = 0.0f;

void game_draw_clear_loading(uint32_t color)
{
    graphics_clear_loading_text(color);

    graphics_show_loading_text(font_small, "Port by S\x82rgio Vieira", SCREEN_WIDTH - 126.0f, SCREEN_HEIGHT - 16.0f, 1.0f, COLOR_LIGHT_GREY, false, COLOR_BLACK);
    graphics_show_loading_text(font_small, "https://github.com/internalregister", SCREEN_WIDTH - 210.0f, SCREEN_HEIGHT - 8.0f, 1.0f, COLOR_LIGHT_GREY, false, COLOR_BLACK);
}

void game_draw_loading_text(const char *text, uint32_t color, uint32_t back_color)
{
    if (g_loading_text_y == 0.0f) game_draw_clear_loading(back_color); 
    graphics_show_loading_text(font_small, text, 0, g_loading_text_y, 1.0f, color, false, COLOR_BLACK);
    g_loading_text_y += 8.0f;
    if (g_loading_text_y >= SCREEN_HEIGHT) g_loading_text_y = 0.0f;
}

void game_draw_card_hint(struct Card *card)
{
    int line_count = 1;
    if (card->extra_chips > 0) line_count++;
    if (card->enhancement != CARD_ENHANCEMENT_NONE && card->enhancement != CARD_ENHANCEMENT_STONE) line_count++;
    if (card->edition != CARD_EDITION_BASE) line_count++;
    if (card->seal != CARD_SEAL_NONE) line_count++;

    float left = card->draw.x - CARD_HINT_WIDTH / 2.0f + CARD_WIDTH / 2.0f;
    if (left > SCREEN_WIDTH - CARD_HINT_WIDTH)
    {
        left = SCREEN_WIDTH - CARD_HINT_WIDTH;
    }
    float height = CARD_HINT_MIN_HEIGHT + 8.0f * line_count;
    float top = card->draw.y - height - 4.0f;
    if (top < 0.0f)
    {
        top = card->draw.y + CARD_HEIGHT + 4.0f;
    }
    game_draw_pill(left, top, CARD_HINT_WIDTH, height, 0xFF888888);
    game_draw_pill(left + 2, top + CARD_HINT_MIN_HEIGHT - 12.0f, CARD_HINT_WIDTH - 4, 8.0f * line_count + 8.0f, COLOR_WHITE);

    char str[64];
    game_set_card_name(card, str);
    float name_scale = graphics_get_text_scale_to_fit(font_small, str, CARD_HINT_WIDTH * 0.9f, 1.0f);
    graphics_draw_text_center(font_small, str, left + CARD_HINT_WIDTH / 2.0f, top + 10.0f, name_scale, COLOR_WHITE);
    
    sprintf(str, "#3+%d#- chips", game_util_get_card_original_chips(card));
    float text_y = top + 14.0f + 12.0f;
    float line_scale = graphics_get_text_formatted_scale_to_fit(font_small, str, NULL, CARD_HINT_WIDTH * 0.9f, 1.0f);
    graphics_draw_text_formatted_center(font_small, str, NULL, left + CARD_HINT_WIDTH / 2.0f, text_y, line_scale, COLOR_BLACK);
    text_y += 8.0f;

    if (card->extra_chips > 0)
    {
        sprintf(str, "#3+%d#- extra chips", card->extra_chips);
        line_scale = graphics_get_text_formatted_scale_to_fit(font_small, str, NULL, CARD_HINT_WIDTH * 0.9f, 1.0f);
        graphics_draw_text_formatted_center(font_small, str, NULL, left + CARD_HINT_WIDTH / 2.0f, text_y, line_scale, COLOR_BLACK);
        text_y += 8.0f;
    }

    if (card->enhancement != CARD_ENHANCEMENT_NONE && card->enhancement != CARD_ENHANCEMENT_STONE)
    {
        line_scale = graphics_get_text_formatted_scale_to_fit(font_small, g_enhancement_hint[card->enhancement], NULL, CARD_HINT_WIDTH * 0.9f, 1.0f);
        graphics_draw_text_formatted_center(font_small, g_enhancement_hint[card->enhancement], NULL, left + CARD_HINT_WIDTH / 2.0f, text_y, line_scale, COLOR_BLACK);
        text_y += 8.0f;
    }

    if (card->edition != CARD_EDITION_BASE)
    {
        line_scale = graphics_get_text_formatted_scale_to_fit(font_small, g_edition_hint[card->edition], NULL, CARD_HINT_WIDTH * 0.9f, 1.0f);
        graphics_draw_text_formatted_center(font_small, g_edition_hint[card->edition], NULL, left + CARD_HINT_WIDTH / 2.0f, text_y, line_scale, COLOR_BLACK);
        text_y += 8.0f;
    }

    if (card->seal != CARD_SEAL_NONE)
    {
        line_scale = graphics_get_text_formatted_scale_to_fit(font_small, g_seal_hint[card->seal], NULL, CARD_HINT_WIDTH * 0.9f, 1.0f);
        graphics_draw_text_formatted_center(font_small, g_seal_hint[card->seal], NULL, left + CARD_HINT_WIDTH / 2.0f, text_y, line_scale, COLOR_BLACK);
    }
}

void game_draw_joker_hint(struct Joker *joker)
{
    int line_count = 0;
    for (int i = HINT_JOKER_TYPE_LENGTH - 1; i >= 0; i--)
    {
        if (g_joker_types[joker->type].hint_lines[i][0] != '\0')
        {
            line_count = i + 1;
            break;
        }
    }

    float top = joker->draw.y + CARD_HEIGHT + 4.0f;
    float height = CARD_HINT_MIN_HEIGHT + 8.0f * line_count + 10.0f + (joker->edition != CARD_EDITION_BASE ? 10.0f : 0.0f);
    float left = joker->draw.x - CARD_HINT_WIDTH / 2.0f + CARD_WIDTH / 2.0f;
    if (left > SCREEN_WIDTH - CARD_HINT_WIDTH)
    {
        left = SCREEN_WIDTH - CARD_HINT_WIDTH;
    }

    if (top + height > SCREEN_HEIGHT)
    {
        top = joker->draw.y - height - 4.0f;
    }

    game_draw_pill(left, top, CARD_HINT_WIDTH, height, 0xFF888888);
    game_draw_pill(left + 2, top + CARD_HINT_MIN_HEIGHT - 12.0f, CARD_HINT_WIDTH - 4, height - CARD_HINT_MIN_HEIGHT + 8.0f, COLOR_WHITE);

    float name_scale = graphics_get_text_scale_to_fit(font_small, g_joker_types[joker->type].name, CARD_HINT_WIDTH * 0.9f, 1.0f);
    graphics_draw_text_center(font_small, g_joker_types[joker->type].name, left + CARD_HINT_WIDTH / 2.0f, top + 10.0f, name_scale, COLOR_WHITE);

    for (int i = 0; i < line_count; i++)
    {
        float line_scale = graphics_get_text_formatted_scale_to_fit(font_small, g_joker_types[joker->type].hint_lines[i], (void*)joker, CARD_HINT_WIDTH * 0.9f, 1.0f);
        graphics_draw_text_formatted_center(font_small, g_joker_types[joker->type].hint_lines[i], (void*)joker, left + CARD_HINT_WIDTH / 2.0f, top + 14.0f + 12.0f + (8.0f * i), line_scale, COLOR_BLACK);
    }

    float current_y = top + 14.0f + 12.0f + (8.0f * line_count);

    if (joker->edition != CARD_EDITION_BASE)
    {
        float line_scale = graphics_get_text_formatted_scale_to_fit(font_small, g_edition_hint[joker->edition], NULL, CARD_HINT_WIDTH * 0.9f, 1.0f);
        graphics_draw_text_formatted_center(font_small, g_edition_hint[joker->edition], NULL, left + CARD_HINT_WIDTH / 2.0f, current_y + 2.0f, line_scale, COLOR_BLACK);
        current_y += 10.0f;
    }

    float line_scale = graphics_get_text_formatted_scale_to_fit(font_small, g_rarity_hint[g_joker_types[joker->type].rarity], NULL, CARD_HINT_WIDTH * 0.9f, 1.0f);
    graphics_draw_text_formatted_center(font_small, g_rarity_hint[g_joker_types[joker->type].rarity], NULL, left + CARD_HINT_WIDTH / 2.0f, current_y + 2.0f, line_scale, COLOR_BLACK);
}

void game_draw_tarot_hint(struct Tarot *tarot)
{
    int line_count = 0;
    for (int i = HINT_TAROT_TYPE_LENGTH - 1; i >= 0; i--)
    {
        if (g_tarot_types[tarot->type].hint_lines[i][0] != '\0')
        {
            line_count = i + 1;
            break;
        }
    }

    float top = tarot->draw.y + CARD_HEIGHT + 4.0f;
    float height = CARD_HINT_MIN_HEIGHT + 8.0f * line_count;    
    float left = tarot->draw.x - CARD_HINT_WIDTH / 2.0f + CARD_WIDTH / 2.0f;
    if (left > SCREEN_WIDTH - CARD_HINT_WIDTH)
    {
        left = SCREEN_WIDTH - CARD_HINT_WIDTH;
    }

    if (top + height > SCREEN_HEIGHT)
    {
        top = tarot->draw.y - height - 4.0f;
    }

    game_draw_pill(left, top, CARD_HINT_WIDTH, height, 0xFF888888);
    game_draw_pill(left + 2, top + CARD_HINT_MIN_HEIGHT - 12.0f, CARD_HINT_WIDTH - 4, 8.0f * line_count + 8.0f, COLOR_WHITE);

    float name_scale = graphics_get_text_scale_to_fit(font_small, g_tarot_types[tarot->type].name, CARD_HINT_WIDTH * 0.9f, 1.0f);
    graphics_draw_text_center(font_small, g_tarot_types[tarot->type].name, left + CARD_HINT_WIDTH / 2.0f, top + 10.0f, name_scale, COLOR_WHITE);
        

    for (int i = 0; i < line_count; i++)
    {
        float line_scale = graphics_get_text_formatted_scale_to_fit(font_small, g_tarot_types[tarot->type].hint_lines[i], (void*)tarot, CARD_HINT_WIDTH * 0.9f, 1.0f);
        graphics_draw_text_formatted_center(font_small, g_tarot_types[tarot->type].hint_lines[i], (void*)tarot, left + CARD_HINT_WIDTH / 2.0f, top + 14.0f + 12.0f + (8.0f * i), line_scale, COLOR_BLACK);
    }
}

void game_draw_planet_hint(struct Planet *planet)
{
    char str[32];
    int line_count = 4;

    float top = planet->draw.y + CARD_HEIGHT + 4.0f;
    float height = CARD_HINT_MIN_HEIGHT + 8.0f * line_count;    
    float left = planet->draw.x - CARD_HINT_WIDTH / 2.0f + CARD_WIDTH / 2.0f;
    if (left > SCREEN_WIDTH - CARD_HINT_WIDTH)
    {
        left = SCREEN_WIDTH - CARD_HINT_WIDTH;
    }

    if (top + height > SCREEN_HEIGHT)
    {
        top = planet->draw.y - height - 4.0f;
    }

    game_draw_pill(left, top, CARD_HINT_WIDTH, height, 0xFF888888);
    game_draw_pill(left + 2, top + CARD_HINT_MIN_HEIGHT - 12.0f, CARD_HINT_WIDTH - 4, 8.0f * line_count + 8.0f, COLOR_WHITE);

    float name_scale = graphics_get_text_scale_to_fit(font_small, g_planet_types[planet->type].name, CARD_HINT_WIDTH * 0.9f, 1.0f);
    graphics_draw_text_center(font_small, g_planet_types[planet->type].name, left + CARD_HINT_WIDTH / 2.0f, top + 10.0f, name_scale, COLOR_WHITE);
    
    struct PlanetType *planet_type = &(g_planet_types[planet->type]);

    float text_y = top + 14.0f + 12.0f;
    sprintf(str, "[lvl.%d] Level Up", g_game_state.poker_hand_level[planet_type->poker_hand]);
    float line_scale = graphics_get_text_formatted_scale_to_fit(font_small, str, (void*)planet, CARD_HINT_WIDTH * 0.9f, 1.0f);
    graphics_draw_text_formatted_center(font_small, str, (void*)planet, left + CARD_HINT_WIDTH / 2.0f, text_y, line_scale, COLOR_BLACK);
    text_y += 8.0f;
    sprintf(str, "#5%s", g_poker_hand_names[planet_type->poker_hand]);
    line_scale = graphics_get_text_formatted_scale_to_fit(font_small, str, (void*)planet, CARD_HINT_WIDTH * 0.9f, 1.0f);
    graphics_draw_text_formatted_center(font_small, str, (void*)planet, left + CARD_HINT_WIDTH / 2.0f, text_y, line_scale, COLOR_BLACK);
    text_y += 8.0f;
    sprintf(str, "#2+%d#- Mult and", planet_type->mult);
    line_scale = graphics_get_text_formatted_scale_to_fit(font_small, str, NULL, CARD_HINT_WIDTH * 0.9f, 1.0f);
    graphics_draw_text_formatted_center(font_small, str, NULL, left + CARD_HINT_WIDTH / 2.0f, text_y, line_scale, COLOR_BLACK);
    text_y += 8.0f;
    sprintf(str, "#3+%d#- chips", planet_type->chips);
    line_scale = graphics_get_text_formatted_scale_to_fit(font_small, str, NULL, CARD_HINT_WIDTH * 0.9f, 1.0f);
    graphics_draw_text_formatted_center(font_small, str, NULL, left + CARD_HINT_WIDTH / 2.0f, text_y, line_scale, COLOR_BLACK);
}

void game_draw_booster_hint(struct BoosterPack *booster)
{
    struct BoosterPackType *booster_type = &(g_booster_types[booster->type][booster->size][booster->image]);

    int line_count = 0;
    for (int i = HINT_BOOSTER_TYPE_LENGTH - 1; i >= 0; i--)
    {
        if (booster_type->hint_lines[i][0] != '\0')
        {
            line_count = i + 1;
            break;
        }
    }

    float top = booster->draw.y + CARD_HEIGHT + 4.0f;
    float height = CARD_HINT_MIN_HEIGHT + 8.0f * line_count;    
    float left = booster->draw.x - CARD_HINT_WIDTH / 2.0f + CARD_WIDTH / 2.0f;

    if (top + height > SCREEN_HEIGHT)
    {
        top = booster->draw.y - height - 14.0f;
    }

    game_draw_pill(left, top, CARD_HINT_WIDTH, height, 0xFF888888);
    game_draw_pill(left + 2, top + CARD_HINT_MIN_HEIGHT - 12.0f, CARD_HINT_WIDTH - 4, 8.0f * line_count + 8.0f, COLOR_WHITE);

    float name_scale = graphics_get_text_scale_to_fit(font_small, booster_type->name, CARD_HINT_WIDTH * 0.9f, 1.0f);
    graphics_draw_text_center(font_small, booster_type->name, left + CARD_HINT_WIDTH / 2.0f, top + 10.0f, name_scale, COLOR_WHITE);        

    for (int i = 0; i < line_count; i++)
    {
        float line_scale = graphics_get_text_formatted_scale_to_fit(font_small, booster_type->hint_lines[i], NULL, CARD_HINT_WIDTH * 0.9f, 1.0f);
        graphics_draw_text_formatted_center(font_small, booster_type->hint_lines[i], NULL, left + CARD_HINT_WIDTH / 2.0f, top + 14.0f + 12.0f + (8.0f * i), line_scale, COLOR_BLACK);
    }
}

static bool g_freeze_cards = false;

float sin_table[256] = {
 0.0000000f,  0.0245412f,  0.0490677f,  0.0735646f,  0.0980171f,  0.1224107f,  0.1467305f,  0.1709619f,
 0.1950903f,  0.2191012f,  0.2429802f,  0.2667128f,  0.2902847f,  0.3136818f,  0.3368898f,  0.3598951f,
 0.3826835f,  0.4052413f,  0.4275551f,  0.4496113f,  0.4713967f,  0.4928982f,  0.5141027f,  0.5349976f,
 0.5555702f,  0.5758082f,  0.5956993f,  0.6152316f,  0.6343933f,  0.6531729f,  0.6715590f,  0.6895406f,
 0.7071068f,  0.7242470f,  0.7409511f,  0.7572088f,  0.7730104f,  0.7883464f,  0.8032075f,  0.8175848f,
 0.8314696f,  0.8448536f,  0.8577286f,  0.8700870f,  0.8819212f,  0.8932243f,  0.9039893f,  0.9142098f,
 0.9238795f,  0.9329928f,  0.9415441f,  0.9495282f,  0.9569404f,  0.9637761f,  0.9700313f,  0.9757021f,
 0.9807853f,  0.9852777f,  0.9891765f,  0.9924796f,  0.9951847f,  0.9972904f,  0.9987954f,  0.9996988f,
 1.0000000f,  0.9996988f,  0.9987954f,  0.9972904f,  0.9951847f,  0.9924795f,  0.9891765f,  0.9852777f,
 0.9807853f,  0.9757022f,  0.9700313f,  0.9637761f,  0.9569403f,  0.9495282f,  0.9415441f,  0.9329928f,
 0.9238796f,  0.9142097f,  0.9039893f,  0.8932243f,  0.8819213f,  0.8700870f,  0.8577286f,  0.8448535f,
 0.8314697f,  0.8175848f,  0.8032075f,  0.7883465f,  0.7730105f,  0.7572088f,  0.7409511f,  0.7242471f,
 0.7071068f,  0.6895405f,  0.6715590f,  0.6531729f,  0.6343933f,  0.6152315f,  0.5956994f,  0.5758082f,
 0.5555702f,  0.5349977f,  0.5141028f,  0.4928981f,  0.4713968f,  0.4496114f,  0.4275551f,  0.4052412f,
 0.3826835f,  0.3598951f,  0.3368898f,  0.3136818f,  0.2902847f,  0.2667127f,  0.2429801f,  0.2191013f,
 0.1950903f,  0.1709618f,  0.1467306f,  0.1224107f,  0.0980171f,  0.0735644f,  0.0490677f,  0.0245412f,
-0.0000001f, -0.0245411f, -0.0490677f, -0.0735646f, -0.0980170f, -0.1224106f, -0.1467305f, -0.1709620f,
-0.1950902f, -0.2191012f, -0.2429802f, -0.2667127f, -0.2902847f, -0.3136818f, -0.3368900f, -0.3598950f,
-0.3826834f, -0.4052414f, -0.4275550f, -0.4496113f, -0.4713968f, -0.4928983f, -0.5141027f, -0.5349976f,
-0.5555703f, -0.5758081f, -0.5956993f, -0.6152316f, -0.6343932f, -0.6531728f, -0.6715590f, -0.6895406f,
-0.7071067f, -0.7242471f, -0.7409512f, -0.7572088f, -0.7730104f, -0.7883465f, -0.8032076f, -0.8175849f,
-0.8314695f, -0.8448535f, -0.8577285f, -0.8700870f, -0.8819213f, -0.8932244f, -0.9039894f, -0.9142097f,
-0.9238795f, -0.9329928f, -0.9415441f, -0.9495282f, -0.9569404f, -0.9637761f, -0.9700312f, -0.9757021f,
-0.9807853f, -0.9852777f, -0.9891765f, -0.9924796f, -0.9951847f, -0.9972904f, -0.9987954f, -0.9996988f,
-1.0000000f, -0.9996988f, -0.9987954f, -0.9972904f, -0.9951847f, -0.9924796f, -0.9891765f, -0.9852777f,
-0.9807853f, -0.9757021f, -0.9700312f, -0.9637761f, -0.9569404f, -0.9495282f, -0.9415441f, -0.9329928f,
-0.9238794f, -0.9142098f, -0.9039894f, -0.8932244f, -0.8819213f, -0.8700870f, -0.8577285f, -0.8448535f,
-0.8314697f, -0.8175849f, -0.8032076f, -0.7883464f, -0.7730104f, -0.7572088f, -0.7409510f, -0.7242472f,
-0.7071069f, -0.6895406f, -0.6715590f, -0.6531728f, -0.6343932f, -0.6152315f, -0.5956995f, -0.5758083f,
-0.5555703f, -0.5349976f, -0.5141027f, -0.4928981f, -0.4713965f, -0.4496115f, -0.4275552f, -0.4052414f,
-0.3826834f, -0.3598950f, -0.3368897f, -0.3136815f, -0.2902848f, -0.2667129f, -0.2429802f, -0.2191012f,
-0.1950902f, -0.1709617f, -0.1467302f, -0.1224108f, -0.0980172f, -0.0735646f, -0.0490676f, -0.0245411f
};

float cos_table[256] = {
 1.0000000f,  0.9996988f,  0.9987954f,  0.9972904f,  0.9951847f,  0.9924796f,  0.9891765f,  0.9852777f,
 0.9807853f,  0.9757021f,  0.9700313f,  0.9637761f,  0.9569404f,  0.9495282f,  0.9415441f,  0.9329928f,
 0.9238795f,  0.9142098f,  0.9039893f,  0.8932243f,  0.8819213f,  0.8700870f,  0.8577286f,  0.8448536f,
 0.8314696f,  0.8175848f,  0.8032075f,  0.7883465f,  0.7730105f,  0.7572088f,  0.7409511f,  0.7242471f,
 0.7071068f,  0.6895406f,  0.6715590f,  0.6531729f,  0.6343933f,  0.6152316f,  0.5956993f,  0.5758082f,
 0.5555702f,  0.5349976f,  0.5141028f,  0.4928982f,  0.4713968f,  0.4496113f,  0.4275551f,  0.4052413f,
 0.3826834f,  0.3598951f,  0.3368898f,  0.3136818f,  0.2902846f,  0.2667128f,  0.2429802f,  0.2191012f,
 0.1950904f,  0.1709619f,  0.1467305f,  0.1224106f,  0.0980171f,  0.0735646f,  0.0490676f,  0.0245413f,
-0.0000000f, -0.0245412f, -0.0490676f, -0.0735646f, -0.0980171f, -0.1224107f, -0.1467305f, -0.1709619f,
-0.1950903f, -0.2191012f, -0.2429802f, -0.2667127f, -0.2902847f, -0.3136817f, -0.3368898f, -0.3598951f,
-0.3826834f, -0.4052413f, -0.4275551f, -0.4496114f, -0.4713966f, -0.4928982f, -0.5141028f, -0.5349977f,
-0.5555702f, -0.5758082f, -0.5956994f, -0.6152315f, -0.6343933f, -0.6531729f, -0.6715590f, -0.6895405f,
-0.7071068f, -0.7242472f, -0.7409511f, -0.7572088f, -0.7730105f, -0.7883465f, -0.8032075f, -0.8175848f,
-0.8314697f, -0.8448535f, -0.8577286f, -0.8700870f, -0.8819212f, -0.8932243f, -0.9039893f, -0.9142098f,
-0.9238795f, -0.9329928f, -0.9415441f, -0.9495282f, -0.9569404f, -0.9637761f, -0.9700313f, -0.9757021f,
-0.9807853f, -0.9852777f, -0.9891765f, -0.9924795f, -0.9951847f, -0.9972905f, -0.9987954f, -0.9996988f,
-1.0000000f, -0.9996988f, -0.9987954f, -0.9972904f, -0.9951847f, -0.9924796f, -0.9891765f, -0.9852777f,
-0.9807853f, -0.9757021f, -0.9700313f, -0.9637761f, -0.9569404f, -0.9495282f, -0.9415441f, -0.9329928f,
-0.9238795f, -0.9142097f, -0.9039893f, -0.8932243f, -0.8819212f, -0.8700869f, -0.8577287f, -0.8448536f,
-0.8314695f, -0.8175849f, -0.8032075f, -0.7883464f, -0.7730106f, -0.7572089f, -0.7409511f, -0.7242470f,
-0.7071068f, -0.6895406f, -0.6715589f, -0.6531729f, -0.6343933f, -0.6152316f, -0.5956992f, -0.5758080f,
-0.5555704f, -0.5349978f, -0.5141028f, -0.4928982f, -0.4713967f, -0.4496112f, -0.4275549f, -0.4052415f,
-0.3826836f, -0.3598951f, -0.3368899f, -0.3136817f, -0.2902845f, -0.2667126f, -0.2429804f, -0.2191014f,
-0.1950904f, -0.1709619f, -0.1467304f, -0.1224105f, -0.0980169f, -0.0735648f, -0.0490678f, -0.0245413f,
 0.0000000f,  0.0245413f,  0.0490678f,  0.0735648f,  0.0980170f,  0.1224106f,  0.1467304f,  0.1709619f,
 0.1950904f,  0.2191014f,  0.2429804f,  0.2667126f,  0.2902846f,  0.3136817f,  0.3368899f,  0.3598951f,
 0.3826836f,  0.4052411f,  0.4275549f,  0.4496112f,  0.4713967f,  0.4928982f,  0.5141028f,  0.5349978f,
 0.5555701f,  0.5758080f,  0.5956993f,  0.6152316f,  0.6343933f,  0.6531729f,  0.6715591f,  0.6895404f,
 0.7071066f,  0.7242470f,  0.7409511f,  0.7572089f,  0.7730106f,  0.7883465f,  0.8032074f,  0.8175848f,
 0.8314696f,  0.8448536f,  0.8577287f,  0.8700871f,  0.8819214f,  0.8932242f,  0.9039893f,  0.9142097f,
 0.9238796f,  0.9329928f,  0.9415441f,  0.9495283f,  0.9569403f,  0.9637761f,  0.9700313f,  0.9757021f,
 0.9807853f,  0.9852777f,  0.9891766f,  0.9924795f,  0.9951847f,  0.9972904f,  0.9987954f,  0.9996988f
};

void game_draw_get_oscillating_position(struct DrawObject *draw, float *x, float *y)
{
    *x = draw->x;
    *y = draw->y;

    if (g_game_state.stage != GAME_STAGE_SHOP && g_settings.move_cards && !g_freeze_cards)
    {
        float t = (float)(g_time + draw->r) / 40.0f;
        float t2 = t / (2 * M_PI);
        int t3 = (t2 - (float)((int)t2)) * 256;
        t3 = CLAMP(t3, 0, 255);
        *x = draw->x + cos_table[t3] * 1.0f;
        *y = draw->y + sin_table[t3] * 1.1f;
    }
}

static bool game_is_shop_item_sharpness_enabled()
{
    return g_game_state.stage == GAME_STAGE_SHOP;
}

static int game_get_item_texture_filter()
{
    return game_is_shop_item_sharpness_enabled() ? GRAPHICS_TEXTURE_FILTER_NEAREST : GRAPHICS_TEXTURE_FILTER_LINEAR;
}

static void game_apply_shop_item_pixel_snap(float *x, float *y)
{
    if (game_is_shop_item_sharpness_enabled())
    {
        *x = floorf(*x + 0.5f);
        *y = floorf(*y + 0.5f);
    }
}

void game_draw_card(struct Card *card, struct DrawObject *draw_override)
{
    struct DrawObject *draw = &card->draw;
    if (draw_override != NULL)
    {
        draw = draw_override;
    }

    float x, y;
    game_draw_get_oscillating_position(draw, &x, &y);

    x -= ((CARD_WIDTH * draw->scale) - CARD_WIDTH) / 2.0f;
    y -= ((CARD_HEIGHT * draw->scale) - CARD_HEIGHT) / 2.0f;
    game_apply_shop_item_pixel_snap(&x, &y);
    float w = CARD_WIDTH * draw->scale;
    float h = CARD_HEIGHT * draw->scale;
    int card_filter = game_get_item_texture_filter();

    graphics_set_texture(tex_enhancers, card_filter);
    graphics_draw_rotated_quad(x, y, w, h, g_enhancement_tex_coords[card->enhancement].x, g_enhancement_tex_coords[card->enhancement].y, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, COLOR_WHITE, card->draw.angle);

    if (card->enhancement != CARD_ENHANCEMENT_STONE)
    {
        if (card->rank < 7)
        {
            graphics_set_texture(tex_deck, card_filter);
            graphics_draw_rotated_quad(x, y, w, h, 1 + (TEXTURE_CARD_WIDTH + 2) * card->rank, 1 + (TEXTURE_CARD_HEIGHT + 2) * card->suit, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, COLOR_WHITE, card->draw.angle);
        }
        else
        {
            graphics_set_texture(tex_deck2, card_filter);
            graphics_draw_rotated_quad(x, y, w, h, 1 + (TEXTURE_CARD_WIDTH + 2) * (card->rank - 7), 1 + (TEXTURE_CARD_HEIGHT + 2) * card->suit, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, COLOR_WHITE, card->draw.angle);
        }
    }

    if (card->edition != CARD_EDITION_BASE && card->edition != CARD_EDITION_NEGATIVE)
    {        
        graphics_set_texture(tex_editions, card_filter);
        graphics_draw_rotated_quad(x, y, w, h, g_editions_tex_coords[card->edition].x, g_editions_tex_coords[card->edition].y, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, 0x7FFFFFFF, card->draw.angle);
    }

    if (card->seal != CARD_SEAL_NONE)
    {
        float seal_w = 16.0f * draw->scale;
        float seal_h = 16.0f * draw->scale;
        float seal_x = x + (w - seal_w) / 2.0f;
        float seal_y = y + h - seal_h - (2.0f * draw->scale);

        graphics_set_texture(tex_enhancers, card_filter);
        graphics_draw_rotated_quad(
            seal_x, seal_y, seal_w, seal_h,
            g_seal_tex_coords[card->seal].x, g_seal_tex_coords[card->seal].y,
            SEAL_TEXTURE_WIDTH, SEAL_TEXTURE_HEIGHT,
            COLOR_WHITE, card->draw.angle);
    }

    if (card->draw.white_factor > 0.0f)
    {
        graphics_set_texture(tex_enhancers, card_filter);
        uint32_t color = 0xFFFFFF | ((uint32_t)(255.0f * (card->draw.white_factor > 1.0f ? 1.0f : card->draw.white_factor))<<24);        
        graphics_draw_quad(x, y, w, h, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].x, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].y, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, color);        
    }
}

void game_draw_joker(struct Joker *joker)
{
    int tex_joker_x, tex_joker_y = 0;
    struct JokerType *joker_type = &g_joker_types[joker->type];
    tex_joker_x = joker_type->u / 7;
    tex_joker_y = joker_type->v / 5; 
    
    float x, y;
    game_draw_get_oscillating_position(&(joker->draw), &x, &y);

    x -= ((CARD_WIDTH * joker->draw.scale) - CARD_WIDTH) / 2.0f;
    y -= ((CARD_HEIGHT * joker->draw.scale) - CARD_HEIGHT) / 2.0f;
    game_apply_shop_item_pixel_snap(&x, &y);
    float w = CARD_WIDTH * joker->draw.scale;
    float h = CARD_HEIGHT * joker->draw.scale;
    int joker_filter = game_get_item_texture_filter();

    if (joker->edition == CARD_EDITION_NEGATIVE)
    {
        graphics_flush_quads();    
        sceGuEnable(GU_COLOR_LOGIC_OP);
        sceGuLogicalOp(GU_COPY_INVERTED);    
        sceGuEnable(GU_ALPHA_TEST);
        sceGuAlphaFunc(GU_GREATER, 0, 0xFF);
    }

    graphics_set_texture(tex_jokers[tex_joker_x][tex_joker_y], joker_filter);
    graphics_draw_rotated_quad(
        x, y, w, h,
        1 + (joker_type->u - tex_joker_x * 7) * (TEXTURE_CARD_WIDTH + 2),
        1 + (joker_type->v - tex_joker_y * 5) * (TEXTURE_CARD_HEIGHT + 2),
        TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, 0xFFFFFFFF, joker->draw.angle);

    if (joker->type == JOKER_TYPE_HOLOGRAM)
    {
        tex_joker_x = 2 / 7;
        tex_joker_y = 9 / 5;
        graphics_set_texture(tex_jokers[tex_joker_x][tex_joker_y], joker_filter);
        graphics_draw_rotated_quad(
            x, y, w, h,
            1 + (2 - tex_joker_x * 7) * (TEXTURE_CARD_WIDTH + 2),
            1 + (9 - tex_joker_y * 5) * (TEXTURE_CARD_HEIGHT + 2),
            TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, 0xFFFFFFFF, joker->draw.angle);
    }

    if (joker->edition == CARD_EDITION_NEGATIVE)
    {
        graphics_flush_quads();
        sceGuDisable(GU_ALPHA_TEST);
        sceGuDisable(GU_COLOR_LOGIC_OP);
    }

    if (joker->edition != CARD_EDITION_BASE && joker->edition != CARD_EDITION_NEGATIVE)
    {        
        graphics_set_texture(tex_editions, joker_filter);
        graphics_draw_rotated_quad(x, y, w, h, g_editions_tex_coords[joker->edition].x, g_editions_tex_coords[joker->edition].y, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, 0x7FFFFFFF, joker->draw.angle);
    }
}

void game_draw_card_frame(struct DrawObject *draw)
{
    game_draw_pill(draw->x - 3.0f, draw->y - 3.0f, CARD_WIDTH + 6.0f, CARD_HEIGHT + 6.0f, COLOR_WHITE);
}

void game_draw_booster_frame(struct DrawObject *draw)
{
    game_draw_pill(draw->x - 3.0f, draw->y - 3.0f, BOOSTER_WIDTH + 6.0f, BOOSTER_HEIGHT + 6.0f, COLOR_WHITE);
}

void game_draw_item_button(struct DrawObject *draw, int gamepad_ui, char *text, char *text2, uint32_t button_color, bool left)
{
    float item_x = draw->x, item_y = draw->y;
    float x = left ? item_x - 40.0f : item_x + CARD_WIDTH;
    float y = item_y + (CARD_HEIGHT / 2.0f) - 20.0f;
    float width = 40.0f;
    float height = text2 != NULL ? 35.0f : 30.0f;
    game_draw_action_button(x, y, width, height, text, text2, gamepad_ui, false, button_color, false);
}

void game_draw_price_tag(struct DrawObject *draw, int price)
{
    float card_x = draw->x, card_y = draw->y;
    // Centered above the item with a "beveled" (pill-with-border) look.
    float tag_y = card_y - 15.0f;
    float tag_w = 30.0f;
    float tag_h = 13.0f;
    float centerX = card_x + (CARD_WIDTH / 2.0f);

    // Outer bevel/border
    game_draw_pill(centerX - (tag_w / 2.0f) - 1.0f, tag_y - 1.0f, tag_w + 2.0f, tag_h + 2.0f, 0xFF1A1A1A);
    game_draw_pill(centerX - (tag_w / 2.0f), tag_y, tag_w, tag_h, COLOR_DARK_GREY_2);

    char str[16];
    sprintf(str, "$%d", price);
    graphics_draw_text_center(font_small, str, centerX, tag_y + 7.0f, 1.0f, COLOR_WHITE);
}

void game_draw_tarot(struct Tarot *tarot)
{
    int tex_tarot_x, tex_tarot_y = 0;
    struct TarotType *tarot_type = &g_tarot_types[tarot->type];
    tex_tarot_x = tarot_type->u / 7;
    tex_tarot_y = tarot_type->v / 5;

    float x, y;
    game_draw_get_oscillating_position(&(tarot->draw), &x, &y);
    game_apply_shop_item_pixel_snap(&x, &y);
    int tarot_filter = game_get_item_texture_filter();

    graphics_set_texture(tex_tarots[tex_tarot_x][tex_tarot_y], tarot_filter);
    graphics_draw_rotated_quad(
        x, y, CARD_WIDTH, CARD_HEIGHT,
        1 + (tarot_type->u - tex_tarot_x * 7) * (TEXTURE_CARD_WIDTH + 2),
        1 + (tarot_type->v - tex_tarot_y * 5) * (TEXTURE_CARD_HEIGHT + 2),
        TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, COLOR_WHITE, tarot->draw.angle);
    
    if (tarot->draw.white_factor > 0.0f)
    {
        graphics_set_texture(tex_enhancers, tarot_filter);
        uint32_t color = 0xFFFFFF | ((uint32_t)(255.0f * (tarot->draw.white_factor > 1.0f ? 1.0f : tarot->draw.white_factor))<<24);        
        graphics_draw_quad(x, y, CARD_WIDTH, CARD_HEIGHT, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].x, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].y, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, color);
    }
}

void game_draw_planet(struct Planet *planet)
{
    int tex_planet_x, tex_planet_y = 0;
    struct PlanetType *planet_type = &g_planet_types[planet->type];
    tex_planet_x = planet_type->u / 7;
    tex_planet_y = planet_type->v / 5;

    float x, y;
    game_draw_get_oscillating_position(&(planet->draw), &x, &y);
    game_apply_shop_item_pixel_snap(&x, &y);
    int planet_filter = game_get_item_texture_filter();
    
    graphics_set_texture(tex_tarots[tex_planet_x][tex_planet_y], planet_filter);
    graphics_draw_rotated_quad(
        x, y, CARD_WIDTH, CARD_HEIGHT,
        1 + (planet_type->u - tex_planet_x * 7) * (TEXTURE_CARD_WIDTH + 2),
        1 + (planet_type->v - tex_planet_y * 5) * (TEXTURE_CARD_HEIGHT + 2),
        TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, COLOR_WHITE, planet->draw.angle);
    
    if (planet->draw.white_factor > 0.0f)
    {
        graphics_set_texture(tex_enhancers, planet_filter);
        uint32_t color = 0xFFFFFF | ((uint32_t)(255.0f * (planet->draw.white_factor > 1.0f ? 1.0f : planet->draw.white_factor))<<24);        
        graphics_draw_quad(x, y, CARD_WIDTH, CARD_HEIGHT, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].x, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].y, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, color);
    }
}

void game_draw_booster(struct BoosterPack *booster)
{
    int text_booster_y = 0;
    struct BoosterPackType *booster_type = &(g_booster_types[booster->type][booster->size][booster->image]);
    text_booster_y = booster_type->v / 5;

    float x, y;
    game_draw_get_oscillating_position(&(booster->draw), &x, &y);
    game_apply_shop_item_pixel_snap(&x, &y);
    int booster_filter = game_get_item_texture_filter();

    graphics_set_texture(tex_boosters[text_booster_y], booster_filter);
    graphics_draw_rotated_quad(
        x, y, BOOSTER_WIDTH, BOOSTER_HEIGHT,
        1 + (booster_type->u) * (TEXTURE_CARD_WIDTH + 2),
        1 + (booster_type->v - text_booster_y * 5) * (TEXTURE_CARD_HEIGHT + 2),
        TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, COLOR_WHITE, booster->draw.angle);
    game_draw_price_tag(&(booster->draw), game_util_get_booster_price(booster));
}

void game_draw_shop_singles()
{
    for (int i = 0; i < g_game_state.shop.total_items; i++)
    {
        if (!g_game_state.shop.items[i].available) continue;
        
        if (g_game_state.shop.items[i].type == ITEM_TYPE_JOKER)
        {
            struct Joker *joker = &(g_game_state.shop.items[i].info.joker);
            // Ensure shop items are stable: reset animation factors.
            joker->draw.angle = 0.0f;
            joker->draw.scale = 1.0f;
            joker->draw.white_factor = 0.0f;
            joker->draw.alpha = 1.0f;
            game_draw_joker(joker);
            game_draw_price_tag(&joker->draw, game_util_get_joker_buy_price(joker));
        }
        else if (g_game_state.shop.items[i].type == ITEM_TYPE_CARD)
        {
            struct Card *card = &(g_game_state.shop.items[i].info.card);
            card->draw.angle = 0.0f;
            card->draw.scale = 1.0f;
            card->draw.white_factor = 0.0f;
            card->draw.alpha = 1.0f;
            game_draw_card(card, NULL);
            game_draw_price_tag(&card->draw, 3);
        }
        else if (g_game_state.shop.items[i].type == ITEM_TYPE_PLANET)
        {
            struct Planet *planet = &(g_game_state.shop.items[i].info.planet);
            planet->draw.angle = 0.0f;
            planet->draw.scale = 1.0f;
            planet->draw.white_factor = 0.0f;
            planet->draw.alpha = 1.0f;
            game_draw_planet(planet);
            game_draw_price_tag(&planet->draw, game_util_get_planet_buy_price(planet));
        }
        else if (g_game_state.shop.items[i].type == ITEM_TYPE_TAROT)
        {
            struct Tarot *tarot = &(g_game_state.shop.items[i].info.tarot);
            tarot->draw.angle = 0.0f;
            tarot->draw.scale = 1.0f;
            tarot->draw.white_factor = 0.0f;
            tarot->draw.alpha = 1.0f;
            game_draw_tarot(tarot);
            game_draw_price_tag(&tarot->draw, game_util_get_tarot_buy_price(tarot));
        }
    }
}
void game_draw_shop_boosters()
{
    for (int i = 0; i < g_game_state.shop.total_boosters; i++)
    {
        if (!g_game_state.shop.boosters[i].available) continue;
        
        struct BoosterPack *booster = &(g_game_state.shop.boosters[i].booster);
        booster->draw.angle = 0.0f;
        booster->draw.scale = 1.0f;
        booster->draw.white_factor = 0.0f;
        booster->draw.alpha = 1.0f;
        game_draw_booster(booster);
    }
}

void game_draw_shop_selected_single()
{
    if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_SHOP_SINGLES)
    {
        struct DrawObject *draw = NULL;
        switch (g_game_state.shop.items[g_game_state.highlighted_item].type)
        {
            case ITEM_TYPE_JOKER:
                game_draw_joker_hint(&(g_game_state.shop.items[g_game_state.highlighted_item].info.joker));
                draw = &g_game_state.shop.items[g_game_state.highlighted_item].info.joker.draw;                
                break;
            case ITEM_TYPE_CARD:
                game_draw_card_hint(&(g_game_state.shop.items[g_game_state.highlighted_item].info.card));
                draw = &g_game_state.shop.items[g_game_state.highlighted_item].info.card.draw;
                break;
            case ITEM_TYPE_PLANET:
                game_draw_planet_hint(&(g_game_state.shop.items[g_game_state.highlighted_item].info.planet));
                draw = &g_game_state.shop.items[g_game_state.highlighted_item].info.planet.draw;
                break;
            case ITEM_TYPE_TAROT:
                game_draw_tarot_hint(&(g_game_state.shop.items[g_game_state.highlighted_item].info.tarot));
                draw = &g_game_state.shop.items[g_game_state.highlighted_item].info.tarot.draw;
                break;
            default:
                break;
        }

        game_draw_card_frame(draw);
        game_draw_item_button(draw, GAMEPAD_UI_RIGHT_TRIGGER, "Buy", NULL, COLOR_RED_BUTTON, false);
        if (g_game_state.shop.items[g_game_state.highlighted_item].type == ITEM_TYPE_PLANET ||
            g_game_state.shop.items[g_game_state.highlighted_item].type == ITEM_TYPE_TAROT)
        {
            game_draw_item_button(draw, GAMEPAD_UI_LEFT_TRIGGER, "Buy", "&Use", COLOR_GREEN_BUTTON, true);
        }
    }
}

void game_draw_shop_selected_booster()
{
    if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_SHOP_BOOSTERS)
    {
        struct DrawObject *draw = &(g_game_state.shop.boosters[g_game_state.highlighted_item].booster.draw);        

        game_draw_booster_frame(draw);
        game_draw_item_button(draw, GAMEPAD_UI_RIGHT_TRIGGER, "Open", NULL, COLOR_GREEN_BUTTON, false);
        game_draw_booster_hint(&(g_game_state.shop.boosters[g_game_state.highlighted_item].booster));
    }
}

void game_draw_consumables()
{
    game_draw_pill(DRAW_CONSUMABLES_X, DRAW_CONSUMABLES_Y, DRAW_CONSUMABLES_WIDTH, CARD_HEIGHT, 0x66000000);

    for (int i = 0; i < g_game_state.consumables.item_count; i++)
    {
        if (i == g_game_state.highlighted_item && g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_CONSUMABLES) continue;
        if (g_game_state.consumables.items[i].type == ITEM_TYPE_PLANET) game_draw_planet(&(g_game_state.consumables.items[i].planet));
        if (g_game_state.consumables.items[i].type == ITEM_TYPE_TAROT) game_draw_tarot(&(g_game_state.consumables.items[i].tarot));
    }

    char str[24];
    sprintf(str, "%d/%d", g_game_state.consumables.item_count, g_game_state.consumable_slots);
    graphics_draw_text(font_small, str, DRAW_CONSUMABLES_X + 2.0f, DRAW_CONSUMABLES_Y + CARD_HEIGHT + 1.0f, 1.0f, COLOR_WHITE);

    if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_CONSUMABLES)
    {        
        if (g_game_state.consumables.items[g_game_state.highlighted_item].type == ITEM_TYPE_PLANET)
        {
            struct Planet *selected_planet = &(g_game_state.consumables.items[g_game_state.highlighted_item].planet);
            game_draw_card_frame(&selected_planet->draw);
            game_draw_planet(selected_planet);
            sprintf(str, "$%d", game_util_get_planet_sell_price(selected_planet));
            game_draw_item_button(&selected_planet->draw, GAMEPAD_UI_LEFT_TRIGGER, "Sell", str, COLOR_GREEN_BUTTON, true);
            game_draw_item_button(&selected_planet->draw, GAMEPAD_UI_RIGHT_TRIGGER, "Use", NULL, COLOR_RED_BUTTON, false);
            game_draw_planet_hint(selected_planet);
        }
        else if (g_game_state.consumables.items[g_game_state.highlighted_item].type == ITEM_TYPE_TAROT)
        {
            struct Tarot *selected_tarot = &(g_game_state.consumables.items[g_game_state.highlighted_item].tarot);
            game_draw_card_frame(&selected_tarot->draw);
            game_draw_tarot(selected_tarot);
            sprintf(str, "$%d", game_util_get_tarot_sell_price(selected_tarot));
            game_draw_item_button(&selected_tarot->draw, GAMEPAD_UI_LEFT_TRIGGER, "Sell", str, COLOR_GREEN_BUTTON, true);
            game_draw_item_button(&selected_tarot->draw, GAMEPAD_UI_RIGHT_TRIGGER, "Use", NULL, COLOR_RED_BUTTON, false);
            game_draw_tarot_hint(selected_tarot);
        }
    }
}

void game_draw_top_jokers()
{
    game_draw_pill(DRAW_JOKERS_X, DRAW_JOKERS_Y, DRAW_JOKERS_WIDTH, CARD_HEIGHT, 0x66000000);

    for (int i = 0; i < g_game_state.jokers.joker_count; i++)
    {
        if (i == g_game_state.highlighted_item && g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_JOKERS) continue;
        game_draw_joker(&g_game_state.jokers.jokers[i]);
    }

    char str[24];
    sprintf(str, "%d/%d", g_game_state.jokers.joker_count, g_game_state.joker_slots + g_game_state.jokers.negative_count);
    graphics_draw_text(font_small, str, DRAW_JOKERS_X + 2.0f, DRAW_JOKERS_Y + CARD_HEIGHT + 1.0f, 1.0f, COLOR_WHITE);

    if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_JOKERS)
    {
        struct Joker *selected_joker = &g_game_state.jokers.jokers[g_game_state.highlighted_item];
        game_draw_card_frame(&selected_joker->draw);
        game_draw_joker(selected_joker);
        sprintf(str, "$%d", game_util_get_joker_sell_price(selected_joker));
        game_draw_item_button(&selected_joker->draw, GAMEPAD_UI_LEFT_TRIGGER, "Sell", str, COLOR_GREEN_BUTTON, true);
        game_draw_joker_hint(&(g_game_state.jokers.jokers[g_game_state.highlighted_item]));
    }
}

void game_draw_ingame_hand()
{
    game_draw_pill(DRAW_HAND_X, game_get_hand_y(), DRAW_HAND_WIDTH, CARD_HEIGHT, 0x66000000);

    for (int i = 0; i < g_game_state.hand.card_count; i++)
    {
        game_draw_card(g_game_state.hand.cards[i], NULL);
    }

    if (g_game_state.show_highlighted_card && g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_HAND)
    {
        if (g_game_state.highlighted_item > -1 && g_game_state.highlighted_item < g_game_state.hand.card_count)
        {
            game_draw_card_hint(g_game_state.hand.cards[g_game_state.highlighted_item]);
        }
    }

    for (int i = 0; i < g_game_state.played_hand.card_count; i++)
    {
        game_draw_card(g_game_state.played_hand.cards[i], NULL);
    }

    // Draw card count
    char str[24];
    sprintf(str, "%d/%d", g_game_state.hand.card_count, game_util_get_hand_size());
    graphics_draw_text(font_small, str, DRAW_HAND_X + 2.0f, game_get_hand_y() + CARD_HEIGHT + 1.0f, 1.0f, COLOR_WHITE);

    // Draw other buttons
    if (game_get_hand_y() == DRAW_HAND_Y)
    {
        graphics_set_no_texture();
        float x = DRAW_HAND_X + (DRAW_HAND_WIDTH / 2.0f) - (HAND_ACTION_SORT_WIDTH / 2.0f);
        float y = UI_HAND_ACTIONS_CONTAINER_Y;
        game_draw_pill(x, y, HAND_ACTION_SORT_WIDTH, HAND_ACTION_SORT_HEIGHT, COLOR_DARK_GREY_2);
        game_draw_pill(x + HAND_ACTION_INNER_PAD, y + HAND_ACTION_INNER_PAD,
            HAND_ACTION_SORT_WIDTH - HAND_ACTION_INNER_PAD * 2.0f,
            HAND_ACTION_SORT_HEIGHT - HAND_ACTION_INNER_PAD * 2.0f, COLOR_DARK_GREY);
        graphics_draw_text_center(font_small, "Sort Hand", x + (HAND_ACTION_SORT_WIDTH / 2.0f), y + HAND_ACTION_SORT_HEIGHT * 0.25f, 1.0f, COLOR_WHITE);
        if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_SORT_HAND_RANK)
        {
            game_draw_pill(x + HAND_ACTION_INNER_PAD * 3.0f,
                y + HAND_ACTION_SORT_HEIGHT * 0.46f,
                HAND_ACTION_SEGMENT_W + HAND_ACTION_INNER_PAD * 2.0f,
                HAND_ACTION_SEGMENT_H + HAND_ACTION_INNER_PAD * 2.0f, COLOR_WHITE);
        }
        game_draw_pill(x + HAND_ACTION_INNER_PAD * 4.0f,
            y + HAND_ACTION_SORT_HEIGHT * 0.50f,
            HAND_ACTION_SEGMENT_W,
            HAND_ACTION_SEGMENT_H, 0xFF6B7686);
        game_draw_pill(x + HAND_ACTION_INNER_PAD * 5.0f,
            y + HAND_ACTION_SORT_HEIGHT * 0.54f,
            HAND_ACTION_SEGMENT_W - HAND_ACTION_INNER_PAD * 2.0f,
            HAND_ACTION_SEGMENT_H - HAND_ACTION_INNER_PAD * 2.0f, 0xFF343943);
        game_draw_pill(x + HAND_ACTION_INNER_PAD * 5.0f,
            y + HAND_ACTION_SORT_HEIGHT * 0.54f,
            HAND_ACTION_SEGMENT_W - HAND_ACTION_INNER_PAD * 2.0f,
            HAND_ACTION_SEGMENT_H - HAND_ACTION_INNER_PAD * 2.0f, 0xFF008CC8);
        graphics_draw_text_center(font_small, "Rank",
            x + HAND_ACTION_INNER_PAD * 4.0f + HAND_ACTION_SEGMENT_W * 0.5f,
            y + HAND_ACTION_SORT_HEIGHT * 0.71f, 1.0f, COLOR_WHITE);
        if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_SORT_HAND_SUIT)
        {
            game_draw_pill(x + HAND_ACTION_INNER_PAD * 4.0f + HAND_ACTION_SEGMENT_W + HAND_ACTION_SEGMENT_GAP - HAND_ACTION_INNER_PAD,
                y + HAND_ACTION_SORT_HEIGHT * 0.46f,
                HAND_ACTION_SEGMENT_W + HAND_ACTION_INNER_PAD * 2.0f,
                HAND_ACTION_SEGMENT_H + HAND_ACTION_INNER_PAD * 2.0f, COLOR_WHITE);
        }
        game_draw_pill(x + HAND_ACTION_INNER_PAD * 4.0f + HAND_ACTION_SEGMENT_W + HAND_ACTION_SEGMENT_GAP,
            y + HAND_ACTION_SORT_HEIGHT * 0.50f,
            HAND_ACTION_SEGMENT_W,
            HAND_ACTION_SEGMENT_H, 0xFF6B7686);
        game_draw_pill(x + HAND_ACTION_INNER_PAD * 5.0f + HAND_ACTION_SEGMENT_W + HAND_ACTION_SEGMENT_GAP,
            y + HAND_ACTION_SORT_HEIGHT * 0.54f,
            HAND_ACTION_SEGMENT_W - HAND_ACTION_INNER_PAD * 2.0f,
            HAND_ACTION_SEGMENT_H - HAND_ACTION_INNER_PAD * 2.0f, 0xFF343943);
        game_draw_pill(x + HAND_ACTION_INNER_PAD * 5.0f + HAND_ACTION_SEGMENT_W + HAND_ACTION_SEGMENT_GAP,
            y + HAND_ACTION_SORT_HEIGHT * 0.54f,
            HAND_ACTION_SEGMENT_W - HAND_ACTION_INNER_PAD * 2.0f,
            HAND_ACTION_SEGMENT_H - HAND_ACTION_INNER_PAD * 2.0f, 0xFF008CC8);
        graphics_draw_text_center(font_small, "Suit",
            x + HAND_ACTION_INNER_PAD * 4.0f + HAND_ACTION_SEGMENT_W + HAND_ACTION_SEGMENT_GAP + HAND_ACTION_SEGMENT_W * 0.5f,
            y + HAND_ACTION_SORT_HEIGHT * 0.71f, 1.0f, COLOR_WHITE);

        // Play Hand
        x = (DRAW_HAND_X + (DRAW_HAND_WIDTH / 2.0f)) - HAND_ACTION_SORT_WIDTH / 2.0f - HAND_ACTION_ROW_GAP - HAND_ACTION_PRIMARY_WIDTH;
        y = UI_HAND_ACTIONS_CONTAINER_Y + 2.0f;
        game_draw_action_button(x, y, HAND_ACTION_PRIMARY_WIDTH, HAND_ACTION_PRIMARY_HEIGHT,
            "Play Hand", NULL,
            GAMEPAD_UI_SQUARE, false,
            COLOR_LIGHT_BLUE,
            g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_HAND_PLAY);

        // Discard
        x = (DRAW_HAND_X + (DRAW_HAND_WIDTH / 2.0f)) + HAND_ACTION_SORT_WIDTH / 2.0f + HAND_ACTION_ROW_GAP;
        game_draw_action_button(x, y, HAND_ACTION_PRIMARY_WIDTH, HAND_ACTION_PRIMARY_HEIGHT,
            "Discard", NULL,
            GAMEPAD_UI_TRIANGLE, true,
            COLOR_LIGHT_RED,
            g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_HAND_DISCARD);
    }
}

void game_draw_booster_hand()
{
    game_draw_pill(DRAW_HAND_X, DRAW_HAND_Y_IN_BOOSTER, DRAW_HAND_WIDTH, CARD_HEIGHT, 0x66000000);

    for (int i = 0; i < g_game_state.hand.card_count; i++)
    {
        game_draw_card(g_game_state.hand.cards[i], NULL);
    }
}

void game_draw_booster_hand_selected_card()
{
    if (g_game_state.show_highlighted_card && g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_HAND)
    {
        if (g_game_state.highlighted_item > -1 && g_game_state.highlighted_item < g_game_state.hand.card_count)
        {
            game_draw_card_hint(g_game_state.hand.cards[g_game_state.highlighted_item]);
        }
    }
}

#define DECK_WIDGET_SAFE_RIGHT        (SCREEN_WIDTH * 0.02f)
#define DECK_WIDGET_SAFE_BOTTOM       (SCREEN_HEIGHT * 0.06f)
#define DECK_WIDGET_SHOP_Y_OFFSET     -4.0f
#define DECK_WIDGET_SCALE             1.25f
#define DECK_WIDGET_COUNTER_WIDTH     (CARD_WIDTH * 0.95f)
#define DECK_WIDGET_COUNTER_HEIGHT    (CARD_HEIGHT * 0.18f)

static void game_get_deck_widget_layout(float *x, float *y, float *w, float *h)
{
    *w = CARD_WIDTH * DECK_WIDGET_SCALE;
    *h = CARD_HEIGHT * DECK_WIDGET_SCALE;

    *x = UI_DECK_CONTAINER_X + (UI_DECK_CONTAINER_WIDTH - *w) * 0.5f;
    *y = UI_DECK_CONTAINER_Y + (UI_DECK_CONTAINER_HEIGHT - *h) * 0.5f;

    if (g_game_state.stage == GAME_STAGE_SHOP)
    {
        *y += DECK_WIDGET_SHOP_Y_OFFSET;
    }
}

void game_draw_deck()
{
    char str[24];

    float deck_x, deck_y, deck_w, deck_h;
    game_get_deck_widget_layout(&deck_x, &deck_y, &deck_w, &deck_h);

    if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_DECK)
    {
        game_draw_pill(deck_x - 3.0f, deck_y - 3.0f, deck_w + 6.0f, deck_h + 6.0f, COLOR_WHITE);
    }

    // Draw deck
    graphics_set_texture(tex_enhancers, GRAPHICS_TEXTURE_FILTER_LINEAR);
    graphics_draw_quad(deck_x, deck_y, deck_w, deck_h, 0, 0, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, COLOR_WHITE);

    // Deck size
    sprintf(str, "%d/%d", g_game_state.current_deck.card_count, g_game_state.full_deck.card_count);
    graphics_set_no_texture();
    graphics_draw_quad(deck_x + deck_w - DECK_WIDGET_COUNTER_WIDTH, deck_y + deck_h + 2.0f,
        DECK_WIDGET_COUNTER_WIDTH, DECK_WIDGET_COUNTER_HEIGHT, 0, 0, 0, 0, COLOR_DARK_GREY_2);
    graphics_draw_quad(deck_x + deck_w - DECK_WIDGET_COUNTER_WIDTH + 1.0f, deck_y + deck_h + 3.0f,
        DECK_WIDGET_COUNTER_WIDTH - 2.0f, DECK_WIDGET_COUNTER_HEIGHT - 2.0f, 0, 0, 0, 0, COLOR_DARK_GREY);
    graphics_draw_text_center(font_small, str,
        deck_x + deck_w - (DECK_WIDGET_COUNTER_WIDTH / 2.0f),
        deck_y + deck_h + 8.0f,
        1.0f, COLOR_WHITE);
}

void game_draw_left_info()
{
    static int shop_anim = 0;
    char str[24];

    const float panel_x = UI_SCORE_PANEL_X;
    const float panel_y = UI_SCORE_PANEL_Y;
    const float panel_w = UI_SCORE_PANEL_WIDTH;
    const float panel_h = UI_SCORE_PANEL_HEIGHT;
    const float pad = panel_w * 0.04f;
    const float inner_x = panel_x + pad;
    const float inner_w = panel_w - pad * 2.0f;
    const float section_gap = panel_h * 0.007f;
    const float value_h = panel_h * 0.045f;
    const float button_h = panel_h * 0.075f;
    const float centerX = panel_x + panel_w * 0.5f;

    game_draw_pill(panel_x, panel_y, panel_w, panel_h, HUD_BG_COLOR);
    game_draw_pill(panel_x + pad, panel_y + pad, panel_w - pad * 2.0f, panel_h - pad * 2.0f, HUD_RAIL_INNER_COLOR);

    float current_row_y = panel_y + panel_h * 0.03f;

    switch (g_game_state.stage)
    {
        case GAME_STAGE_INGAME:
        {
            const struct BlindPanelStyle *style = game_get_blind_panel_style(g_game_state.blind);
            // Blind Title Pill
            game_draw_pill(inner_x, current_row_y - 6.0f, inner_w, 14.0f, style->header_fill);
            graphics_draw_text_center(font_small, game_util_get_blind_name(g_game_state.blind), centerX, current_row_y + 1.0f, 1.0f, COLOR_WHITE);
            
            // Header Content Box (Matches target box with badge on left)
            float info_box_y = current_row_y + 10.0f;
            float info_box_h = (panel_h * HUD_ROW_HEADER_H) - 16.0f;
            game_draw_hud_value_box(inner_x, info_box_y, inner_w, info_box_h);

            // Badge Chip (Animated) - Reduced to 24px to prevent sidebar crowding.
            const float badge_size = 24.0f;
            float badge_x = floorf(inner_x + 6.0f);
            float badge_y = floorf(info_box_y + (info_box_h - badge_size) * 0.5f);
            
            // The atlas contains 21 frames of animation horizontally.
            // Clamp to 15 frames to respect the 512px hardware texture width limit.
            int anim_frame = (g_game_counter / 10) % 15;
            graphics_set_texture(tex_blind_chips, GRAPHICS_TEXTURE_FILTER_NEAREST);
            graphics_draw_quad(badge_x, badge_y, badge_size, badge_size, (int16_t)(anim_frame * 34), (int16_t)(g_game_state.blind * 34), 34, 34, COLOR_WHITE);
            
            graphics_draw_text(font_small, "Score at least", badge_x + badge_size + 8.0f, info_box_y + 3.0f, 0.8f, HUD_LABEL_COLOR);
            fmt_num(str, game_get_current_blind_score());
            float score_scale = graphics_get_text_scale_to_fit(font_big, str, inner_w - badge_size - 14.0f, 1.0f);
            graphics_draw_text(font_big, str, badge_x + badge_size + 8.0f, info_box_y + 11.0f, score_scale, COLOR_WHITE);
            break;
        }
        case GAME_STAGE_SHOP:
        {
            game_draw_hud_section(inner_x, current_row_y - panel_h * 0.022f, inner_w, panel_h * HUD_ROW_HEADER_H);
            if (g_game_counter % 10 == 0) shop_anim++;
            if (shop_anim > 3) shop_anim = 0;
            graphics_set_texture(tex_shop, GRAPHICS_TEXTURE_FILTER_LINEAR);
            graphics_draw_quad(inner_x + panel_w * 0.01f, current_row_y - panel_h * 0.018f, inner_w - panel_w * 0.02f, panel_h * HUD_ROW_HEADER_H * 0.9f, 113 * shop_anim, 0, 113, 57, COLOR_WHITE);
            break;
        }
    }

    current_row_y = panel_y + (panel_h * HUD_ROW_HEADER_H) + section_gap;

    // --- SECTION: ROUND SCORE ---
    game_draw_hud_section(inner_x, current_row_y, inner_w, panel_h * HUD_ROW_ROUND_SCORE_H);
    graphics_draw_text(font_small, "Round score", inner_x + 4.0f, current_row_y + 4.0f, 1.0f, COLOR_WHITE);
    
    float score_y = current_row_y + 14.0f;
    game_draw_hud_value_box(inner_x + 4.0f, score_y, inner_w - 8.0f, value_h);
    float score_box_w = (inner_w - panel_w * 0.04f) * 0.9f;
    fmt_num(str, g_game_state.score);
    float score_scale = graphics_get_text_scale_to_fit(font_big, str, score_box_w, 1.0f);
    graphics_draw_text_center(font_big, str, centerX, score_y + value_h * 0.5f, score_scale, COLOR_WHITE);

    current_row_y += (panel_h * HUD_ROW_ROUND_SCORE_H) + section_gap;

    // --- SECTION: HAND NAME ---
    game_draw_hud_section(inner_x, current_row_y, inner_w, panel_h * HUD_ROW_HAND_INFO_H);
    // Draw poker hand
    float hand_name_scale = graphics_get_text_scale_to_fit(font_small, g_poker_hand_names[g_game_state.current_poker_hand], inner_w * 0.95f, 1.0f);
    graphics_draw_text_center(font_small, g_poker_hand_names[g_game_state.current_poker_hand], centerX, current_row_y + 8.0f, hand_name_scale, HUD_LABEL_COLOR);
    // Draw poker hand level
    if (g_game_state.current_poker_hand != GAME_POKER_HAND_NONE)
    {
        sprintf(str, "Level %d", g_game_state.poker_hand_level[g_game_state.current_poker_hand]);
        float level_scale = graphics_get_text_scale_to_fit(font_small, str, inner_w * 0.9f, 1.0f);
        graphics_draw_text_center(font_small, str, centerX, current_row_y + 20.0f, level_scale, HUD_META_COLOR);
    }

    current_row_y += (panel_h * HUD_ROW_HAND_INFO_H) + section_gap;

    // --- SECTION: CHIPS & MULT PILLS ---
    float pill_w = inner_w * 0.46f;
    float pill_x2 = centerX + (inner_w * 0.04f);
    game_draw_pill(inner_x, current_row_y, pill_w, value_h, COLOR_LIGHT_BLUE);
    game_draw_pill(pill_x2, current_row_y, pill_w, value_h, COLOR_LIGHT_RED);

    float pill_text_y = current_row_y + value_h * 0.5f;
    fmt_num(str, (double)g_game_state.current_base_chips);
    graphics_draw_text_center(font_small, str, inner_x + pill_w * 0.5f, pill_text_y, 1.0f, COLOR_WHITE);
    graphics_draw_text_center(font_small, "x", centerX, pill_text_y, 1.0f, 0xFF8888FF);
    fmt_num(str, (double)g_game_state.current_base_mult);
    graphics_draw_text_center(font_small, str, pill_x2 + pill_w * 0.5f, pill_text_y, 1.0f, COLOR_WHITE);

    current_row_y += (panel_h * HUD_ROW_CHIPS_MULT_H) + section_gap * 2.0f;

    // --- SECTION: HANDS & DISCARDS ---
    game_draw_hud_section(inner_x, current_row_y, inner_w, panel_h * HUD_ROW_STATS_H);

    graphics_draw_text(font_small, "Hands", panel_x + panel_w * 0.08f, current_row_y + 4.0f, 1.0f, HUD_LABEL_COLOR);
    graphics_draw_text(font_small, "Discards", panel_x + panel_w * 0.50f, current_row_y + 4.0f, 1.0f, HUD_LABEL_COLOR);
    
    float stats_box_y = current_row_y + 14.0f;
    game_draw_hud_value_box(panel_x + panel_w * 0.06f, stats_box_y, panel_w * 0.39f, value_h);
    game_draw_hud_value_box(panel_x + panel_w * 0.55f, stats_box_y, panel_w * 0.39f, value_h);

    sprintf(str, "%d", g_game_state.current_hands);
    graphics_draw_text_center(font_small, str, panel_x + panel_w * 0.25f, stats_box_y + value_h * 0.5f, 1.0f, COLOR_TEXT_BLUE);
    sprintf(str, "%d", g_game_state.current_discards);
    graphics_draw_text_center(font_small, str, panel_x + panel_w * 0.75f, stats_box_y + value_h * 0.5f, 1.0f, COLOR_LIGHT_RED);

    current_row_y += (panel_h * HUD_ROW_STATS_H) + section_gap;

    // --- SECTION: WEALTH ---
    game_draw_hud_section(inner_x, current_row_y, inner_w, panel_h * HUD_ROW_WEALTH_H);
    game_draw_hud_value_box(panel_x + panel_w * 0.06f, current_row_y + 2.0f, panel_w * 0.88f, value_h);

    sprintf(str, "$%d", g_game_state.wealth);
    float wealth_scale = graphics_get_text_scale_to_fit(font_big, str, inner_w * 0.85f, 1.0f);
    graphics_draw_text_center(font_big, str, centerX, current_row_y + 2.0f + value_h * 0.5f, wealth_scale, COLOR_TEXT_YELLOW);

    current_row_y += (panel_h * HUD_ROW_WEALTH_H) + section_gap;

    // --- SECTION: FOOTER (ANTE/ROUND) ---
    game_draw_hud_section(inner_x, current_row_y, inner_w, panel_h * HUD_ROW_FOOTER_H);

    graphics_draw_text(font_small, "Ante", panel_x + panel_w * 0.08f, current_row_y + 4.0f, 1.0f, HUD_LABEL_COLOR);
    graphics_draw_text(font_small, "Round", panel_x + panel_w * 0.50f, current_row_y + 4.0f, 1.0f, HUD_LABEL_COLOR);
    
    float footer_box_y = current_row_y + 14.0f;
    game_draw_hud_value_box(panel_x + panel_w * 0.06f, footer_box_y, panel_w * 0.39f, value_h);
    game_draw_hud_value_box(panel_x + panel_w * 0.55f, footer_box_y, panel_w * 0.39f, value_h);

    sprintf(str, "#5%d#-/8", g_game_state.ante);
    graphics_draw_text_formatted_center(font_big, str, NULL, panel_x + panel_w * 0.25f, footer_box_y + value_h * 0.5f, 1.0f, COLOR_WHITE);
    sprintf(str, "%d", g_game_state.round);
    graphics_draw_text_center(font_big, str, panel_x + panel_w * 0.75f, footer_box_y + value_h * 0.5f, 1.0f, COLOR_TEXT_ORANGE);
    
    current_row_y += (panel_h * HUD_ROW_FOOTER_H) + section_gap;

    // Dual Footer Buttons: Run Info & Options (Matches Target HUD)
    float footer_btn_w = (inner_w * 0.5f) - 2.0f;
    game_draw_action_button(inner_x, current_row_y + 4.0f, footer_btn_w, button_h,
        "Run Info", NULL, -1, false, 0xFF4A4DFF, // High-contrast Red
        g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_RUN_INFO);

    game_draw_action_button(inner_x + footer_btn_w + 4.0f, current_row_y + 4.0f, footer_btn_w, button_h,
        "Options", NULL, -1, false, 0xFFFF9300, // High-contrast Orange
        false);
}

void game_draw_cash_out_panel()
{
    char str[32];

    int x = 140;
    float y = g_game_state.cash_out_panel_y;
    game_draw_pill(x, y, 250, SCREEN_HEIGHT - 100, COLOR_DARK_GREY);
    y += 16;
    if (g_game_state.cash_out_done)
    {        
        sprintf(str, "Cash Out: $%d", g_game_state.cash_out_value);
        float str_size = (float)strlen(str) * 8.0f;
        game_draw_pill((140.0f + 125.0f) - (str_size / 2.0f) - 6.0f, y - 10.0f, str_size + 12.0f, 20.0f, COLOR_WHITE);
        game_draw_pill((140.0f + 125.0f) - (str_size / 2.0f) - 4.0f, y - 8.0f, str_size + 8.0f, 16.0f, COLOR_LIGHT_RED);
        graphics_draw_text_center(font_big, str, (140.0f + 125.0f), y, 1.0f, COLOR_WHITE);
    }
    y += 20;
    if (g_game_state.cash_out_blind > -1)
    {
        graphics_draw_text(font_small, game_util_get_blind_name(g_game_state.blind), x + 4, y, 1.0f, COLOR_WHITE);
        sprintf(str, "$%d", g_game_state.cash_out_blind);
        graphics_draw_text(font_small, str, x + 200, y, 1.0f, COLOR_WHITE);
    }
    y += 16;
    if (g_game_state.cash_out_hands > -1)
    {
        graphics_draw_text(font_small, "Remaining hands", x + 4, y, 1.0f, COLOR_WHITE);
        sprintf(str, "$%d", g_game_state.cash_out_hands);
        graphics_draw_text(font_small, str, x + 200, y, 1.0f, COLOR_WHITE);
    }
    y += 16;
    if (g_game_state.cash_out_interest > -1)
    {
        graphics_draw_text(font_small, "Interest", x + 4, y, 1.0f, COLOR_WHITE);
        sprintf(str, "$%d", g_game_state.cash_out_interest);
        graphics_draw_text(font_small, str, x + 200, y, 1.0f, COLOR_WHITE);
    }
    y += 16;
    if (g_game_state.cash_out_jokers > -1)
    {
        graphics_draw_text(font_small, "Jokers", x + 4, y, 1.0f, COLOR_WHITE);
        sprintf(str, "$%d", g_game_state.cash_out_jokers);
        graphics_draw_text(font_small, str, x + 200, y, 1.0f, COLOR_WHITE);
    }
}

void game_draw_booster_items()
{
    game_draw_pill(DRAW_BOOSTER_ITEMS_X, DRAW_BOOSTER_ITEMS_Y, DRAW_BOOSTER_ITEMS_WIDTH, CARD_HEIGHT, 0x66000000);

    for (int i = 0; i < g_game_state.shop.booster_total_items; i++)
    {
        if (g_game_state.shop.booster_items[i].available)
        {
            switch (g_game_state.shop.booster_items[i].type)
            {
                case ITEM_TYPE_CARD:
                    game_draw_card(&(g_game_state.shop.booster_items[i].info.card), NULL);
                    break;
                case ITEM_TYPE_JOKER:
                    game_draw_joker(&(g_game_state.shop.booster_items[i].info.joker));
                    break;
                case ITEM_TYPE_PLANET:
                    game_draw_planet(&(g_game_state.shop.booster_items[i].info.planet));
                    break;
                case ITEM_TYPE_TAROT:
                    game_draw_tarot(&(g_game_state.shop.booster_items[i].info.tarot));
                    break;                    
            }
        }
    }

    if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_BOOSTER_ITEMS)
    {
        if (g_game_state.highlighted_item > -1 && g_game_state.highlighted_item < g_game_state.shop.booster_total_items)
        {
            struct DrawObject *draw = &(g_game_state.shop.booster_items[g_game_state.highlighted_item].info.card.draw);
            game_draw_card_frame(draw);
            game_draw_item_button(draw, GAMEPAD_UI_RIGHT_TRIGGER, "Select", NULL, COLOR_RED_BUTTON, false);
            switch(g_game_state.shop.booster_items[g_game_state.highlighted_item].type)
            {
                case ITEM_TYPE_CARD:
                    game_draw_card_hint(&(g_game_state.shop.booster_items[g_game_state.highlighted_item].info.card));
                    break;
                case ITEM_TYPE_JOKER:
                    game_draw_joker_hint(&(g_game_state.shop.booster_items[g_game_state.highlighted_item].info.joker));
                    break;
                case ITEM_TYPE_PLANET:
                    game_draw_planet_hint(&(g_game_state.shop.booster_items[g_game_state.highlighted_item].info.planet));
                    break;
                case ITEM_TYPE_TAROT:
                    game_draw_tarot_hint(&(g_game_state.shop.booster_items[g_game_state.highlighted_item].info.tarot));
                    break;
            }
        }
    }
}

void game_draw_shop()
{
    game_draw_left_info();    

    game_draw_deck();

    switch (g_game_state.sub_stage)
    {
        case GAME_SUBSTAGE_SHOP_MAIN:
        {
            float x = LAYOUT_CENTER_X + 2.0f;
            float y = LAYOUT_MID_CLUSTER_Y;
            float shop_h = SCREEN_HEIGHT - y - 2.0f;
            float shop_w = LAYOUT_CENTER_WIDTH - 4.0f;

            // Main background for the shop cluster
            graphics_draw_solid_quad(x, y, shop_w, shop_h, COLOR_DARK_GREY_2);
            
            // Shop Title Label
            graphics_draw_text(font_small, "SHOP", x + 6.0f, y + 4.0f, 1.0f, HUD_LABEL_COLOR);

            // Column 1: Actions + Voucher (as seen in game_shop.jpg)
            game_draw_action_button(DRAW_SHOP_COL1_X, y + 16.0f, DRAW_SHOP_COL1_W, 32.0f,
                "Next Round", NULL,
                -1, false,
                COLOR_RED_BUTTON,
                g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_SHOP_NEXT_ROUND);

            char str[16];
            sprintf(str, "$%d", game_util_get_reroll_cost());
            game_draw_action_button(DRAW_SHOP_COL1_X, y + 54.0f, DRAW_SHOP_COL1_W, 32.0f,
                "Reroll", str,
                -1, false,
                COLOR_GREEN_BUTTON,
                g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_SHOP_REROLL);
            
            // Voucher Section (Bottom Left)
            graphics_draw_text(font_small, "VOUCHER", DRAW_SHOP_VOUCHER_X, y + 90.0f, 1.0f, HUD_META_COLOR);
            game_draw_hud_value_box(DRAW_SHOP_VOUCHER_X - 4.0f, y + 101.0f, DRAW_SHOP_VOUCHER_WIDTH + 8.0f, 72.0f);

            // Column 2: Singles + Booster Packs
            
            // Singles (Top Right)
            graphics_draw_text(font_small, "SINGLES", DRAW_SHOP_SINGLE_X, y + 4.0f, 1.0f, HUD_META_COLOR);
            game_draw_hud_value_box(DRAW_SHOP_SINGLE_X - 4.0f, y + 15.0f, DRAW_SHOP_SINGLE_WIDTH + 8.0f, 72.0f);
            game_draw_shop_singles();

            // Booster Packs (Bottom Right)
            graphics_draw_text(font_small, "BOOSTER PACKS", DRAW_SHOP_BOOSTER_X, y + 90.0f, 1.0f, HUD_META_COLOR);
            game_draw_hud_value_box(DRAW_SHOP_BOOSTER_X - 4.0f, y + 101.0f, DRAW_SHOP_BOOSTER_WIDTH + 8.0f, 72.0f);
            game_draw_shop_boosters();
            
            game_draw_shop_selected_single();
            
            game_draw_shop_selected_booster();

            break;
        }
        case GAME_SUBSTAGE_SHOP_BOOSTER_HAND:
        {
            game_draw_booster_hand();
        }
        case GAME_SUBSTAGE_SHOP_BOOSTER_NO_HAND:
        {
            graphics_draw_solid_quad(DRAW_BOOSTER_ITEMS_X + (DRAW_BOOSTER_ITEMS_WIDTH / 2.0f) - 50.0f, DRAW_BOOSTER_ITEMS_Y + CARD_HEIGHT, 100.0f, 30.0f, COLOR_DARK_GREY_2);            
            graphics_draw_text_center(font_big, g_game_state.shop.opened_booster.size == BOOSTER_PACK_SIZE_MEGA ? "Choose 2" : "Choose 1",
                DRAW_BOOSTER_ITEMS_X + (DRAW_BOOSTER_ITEMS_WIDTH / 2.0f), DRAW_BOOSTER_ITEMS_Y + CARD_HEIGHT + 15.0f, 1.0f, COLOR_WHITE);

            game_draw_booster_items();
            
            break;
        }
    }

    game_draw_top_jokers();

    game_draw_consumables();

    switch (g_game_state.sub_stage)
    {
        case GAME_SUBSTAGE_SHOP_BOOSTER_HAND:
        {
            game_draw_booster_hand_selected_card();
            break;
        }
    }
}

void game_draw_score_number()
{
    if (g_game_state.score_number.show_score_number)
    {
        float x = 0.0f, y = 0.0f;
        if (g_game_state.score_number.card != NULL)
        {
            x = g_game_state.score_number.card->draw.x + CARD_WIDTH / 2.0f;
            y = g_game_state.score_number.card->draw.y - 16.0f;
        }
        else if (g_game_state.score_number.joker != NULL)
        {
            x = g_game_state.score_number.joker->draw.x + CARD_WIDTH / 2.0f;
            y = g_game_state.score_number.joker->draw.y + CARD_HEIGHT + 10.0f;
        }

        graphics_set_no_texture();
        float quad_size = 16.0f * g_game_state.score_number.back_quad_scale;

        switch (g_game_state.score_number.score_number_type)
        {
            case SCORE_NUMBER_ADD_CHIPS:
                graphics_draw_rotated_quad(x - quad_size / 2.0f, y - quad_size / 2.0f, quad_size, quad_size, 0, 0, 0, 0, COLOR_BLUE, g_game_state.score_number.back_quad_angle);
                graphics_draw_text_center(font_big, g_game_state.score_number.text, x, y, g_game_state.score_number.text_scale, COLOR_WHITE);
                break;
            case SCORE_NUMBER_ADD_MULT:
                graphics_draw_rotated_quad(x - quad_size / 2.0f, y - quad_size / 2.0f, quad_size, quad_size, 0, 0, 0, 0, COLOR_RED, g_game_state.score_number.back_quad_angle);
                graphics_draw_text_center(font_big, g_game_state.score_number.text, x, y, g_game_state.score_number.text_scale * 0.8, COLOR_WHITE);
                break;
            case SCORE_NUMBER_MULT_MULT:
            case SCORE_NUMBER_AGAIN:
                graphics_draw_rotated_quad(x - quad_size / 2.0f, y - quad_size / 2.0f, quad_size, quad_size, 0, 0, 0, 0, COLOR_RED, g_game_state.score_number.back_quad_angle);
                graphics_draw_text_center(font_big, g_game_state.score_number.text, x, y, g_game_state.score_number.text_scale * 0.8, COLOR_WHITE);
                break;
            case SCORE_NUMBER_ADD_MONEY:
                graphics_draw_rotated_quad(x - quad_size / 2.0f, y - quad_size / 2.0f, quad_size, quad_size, 0, 0, 0, 0, COLOR_YELLOW, g_game_state.score_number.back_quad_angle);
                graphics_draw_text_center(font_big, g_game_state.score_number.text, x, y, g_game_state.score_number.text_scale, COLOR_WHITE);
            default:
                break;
        }
    }
}

#define END_GAME_WIDTH 240
#define END_GAME_HEIGHT 220

void game_draw_end_game_info(const char *title, int color)
{
    char str[32];

    float x = (SCREEN_WIDTH - END_GAME_WIDTH) / 2.0f;
    float y = (SCREEN_HEIGHT - END_GAME_HEIGHT) / 2.0f;
    graphics_draw_solid_quad(x, y, END_GAME_WIDTH, END_GAME_HEIGHT, COLOR_DARK_GREY);

    y += 12.0f;
    graphics_draw_text_center(font_big, title, SCREEN_WIDTH / 2.0f, y, 1.0f, color);

    y += 20.0f;
    graphics_draw_solid_quad(x + 6.0f,  y - 6.0f, (END_GAME_WIDTH / 2.0f) - 12.0f, 20.0f, COLOR_DARK_GREY_2);
    graphics_draw_solid_quad(x + 70.0f, y - 2.0f, (END_GAME_WIDTH / 4.0f) - 24.0f, 12.0f, COLOR_DARK_GREY);
    graphics_draw_text_center(font_small, "Ante", x + 28.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    sprintf(str, "%d", g_game_state.ante);
    graphics_draw_text_center(font_small, str, x + (END_GAME_WIDTH / 4.0f) + 28.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    graphics_draw_solid_quad((END_GAME_WIDTH / 2.0f) + x + 6.0f, y - 6.0f, (END_GAME_WIDTH / 2.0f) - 12.0f, 20.0f, COLOR_DARK_GREY_2);
    graphics_draw_solid_quad((END_GAME_WIDTH / 2.0f) + x + 70.0f, y - 2.0f, (END_GAME_WIDTH / 4.0f) - 24.0f, 12.0f, COLOR_DARK_GREY);
    graphics_draw_text_center(font_small, "Round", (END_GAME_WIDTH / 2.0f) + x + 28.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    sprintf(str, "%d", g_game_state.round);
    graphics_draw_text_center(font_small, str, (END_GAME_WIDTH / 2.0f) + x + (END_GAME_WIDTH / 4.0f) + 28.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    y += 22.0f;
    graphics_draw_solid_quad(x + 6.0f, y - 6.0f, END_GAME_WIDTH - 12.0f, 20.0f, COLOR_DARK_GREY_2);
    graphics_draw_solid_quad(x + (END_GAME_WIDTH / 2.0f), y - 2.0f, (END_GAME_WIDTH / 2.0f) - 14.0f, 12.0f, COLOR_DARK_GREY);
    graphics_draw_text_center(font_small, "Best Hand", x + 58.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    fmt_num(str, g_game_state.stats.max_score);
    float best_hand_scale = graphics_get_text_scale_to_fit(font_small, str, (END_GAME_WIDTH / 2.0f) - 20.0f, 1.0f);
    graphics_draw_text_center(font_small, str, x + (END_GAME_WIDTH / 2.0f) + 54.0f, y + 4.0f, best_hand_scale, COLOR_WHITE);
    y += 22.0f;
    graphics_draw_solid_quad(x + 6.0f, y - 6.0f, END_GAME_WIDTH - 12.0f, 20.0f, COLOR_DARK_GREY_2);
    graphics_draw_solid_quad(x + (END_GAME_WIDTH / 2.0f), y - 2.0f, (END_GAME_WIDTH / 2.0f) - 14.0f, 12.0f, COLOR_DARK_GREY);
    graphics_draw_text_center(font_small, "Most Played Hand", x + 58.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    float most_played_scale = graphics_get_text_scale_to_fit(font_small, g_poker_hand_names[g_game_state.stats.most_played_poker_hand], (END_GAME_WIDTH / 2.0f) - 20.0f, 1.0f);
    graphics_draw_text_center(font_small, g_poker_hand_names[g_game_state.stats.most_played_poker_hand], x + (END_GAME_WIDTH / 2.0f) + 54.0f, y + 4.0f, most_played_scale, COLOR_WHITE);
    y += 22.0f;
    graphics_draw_solid_quad(x + 6.0f, y - 6.0f, END_GAME_WIDTH - 12.0f, 20.0f, COLOR_DARK_GREY_2);
    graphics_draw_solid_quad(x + (END_GAME_WIDTH / 2.0f), y - 2.0f, (END_GAME_WIDTH / 2.0f) - 14.0f, 12.0f, COLOR_DARK_GREY);
    graphics_draw_text_center(font_small, "Cards Played", x + 58.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    sprintf(str, "%d", g_game_state.stats.cards_played);
    graphics_draw_text_center(font_small, str, x + (END_GAME_WIDTH / 2.0f) + 54.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    y += 22.0f;
    graphics_draw_solid_quad(x + 6.0f, y - 6.0f, END_GAME_WIDTH - 12.0f, 20.0f, COLOR_DARK_GREY_2);
    graphics_draw_solid_quad(x + (END_GAME_WIDTH / 2.0f), y - 2.0f, (END_GAME_WIDTH / 2.0f) - 14.0f, 12.0f, COLOR_DARK_GREY);
    graphics_draw_text_center(font_small, "Cards Discarded", x + 58.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    sprintf(str, "%d", g_game_state.stats.cards_discarded);
    graphics_draw_text_center(font_small, str, x + (END_GAME_WIDTH / 2.0f) + 54.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    y += 22.0f;
    graphics_draw_solid_quad(x + 6.0f, y - 6.0f, END_GAME_WIDTH - 12.0f, 20.0f, COLOR_DARK_GREY_2);
    graphics_draw_solid_quad(x + (END_GAME_WIDTH / 2.0f), y - 2.0f, (END_GAME_WIDTH / 2.0f) - 14.0f, 12.0f, COLOR_DARK_GREY);
    graphics_draw_text_center(font_small, "Cards Purchased", x + 58.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    sprintf(str, "%d", g_game_state.stats.cards_purchased);
    graphics_draw_text_center(font_small, str, x + (END_GAME_WIDTH / 2.0f) + 54.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    y += 22.0f;
    graphics_draw_solid_quad(x + 6.0f, y - 6.0f, END_GAME_WIDTH - 12.0f, 20.0f, COLOR_DARK_GREY_2);
    graphics_draw_solid_quad(x + (END_GAME_WIDTH / 2.0f), y - 2.0f, (END_GAME_WIDTH / 2.0f) - 14.0f, 12.0f, COLOR_DARK_GREY);
    graphics_draw_text_center(font_small, "Times Rerolled", x + 58.0f, y + 4.0f, 1.0f, COLOR_WHITE);
    sprintf(str, "%d", g_game_state.stats.times_rerolled);
    graphics_draw_text_center(font_small, str, x + (END_GAME_WIDTH / 2.0f) + 54.0f, y + 4.0f, 1.0f, COLOR_WHITE);    
    y += 30.0f;
    game_draw_action_button(x + (END_GAME_WIDTH - 120.0f) / 2.0f, y - 8.0f, 120.0f, 24.0f, 
        "Start New Game", NULL, -1, false, COLOR_RED_BUTTON, true);
}

void game_draw_you_win_panel()
{
    game_draw_end_game_info("YOU WIN", COLOR_LIGHT_BLUE);
}

void game_draw_game_over_panel()
{
    game_draw_end_game_info("GAME OVER", COLOR_LIGHT_RED);
}

void game_draw_ingame()
{
    game_draw_left_info();

    game_draw_deck();

    game_draw_top_jokers();

    game_draw_consumables();

    switch (g_game_state.sub_stage)
    {
        case GAME_SUBSTAGE_INGAME_PICK_HAND:
        {
            game_draw_ingame_hand();            
            break;
        }
        case GAME_SUBSTAGE_INGAME_WON:
        {
            game_draw_cash_out_panel();
            break;
        }
        case GAME_SUBSTAGE_INGAME_WON_END:
        {
            game_draw_you_win_panel();
            break;
        }
        case GAME_SUBSTAGE_INGAME_LOST_END:
        {
            game_draw_game_over_panel();
            break;
        }
    }

    game_draw_score_number();
}

void game_draw_blind_select()
{
    char str[24];

    game_draw_left_info();    

    game_draw_deck();

    float panel_width = (LAYOUT_CENTER_WIDTH - LAYOUT_PANEL_GAP * 2.0f) / 3.0f;
    float x = LAYOUT_CENTER_X + 2.0f;
    for(int i = 0; i < 3; i++)
    {
        const struct BlindPanelStyle *style = game_get_blind_panel_style(i);
        graphics_set_no_texture();

        float panel_top = LAYOUT_MID_CLUSTER_Y;
        float panel_height = SCREEN_HEIGHT - panel_top;
        bool is_selected = i == g_game_state.blind;
        bool is_focused = is_selected && g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_BLIND;

        if (is_focused)
        {
            game_draw_pill(x - 2.0f, panel_top - 2.0f, panel_width + 4.0f, panel_height + 4.0f, COLOR_WHITE);
        }

        game_draw_pill(x - 1.0f, panel_top - 1.0f, panel_width + 2.0f, panel_height + 2.0f, style->border_color);
        game_draw_pill(x, panel_top, panel_width, panel_height, style->panel_fill);
        game_draw_pill(x + 1.0f, panel_top + 1.0f, panel_width - 2.0f, 14.0f, style->header_fill);
        game_draw_pill(x + 2.0f, panel_top + 18.0f, panel_width - 4.0f, panel_height - 22.0f, 0x22101010);
        game_draw_pill(x + 1.0f, panel_top + 30.0f, panel_width - 2.0f, 1.0f, style->border_color);

        if (!is_selected)
        {
            // Keep unavailable blinds visibly distinct without losing text readability.
            game_draw_pill(x + 1.0f, panel_top + 1.0f, panel_width - 2.0f, panel_height - 2.0f, 0x66101010);
            graphics_draw_text_center(font_small, "Locked", x + panel_width / 2.0f, panel_top + panel_height - 15.0f, 1.0f, 0xFFBBBBBB);
        }

        float y = panel_top + 10.0f;
        float blind_name_scale = graphics_get_text_scale_to_fit(font_small, game_util_get_blind_name(i), panel_width - 4.0f, 1.0f);
        graphics_draw_text(font_small, game_util_get_blind_name(i), x + 3, y + 1, blind_name_scale, COLOR_BLACK);
        graphics_draw_text(font_small, game_util_get_blind_name(i), x + 2, y, blind_name_scale, COLOR_WHITE);

        // Draw the blind chip icon (animated) centered below the name. 
        // Locked blinds are tinted grey to match the card state.
        int anim_frame = (g_game_counter / 10) % 15;
        graphics_set_texture(tex_blind_chips, GRAPHICS_TEXTURE_FILTER_NEAREST);
        graphics_draw_quad(floorf(x + (panel_width - 24.0f) / 2.0f), floorf(y + 12.0f), 24.0f, 24.0f, (int16_t)(anim_frame * 34), (int16_t)(i * 34), 34, 34, is_selected ? COLOR_WHITE : 0xFF888888);

        y += 46;
        graphics_draw_text(font_small, "Score at least", x + 2, y, 1.0f, 0xFFE8E8E8);

        y += 10;
        fmt_num(str, game_get_ante_base_score() * (1.0 + ((double)i * 0.5)));
        game_draw_pill(x + 1.0f, y - 1.0f, panel_width - 2.0f, 11.0f, style->badge_color);
        float score_req_scale = graphics_get_text_scale_to_fit(font_small, str, panel_width - 6.0f, 1.0f);
        graphics_draw_text(font_small, str, x + 3, y + 1, score_req_scale, COLOR_BLACK);
        graphics_draw_text(font_small, str, x + 2, y, score_req_scale, style->score_color);

        y += 18.0f;
        graphics_draw_text(font_small, "Reward", x + 2.0f, y, 1.0f, HUD_LABEL_COLOR);
        graphics_draw_text(font_small, "$$$", x + panel_width - 22.0f, y, 1.0f, COLOR_TEXT_YELLOW);

        if (is_selected)
        {
            float action_y = panel_top + panel_height - 58.0f;
            graphics_draw_quad(x + 3.0f, action_y - 4.0f, panel_width - 6.0f, 54.0f, 0, 0, 0, 0, 0xAA1A1A1A);

            game_draw_action_button(
                x + 6.0f, action_y,
                panel_width - 12.0f, 18.0f,
                "Select", game_util_get_blind_name(i),
                GAMEPAD_UI_CROSS, false,
                COLOR_GREEN_BUTTON,
                is_focused);

            graphics_draw_text_center(font_small, "or", x + panel_width / 2.0f, action_y + 21.0f, 1.0f, HUD_LABEL_COLOR);

            game_draw_action_button(
                x + 6.0f, action_y + 26.0f,
                panel_width - 12.0f, 18.0f,
                "Skip Blind", NULL,
                GAMEPAD_UI_CIRCLE, false,
                0xFF5A5A5A,
                false);
        }

        x += panel_width + LAYOUT_PANEL_GAP;
    }

    game_draw_top_jokers();

    game_draw_consumables();

    game_draw_score_number();
}

int inner_game_draw_offscreen_deck_info_rank_func(const void *a, const void *b)
{
    struct Card *card_a = *((struct Card **)a);
    struct Card *card_b = *((struct Card **)b);

    if (card_a == NULL) return 1;
    if (card_b == NULL) return -1;

    int rank_a = card_a->enhancement == CARD_ENHANCEMENT_STONE ? -1 : card_a->rank;
    int rank_b = card_b->enhancement == CARD_ENHANCEMENT_STONE ? -1 : card_b->rank;

    return rank_b - rank_a;
}

void game_draw_offscreen_deck_info()
{
    graphics_set_offscreen_render_target();

    graphics_draw_solid_quad(0, 0, DRAW_DECK_INFO_WIDTH, DRAW_DECK_INFO_HEIGHT, COLOR_DARK_GREY_2);
    graphics_draw_solid_quad(2, 2, DRAW_DECK_INFO_WIDTH - 4.0f, DRAW_DECK_INFO_HEIGHT - 4.0f, COLOR_DARK_GREY);

    for (int i = 0; i < 4; i++)
    {
        g_game_state.deck_info.card_count[i] = 0;
    }

    for (int j = 0; j < CARD_RANK_COUNT; j++) { g_game_state.deck_info.rank_count[j] = 0; g_game_state.deck_info.effective_rank_count[j] = 0; }
    for (int j = 0; j < 4; j++) { g_game_state.deck_info.suit_count[j] = 0; g_game_state.deck_info.effective_suit_count[j] = 0; }

    g_game_state.deck_info.face_count = 0;
    g_game_state.deck_info.effective_face_count = 0;
    g_game_state.deck_info.number_count = 0;
    g_game_state.deck_info.effective_number_count = 0;

    for (int i = 0; i < (g_game_state.deck_info.partial ? g_game_state.current_deck.card_count : g_game_state.full_deck.card_count); i++)
    {
        struct Card *card = g_game_state.full_deck.cards[i];
        if (g_game_state.deck_info.partial)
        {
            card = g_game_state.current_deck.cards[i];
        }

        for (int j = 0; j < CARD_RANK_COUNT; j++)
        {
            if (card->rank == j)
            {
                g_game_state.deck_info.rank_count[j]++;
            }

            if (game_util_is_card_rank(card, j))
            {
                g_game_state.deck_info.effective_rank_count[j]++;
            }
        }

        for (int j = 0; j < 4; j++)
        {
            if (card->suit == j)
            {
                g_game_state.deck_info.suit_count[j]++;
            }

            if (game_util_is_card_suit(card, j))
            {
                g_game_state.deck_info.effective_suit_count[j]++;
            }
        }

        if (card->rank == CARD_RANK_JACK || card->rank == CARD_RANK_QUEEN || card->rank == CARD_RANK_KING)
        {
            g_game_state.deck_info.face_count++;
        }
        else
        {
            g_game_state.deck_info.number_count++;
        }

        if (game_util_is_card_face(card))
        {
            g_game_state.deck_info.effective_face_count++;
        }
        else
        {
            g_game_state.deck_info.effective_number_count++;
        }
    }

    for (int i = 0; i < g_game_state.full_deck.card_count; i++)
    {
        struct Card *card = g_game_state.full_deck.cards[i];

        g_game_state.deck_info.cards[card->suit][g_game_state.deck_info.card_count[card->suit]] = card;
        g_game_state.deck_info.card_count[card->suit]++;
    }

    for (int i = 0; i < 4; i++)
    {
        qsort((void*)(g_game_state.deck_info.cards[i]), g_game_state.deck_info.card_count[i], sizeof(struct Card *), inner_game_draw_offscreen_deck_info_rank_func);
    }

    bool in_current_deck = false;

    g_freeze_cards = true;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < g_game_state.deck_info.card_count[i]; j++)
        {
            struct Card *card = g_game_state.deck_info.cards[i][j];
            struct DrawObject *draw = &(g_game_state.deck_info.draw[i][j]);
            game_init_draw_object(draw);
            draw->x = game_util_get_item_position(j, g_game_state.deck_info.card_count[i], 90.0f, DRAW_DECK_INFO_WIDTH - 96.0f, CARD_WIDTH);
            draw->y = 6.0f + 50.0f * i;

            game_draw_card(card, draw);

            in_current_deck = false;
            for(int w = 0; w < g_game_state.current_deck.card_count; w++)
            {
                if (g_game_state.current_deck.cards[w] == card)
                {
                    in_current_deck = true;
                    break;
                }
            }

            g_game_state.deck_info.in_current_deck[i][j] = in_current_deck;

            if (g_game_state.deck_info.partial && !in_current_deck)
            {
                graphics_set_texture(tex_enhancers, GRAPHICS_TEXTURE_FILTER_LINEAR);
                graphics_draw_quad(draw->x, draw->y, CARD_WIDTH, CARD_HEIGHT, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].x, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].y, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, 0xC0FFFFFF & COLOR_DARK_GREY);
            }

            draw->x += (SCREEN_WIDTH - DRAW_DECK_INFO_WIDTH) / 2.0f;
            draw->y += (SCREEN_HEIGHT - DRAW_DECK_INFO_HEIGHT) / 2.0f;
        }
    }    

    graphics_unset_offscreen_render_target();
}

// void game_draw_deck_draw_cards()
// {
//     float left = (SCREEN_WIDTH - DRAW_DECK_INFO_WIDTH) / 2.0f;
//     float top = (SCREEN_HEIGHT - DRAW_DECK_INFO_HEIGHT) / 2.0f;

//     graphics_draw_solid_quad(left + 0, top + 0, DRAW_DECK_INFO_WIDTH, DRAW_DECK_INFO_HEIGHT, COLOR_DARK_GREY_2);
//     graphics_draw_solid_quad(left + 2, top + 2, DRAW_DECK_INFO_WIDTH - 4.0f, DRAW_DECK_INFO_HEIGHT - 4.0f, COLOR_DARK_GREY);

//     for (int i = 0; i < 4; i++)
//     {
//         for (int j = 0; j < g_game_state.deck_info.card_count[i]; j++)
//         {
//             struct Card *card = g_game_state.deck_info.cards[i][j];
//             struct DrawObject *draw = &(g_game_state.deck_info.draw[i][j]);

//             game_draw_card(card, draw);

//             if (g_game_state.deck_info.partial && !g_game_state.deck_info.in_current_deck[i][j])
//             {
//                 graphics_set_texture(tex_enhancers, GRAPHICS_TEXTURE_FILTER_LINEAR);
//                 graphics_draw_quad(draw->x, draw->y, CARD_WIDTH, CARD_HEIGHT, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].x, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].y, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, 0xC0FFFFFF & COLOR_DARK_GREY);
//             }
//         }
//     }
// }

void game_draw_deck_info()
{
    char str[16];
    float left = (SCREEN_WIDTH - DRAW_DECK_INFO_WIDTH) / 2.0f;
    float top = (SCREEN_HEIGHT - DRAW_DECK_INFO_HEIGHT) / 2.0f;

    graphics_set_texture(-2, 0);
    graphics_draw_quad(left, top, DRAW_DECK_INFO_WIDTH, DRAW_DECK_INFO_HEIGHT, 0, 0, DRAW_DECK_INFO_WIDTH, DRAW_DECK_INFO_HEIGHT, COLOR_WHITE);

    graphics_draw_solid_quad(left + 4.0f, top + 6.0f, 80.0f, 14.0f, COLOR_DARK_GREY_2);
    if (g_game_state.deck_info.partial)
    {
        graphics_draw_text_center(font_small, "Remaining", left + 35.0f, top + 7.0f + 6.0f, 1.0f, COLOR_WHITE);
    }
    else
    {
        graphics_draw_text_center(font_small, "Full", left + 35.0f, top + 7.0f + 6.0f, 1.0f, COLOR_WHITE);
    }
    graphics_set_texture(tex_gamepad_ui, GRAPHICS_TEXTURE_FILTER_LINEAR);
    graphics_draw_quad(left + 4.0f + 60.0f, top + 5.0f, 18.0f, 18.0f, g_gamepad_ui_text_coords[GAMEPAD_UI_LEFT_TRIGGER].x, g_gamepad_ui_text_coords[GAMEPAD_UI_LEFT_TRIGGER].y, 32, 32, COLOR_WHITE);

    graphics_draw_solid_quad(left + 4.0f, top + 4.0f + 20.0f, 80.0f, 14.0f, COLOR_DARK_GREY_2);
    if (g_game_state.deck_info.effective)
    {
        graphics_draw_text_center(font_small, "Effective", left + 35.0f, top + 20.0f + 4.0f + 7.0f, 1.0f, COLOR_LIGHT_BLUE);
    }
    else
    {
        graphics_draw_text_center(font_small, "Base", left + 35.0f, top + 20.0f + 4.0f + 7.0f, 1.0f, COLOR_WHITE);
    }
    graphics_set_texture(tex_gamepad_ui, GRAPHICS_TEXTURE_FILTER_LINEAR);
    graphics_draw_quad(left + 4.0f + 60.0f, top + 5.0f + 18.0f, 18.0f, 18.0f, g_gamepad_ui_text_coords[GAMEPAD_UI_RIGHT_TRIGGER].x, g_gamepad_ui_text_coords[GAMEPAD_UI_RIGHT_TRIGGER].y, 32, 32, COLOR_WHITE);

    float suit_left = 60.0f;
    float suit_top = 48.0f;

    int c = 0;
    for (int i = CARD_RANK_COUNT - 1; i >= 0; i--, c++)
    {
        graphics_draw_solid_quad(left + suit_left - 7.0f, top + suit_top - 6.0f + c * 14.0f, 14.0f, 12.0f, COLOR_DARK_GREY_2);
        graphics_draw_text_center(font_small, g_short_rank_name[i], left + suit_left, top + suit_top + c * 14.0f, 1.0f, COLOR_LIGHT_GREY);
        if (g_game_state.deck_info.effective)
        {
            sprintf(str, "%d", g_game_state.deck_info.effective_rank_count[i]);
        }
        else
        {
            sprintf(str, "%d", g_game_state.deck_info.rank_count[i]);
        }        
        graphics_draw_text_center(font_small, str, left + suit_left + 18.0f, top + suit_top + c * 14.0f, 1.0f, g_game_state.deck_info.effective ? COLOR_LIGHT_BLUE : COLOR_WHITE);
    }

    float other_left = 8.0f;
    float other_top = 54.0f;

    for (int i = 0; i < UI_ASSETS_COUNT - 2; i++)
    {
        graphics_draw_solid_quad(left + other_left - 2.0f, top + other_top - 2.0f + i * 28.0f, 22.0f, 22.0f, COLOR_LIGHT_GREY_2);
        graphics_set_texture(tex_ui_assets, GRAPHICS_TEXTURE_FILTER_NEAREST);

        int asset = i+2;
        if (asset == 5) { asset = 6; }
        else if (asset == 6) { asset = 5; }
        graphics_draw_quad(left + other_left, top + other_top + i * 28.0f, 18, 18, g_ui_assets_tex_coords[asset].x, g_ui_assets_tex_coords[asset].y, 18, 18, COLOR_WHITE);
        if (g_game_state.deck_info.effective)
        {
            if (i == 0) { sprintf(str, "%d", g_game_state.deck_info.effective_face_count); }
            else if (i == 1) { sprintf(str, "%d", g_game_state.deck_info.effective_number_count); }
            else { sprintf(str, "%d", g_game_state.deck_info.effective_suit_count[i - 2]); }
        }
        else
        {
            if (i == 0) { sprintf(str, "%d", g_game_state.deck_info.face_count); }
            else if (i == 1) { sprintf(str, "%d", g_game_state.deck_info.number_count); }
            else { sprintf(str, "%d", g_game_state.deck_info.suit_count[i - 2]); }
        }
        graphics_draw_text_center(font_small, str, left + other_left + 32.0f, top + other_top + i * 28.0f + 9.0f, 1.0f, g_game_state.deck_info.effective ? COLOR_LIGHT_BLUE : COLOR_WHITE);
    }

    if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_DECK_INFO_CARD)
    {
        g_freeze_cards = true;
        game_draw_card(&g_game_state.deck_info.current_card, NULL);
        if (g_game_state.deck_info.partial && !g_game_state.deck_info.in_current_deck[g_game_state.deck_info.current_card_y][g_game_state.deck_info.current_card_x])
        {
            graphics_set_texture(tex_enhancers, GRAPHICS_TEXTURE_FILTER_LINEAR);
            graphics_draw_quad(g_game_state.deck_info.current_card.draw.x, g_game_state.deck_info.current_card.draw.y, CARD_WIDTH, CARD_HEIGHT, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].x, g_enhancement_tex_coords[CARD_ENHANCEMENT_NONE].y, TEXTURE_CARD_WIDTH, TEXTURE_CARD_HEIGHT, 0xC0FFFFFF & COLOR_DARK_GREY);
        }
        game_draw_card_frame(&(g_game_state.deck_info.current_card.draw));
        game_draw_card_hint(&g_game_state.deck_info.current_card);
    }

    if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_DECK_INFO_CLOSE)
    {
        graphics_draw_solid_quad((SCREEN_WIDTH - 50.0f) / 2.0f - 2.0f, 237.0f - 2.0f, 50.0f + 4.0f, 15.0f + 4.0f, COLOR_WHITE);
    }
    graphics_draw_solid_quad((SCREEN_WIDTH - 50.0f) / 2.0f, 237.0f, 50.0f, 15.0f, COLOR_RED_BUTTON);
    graphics_draw_text_center(font_small, "Close", (SCREEN_WIDTH) / 2.0f, 244.0f, 1.0f, COLOR_WHITE);

}

void game_draw_run_info()
{
    float left = (SCREEN_WIDTH - DRAW_RUN_INFO_WIDTH) / 2.0f;
    float top = (SCREEN_HEIGHT - DRAW_RUN_INFO_HEIGHT) / 2.0f;

    graphics_draw_solid_quad(left, top, DRAW_RUN_INFO_WIDTH, DRAW_RUN_INFO_HEIGHT, COLOR_DARK_GREY_2);
    graphics_draw_solid_quad(left + 2, top + 2, DRAW_RUN_INFO_WIDTH - 4.0f, DRAW_RUN_INFO_HEIGHT - 4.0f, COLOR_DARK_GREY);

    graphics_draw_text_center(font_big, "Poker Hands", SCREEN_WIDTH / 2.0f, top + 12.0f, 1.0f, COLOR_WHITE);

    float y = 8.0f;
    // Define column anchors relative to 'left'
    const float col_lvl_label = 20.0f;
    const float col_lvl_val   = 40.0f;
    const float col_name      = 110.0f;
    const float col_chips     = 200.0f;
    const float col_mult      = 250.0f;
    const float col_count     = 295.0f;

    char str[16];
    top += 20.0f;
    for (int i = 1; i < GAME_POKER_HAND_COUNT; i++)
    {
        game_draw_pill(left + 4.0f, top + y - 2.0f, 304.0f, 12.0f, COLOR_LIGHT_GREY);
        
        graphics_draw_solid_quad(left + 179.0f, top + y - 2.0f, 92.0f, 12.0f, COLOR_DARK_GREY_2);
        graphics_draw_solid_quad(left + 180.0f, top + y - 1.0f, 40.0f, 10.0f, COLOR_LIGHT_BLUE);
        graphics_draw_solid_quad(left + 230.0f, top + y - 1.0f, 40.0f, 10.0f, COLOR_LIGHT_RED);

        graphics_draw_solid_quad(left + 285.0f, top + y - 1.0f, 20.0f, 10.0f, COLOR_DARK_GREY_2);

        graphics_draw_text_center(font_small, "Lvl.", left + col_lvl_label, top + y + 4.0f, 1.0f, COLOR_WHITE);

        sprintf(str, "%d", g_game_state.poker_hand_level[i]);
        graphics_draw_text_center(font_small, str, left + col_lvl_val, top + y + 4.0f, 1.0f, COLOR_WHITE);

        float row_name_scale = graphics_get_text_scale_to_fit(font_small, g_poker_hand_names[i], 120.0f, 1.0f);
        graphics_draw_text_center(font_small, g_poker_hand_names[i], left + col_name, top + y + 4.0f, row_name_scale, COLOR_WHITE);

        sprintf(str, "%d", g_game_state.poker_hand_base_chips[i]);
        float chips_scale = graphics_get_text_scale_to_fit(font_small, str, 45.0f, 1.0f);
        graphics_draw_text_center(font_small, str, left + col_chips, top + y + 4.0f, chips_scale, COLOR_WHITE);

        graphics_draw_text_center(font_small, "x", left + 226.0f, top + y + 4.0f, 1.0f, COLOR_LIGHT_RED);

        sprintf(str, "%d", g_game_state.poker_hand_base_mult[i]);
        float mult_scale = graphics_get_text_scale_to_fit(font_small, str, 45.0f, 1.0f);
        graphics_draw_text_center(font_small, str, left + col_mult, top + y + 4.0f, mult_scale, COLOR_WHITE);

        graphics_draw_text_center(font_small, "#", left + col_mult + 30.0f, top + y + 4.0f, 1.0f, COLOR_WHITE);
        sprintf(str, "%d", g_game_state.stats.poker_hands_played[i]);
        float count_scale = graphics_get_text_scale_to_fit(font_small, str, 18.0f, 1.0f);
        graphics_draw_text_center(font_small, str, left + col_count, top + y + 4.0f, count_scale, COLOR_TEXT_ORANGE);

        y += 17.0f;
    }

    if (g_game_state.input_focused_zone == INPUT_FOCUSED_ZONE_RUN_INFO_CLOSE)
    {
        graphics_draw_solid_quad((SCREEN_WIDTH - 50.0f) / 2.0f - 2.0f, 240.0f - 2.0f, 50.0f + 4.0f, 15.0f + 4.0f, COLOR_WHITE);
    }
    graphics_draw_solid_quad((SCREEN_WIDTH - 50.0f) / 2.0f, 240.0f, 50.0f, 15.0f, COLOR_RED_BUTTON);
    graphics_draw_text_center(font_small, "Close", (SCREEN_WIDTH) / 2.0f, 247.0f, 1.0f, COLOR_WHITE);
}

uint64_t last_tick = 0;
uint32_t tick_res = 0;
float curr_ms = 1.0f;
float curr_fps = 0.0f;
int g_frame_count;

void game_draw_debug_info()
{
    g_time++;

    if (last_tick == 0) { sceRtcGetCurrentTick(&last_tick); }
    if (tick_res == 0) { tick_res = sceRtcGetTickResolution(); }

    g_frame_count++;

    uint64_t curr_tick;
	sceRtcGetCurrentTick(&curr_tick);

    if ((curr_tick-last_tick) >= tick_res)
    {
        curr_fps = 1.0f / curr_ms;

        double time_span = ((curr_tick-last_tick)) / (float)tick_res;
        curr_ms = time_span / g_frame_count;
        g_frame_count = 0;
        sceRtcGetCurrentTick(&last_tick);
    }

    #ifdef DEBUG
    char str[32];
    sprintf(str, "FPS: %0.2f", curr_fps);
    graphics_draw_text(font_small, str, 400, 0, 1.0f, COLOR_WHITE);
    if (g_settings.audio)
    {
        sprintf(str, "audio w read: %d", g_debug_info.audio_wait_read);
        graphics_draw_text(font_small, str, 370, 8, 1.0f, COLOR_WHITE);
        sprintf(str, "audio w write: %d", g_debug_info.audio_wait_write);
        graphics_draw_text(font_small, str, 364, 16, 1.0f, COLOR_WHITE);
    }
    #endif
}

static void game_draw_background_depth_layers()
{
    graphics_set_no_texture();

    bool use_blind_background = (g_game_state.stage == GAME_STAGE_MENU || g_game_state.blind == GAME_BLIND_SMALL || g_game_state.blind == GAME_BLIND_LARGE) && tex_blind_background >= 0;
    if (use_blind_background)
    {
        game_draw_static_background_texture(tex_blind_background);
        graphics_draw_quad(0.0f, 0.0f, SCREEN_WIDTH, 24.0f, 0, 0, 0, 0, 0x08000000);
        graphics_draw_quad(0.0f, SCREEN_HEIGHT - 24.0f, SCREEN_WIDTH, 24.0f, 0, 0, 0, 0, 0x08000000);
        graphics_draw_quad(LAYOUT_CENTER_X - 10.0f, 32.0f, LAYOUT_CENTER_WIDTH + 20.0f, SCREEN_HEIGHT - 90.0f, 0, 0, 0, 0, 0x04000000);
    }
    else
    {
        // Layer 1: broad atmospheric modulation (top cool cast + bottom weight).
        graphics_draw_quad(0.0f, 0.0f, SCREEN_WIDTH, 44.0f, 0, 0, 0, 0, 0x10253A44);
        graphics_draw_quad(0.0f, SCREEN_HEIGHT - 62.0f, SCREEN_WIDTH, 62.0f, 0, 0, 0, 0, 0x13000000);
        graphics_draw_quad(LAYOUT_CENTER_X - 10.0f, 32.0f, LAYOUT_CENTER_WIDTH + 20.0f, SCREEN_HEIGHT - 90.0f, 0, 0, 0, 0, 0x06243F50);
    }

    // Layer 2: soft vignette + sparse scanline texture for depth.
    for (int i = 0; i < 9; i++)
    {
        uint8_t a = (uint8_t)(16 - i);
        uint32_t edge = ((uint32_t)a << 24);
        float inset = (float)i;

        graphics_draw_quad(inset, inset, SCREEN_WIDTH - inset * 2.0f, 1.0f, 0, 0, 0, 0, edge);
        graphics_draw_quad(inset, SCREEN_HEIGHT - 1.0f - inset, SCREEN_WIDTH - inset * 2.0f, 1.0f, 0, 0, 0, 0, edge);
        graphics_draw_quad(inset, inset, 1.0f, SCREEN_HEIGHT - inset * 2.0f, 0, 0, 0, 0, edge);
        graphics_draw_quad(SCREEN_WIDTH - 1.0f - inset, inset, 1.0f, SCREEN_HEIGHT - inset * 2.0f, 0, 0, 0, 0, edge);
    }

    for (int y = 28; y < SCREEN_HEIGHT; y += 14)
    {
        uint32_t line = ((y / 14) % 2 == 0) ? 0x05000000 : 0x03000000;
        graphics_draw_quad(0.0f, (float)y, SCREEN_WIDTH, 1.0f, 0, 0, 0, 0, line);
    }
}

void game_draw_title_screen()
{
    // Debugging: Draw a solid background to ensure the screen is not entirely black due to drawing issues.
    graphics_set_no_texture();
    graphics_draw_solid_quad(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFF0000FF); // Bright blue background to indicate drawing is active

    // 1. Draw the Background (balatro_bg_alt.png)
    if (tex_title_bg_alt >= 0) {
        // We sample the full 512x272 buffer which contains the squashed wallpaper.
        graphics_set_texture(tex_title_bg_alt, GRAPHICS_TEXTURE_FILTER_LINEAR);
        graphics_draw_quad(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 512, 272, COLOR_WHITE);
    } else {
        // Fallback if background texture failed to load
        graphics_set_no_texture();
        graphics_draw_solid_quad(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFF800000); // Dark red fallback
        if (font_small >= 0) graphics_draw_text_center(font_small, "BG Texture Missing!", SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f - 20.0f, 1.0f, COLOR_WHITE);
    }

    // 2. Draw the Mascot Card (Ace of Spades)
    struct Card ace;
    memset(&ace, 0, sizeof(struct Card));
    ace.rank = 12; // Ace
    ace.suit = 0;  // Spades
    
    // Scale reduced to 1.0f to allow the logo to be visible and fix layout crowding.
    ace.draw.scale = 1.0f;
    ace.draw.x = (SCREEN_WIDTH / 2.0f) - (CARD_WIDTH / 2.0f);
    ace.draw.y = (SCREEN_HEIGHT / 2.0f) - (CARD_HEIGHT / 2.0f) + 35.0f;
    
    // Subtle float animation
    ace.draw.angle = sinf(g_game_counter * 0.04f) * 0.04f;
    ace.draw.alpha = 1.0f;
    
    // Only draw the card if the main deck texture is loaded, and fonts are available for potential debug messages.
    // Assuming tex_deck is the primary texture for cards.
    if (tex_deck >= 0 && font_small >= 0) {
        game_draw_card(&ace, NULL);
    } else {
        graphics_set_no_texture();
        graphics_draw_solid_quad(SCREEN_WIDTH / 2.0f - 50.0f, SCREEN_HEIGHT / 2.0f + 20.0f, 100.0f, 50.0f, 0xFF008080); // Teal fallback
        if (font_small >= 0) graphics_draw_text_center(font_small, "Card Texture Missing!", SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f + 40.0f, 1.0f, COLOR_WHITE);
    }

    // 4. Draw the Prompt
    float prompt_y = SCREEN_HEIGHT * 0.88f;
    uint32_t text_color = (g_game_counter % 60 < 30) ? COLOR_WHITE : 0xFFCCCCCC;
    if (font_small >= 0) graphics_draw_text_center(font_small, "PRESS X", SCREEN_WIDTH / 2.0f, prompt_y, 1.0f, text_color);
}

void game_draw()
{
    graphics_begin_draw();
    graphics_clear(COLOR_BACKGROUND_2);

    if (g_game_state.stage == GAME_STAGE_BLINDS ||
        g_game_state.stage == GAME_STAGE_INGAME ||
        g_game_state.stage == GAME_STAGE_SHOP ||
        g_game_state.stage == GAME_STAGE_MENU)
    {
        game_draw_background_depth_layers();
    }

    switch (g_game_state.stage)
    {
        case GAME_STAGE_MENU:
            game_draw_title_screen();
            break;
        case GAME_STAGE_BLINDS:
            game_draw_blind_select();            
            break;
        case GAME_STAGE_INGAME:
            game_draw_ingame();
            break;
        case GAME_STAGE_SHOP:
        {
            game_draw_shop();
            break;
        }
    }

    if (g_game_state.sub_stage == GAME_SUBSTAGE_DECK_INFO)
    {
        game_draw_deck_info();
    }

    if (g_game_state.sub_stage == GAME_SUBSTAGE_RUN_INFO)
    {
        game_draw_run_info();
    }

    if (g_settings.debug_overlay)
    {
        game_draw_debug_info();
    }

    graphics_end_draw();
}
