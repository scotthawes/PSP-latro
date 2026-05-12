#include "global.h"

#include <vorbis/vorbisfile.h>
#include <errno.h>
#include <pspiofilemgr.h>

#ifdef DEBUG
static SceUID g_audio_log_fd = -1;

static void audio_open_log_file(void)
{
    if (g_audio_log_fd >= 0)
    {
        return;
    }

    const char *paths[] = {
        "ms0:/PSP/GAME/PSPALATRO/audio_debug.log",
        "ms0:/PSP/audio_debug.log",
        "audio_debug.log"
    };

    for (int i = 0; i < 3; i++)
    {
        g_audio_log_fd = sceIoOpen(paths[i], PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
        if (g_audio_log_fd >= 0)
        {
            break;
        }
    }
}

static void audio_close_log_file(void)
{
    if (g_audio_log_fd >= 0)
    {
        sceIoClose(g_audio_log_fd);
        g_audio_log_fd = -1;
    }
}

static void audio_debug_printf(const char *fmt, ...)
{
    va_list args;
    char msg[512];
    int len;

    va_start(args, fmt);
    len = vsnprintf(msg, sizeof(msg), fmt, args);
    va_end(args);

    if (len <= 0)
    {
        return;
    }

    /* Keep normal debug console output. */
    fputs(msg, stdout);

    if (g_audio_log_fd < 0)
    {
        audio_open_log_file();
    }

    if (g_audio_log_fd >= 0)
    {
        int write_len = len;
        if (write_len >= (int)sizeof(msg))
        {
            write_len = (int)sizeof(msg) - 1;
        }
        sceIoWrite(g_audio_log_fd, msg, write_len);
    }
}

#undef DEBUG_PRINTF
#define DEBUG_PRINTF(...) audio_debug_printf(__VA_ARGS__)
#endif

#define AUDIO_BUFFER_CHUNKS  24
#define AUDIO_BUFFER_SIZE   4096

static inline int audio_lock(void)
{
    return sceKernelCpuSuspendIntr();
}

static inline void audio_unlock(int intr)
{
    sceKernelCpuResumeIntr(intr);
}

struct audio_buffer_data
{
    char data[AUDIO_BUFFER_SIZE];    
};

struct audio_buffer
{
    struct audio_buffer_data chunks[AUDIO_BUFFER_CHUNKS];
    int read_pos;
    int read_offset;
    int write_pos;
    int written;

    int ogg_id;

    float speed;
    int src_buffer_size;
};

volatile struct audio_buffer g_audio_buffer;

#define MAX_OGG_FILES   10

struct OggFile
{
    bool in_use;
    char* cur_ptr;
    char* file_ptr;
    size_t file_size;

    OggVorbis_File vorbis_file;
} g_ogg_files[MAX_OGG_FILES];

void audio_callback(void* buf, unsigned int length, void *userdata)
{    
    static int callback_count = 0;
    static int underrun_count = 0;
    const unsigned int bytes_per_frame = 4; /* 16-bit stereo */
    unsigned int bytes_requested = length * bytes_per_frame;
    callback_count++;
    
    if (callback_count % 300 == 0)
    {
        DEBUG_PRINTF("[AUDIO] callback #%d: ogg_id=%d, written=%d/%d, underruns=%d, read_pos=%d, write_pos=%d\n",
                     callback_count, g_audio_buffer.ogg_id, g_audio_buffer.written, 
                     AUDIO_BUFFER_CHUNKS, underrun_count, g_audio_buffer.read_pos, g_audio_buffer.write_pos);
    }
    
    if (g_audio_buffer.ogg_id > -1)
    {
        int intr = audio_lock();
        if (g_audio_buffer.written > 0)
        {
            unsigned int remaining = bytes_requested;
            char *out = (char *)buf;

            while (remaining > 0 && g_audio_buffer.written > 0)
            {
                volatile char *srcv = g_audio_buffer.chunks[g_audio_buffer.read_pos].data + g_audio_buffer.read_offset;
                unsigned int avail = (unsigned int)(AUDIO_BUFFER_SIZE - g_audio_buffer.read_offset);
                unsigned int copy_len = (remaining < avail ? remaining : avail);

                memcpy(out, (const void *)srcv, copy_len);

                out += copy_len;
                remaining -= copy_len;
                g_audio_buffer.read_offset += (int)copy_len;

                if (g_audio_buffer.read_offset >= AUDIO_BUFFER_SIZE)
                {
                    g_audio_buffer.read_offset = 0;
                    g_audio_buffer.read_pos = (g_audio_buffer.read_pos + 1) % AUDIO_BUFFER_CHUNKS;
                    g_audio_buffer.written--;
                }
            }

            if (remaining > 0)
            {
                g_debug_info.audio_wait_read++;
                underrun_count++;
                memset(out, 0, remaining);
            }

            audio_unlock(intr);
        }
        else
        {
            audio_unlock(intr);

            g_debug_info.audio_wait_read++;
            underrun_count++;
            if (underrun_count % 100 == 0)
            {
                DEBUG_PRINTF("[AUDIO] UNDERRUN #%d at callback #%d!\n", underrun_count, callback_count);
            }
            /* Audio underrun: zero-fill buffer to prevent garbage samples */
            memset(buf, 0, bytes_requested);
        }
    }
    else
    {
        memset(buf, 0, bytes_requested);
    }
}

void audio_init()
{
    int ret = pspAudioInit();
    if (ret < 0)
    {
        DEBUG_PRINTF("[AUDIO] pspAudioInit() failed: %d\n", ret);
    }
    else
    {
        DEBUG_PRINTF("[AUDIO] pspAudioInit() success\n");
    }
    
    pspAudioSetChannelCallback(0, audio_callback, NULL);
    DEBUG_PRINTF("[AUDIO] pspAudioSetChannelCallback() set\n");

    /* Initialize audio buffer state */
    g_audio_buffer.ogg_id = -1;
    g_audio_buffer.read_pos = 0;
    g_audio_buffer.read_offset = 0;
    g_audio_buffer.write_pos = 0;
    g_audio_buffer.written = 0;
    g_audio_buffer.speed = 1.0f;
    g_audio_buffer.src_buffer_size = AUDIO_BUFFER_SIZE;

    g_debug_info.audio_wait_read = 0;
    g_debug_info.audio_wait_write = 0;
}

struct sample_t
{
    short l, r;
};
static char temp_buffer[AUDIO_BUFFER_SIZE];
static char out_buffer[AUDIO_BUFFER_SIZE];

static int decode_count = 0;

void audio_update()
{
    int intr = audio_lock();
    int ogg_id = g_audio_buffer.ogg_id;
    float speed = g_audio_buffer.speed;
    int src_buffer_size = g_audio_buffer.src_buffer_size;
    int buffer_full = (g_audio_buffer.written >= AUDIO_BUFFER_CHUNKS);
    audio_unlock(intr);

    if (ogg_id < 0)
    {
        return;
    }

    if (buffer_full)
    {
        g_debug_info.audio_wait_write++;
        return;
    }

    decode_count++;

    int current_section;
    long ret = 0, total = 0;
    char *buf_ptr = temp_buffer;
    int decode_errors = 0;

    memset(temp_buffer, 0, sizeof(temp_buffer));

    while (total < src_buffer_size)
    {
        ret = ov_read(&(g_ogg_files[ogg_id].vorbis_file),
                      buf_ptr,
                      src_buffer_size - total,
                      0, 2, 1, &current_section);
        
        if (ret < 0)
        {
            /* ov_read error */
            DEBUG_PRINTF("[AUDIO] ov_read error: %ld at offset %ld\n", ret, total);
            decode_errors++;
            if (decode_errors > 3)
                break; /* Prevent infinite loop on persistent errors */
        }
        else if (ret == 0)
        {
            /* End of stream: seek back to beginning */
            int seek_ret = ov_pcm_seek(&(g_ogg_files[ogg_id].vorbis_file), 0);
            if (seek_ret != 0)
            {
                DEBUG_PRINTF("[AUDIO] ov_pcm_seek error: %d\n", seek_ret);
            }
            continue;
        }
        else
        {
            total += ret;
            buf_ptr += ret;
        }
    }

    struct sample_t *dst = (struct sample_t *)out_buffer;
    struct sample_t *src = (struct sample_t *)temp_buffer;

    if (fabsf(speed - 1.0f) < 0.0001f)
    {
        /* Fast path for normal playback: no resampling artifacts and lower CPU overhead. */
        memcpy(dst, src, AUDIO_BUFFER_SIZE);
    }
    else
    {
        /* Linear interpolation for non-1.0 speed avoids rough nearest-neighbor artifacts. */
        float pos = 0.0f;
        float inc = speed;
        int out_samples = (AUDIO_BUFFER_SIZE / (int)sizeof(struct sample_t));
        int in_samples = (int)(src_buffer_size / (int)sizeof(struct sample_t));
        if (in_samples < 2)
            in_samples = 2;

        for (int i = 0; i < out_samples; i++)
        {
            int i0 = (int)pos;
            if (i0 > in_samples - 2)
                i0 = in_samples - 2;
            int i1 = i0 + 1;
            float t = pos - (float)i0;

            float l = (1.0f - t) * (float)src[i0].l + t * (float)src[i1].l;
            float r = (1.0f - t) * (float)src[i0].r + t * (float)src[i1].r;

            dst[i].l = (short)l;
            dst[i].r = (short)r;

            pos += inc;
            if (pos > (float)(in_samples - 1))
                pos = (float)(in_samples - 1);
        }
    }

    intr = audio_lock();
    if (g_audio_buffer.ogg_id != ogg_id || g_audio_buffer.written >= AUDIO_BUFFER_CHUNKS)
    {
        audio_unlock(intr);
        return;
    }

    memcpy((void *)g_audio_buffer.chunks[g_audio_buffer.write_pos].data, out_buffer, AUDIO_BUFFER_SIZE);
    g_audio_buffer.write_pos = (g_audio_buffer.write_pos + 1) % AUDIO_BUFFER_CHUNKS;
    g_audio_buffer.written++;

    if (decode_count % 300 == 0)
    {
        DEBUG_PRINTF("[AUDIO] decode=%d fill=%d/%d read=%d write=%d\n",
                     decode_count,
                     g_audio_buffer.written,
                     AUDIO_BUFFER_CHUNKS,
                     g_audio_buffer.read_pos,
                     g_audio_buffer.write_pos);
    }

    audio_unlock(intr);
}

static SceUID s_audio_thread_id = -1;

static int audio_thread_func(SceSize args, void *argp)
{
    DEBUG_PRINTF("[AUDIO] Audio decode thread started, monitoring buffer fill\n");

    const int target_fill = AUDIO_BUFFER_CHUNKS - 3;
    int idle_count = 0;
    while (1)
    {
        int intr = audio_lock();
        int fill = g_audio_buffer.written;
        audio_unlock(intr);

        if (fill < target_fill)
        {
            /* Decode a short burst to restore headroom after frame-time spikes. */
            int burst = target_fill - fill;
            if (burst > 4)
                burst = 4;

            for (int i = 0; i < burst; i++)
            {
                audio_update();
            }

            idle_count = 0;
            sceKernelDelayThread(250); /* Keep latency low while still yielding. */
        }
        else
        {
            idle_count++;
            if (idle_count % 1000 == 0)
            {
                DEBUG_PRINTF("[AUDIO] Thread sleeping (buffer full): fill=%d/%d\n",
                             g_audio_buffer.written, AUDIO_BUFFER_CHUNKS);
            }
            sceKernelDelayThread(1000); /* 1 ms */
        }
    }
    return 0;
}

void audio_start_thread()
{
    DEBUG_PRINTF("[AUDIO] audio_start_thread entered\n");

    s_audio_thread_id = sceKernelCreateThread("audio_decode", audio_thread_func, 0x12, 0x4000, 0, NULL);
    if (s_audio_thread_id < 0)
    {
        DEBUG_PRINTF("[AUDIO] sceKernelCreateThread() failed: %d\n", s_audio_thread_id);
        return;
    }
    
    DEBUG_PRINTF("[AUDIO] audio_decode thread created: id=%d\n", s_audio_thread_id);
    
    int ret = sceKernelStartThread(s_audio_thread_id, 0, NULL);
    if (ret < 0)
    {
        DEBUG_PRINTF("[AUDIO] sceKernelStartThread() failed: %d\n", ret);
        sceKernelDeleteThread(s_audio_thread_id);
        s_audio_thread_id = -1;
    }
    else
    {
        DEBUG_PRINTF("[AUDIO] audio_decode thread started\n");
    }
}

void audio_end()
{
    if (s_audio_thread_id >= 0)
    {
        int ret = sceKernelTerminateThread(s_audio_thread_id);
        if (ret < 0)
            DEBUG_PRINTF("[AUDIO] sceKernelTerminateThread() failed: %d\n", ret);
        
        ret = sceKernelDeleteThread(s_audio_thread_id);
        if (ret < 0)
            DEBUG_PRINTF("[AUDIO] sceKernelDeleteThread() failed: %d\n", ret);
        
        s_audio_thread_id = -1;
        DEBUG_PRINTF("[AUDIO] audio_decode thread stopped and deleted\n");
    }
    
    pspAudioEnd();
    DEBUG_PRINTF("[AUDIO] pspAudioEnd() called\n");

#ifdef DEBUG
    audio_close_log_file();
#endif
}

size_t audio_ogg_callback_read_ogg(void* dst, size_t size1, size_t size2, void* fh)
{
    struct OggFile* of = (struct OggFile*)(fh);
    if (size1 == 0 || size2 == 0)
        return 0;

    size_t requested_bytes = size1 * size2;
    size_t len = requested_bytes;
    if ( of->cur_ptr + len > of->file_ptr + of->file_size )
    {
        len = of->file_ptr + of->file_size - of->cur_ptr;
    }
    memcpy( dst, of->cur_ptr, len );
    of->cur_ptr += len;

    /* Vorbis callbacks must match fread semantics: return number of items read. */
    return len / size1;
}

int audio_ogg_callback_seek_ogg( void *fh, ogg_int64_t to, int type ) {
    struct OggFile* of = (struct OggFile*)(fh);

    switch( type ) {
        case SEEK_CUR:
            of->cur_ptr += to;
            break;
        case SEEK_END:
            of->cur_ptr = of->file_ptr + of->file_size - to;
            break;
        case SEEK_SET:
            of->cur_ptr = of->file_ptr + to;
            break;
        default:
            return -1;
    }
    if ( of->cur_ptr < of->file_ptr ) {
        of->cur_ptr = of->file_ptr;
        return -1;
    }
    if ( of->cur_ptr > of->file_ptr + of->file_size ) {
        of->cur_ptr = of->file_ptr + of->file_size;
        return -1;
    }
    return 0;
}

int audio_ogg_callback_close_ogg(void* fh)
{
    return 0;
}

long audio_ogg_callback_tell_ogg( void *fh )
{
    struct OggFile* of = (struct OggFile*)(fh);
    return (of->cur_ptr - of->file_ptr);
}

int audio_load_ogg(char *filename)
{
    FILE *fp_ogg = fopen(filename, "rb");
    if (fp_ogg == NULL)
    {
        DEBUG_PRINTF("[AUDIO] audio_load_ogg: fopen() failed for \"%s\" (errno=%d)\n", filename, errno);
        return -1;
    }
    
    if (fseek(fp_ogg, 0, SEEK_END) != 0)
    {
        DEBUG_PRINTF("[AUDIO] audio_load_ogg: fseek(SEEK_END) failed for \"%s\"\n", filename);
        fclose(fp_ogg);
        return -1;
    }
    
    long fsize = ftell(fp_ogg);
    if (fsize <= 0)
    {
        DEBUG_PRINTF("[AUDIO] audio_load_ogg: invalid file size %ld for \"%s\"\n", fsize, filename);
        fclose(fp_ogg);
        return -1;
    }
    
    if (fseek(fp_ogg, 0, SEEK_SET) != 0)
    {
        DEBUG_PRINTF("[AUDIO] audio_load_ogg: fseek(SEEK_SET) failed for \"%s\"\n", filename);
        fclose(fp_ogg);
        return -1;
    }
    
    g_ogg_files[0].file_ptr = malloc(fsize + 1);
    if (g_ogg_files[0].file_ptr == NULL)
    {
        DEBUG_PRINTF("[AUDIO] audio_load_ogg: malloc(%ld) failed\n", fsize);
        fclose(fp_ogg);
        return -1;
    }
    
    size_t read_size = fread(g_ogg_files[0].file_ptr, 1, fsize, fp_ogg);
    if (read_size != (size_t)fsize)
    {
        DEBUG_PRINTF("[AUDIO] audio_load_ogg: fread() returned %zu, expected %ld\n", read_size, fsize);
        free(g_ogg_files[0].file_ptr);
        g_ogg_files[0].file_ptr = NULL;
        fclose(fp_ogg);
        return -1;
    }
    
    fclose(fp_ogg);

    ov_callbacks callbacks;
    
    g_ogg_files[0].cur_ptr = g_ogg_files[0].file_ptr;
    g_ogg_files[0].file_size = fsize;
    g_ogg_files[0].in_use = true;

    callbacks.read_func = audio_ogg_callback_read_ogg;
    callbacks.seek_func = audio_ogg_callback_seek_ogg;
    callbacks.close_func = audio_ogg_callback_close_ogg;
    callbacks.tell_func = audio_ogg_callback_tell_ogg;

    if (ov_open_callbacks((void *)&(g_ogg_files[0]), &(g_ogg_files[0].vorbis_file), NULL, -1, callbacks) != 0)
    {
        DEBUG_PRINTF("[AUDIO] audio_load_ogg: ov_open_callbacks() failed for \"%s\"\n", filename);
        free(g_ogg_files[0].file_ptr);
        g_ogg_files[0].file_ptr = NULL;
        g_ogg_files[0].cur_ptr = NULL;
        g_ogg_files[0].file_size = 0;
        g_ogg_files[0].in_use = false;
        return -1;
    }

#ifdef DEBUG    
    {
        vorbis_info *vi = ov_info(&(g_ogg_files[0].vorbis_file), -1);
        if (vi)
        {
            DEBUG_PRINTF("[AUDIO] Ogg file loaded: \"%s\" (%ld bytes, %d ch @ %ld Hz)\n", filename, fsize, vi->channels, vi->rate);
        }
        else
        {
            DEBUG_PRINTF("[AUDIO] Ogg file loaded: \"%s\" (%ld bytes)\n", filename, fsize);
        }
    }
#endif

    return 0;
}

int audio_load_ogg_from_archive(char *filename)
{
    size_t file_size = 0;
    DEBUG_PRINTF("[AUDIO] audio_load_ogg_from_archive: attempting to load \"%s\"\n", filename);
    
    g_ogg_files[0].file_ptr = (char *)archive_load_file_entry(filename, &file_size);
    if (g_ogg_files[0].file_ptr == NULL || file_size == 0)
    {
        DEBUG_PRINTF("[AUDIO] ERROR: archive_load_file_entry() returned NULL or size=0 for \"%s\"\n", filename);
        DEBUG_PRINTF("[AUDIO]   file_ptr=%p, file_size=%zu\n", g_ogg_files[0].file_ptr, file_size);
        g_ogg_files[0].cur_ptr = NULL;
        g_ogg_files[0].file_size = 0;
        g_ogg_files[0].in_use = false;
        return -1;
    }
    
    DEBUG_PRINTF("[AUDIO] File loaded from archive: %zu bytes\n", file_size);

    ov_callbacks callbacks;
    
    g_ogg_files[0].cur_ptr = g_ogg_files[0].file_ptr;
    g_ogg_files[0].file_size = file_size;
    g_ogg_files[0].in_use = true;

    callbacks.read_func = audio_ogg_callback_read_ogg;
    callbacks.seek_func = audio_ogg_callback_seek_ogg;
    callbacks.close_func = audio_ogg_callback_close_ogg;
    callbacks.tell_func = audio_ogg_callback_tell_ogg;

    if (ov_open_callbacks((void *)&(g_ogg_files[0]), &(g_ogg_files[0].vorbis_file), NULL, -1, callbacks) != 0)
    {
        DEBUG_PRINTF("[AUDIO] ERROR: ov_open_callbacks() failed for \"%s\"\n", filename);
        free(g_ogg_files[0].file_ptr);
        g_ogg_files[0].file_ptr = NULL;
        g_ogg_files[0].cur_ptr = NULL;
        g_ogg_files[0].file_size = 0;
        g_ogg_files[0].in_use = false;
        return -1;
    }

#ifdef DEBUG    
    {
        vorbis_info *vi = ov_info(&(g_ogg_files[0].vorbis_file), -1);
        if (vi)
        {
            DEBUG_PRINTF("[AUDIO] Ogg file loaded from archive: \"%s\" (%zu bytes, %d ch @ %ld Hz)\n", filename, file_size, vi->channels, vi->rate);
        }
        else
        {
            DEBUG_PRINTF("[AUDIO] Ogg file loaded from archive: \"%s\" (%zu bytes)\n", filename, file_size);
        }
    }
#endif

    return 0;
}

void audio_play_ogg(int ogg_id, float speed)
{
    if (ogg_id < 0)
    {
        DEBUG_PRINTF("[AUDIO] audio_play_ogg: invalid ogg_id %d\n", ogg_id);
        return;
    }
    
    DEBUG_PRINTF("[AUDIO] audio_play_ogg: starting playback with ogg_id=%d, speed=%.2f\n", ogg_id, speed);
    
    g_audio_buffer.ogg_id = ogg_id;
    g_audio_buffer.read_pos = 0;
    g_audio_buffer.read_offset = 0;
    g_audio_buffer.write_pos = 0;
    g_audio_buffer.written = 0;
    
    /* Clamp speed to valid range [0.5f, 1.0f] */
    g_audio_buffer.speed = (speed > 1.0f ? 1.0f : (speed <= 0.0f ? 1.0f : speed));
    
    /* Always use full buffer size for now - speed scaling can be tuned later */
    g_audio_buffer.src_buffer_size = AUDIO_BUFFER_SIZE;

    /* Prime a larger initial reserve to avoid early gameplay cuts. */
    for (int i = 0; i < 12 && g_audio_buffer.written < AUDIO_BUFFER_CHUNKS; i++)
    {
        audio_update();
    }
    
    DEBUG_PRINTF("[AUDIO] Buffer configured: src_size=%d, speed=%.2f\n", 
                 g_audio_buffer.src_buffer_size, g_audio_buffer.speed);
}

void audio_stop()
{
    g_audio_buffer.ogg_id = -1;
    g_audio_buffer.read_pos = 0;
    g_audio_buffer.read_offset = 0;
    g_audio_buffer.write_pos = 0;
    g_audio_buffer.written = 0;
}

void audio_destroy_ogg(int ogg_id)
{
    if (g_audio_buffer.ogg_id == ogg_id)
    {
        audio_stop();
    }

    ov_clear(&(g_ogg_files[ogg_id].vorbis_file));
    free(g_ogg_files[ogg_id].file_ptr);
    g_ogg_files[ogg_id].in_use = false;
}

#ifdef DEBUG
/*
 * audio.c temporarily routes DEBUG_PRINTF through audio_debug_printf.
 * Restore the default DEBUG_PRINTF macro for files included after audio.c
 * in the unity build (main.c includes multiple .c files directly).
 */
#undef DEBUG_PRINTF
#define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#endif


