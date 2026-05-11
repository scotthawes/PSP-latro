#include "global.h"

PSP_MODULE_INFO("PSPalatro", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

#include "callbacks.c"
#include "archive.c"
#include "ini.c"
#include "audio.c"
#include "graphics.c"
#include "input.c"
#include "draw.c"
#include "game_util.c"
#include "entity_event.c"
#include "automated_events.c"
#include "menu.c"
#include "game_input.c"
#include "game.c"

struct DebugInfo g_debug_info;

int ogg_id = -1;
bool g_init_resources_ok = false;

static void boot_log(const char *message)
{
    const char *paths[] = {
        "ms0:/PSP/GAME/PSPALATRO/pspalatro_boot.log",
        "ms0:/PSP/GAME/PSPALATRO/./pspalatro_boot.log",
        "/PSP/GAME/PSPALATRO/pspalatro_boot.log",
        "pspalatro_boot.log"
    };

    for (int i = 0; i < (int)(sizeof(paths) / sizeof(paths[0])); i++)
    {
        FILE *f = fopen(paths[i], "a");
        if (f)
        {
            fprintf(f, "%s\n", message);
            fclose(f);
            break;
        }
    }
}

bool init_more_stuff()
{
    boot_log("init_more_stuff: begin");

    if (!game_init_load_file_values()) return false;
    boot_log("init_more_stuff: settings loaded");

    if (g_settings.overclock)
    {
        game_draw_loading_text("Setting CPU clock to 333Mhz", COLOR_WHITE, COLOR_BLACK);
        scePowerSetClockFrequency(333, 333, 166);
        boot_log("init_more_stuff: overclock enabled");
    }
    
    bool archive_opened = archive_open(g_settings.archive_file_name);

    if (!archive_opened)
    {
        const char *fallback_archives[] = {
            "game.love",
            "build/Balatro V0.11.vpk",
            "third_party/balatro-v011-reference/game.love",
            "Balatro V0.11.vpk",
            "Balatro.exe"
        };
        bool opened_archive = false;
        for (int i = 0; i < (int)(sizeof(fallback_archives) / sizeof(fallback_archives[0])); i++)
        {
            if (archive_open(fallback_archives[i]))
            {
                strncpy(g_settings.archive_file_name, fallback_archives[i], sizeof(g_settings.archive_file_name) - 1);
                g_settings.archive_file_name[sizeof(g_settings.archive_file_name) - 1] = '\0';
                opened_archive = true;
                break;
            }
        }

        if (!opened_archive)
        {
            boot_log("init_more_stuff: archive open failed");
            archive_opened = false;
        }
        else
        {
            archive_opened = true;
        }

        boot_log("init_more_stuff: archive opened using fallback");
    }
    else
    {
        boot_log("init_more_stuff: archive opened from settings");
    }

    game_init_logic();    
    boot_log("init_more_stuff: game_init_logic done");

    if (archive_opened)
    {
        if (game_init_draw())
        {
            g_init_resources_ok = true;
            boot_log("init_more_stuff: game_init_draw done");
        }
        else
        {
            g_init_resources_ok = false;
            boot_log("init_more_stuff: game_init_draw failed");
        }
    }
    else
    {
        g_init_resources_ok = false;
    }
    
    if (g_settings.audio && g_init_resources_ok)
    {
        audio_start_thread();
        boot_log("init_more_stuff: audio thread started");

        game_draw_loading_text("Loading audio file", COLOR_WHITE, COLOR_BLACK);
        ogg_id = audio_load_ogg_from_archive("resources/sounds/music1.ogg");
        if (ogg_id < 0)
        {
            boot_log("init_more_stuff: audio load failed");
            g_settings.audio = false;
        }

        if (g_settings.audio)
        {
            audio_play_ogg(ogg_id, 0.8f);
            boot_log("init_more_stuff: audio playback started");
        }
    }

    if (archive_opened)
    {
        archive_close();
    }

    event_init();
    boot_log("init_more_stuff: event_init done");

    return true;
}

int main(int argc, char **argv)
{
    const char *truncate_paths[] = {
        "ms0:/PSP/GAME/PSPALATRO/pspalatro_boot.log",
        "ms0:/PSP/GAME/PSPALATRO/./pspalatro_boot.log",
        "/PSP/GAME/PSPALATRO/pspalatro_boot.log",
        "pspalatro_boot.log"
    };

    for (int i = 0; i < (int)(sizeof(truncate_paths) / sizeof(truncate_paths[0])); i++)
    {
        FILE *f = fopen(truncate_paths[i], "w");
        if (f)
        {
            fclose(f);
            break;
        }
    }

    boot_log("main: start");

    DEBUG_PRINTF("PSPalatro");

    setup_callbacks();

    sceKernelDcacheWritebackAll();

    audio_init();
    boot_log("main: audio_init done");

    graphics_init();
    boot_log("main: graphics_init done");

    // Render-path self-test: should show obvious full-screen color flashes before game init.
    for (int i = 0; i < 90; i++)
    {
        graphics_begin_draw();
        if ((i / 15) % 3 == 0)
        {
            graphics_clear(COLOR_RED);
        }
        else if ((i / 15) % 3 == 1)
        {
            graphics_clear(COLOR_GREEN);
        }
        else
        {
            graphics_clear(COLOR_BLUE);
        }
        graphics_end_draw();
    }
    boot_log("main: render self-test done");

    input_init();
    boot_log("main: input_init done");

    game_init_draw_load_fonts();
    boot_log("main: fonts loaded");

    if (init_more_stuff())
    {
        boot_log("main: entering gameplay loop");
        while(running())
        {
            input_update();
            game_update();
            game_draw();
        }
    }
    else
    {
        boot_log("main: entering failure loop");
        while(running())
        {
            graphics_begin_draw();
            graphics_clear(COLOR_RED);

            // Font-free failure indicator: if fonts are broken, this is still obvious.
            graphics_set_no_texture();
            graphics_draw_quad(30, 30, SCREEN_WIDTH - 60, SCREEN_HEIGHT - 60, 0, 0, 0, 0, COLOR_BLACK);
            graphics_draw_quad(40, 40, SCREEN_WIDTH - 80, SCREEN_HEIGHT - 80, 0, 0, 0, 0, COLOR_RED);

            graphics_draw_text_center(font_small, "Initialization failed", SCREEN_WIDTH / 2.0f, 120.0f, 1.0f, COLOR_WHITE);
            graphics_draw_text_center(font_small, g_settings.archive_file_name, SCREEN_WIDTH / 2.0f, 140.0f, 1.0f, COLOR_WHITE);
            graphics_end_draw();
        }
    }

    if (g_settings.audio)
    {
        audio_stop();
        audio_destroy_ogg(ogg_id);
    }

    audio_end();    

    sceGuTerm();
	sceKernelExitGame();

    return 0;
}
