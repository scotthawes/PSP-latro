// menu.c - Title screen and main menu implementation
// Included as a unity-build unit from main.c

// Forward declarations
static void menu_draw_rect(float x, float y, float w, float h, uint32_t color);
static void menu_draw_rect_border(float x, float y, float w, float h, uint32_t color, int thickness);

// ----------------------------------------------------------------
// Menu item definitions
// ----------------------------------------------------------------

#define MENU_MAIN_ITEM_NEW_RUN  0
#define MENU_MAIN_ITEM_OPTIONS  1
#define MENU_MAIN_ITEM_QUIT     2
#define MENU_MAIN_ITEM_COLLECTION 3
#define MENU_MAIN_ITEM_COUNT    4

static const char *s_main_menu_labels[MENU_MAIN_ITEM_COUNT] = {
    "Play",
    "Options",
    "Quit",
    "Collection"
};

static void menu_activate_main_item()
{
    switch (g_game_state.menu_selected_item)
    {
        case MENU_MAIN_ITEM_NEW_RUN:
            if (g_init_resources_ok)
            {
                game_begin_new_run();
            }
            break;
        case MENU_MAIN_ITEM_OPTIONS:
            g_game_state.menu_selected_item = 0;
            g_game_state.sub_stage = GAME_SUBSTAGE_MENU_OPTIONS;
            g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_MENU_OPTIONS;
            break;
        case MENU_MAIN_ITEM_QUIT:
            sceKernelExitGame();
            break;
        case MENU_MAIN_ITEM_COLLECTION:
            // Placeholder route for upcoming collection screen work.
            g_game_state.menu_selected_item = 0;
            g_game_state.sub_stage = GAME_SUBSTAGE_MENU_OPTIONS;
            g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_MENU_OPTIONS;
            break;
    }
}

static int s_title_wallpaper_texture = -1;
static int s_main_wallpaper_texture = -1;
static bool s_wallpapers_initialized = false;
static int s_wallpaper_variant = 0;
static int s_title_logo_texture = -1;
static int s_profile_icon_texture = -1;
static int s_language_icon_texture = -1;
static int s_social_discord_icon_texture = -1;
static int s_social_x_icon_texture = -1;
static int s_action_button_textures[MENU_MAIN_ITEM_COUNT] = { -1, -1, -1, -1 };

#define MENU_WALLPAPER_VARIANT_COUNT 2
static int s_title_wallpaper_textures[MENU_WALLPAPER_VARIANT_COUNT] = { -1, -1 };
static int s_main_wallpaper_textures[MENU_WALLPAPER_VARIANT_COUNT] = { -1, -1 };
static int s_title_logo_textures[MENU_WALLPAPER_VARIANT_COUNT] = { -1, -1 };

#define MENU_TITLE_BACKGROUND_PATH "backgrounds/balatro_bg_alt.png"
#define MENU_TITLE_LOGO_PATH "balatro-ui-optimized/logo_main_psp.png"
#define MENU_PROFILE_ICON_PATH "balatro-ui-optimized/icons/icon_r0_c0.png"
#define MENU_LANGUAGE_ICON_PATH "balatro-ui-optimized/icons/icon_r0_c1.png"
#define MENU_SOCIAL_DISCORD_ICON_PATH "balatro-ui-optimized/icons/icon_r0_c2.png"
#define MENU_SOCIAL_X_ICON_PATH "balatro-ui-optimized/icons/icon_r0_c3.png"
#define MENU_ACTION_BUTTON_PLAY_PATH "balatro-ui-optimized/buttons/button_r0_c0.png"
#define MENU_ACTION_BUTTON_OPTIONS_PATH "balatro-ui-optimized/buttons/button_r0_c1.png"
#define MENU_ACTION_BUTTON_QUIT_PATH "balatro-ui-optimized/buttons/button_r0_c2.png"
#define MENU_ACTION_BUTTON_COLLECTION_PATH "balatro-ui-optimized/buttons/button_r0_c3.png"

struct MenuVariantAssetMap
{
    const char *title_logo;
};

static const struct MenuVariantAssetMap s_menu_variant_assets[MENU_WALLPAPER_VARIANT_COUNT] = {
    {
        MENU_TITLE_LOGO_PATH
    },
    {
        MENU_TITLE_LOGO_PATH
    }
};

#define MENU_OPTIONS_ITEM_OVERCLOCK 0
#define MENU_OPTIONS_ITEM_AUDIO     1
#define MENU_OPTIONS_ITEM_THEME     2
#define MENU_OPTIONS_ITEM_BACK      3
#define MENU_OPTIONS_ITEM_COUNT     4

// Wallpaper loading removed per user constraint: do not use wallpapers/ directory

static void menu_load_logo_variant(int variant)
{
    if (variant < 0 || variant >= MENU_WALLPAPER_VARIANT_COUNT)
    {
        return;
    }

    if (s_title_logo_textures[variant] >= 0)
    {
        return;
    }

    s_title_logo_textures[variant] = graphics_load_wallpaper(s_menu_variant_assets[variant].title_logo);
}

static void menu_apply_active_variant_assets()
{
    if (s_wallpaper_variant < 0 || s_wallpaper_variant >= MENU_WALLPAPER_VARIANT_COUNT)
    {
        s_wallpaper_variant = 0;
    }

    // Title background comes from PSP-safe non-wallpapers assets.
    s_title_wallpaper_texture = s_title_wallpaper_textures[s_wallpaper_variant];
    s_main_wallpaper_texture = -1;

    if (s_title_logo_textures[s_wallpaper_variant] < 0)
    {
        s_title_logo_textures[s_wallpaper_variant] = s_title_logo_textures[0];
    }

    s_title_logo_texture = s_title_logo_textures[s_wallpaper_variant];
}

// Wallpaper variant loading removed per user constraint: do not use wallpapers/ directory

// ----------------------------------------------------------------
// Draw helpers
// ----------------------------------------------------------------

// Draw a solid rectangle using the existing GU quad batcher
static void menu_draw_rect(float x, float y, float w, float h, uint32_t color)
{
    graphics_set_texture(-1, -1);
    graphics_draw_quad(x, y, w, h, 0, 0, 0, 0, color);
}

// Draw a bordered rectangle (outline only)
static void menu_draw_rect_border(float x, float y, float w, float h, uint32_t color, int thickness)
{
    graphics_set_texture(-1, -1);
    // Top
    graphics_draw_quad(x, y, w, thickness, 0, 0, 0, 0, color);
    // Bottom
    graphics_draw_quad(x, y + h - thickness, w, thickness, 0, 0, 0, 0, color);
    // Left
    graphics_draw_quad(x, y, thickness, h, 0, 0, 0, 0, color);
    // Right
    graphics_draw_quad(x + w - thickness, y, thickness, h, 0, 0, 0, 0, color);
}

// Draw a horizontal line
static void menu_draw_line_h(float x, float y, float w, uint32_t color)
{
    graphics_set_texture(-1, -1);
    graphics_draw_quad(x, y, w, 1, 0, 0, 0, 0, color);
}

static void menu_draw_texture_fill(int texture_id, float x, float y, float w, float h, uint32_t tint)
{
    if (texture_id < 0)
    {
        return;
    }

    int content_w = 0;
    int content_h = 0;
    if (!graphics_get_texture_content_size(texture_id, &content_w, &content_h) || content_w <= 0 || content_h <= 0)
    {
        content_w = (int)w;
        content_h = (int)h;
    }

    graphics_set_texture(texture_id, GRAPHICS_TEXTURE_FILTER_LINEAR);
    graphics_draw_quad(x, y, w, h, 0, 0, content_w, content_h, tint);
}

static void menu_validate_rect(const char *name, float x, float y, float w, float h)
{
    if (x < 0.0f || y < 0.0f || (x + w) > SCREEN_WIDTH || (y + h) > SCREEN_HEIGHT)
    {
        DEBUG_PRINTF("[MENU] layout overflow: %s (x=%.1f y=%.1f w=%.1f h=%.1f)\n", name, x, y, w, h);
    }
}

// Draw a decorative horizontal separator with accent
static void menu_draw_separator(float x, float y, float w, uint32_t accent_color)
{
    // Left accent cap
    menu_draw_rect(x, y - 1, 4, 3, accent_color);
    // Main line
    menu_draw_line_h(x + 4, y, w - 8, 0xFF404040);
    // Right accent cap
    menu_draw_rect(x + w - 4, y - 1, 4, 3, accent_color);
}

// ----------------------------------------------------------------
// Function to render wallpaper
// ----------------------------------------------------------------

static void menu_init_wallpapers()
{
    if (s_wallpapers_initialized)
    {
        return;
    }

    for (int i = 0; i < MENU_WALLPAPER_VARIANT_COUNT; i++)
    {
        s_title_wallpaper_textures[i] = -1;
        s_main_wallpaper_textures[i] = -1;
        s_title_logo_textures[i] = -1;
    }

    s_wallpaper_variant = CLAMP(g_settings.wallpaper_variant, 0, MENU_WALLPAPER_VARIANT_COUNT - 1);

    s_title_wallpaper_textures[0] = graphics_load_wallpaper(MENU_TITLE_BACKGROUND_PATH);
    if (s_title_wallpaper_textures[0] < 0)
    {
        DEBUG_PRINTF("[MENU] failed to load title background: %s\n", MENU_TITLE_BACKGROUND_PATH);
    }
    s_title_wallpaper_textures[1] = s_title_wallpaper_textures[0];

    s_profile_icon_texture = graphics_load_wallpaper(MENU_PROFILE_ICON_PATH);
    s_language_icon_texture = graphics_load_wallpaper(MENU_LANGUAGE_ICON_PATH);
    s_social_discord_icon_texture = graphics_load_wallpaper(MENU_SOCIAL_DISCORD_ICON_PATH);
    s_social_x_icon_texture = graphics_load_wallpaper(MENU_SOCIAL_X_ICON_PATH);

    s_action_button_textures[MENU_MAIN_ITEM_NEW_RUN] = graphics_load_wallpaper(MENU_ACTION_BUTTON_PLAY_PATH);
    s_action_button_textures[MENU_MAIN_ITEM_OPTIONS] = graphics_load_wallpaper(MENU_ACTION_BUTTON_OPTIONS_PATH);
    s_action_button_textures[MENU_MAIN_ITEM_QUIT] = graphics_load_wallpaper(MENU_ACTION_BUTTON_QUIT_PATH);
    s_action_button_textures[MENU_MAIN_ITEM_COLLECTION] = graphics_load_wallpaper(MENU_ACTION_BUTTON_COLLECTION_PATH);

    // Main menu keeps solid fallback; title uses mapped PSP assets.
    menu_load_logo_variant(0);
    menu_load_logo_variant(s_wallpaper_variant);
    menu_apply_active_variant_assets();

    s_wallpapers_initialized = true;
}

void menu_draw_wallpaper(int texture_id) {
    if (texture_id < 0)
    {
        menu_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFF0F0F0F);
        return;
    }

    int content_w = SCREEN_WIDTH;
    int content_h = SCREEN_HEIGHT;
    if (!graphics_get_texture_content_size(texture_id, &content_w, &content_h))
    {
        content_w = SCREEN_WIDTH;
        content_h = SCREEN_HEIGHT;
        DEBUG_PRINTF("[WALLPAPER] texture %d missing size metadata, using screen size UV\n", texture_id);
    }

    graphics_set_texture(texture_id, GRAPHICS_TEXTURE_FILTER_LINEAR);
    graphics_draw_quad(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, content_w, content_h, 0xFFFFFFFF);
}

// ----------------------------------------------------------------
// Title screen draw
// ----------------------------------------------------------------
static void menu_draw_title()
{
    menu_init_wallpapers();
    menu_draw_wallpaper(s_title_wallpaper_texture);

    // Keep more of the background art visible to match reference tone.
    menu_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x4A090909);

    // Subtle top vignette behind logo for readability without boxing in the art.
    float logo_backdrop_y = 22.0f;
    float logo_backdrop_h = 120.0f;
    menu_draw_rect(0, logo_backdrop_y, SCREEN_WIDTH, logo_backdrop_h, 0x32000000);
    menu_validate_rect("logo_backdrop", 0.0f, logo_backdrop_y, SCREEN_WIDTH, logo_backdrop_h);

    if (s_title_logo_texture >= 0)
    {
        int logo_w = 0;
        int logo_h = 0;
        if (!graphics_get_texture_content_size(s_title_logo_texture, &logo_w, &logo_h))
        {
            logo_w = 185;
            logo_h = 120;
        }

        float logo_scale = 1.28f;
        float logo_draw_w = logo_w * logo_scale;
        float logo_draw_h = logo_h * logo_scale;
        float logo_x = (SCREEN_WIDTH - logo_draw_w) * 0.5f;
        float logo_y = 24.0f;
        menu_validate_rect("title_logo", logo_x, logo_y, logo_draw_w, logo_draw_h);

        graphics_set_texture(s_title_logo_texture, GRAPHICS_TEXTURE_FILTER_LINEAR);
        graphics_draw_quad(logo_x, logo_y, logo_draw_w, logo_draw_h, 0, 0, logo_w, logo_h, 0xFFFFFFFF);
    }
    else
    {
        // Fallback title text when optimized logo is unavailable.
        graphics_draw_text_center(font_big, "PSPalatro", SCREEN_WIDTH / 2.0f, 75.0f, 2.5f, 0xFFC8AA6E);
    }

    // Bottom action bar
    float bar_x = 88.0f;
    float bar_y = 222.0f;
    float bar_w = SCREEN_WIDTH - 176.0f;
    float bar_h = 32.0f;
    menu_draw_rect(bar_x, bar_y, bar_w, bar_h, 0x9C171717);
    menu_draw_rect_border(bar_x, bar_y, bar_w, bar_h, 0xFF3B546C, 1);
    menu_validate_rect("action_bar", bar_x, bar_y, bar_w, bar_h);

    float button_gap = 6.0f;
    float button_w = (bar_w - 10.0f - (MENU_MAIN_ITEM_COUNT - 1) * button_gap) / MENU_MAIN_ITEM_COUNT;
    float button_h = 22.0f;
    float button_y = bar_y + 5.0f;
    float button_x = bar_x + 5.0f;

    for (int i = 0; i < MENU_MAIN_ITEM_COUNT; i++)
    {
        uint32_t fill = 0xFF2A2A2A;
        uint32_t border = 0xFF555555;
        uint32_t text = 0xFFE0E0E0;
        uint32_t texture_tint = 0xCCFFFFFF;

        if (i == g_game_state.menu_selected_item)
        {
            fill = 0xFF1F4F5F;
            border = 0xFFC8AA6E;
            text = 0xFFFFE3A8;
            texture_tint = 0xFFFFFFFF;
        }

        menu_draw_rect(button_x, button_y, button_w, button_h, fill);
        menu_draw_texture_fill(s_action_button_textures[i], button_x + 1.0f, button_y + 1.0f, button_w - 2.0f, button_h - 2.0f, texture_tint);
        menu_draw_rect_border(button_x, button_y, button_w, button_h, border, 1);
        graphics_draw_text_center(font_small, s_main_menu_labels[i],
                      button_x + button_w * 0.5f, button_y + 6.0f, 0.8f, text);
        menu_validate_rect("action_button", button_x, button_y, button_w, button_h);

        button_x += button_w + button_gap;
    }

    // Profile panel (bottom-left)
    float profile_x = 18.0f;
    float profile_y = 222.0f;
    float profile_w = 86.0f;
    float profile_h = 32.0f;
    menu_draw_rect(profile_x, profile_y, profile_w, profile_h, 0xBC15202B);
    menu_draw_rect_border(profile_x, profile_y, profile_w, profile_h, 0xFF5A738C, 1);
    menu_validate_rect("profile_panel", profile_x, profile_y, profile_w, profile_h);
    graphics_draw_text(font_small, "Profile", profile_x + 22.0f, profile_y + 6.0f, 0.74f, 0xFFE0E6EC);
    graphics_draw_text(font_small, "P1", profile_x + 22.0f, profile_y + 16.0f, 0.84f, 0xFFFFFFFF);

    if (s_profile_icon_texture >= 0)
    {
        int iw = 0;
        int ih = 0;
        if (!graphics_get_texture_content_size(s_profile_icon_texture, &iw, &ih) || iw <= 0 || ih <= 0)
        {
            iw = 24;
            ih = 24;
        }
        graphics_set_texture(s_profile_icon_texture, GRAPHICS_TEXTURE_FILTER_LINEAR);
        graphics_draw_quad(profile_x + 3.0f, profile_y + 2.0f, 16.0f, 16.0f, 0, 0, iw, ih, 0xFFFFFFFF);
    }

    // Language selector chip (bottom-right)
    float lang_w = 90.0f;
    float lang_h = 26.0f;
    float lang_x = SCREEN_WIDTH - 18.0f - lang_w;
    float lang_y = 226.0f;
    menu_draw_rect(lang_x, lang_y, lang_w, lang_h, 0xB0242424);
    menu_draw_rect_border(lang_x, lang_y, lang_w, lang_h, 0xFF6C6C6C, 1);
    menu_validate_rect("language_chip", lang_x, lang_y, lang_w, lang_h);
    graphics_draw_text(font_small, "English", lang_x + 24.0f, lang_y + 9.0f, 0.75f, 0xFFF4F4F4);

    if (s_language_icon_texture >= 0)
    {
        int iw = 0;
        int ih = 0;
        if (!graphics_get_texture_content_size(s_language_icon_texture, &iw, &ih) || iw <= 0 || ih <= 0)
        {
            iw = 24;
            ih = 24;
        }
        graphics_set_texture(s_language_icon_texture, GRAPHICS_TEXTURE_FILTER_LINEAR);
        graphics_draw_quad(lang_x + 4.0f, lang_y + 5.0f, 14.0f, 14.0f, 0, 0, iw, ih, 0xFFFFFFFF);
    }

    // Utility/social icon chips near lower-right cluster
    float social_y = 198.0f;
    float social_size = 22.0f;
    float social_gap = 6.0f;
    float social_x_right = SCREEN_WIDTH - 18.0f - social_size;
    float social_x_left = social_x_right - social_gap - social_size;

    menu_draw_rect(social_x_left, social_y, social_size, social_size, 0xB0222233);
    menu_draw_rect_border(social_x_left, social_y, social_size, social_size, 0xFF5A738C, 1);
    menu_draw_rect(social_x_right, social_y, social_size, social_size, 0xB0222233);
    menu_draw_rect_border(social_x_right, social_y, social_size, social_size, 0xFF5A738C, 1);
    menu_validate_rect("social_left_chip", social_x_left, social_y, social_size, social_size);
    menu_validate_rect("social_right_chip", social_x_right, social_y, social_size, social_size);

    if (s_social_discord_icon_texture >= 0)
    {
        int iw = 0;
        int ih = 0;
        if (!graphics_get_texture_content_size(s_social_discord_icon_texture, &iw, &ih) || iw <= 0 || ih <= 0)
        {
            iw = 24;
            ih = 24;
        }
        graphics_set_texture(s_social_discord_icon_texture, GRAPHICS_TEXTURE_FILTER_LINEAR);
        graphics_draw_quad(social_x_left + 4.0f, social_y + 4.0f, 14.0f, 14.0f, 0, 0, iw, ih, 0xFFFFFFFF);
    }
    else
    {
        graphics_draw_text_center(font_small, "D", social_x_left + social_size * 0.5f, social_y + 7.0f, 0.8f, 0xFFE8E8FF);
    }

    if (s_social_x_icon_texture >= 0)
    {
        int iw = 0;
        int ih = 0;
        if (!graphics_get_texture_content_size(s_social_x_icon_texture, &iw, &ih) || iw <= 0 || ih <= 0)
        {
            iw = 24;
            ih = 24;
        }
        graphics_set_texture(s_social_x_icon_texture, GRAPHICS_TEXTURE_FILTER_LINEAR);
        graphics_draw_quad(social_x_right + 4.0f, social_y + 4.0f, 14.0f, 14.0f, 0, 0, iw, ih, 0xFFFFFFFF);
    }
    else
    {
        graphics_draw_text_center(font_small, "X", social_x_right + social_size * 0.5f, social_y + 7.0f, 0.8f, 0xFFE8E8FF);
    }

    // Minimal top-right version label (no heavy chip)
    float version_x = SCREEN_WIDTH - 68.0f;
    float version_y = 8.0f;
    graphics_draw_text(font_small, "v0.1-FULL", version_x, version_y, 0.62f, 0xFFD0D0D0);
    menu_validate_rect("version_label", version_x, version_y, 60.0f, 8.0f);
}

// ----------------------------------------------------------------
// Main menu draw
// ----------------------------------------------------------------
static void menu_draw_main()
{
    // Unified layout: main shares title screen composition.
    menu_draw_title();
}

// ----------------------------------------------------------------
// Options menu draw
// ----------------------------------------------------------------
static void menu_draw_options()
{
    // Dark background
    menu_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFF0F0F0F);

    // Top decorative stripe
    menu_draw_rect(0, 0, SCREEN_WIDTH, 6, 0xFFC8AA6E);
    menu_draw_rect(0, 6, SCREEN_WIDTH, 1, 0xFF7A6F42);

    // Title banner
    menu_draw_rect(0, 12, SCREEN_WIDTH, 35, 0xFF1A1A1A);
    menu_draw_rect_border(0, 12, SCREEN_WIDTH, 35, 0xFF006D96, 1);
    graphics_draw_text_center(font_big, "Options", SCREEN_WIDTH / 2.0f, 25.0f, 1.8f, 0xFFC8AA6E);

    float panel_x = 60.0f;
    float panel_y = 65.0f;
    float panel_w = SCREEN_WIDTH - 120.0f;
    float item_h  = 32.0f;
    float panel_h = MENU_OPTIONS_ITEM_COUNT * item_h + 16.0f;

    // Panel with border
    menu_draw_rect(panel_x, panel_y, panel_w, panel_h, 0xFF1A1A1A);
    menu_draw_rect_border(panel_x, panel_y, panel_w, panel_h, 0xFF006D96, 2);

    const char *overclock_val = g_settings.overclock ? "ON" : "OFF";
    const char *audio_val     = g_settings.audio     ? "ON" : "OFF";
    const char *theme_val = (s_wallpaper_variant == 0) ? "Classic" : "Alt";

    char option_labels[MENU_OPTIONS_ITEM_COUNT][50];
    snprintf(option_labels[MENU_OPTIONS_ITEM_OVERCLOCK], sizeof(option_labels[0]), "Overclock:  %s", overclock_val);
    snprintf(option_labels[MENU_OPTIONS_ITEM_AUDIO],     sizeof(option_labels[1]), "Audio:      %s",     audio_val);
    snprintf(option_labels[MENU_OPTIONS_ITEM_THEME], sizeof(option_labels[2]), "Theme:      %s", theme_val);
    snprintf(option_labels[MENU_OPTIONS_ITEM_BACK],      sizeof(option_labels[3]), "Back");

    for (int i = 0; i < MENU_OPTIONS_ITEM_COUNT; i++)
    {
        float item_y = panel_y + 8.0f + i * item_h;
        float item_w = panel_w - 16.0f;

        if (i == g_game_state.menu_selected_item)
        {
            // Highlight background
            menu_draw_rect(panel_x + 8, item_y, item_w, item_h - 4, 0xFF1F4F5F);
            menu_draw_rect_border(panel_x + 8, item_y, item_w, item_h - 4, 0xFFC8AA6E, 1);
            graphics_draw_text_center(font_small, option_labels[i],
                                      SCREEN_WIDTH / 2.0f, item_y + 10.0f, 1.2f, 0xFFC8AA6E);
        }
        else
        {
            graphics_draw_text_center(font_small, option_labels[i],
                                      SCREEN_WIDTH / 2.0f, item_y + 10.0f, 1.2f, 0xFFBBBBBB);
        }

        // Divider line between items
        if (i < MENU_OPTIONS_ITEM_COUNT - 1)
        {
            menu_draw_line_h(panel_x + 8, item_y + item_h - 1, item_w, 0xFF333333);
        }
    }

    // Control hints at bottom
    float hint_y = SCREEN_HEIGHT - 28.0f;
    menu_draw_separator(30, hint_y, SCREEN_WIDTH - 60, 0xFF006D96);
    graphics_draw_text_center(font_small, "[Up/Down]  Navigate   [X]  Toggle/Select   [O]  Back",
                              SCREEN_WIDTH / 2.0f, hint_y + 12.0f, 0.9f, 0xFF888888);
}

// ----------------------------------------------------------------
// Top-level menu draw dispatcher
// ----------------------------------------------------------------
void menu_draw()
{
    if (g_game_state.sub_stage == GAME_SUBSTAGE_MENU_MAIN ||
        g_game_state.sub_stage == GAME_SUBSTAGE_MENU_TITLE)
    {
        if (g_game_state.menu_selected_item < 0 || g_game_state.menu_selected_item >= MENU_MAIN_ITEM_COUNT)
        {
            g_game_state.menu_selected_item = 0;
        }
    }
    else if (g_game_state.sub_stage == GAME_SUBSTAGE_MENU_OPTIONS)
    {
        if (g_game_state.menu_selected_item < 0 || g_game_state.menu_selected_item >= MENU_OPTIONS_ITEM_COUNT)
        {
            g_game_state.menu_selected_item = 0;
        }
    }

    switch (g_game_state.sub_stage)
    {
        case GAME_SUBSTAGE_MENU_TITLE:
            menu_draw_title();
            break;
        case GAME_SUBSTAGE_MENU_MAIN:
            menu_draw_main();
            break;
        case GAME_SUBSTAGE_MENU_OPTIONS:
            menu_draw_options();
            break;
        default:
            menu_draw_title();
            break;
    }
}

// ----------------------------------------------------------------
// Input: title screen
// ----------------------------------------------------------------
void menu_input_title(bool no_input)
{
    if (no_input) return;

    if (input_was_button_pressed(INPUT_BUTTON_LEFT) ||
        input_was_button_pressed(INPUT_BUTTON_UP))
    {
        g_game_state.menu_selected_item--;
        if (g_game_state.menu_selected_item < 0)
            g_game_state.menu_selected_item = MENU_MAIN_ITEM_COUNT - 1;
    }
    else if (input_was_button_pressed(INPUT_BUTTON_RIGHT) ||
             input_was_button_pressed(INPUT_BUTTON_DOWN))
    {
        g_game_state.menu_selected_item++;
        if (g_game_state.menu_selected_item >= MENU_MAIN_ITEM_COUNT)
            g_game_state.menu_selected_item = 0;
    }
    else if (input_was_button_pressed(INPUT_BUTTON_CROSS))
    {
        menu_activate_main_item();
    }
}

// ----------------------------------------------------------------
// Input: main menu
// ----------------------------------------------------------------
void menu_input_main(bool no_input)
{
    menu_input_title(no_input);
}

// ----------------------------------------------------------------
// Input: options menu
// ----------------------------------------------------------------
void menu_input_options(bool no_input)
{
    if (no_input) return;

    if (input_was_button_pressed(INPUT_BUTTON_UP))
    {
        g_game_state.menu_selected_item--;
        if (g_game_state.menu_selected_item < 0)
            g_game_state.menu_selected_item = MENU_OPTIONS_ITEM_COUNT - 1;
    }
    else if (input_was_button_pressed(INPUT_BUTTON_DOWN))
    {
        g_game_state.menu_selected_item++;
        if (g_game_state.menu_selected_item >= MENU_OPTIONS_ITEM_COUNT)
            g_game_state.menu_selected_item = 0;
    }
    else if (input_was_button_pressed(INPUT_BUTTON_CROSS))
    {
        switch (g_game_state.menu_selected_item)
        {
            case MENU_OPTIONS_ITEM_OVERCLOCK:
                g_settings.overclock = !g_settings.overclock;
                if (g_settings.overclock)
                    scePowerSetClockFrequency(333, 333, 166);
                else
                    scePowerSetClockFrequency(222, 222, 111);
                break;
            case MENU_OPTIONS_ITEM_AUDIO:
                // Toggle display only — changing audio at runtime requires restart
                g_settings.audio = !g_settings.audio;
                break;
            case MENU_OPTIONS_ITEM_THEME:
                s_wallpaper_variant = (s_wallpaper_variant + 1) % MENU_WALLPAPER_VARIANT_COUNT;

                // Load selected logo variant (theme toggle)
                menu_load_logo_variant(s_wallpaper_variant);
                menu_apply_active_variant_assets();
                g_settings.wallpaper_variant = s_wallpaper_variant;
                game_save_file_values();
                break;
            case MENU_OPTIONS_ITEM_BACK:
                g_game_state.menu_selected_item = MENU_MAIN_ITEM_OPTIONS;
                g_game_state.sub_stage = GAME_SUBSTAGE_MENU_TITLE;
                g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_MENU_TITLE;
                break;
        }
    }
    else if (input_was_button_pressed(INPUT_BUTTON_CIRCLE))
    {
        g_game_state.menu_selected_item = MENU_MAIN_ITEM_OPTIONS;
        g_game_state.sub_stage = GAME_SUBSTAGE_MENU_TITLE;
        g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_MENU_TITLE;
    }
}
