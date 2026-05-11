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

#define MENU_OPTIONS_ITEM_OVERCLOCK 0
#define MENU_OPTIONS_ITEM_AUDIO     1
#define MENU_OPTIONS_ITEM_BACK      2
#define MENU_OPTIONS_ITEM_COUNT     3

// ----------------------------------------------------------------
// Draw helpers
// ----------------------------------------------------------------

// Draw a solid rectangle using the existing GU quad batcher
static void menu_draw_rect(float x, float y, float w, float h, uint32_t color)
{
    graphics_draw_quad(x, y, w, h, 0, 0, 0, 0, color);
}

// ----------------------------------------------------------------
// Title screen draw
// ----------------------------------------------------------------
static void menu_draw_title()
{
    // Dark background
    menu_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFF1A1A0A);

    // Decorative top bar
    menu_draw_rect(0, 0, SCREEN_WIDTH, 4, COLOR_TEXT_GREEN);
    // Decorative bottom bar
    menu_draw_rect(0, SCREEN_HEIGHT - 4, SCREEN_WIDTH, 4, COLOR_TEXT_GREEN);

    // Title "PSPalatro" centered
    graphics_draw_text_center(font_big, "PSPalatro", SCREEN_WIDTH / 2.0f, 80.0f, 2.0f, COLOR_TEXT_GREEN);

    // Subtitle
    graphics_draw_text_center(font_small, "A Balatro Port for PSP", SCREEN_WIDTH / 2.0f, 120.0f, 1.0f, COLOR_LIGHT_GREY);

    // Blinking "Press X to Start" — blink every ~45 frames (1.5 s at 30 fps)
    if ((g_game_counter / 30) % 2 == 0)
    {
        graphics_draw_text_center(font_small, "Press X to Start", SCREEN_WIDTH / 2.0f, 200.0f, 1.0f, COLOR_WHITE);
    }

    // Version footer
    graphics_draw_text_center(font_small, "v0.1  |  github.com/scotthawes/PSP-latro",
                              SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 12.0f, 1.0f, 0xFF606060);
}

// ----------------------------------------------------------------
// Main menu draw
// ----------------------------------------------------------------
static void menu_draw_main()
{
    // Dark background
    menu_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFF1A1A0A);

    // Title banner at top
    menu_draw_rect(0, 0, SCREEN_WIDTH, 40, 0xFF2A2A10);
    graphics_draw_text_center(font_big, "PSPalatro", SCREEN_WIDTH / 2.0f, 10.0f, 1.5f, COLOR_TEXT_GREEN);

    // Menu panel background
    float panel_x   = SCREEN_WIDTH  / 2.0f - 80.0f;
    float panel_y   = 70.0f;
    float panel_w   = 160.0f;
    float item_h    = 24.0f;
    float panel_h   = MENU_MAIN_ITEM_COUNT * item_h + 16.0f;

    menu_draw_rect(panel_x - 2, panel_y - 2, panel_w + 4, panel_h + 4, COLOR_TEXT_GREEN);
    menu_draw_rect(panel_x, panel_y, panel_w, panel_h, 0xFF252510);

    for (int i = 0; i < MENU_MAIN_ITEM_COUNT; i++)
    {
        float item_y = panel_y + 8.0f + i * item_h;

        if (i == g_game_state.menu_selected_item)
        {
            // Highlight bar
            menu_draw_rect(panel_x + 2, item_y - 1, panel_w - 4, item_h - 2, 0xFF3A3A18);
            graphics_draw_text_center(font_small, s_main_menu_labels[i],
                                      SCREEN_WIDTH / 2.0f, item_y + 4.0f, 1.0f, COLOR_TEXT_GREEN);
        }
        else
        {
            graphics_draw_text_center(font_small, s_main_menu_labels[i],
                                      SCREEN_WIDTH / 2.0f, item_y + 4.0f, 1.0f, COLOR_LIGHT_GREY_2);
        }
    }

    // Controls hint
    graphics_draw_text_center(font_small, "Up/Down: Navigate   X: Select",
                              SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 14.0f, 1.0f, COLOR_LIGHT_GREY);
}

// ----------------------------------------------------------------
// Options menu draw
// ----------------------------------------------------------------
static void menu_draw_options()
{
    // Dark background
    menu_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFF1A1A0A);

    // Title banner
    menu_draw_rect(0, 0, SCREEN_WIDTH, 40, 0xFF2A2A10);
    graphics_draw_text_center(font_big, "Options", SCREEN_WIDTH / 2.0f, 10.0f, 1.5f, COLOR_TEXT_GREEN);

    float panel_x = SCREEN_WIDTH / 2.0f - 100.0f;
    float panel_y = 70.0f;
    float panel_w = 200.0f;
    float item_h  = 24.0f;
    float panel_h = MENU_OPTIONS_ITEM_COUNT * item_h + 16.0f;

    menu_draw_rect(panel_x - 2, panel_y - 2, panel_w + 4, panel_h + 4, COLOR_TEXT_GREEN);
    menu_draw_rect(panel_x, panel_y, panel_w, panel_h, 0xFF252510);

    const char *overclock_val = g_settings.overclock ? "ON" : "OFF";
    const char *audio_val     = g_settings.audio     ? "ON" : "OFF";

    char option_labels[MENU_OPTIONS_ITEM_COUNT][40];
    snprintf(option_labels[MENU_OPTIONS_ITEM_OVERCLOCK], sizeof(option_labels[0]), "Overclock: %s", overclock_val);
    snprintf(option_labels[MENU_OPTIONS_ITEM_AUDIO],     sizeof(option_labels[1]), "Audio: %s",     audio_val);
    snprintf(option_labels[MENU_OPTIONS_ITEM_BACK],      sizeof(option_labels[2]), "Back");

    for (int i = 0; i < MENU_OPTIONS_ITEM_COUNT; i++)
    {
        float item_y = panel_y + 8.0f + i * item_h;

        if (i == g_game_state.menu_selected_item)
        {
            menu_draw_rect(panel_x + 2, item_y - 1, panel_w - 4, item_h - 2, 0xFF3A3A18);
            graphics_draw_text_center(font_small, option_labels[i],
                                      SCREEN_WIDTH / 2.0f, item_y + 4.0f, 1.0f, COLOR_TEXT_GREEN);
        }
        else
        {
            graphics_draw_text_center(font_small, option_labels[i],
                                      SCREEN_WIDTH / 2.0f, item_y + 4.0f, 1.0f, COLOR_LIGHT_GREY_2);
        }
    }

    graphics_draw_text_center(font_small, "Up/Down: Navigate   X: Toggle/Select   O: Back",
                              SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 14.0f, 1.0f, COLOR_LIGHT_GREY);
}

// ----------------------------------------------------------------
// Top-level menu draw dispatcher
// ----------------------------------------------------------------
void menu_draw()
{
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
                game_begin_new_run();
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
