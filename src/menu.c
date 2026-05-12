// menu.c - Title screen and main menu implementation
// Included as a unity-build unit from main.c

// ----------------------------------------------------------------
// Menu item definitions
// ----------------------------------------------------------------

#define MENU_MAIN_ITEM_NEW_RUN  0
#define MENU_MAIN_ITEM_OPTIONS  1
#define MENU_MAIN_ITEM_QUIT     2
#define MENU_MAIN_ITEM_COUNT    3

static const char *s_main_menu_labels[MENU_MAIN_ITEM_COUNT] = {
    "New Run",
    "Options",
    "Quit"
};

static int s_title_wallpaper_texture = -1;
static int s_main_wallpaper_texture = -1;
static bool s_wallpapers_initialized = false;
static int s_wallpaper_variant = 0;
static int s_title_logo_texture = -1;

#define MENU_WALLPAPER_VARIANT_COUNT 2
static int s_title_wallpaper_textures[MENU_WALLPAPER_VARIANT_COUNT] = { -1, -1 };
static int s_main_wallpaper_textures[MENU_WALLPAPER_VARIANT_COUNT] = { -1, -1 };

#define MENU_OPTIONS_ITEM_OVERCLOCK 0
#define MENU_OPTIONS_ITEM_AUDIO     1
#define MENU_OPTIONS_ITEM_WALLPAPER 2
#define MENU_OPTIONS_ITEM_BACK      3
#define MENU_OPTIONS_ITEM_COUNT     4

static int menu_load_wallpaper_from_candidates(const char *candidates[], int candidate_count)
{
    for (int i = 0; i < candidate_count; i++)
    {
        DEBUG_PRINTF("[WALLPAPER] trying: %s\n", candidates[i]);
        int texture = graphics_load_wallpaper(candidates[i]);
        if (texture >= 0)
        {
            DEBUG_PRINTF("[WALLPAPER] loaded: %s (texture=%d)\n", candidates[i], texture);
            return texture;
        }
        DEBUG_PRINTF("[WALLPAPER] failed: %s\n", candidates[i]);
    }

    return -1;
}

static void menu_load_wallpaper_variant(int variant)
{
    if (variant < 0 || variant >= MENU_WALLPAPER_VARIANT_COUNT)
    {
        return;
    }

    if (s_title_wallpaper_textures[variant] >= 0 && s_main_wallpaper_textures[variant] >= 0)
    {
        return;
    }

    if (variant == 0)
    {
        const char *title_candidates[] = {
            "wallpapers/title_wallpaper.png"
        };
        const char *main_candidates[] = {
            "wallpapers/main_menu_wallpaper.png"
        };

        s_title_wallpaper_textures[0] = menu_load_wallpaper_from_candidates(title_candidates, 1);
        s_main_wallpaper_textures[0] = menu_load_wallpaper_from_candidates(main_candidates, 1);
    }
    else
    {
        const char *title_candidates[] = {
            "wallpapers/title_wallpaper_b.png"
        };
        const char *main_candidates[] = {
            "wallpapers/main_menu_wallpaper_b.png"
        };

        s_title_wallpaper_textures[1] = menu_load_wallpaper_from_candidates(title_candidates, 1);
        s_main_wallpaper_textures[1] = menu_load_wallpaper_from_candidates(main_candidates, 1);
    }
}

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
    }

    s_wallpaper_variant = CLAMP(g_settings.wallpaper_variant, 0, MENU_WALLPAPER_VARIANT_COUNT - 1);

    // Load only the currently selected variant during startup to reduce first-frame load cost.
    menu_load_wallpaper_variant(s_wallpaper_variant);
    if (s_title_wallpaper_textures[s_wallpaper_variant] < 0 || s_main_wallpaper_textures[s_wallpaper_variant] < 0)
    {
        menu_load_wallpaper_variant(0);
    }

    if (s_title_wallpaper_textures[s_wallpaper_variant] < 0)
    {
        s_title_wallpaper_textures[s_wallpaper_variant] = s_title_wallpaper_textures[0];
    }
    if (s_main_wallpaper_textures[s_wallpaper_variant] < 0)
    {
        s_main_wallpaper_textures[s_wallpaper_variant] = s_main_wallpaper_textures[0];
    }

    s_title_wallpaper_texture = s_title_wallpaper_textures[s_wallpaper_variant];
    s_main_wallpaper_texture = s_main_wallpaper_textures[s_wallpaper_variant];

    if (s_title_logo_texture < 0)
    {
        s_title_logo_texture = graphics_load_wallpaper("balatro-ui-optimized/logo_main_psp.png");
    }

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

    // Dark overlay to keep text legible while preserving wallpaper visibility.
    menu_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xA00F0F0F);

    // Decorative top stripe with Balatro gold
    menu_draw_rect(0, 0, SCREEN_WIDTH, 6, 0xFFC8AA6E);
    menu_draw_rect(0, 6, SCREEN_WIDTH, 1, 0xFF7A6F42);

    // Decorative bottom stripe
    menu_draw_rect(0, SCREEN_HEIGHT - 7, SCREEN_WIDTH, 1, 0xFF7A6F42);
    menu_draw_rect(0, SCREEN_HEIGHT - 6, SCREEN_WIDTH, 6, 0xFFC8AA6E);

    // Side accent lines
    menu_draw_rect(0, 0, 2, SCREEN_HEIGHT, 0xFF006D96);
    menu_draw_rect(SCREEN_WIDTH - 2, 0, 2, SCREEN_HEIGHT, 0xFF006D96);

    // Central title area with subtle background
    float title_area_y = 50.0f;
    float title_area_h = 120.0f;
    menu_draw_rect(20, title_area_y, SCREEN_WIDTH - 40, title_area_h, 0xFF1A1A1A);
    menu_draw_rect_border(20, title_area_y, SCREEN_WIDTH - 40, title_area_h, 0xFF006D96, 2);

    if (s_title_logo_texture >= 0)
    {
        int logo_w = 0;
        int logo_h = 0;
        if (!graphics_get_texture_content_size(s_title_logo_texture, &logo_w, &logo_h))
        {
            logo_w = 185;
            logo_h = 120;
        }

        float logo_scale = 1.0f;
        float logo_draw_w = logo_w * logo_scale;
        float logo_draw_h = logo_h * logo_scale;
        float logo_x = (SCREEN_WIDTH - logo_draw_w) * 0.5f;
        float logo_y = 54.0f;

        graphics_set_texture(s_title_logo_texture, GRAPHICS_TEXTURE_FILTER_LINEAR);
        graphics_draw_quad(logo_x, logo_y, logo_draw_w, logo_draw_h, 0, 0, logo_w, logo_h, 0xFFFFFFFF);
    }
    else
    {
        // Fallback title text when optimized logo is unavailable.
        graphics_draw_text_center(font_big, "PSPalatro", SCREEN_WIDTH / 2.0f, 75.0f, 2.5f, 0xFFC8AA6E);
    }

    // Subtitle
    graphics_draw_text_center(font_small, "A Balatro Port for PSP", SCREEN_WIDTH / 2.0f, 115.0f, 1.2f, 0xFF888888);

    // Decorative separator
    menu_draw_separator(40, 135, SCREEN_WIDTH - 80, 0xFF006D96);

    // Blinking "Press X to Start" with pulsing effect
    if ((g_game_counter / 30) % 2 == 0)
    {
        float pulse = (float)((g_game_counter / 5) % 12) / 12.0f;
        uint8_t alpha = 200 + (uint8_t)(50 * pulse);
        uint32_t pulsing_color = (alpha << 24) | 0x00C8AA6E;
        graphics_draw_text_center(font_small, "Press X to Start", SCREEN_WIDTH / 2.0f, 155.0f, 1.3f, pulsing_color);
    }

    // Version footer with better styling
    menu_draw_separator(30, 225, SCREEN_WIDTH - 60, 0xFF006D96);
    graphics_draw_text_center(font_small, "v0.1", SCREEN_WIDTH / 2.0f, 238.0f, 0.9f, 0xFF666666);
    graphics_draw_text_center(font_small, "github.com/GreenNinja2525/PSP-latro",
                              SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 14.0f, 0.8f, 0xFF505050);
}

// ----------------------------------------------------------------
// Main menu draw
// ----------------------------------------------------------------
static void menu_draw_main()
{
    menu_init_wallpapers();
    menu_draw_wallpaper(s_main_wallpaper_texture);

    // Dark overlay to keep text legible while preserving wallpaper visibility.
    menu_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xA00F0F0F);

    // Top decorative stripe
    menu_draw_rect(0, 0, SCREEN_WIDTH, 6, 0xFFC8AA6E);
    menu_draw_rect(0, 6, SCREEN_WIDTH, 1, 0xFF7A6F42);

    // Title banner with content
    menu_draw_rect(0, 12, SCREEN_WIDTH, 35, 0xFF1A1A1A);
    menu_draw_rect_border(0, 12, SCREEN_WIDTH, 35, 0xFF006D96, 1);
    graphics_draw_text_center(font_big, "Main Menu", SCREEN_WIDTH / 2.0f, 25.0f, 1.8f, 0xFFC8AA6E);

    // Menu panel
    float panel_x   = 60.0f;
    float panel_y   = 65.0f;
    float panel_w   = SCREEN_WIDTH - 120.0f;
    float item_h    = 32.0f;
    float panel_h   = MENU_MAIN_ITEM_COUNT * item_h + 16.0f;

    // Panel background with border
    menu_draw_rect(panel_x, panel_y, panel_w, panel_h, 0xFF1A1A1A);
    menu_draw_rect_border(panel_x, panel_y, panel_w, panel_h, 0xFF006D96, 2);

    for (int i = 0; i < MENU_MAIN_ITEM_COUNT; i++)
    {
        float item_y = panel_y + 8.0f + i * item_h;
        float item_w = panel_w - 16.0f;

        if (i == g_game_state.menu_selected_item)
        {
            // Highlight background
            menu_draw_rect(panel_x + 8, item_y, item_w, item_h - 4, 0xFF1F4F5F);
            menu_draw_rect_border(panel_x + 8, item_y, item_w, item_h - 4, 0xFFC8AA6E, 1);
            graphics_draw_text_center(font_small, s_main_menu_labels[i],
                                      SCREEN_WIDTH / 2.0f, item_y + 10.0f, 1.2f, 0xFFC8AA6E);
        }
        else
        {
            graphics_draw_text_center(font_small, s_main_menu_labels[i],
                                      SCREEN_WIDTH / 2.0f, item_y + 10.0f, 1.2f, 0xFFBBBBBB);
        }

        // Divider line between items
        if (i < MENU_MAIN_ITEM_COUNT - 1)
        {
            menu_draw_line_h(panel_x + 8, item_y + item_h - 1, item_w, 0xFF333333);
        }
    }

    // Control hints at bottom
    float hint_y = SCREEN_HEIGHT - 28.0f;
    menu_draw_separator(30, hint_y, SCREEN_WIDTH - 60, 0xFF006D96);
    graphics_draw_text_center(font_small, "[Up/Down]  Navigate   [X]  Select   [O]  Back",
                              SCREEN_WIDTH / 2.0f, hint_y + 12.0f, 0.9f, 0xFF888888);

    if (!g_init_resources_ok)
    {
        graphics_draw_text_center(font_small, "⚠ Assets not loaded - New Run disabled",
                                  SCREEN_WIDTH / 2.0f, 50.0f, 1.0f, 0xFFFF6B6B);
    }
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
    const char *wallpaper_val = (s_wallpaper_variant == 0) ? "A" : "B";

    char option_labels[MENU_OPTIONS_ITEM_COUNT][50];
    snprintf(option_labels[MENU_OPTIONS_ITEM_OVERCLOCK], sizeof(option_labels[0]), "Overclock:  %s", overclock_val);
    snprintf(option_labels[MENU_OPTIONS_ITEM_AUDIO],     sizeof(option_labels[1]), "Audio:      %s",     audio_val);
    snprintf(option_labels[MENU_OPTIONS_ITEM_WALLPAPER], sizeof(option_labels[2]), "Wallpaper:  %s", wallpaper_val);
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
    if (g_game_state.sub_stage == GAME_SUBSTAGE_MENU_MAIN)
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

    if (input_was_button_pressed(INPUT_BUTTON_CROSS) ||
        input_was_button_pressed(INPUT_BUTTON_CIRCLE))
    {
        g_game_state.menu_selected_item = 0;
        g_game_state.sub_stage = GAME_SUBSTAGE_MENU_MAIN;
        g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_MENU_MAIN;
    }
}

// ----------------------------------------------------------------
// Input: main menu
// ----------------------------------------------------------------
void menu_input_main(bool no_input)
{
    if (no_input) return;

    if (input_was_button_pressed(INPUT_BUTTON_UP))
    {
        g_game_state.menu_selected_item--;
        if (g_game_state.menu_selected_item < 0)
            g_game_state.menu_selected_item = MENU_MAIN_ITEM_COUNT - 1;
    }
    else if (input_was_button_pressed(INPUT_BUTTON_DOWN))
    {
        g_game_state.menu_selected_item++;
        if (g_game_state.menu_selected_item >= MENU_MAIN_ITEM_COUNT)
            g_game_state.menu_selected_item = 0;
    }
    else if (input_was_button_pressed(INPUT_BUTTON_CROSS))
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
        }
    }
    else if (input_was_button_pressed(INPUT_BUTTON_CIRCLE))
    {
        // Back to title screen
        g_game_state.menu_selected_item = 0;
        g_game_state.sub_stage = GAME_SUBSTAGE_MENU_TITLE;
        g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_MENU_TITLE;
    }
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
            case MENU_OPTIONS_ITEM_WALLPAPER:
                s_wallpaper_variant = (s_wallpaper_variant + 1) % MENU_WALLPAPER_VARIANT_COUNT;

                // Load selected variant on demand instead of decoding all variants at boot.
                menu_load_wallpaper_variant(s_wallpaper_variant);
                if (s_title_wallpaper_textures[s_wallpaper_variant] < 0)
                {
                    s_title_wallpaper_textures[s_wallpaper_variant] = s_title_wallpaper_textures[0];
                }
                if (s_main_wallpaper_textures[s_wallpaper_variant] < 0)
                {
                    s_main_wallpaper_textures[s_wallpaper_variant] = s_main_wallpaper_textures[0];
                }

                s_title_wallpaper_texture = s_title_wallpaper_textures[s_wallpaper_variant];
                s_main_wallpaper_texture = s_main_wallpaper_textures[s_wallpaper_variant];
                g_settings.wallpaper_variant = s_wallpaper_variant;
                game_save_file_values();
                break;
            case MENU_OPTIONS_ITEM_BACK:
                g_game_state.menu_selected_item = MENU_MAIN_ITEM_OPTIONS;
                g_game_state.sub_stage = GAME_SUBSTAGE_MENU_MAIN;
                g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_MENU_MAIN;
                break;
        }
    }
    else if (input_was_button_pressed(INPUT_BUTTON_CIRCLE))
    {
        g_game_state.menu_selected_item = MENU_MAIN_ITEM_OPTIONS;
        g_game_state.sub_stage = GAME_SUBSTAGE_MENU_MAIN;
        g_game_state.input_focused_zone = INPUT_FOCUSED_ZONE_MENU_MAIN;
    }
}
