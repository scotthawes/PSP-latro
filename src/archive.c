#include "global.h"
#include <zip.h>
#include <unistd.h>
#include <errno.h>

zip_source_t *g_archive_source = NULL;
zip_t *g_archive_file = NULL;

bool archive_open(const char *filename)
{
    char str[128];
    char cwd[256] = {0};
    char attempts[7][256] = {{0}};
    int attempt_count = 0;

    if (!getcwd(cwd, sizeof(cwd)))
    {
        strcpy(cwd, "(failed)");
    }

    DEBUG_PRINTF("archive_open cwd: %s\n", cwd);

    // Search common runtime locations.
    snprintf(attempts[attempt_count++], 256, "%s", filename);
    snprintf(attempts[attempt_count++], 256, "./%s", filename);
    snprintf(attempts[attempt_count++], 256, "../%s", filename);
    snprintf(attempts[attempt_count++], 256, "../../%s", filename);
    snprintf(attempts[attempt_count++], 256, "ms0:/PSP/GAME/PSPALATRO/%s", filename);
    snprintf(attempts[attempt_count++], 256, "ms0:/PSP/GAME/PSPALATRO/./%s", filename);
    snprintf(attempts[attempt_count++], 256, "/PSP/GAME/PSPALATRO/%s", filename);

    FILE *fp = NULL;
    int attempt_index = -1;
    for (int i = 0; i < attempt_count; i++)
    {
        fp = fopen(attempts[i], "rb");
        if (fp)
        {
            attempt_index = i;
            DEBUG_PRINTF("archive_open success[%d]: %s\n", i, attempts[i]);
            break;
        }

        DEBUG_PRINTF("archive_open fail[%d] errno=%d path=%s\n", i, errno, attempts[i]);
    }

    if (!fp)
    {
        snprintf(str, sizeof(str), "Can't open %s", filename);
        game_draw_loading_text(str, COLOR_TEXT_RED, COLOR_BLACK);
        DEBUG_PRINTF("archive_open failed for %s\n", filename);
        DEBUG_PRINTF("archive_open cwd: %s\n", cwd);
        return false;
    }

    game_draw_loading_text("Opening archive...", COLOR_WHITE, COLOR_BLACK);
    
    // Look for start of archive in the file
    size_t count = 0;
    uint8_t buffer[4];
    while(true)
    {
        uint8_t c = fgetc(fp);
        if (feof(fp))
        {
            sprintf(str, "Error finding archive header in file \"%s\"", filename);
            game_draw_loading_text(str, COLOR_TEXT_RED, COLOR_BLACK);
            return false;
        }

        if (count < 4)
        {
            buffer[count] = c;
        }
        else
        {
            buffer[3] = c;
        }

        if (count >= 3 && buffer[0] == 'P' && buffer[1] == 'K' && buffer[2] == 0x03 && buffer[3] == 0x04)
        {
            count -= 3;
            break;
        }

        count++;

        if (count > 3)
        {
            buffer[0] = buffer[1];
            buffer[1] = buffer[2];
            buffer[2] = buffer[3];
        }
    }
    fclose(fp);

    DEBUG_PRINTF("Archive header found at offset %zu\n", count);

    zip_error_t error;
    zip_error_init(&error);
    g_archive_source = zip_source_file_create(attempts[attempt_index], count, -1, &error);
    if (!g_archive_source)
    {
        sprintf(str, "Error creating zip source: %s", zip_error_strerror(&error));
        game_draw_loading_text(str, COLOR_TEXT_RED, COLOR_BLACK);
        DEBUG_PRINTF("zip_source_file_create failed: %s\n", zip_error_strerror(&error));
        zip_error_fini(&error);
        return false;
    }
    
    g_archive_file = zip_open_from_source(g_archive_source, 0, &error);
    if (!g_archive_file)
    {
        sprintf(str, "Error opening zip: %s", zip_error_strerror(&error));
        game_draw_loading_text(str, COLOR_TEXT_RED, COLOR_BLACK);
        DEBUG_PRINTF("zip_open_from_source failed: %s\n", zip_error_strerror(&error));
        zip_error_fini(&error);
        return false;
    }
    
    zip_error_fini(&error);

    return true;
}

void archive_close()
{
    if (g_archive_file)
    {
        zip_close(g_archive_file);
        g_archive_source = NULL;
        g_archive_file = NULL;
    }
}

uint8_t *archive_load_file_entry(const char *filename, size_t *file_size)
{
    if (g_archive_file == NULL) return NULL;

    *file_size = archive_get_file_entry_size(filename);

    if (*file_size == 0) return NULL;

    uint8_t *buffer = (uint8_t *)malloc(*file_size);
    zip_file_t *f = zip_fopen(g_archive_file, filename, 0);
    zip_fread(f, buffer, *file_size);
    zip_fclose(f);

    return buffer;
}

size_t archive_get_file_entry_size(const char *filename)
{
    zip_stat_t stat;
    if (g_archive_file == NULL) return 0;
    if (zip_stat(g_archive_file, filename, 0, &stat) != 0) return 0;
    
    return stat.size;
}
