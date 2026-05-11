#include "global.h"

#include <vorbis/vorbisfile.h>
#include <errno.h>

#define AUDIO_BUFFER_CHUNKS  8
#define AUDIO_BUFFER_SIZE   4096

struct audio_buffer_data
{
    char data[AUDIO_BUFFER_SIZE];    
};

struct audio_buffer
{
    struct audio_buffer_data chunks[AUDIO_BUFFER_CHUNKS];
    int read_pos;
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
    callback_count++;
    
    if (callback_count % 100 == 0)
    {
        DEBUG_PRINTF("[AUDIO] callback #%d: ogg_id=%d, written=%d/%d, read_pos=%d, write_pos=%d\n",
                     callback_count, g_audio_buffer.ogg_id, g_audio_buffer.written, 
                     AUDIO_BUFFER_CHUNKS, g_audio_buffer.read_pos, g_audio_buffer.write_pos);
    }
    
    if (g_audio_buffer.ogg_id > -1)
    {
        if (g_audio_buffer.written > 0)
        {
            memcpy((void*)buf, (void*)(g_audio_buffer.chunks[g_audio_buffer.read_pos].data), AUDIO_BUFFER_SIZE);
            g_audio_buffer.read_pos = (g_audio_buffer.read_pos + 1) % AUDIO_BUFFER_CHUNKS;
            g_audio_buffer.written--;
        }
        else
        {
            g_debug_info.audio_wait_read++;
            /* Audio underrun: zero-fill buffer to prevent garbage samples */
            memset(buf, 0, AUDIO_BUFFER_SIZE);
        }
    }
    else
    {
        memset(buf, 0, AUDIO_BUFFER_SIZE);
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

void audio_update()
{
    if (g_audio_buffer.ogg_id > -1 && g_audio_buffer.written < AUDIO_BUFFER_CHUNKS)
    {
        int current_section;
        long ret = 0, total = 0;
        char *buf_ptr = temp_buffer;
        int decode_errors = 0;
        
        while (total < g_audio_buffer.src_buffer_size)
        {
            ret = ov_read(&(g_ogg_files[g_audio_buffer.ogg_id].vorbis_file),
                          buf_ptr,
                          g_audio_buffer.src_buffer_size - total,
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
                int seek_ret = ov_pcm_seek(&(g_ogg_files[g_audio_buffer.ogg_id].vorbis_file), 0);
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

        struct sample_t *dst = (struct sample_t *)g_audio_buffer.chunks[g_audio_buffer.write_pos].data;
        struct sample_t *src = (struct sample_t *)temp_buffer;
        float i_src = 0.0f;
        float inc = g_audio_buffer.speed;

        int size = (AUDIO_BUFFER_SIZE / 4);

        int index = 0;
        for (int i = 0; i < size; i++)
        {
            index = roundf(i_src);
            dst[i].l = src[index].l;
            dst[i].r = src[index].r;

            i_src += inc;
        }

        g_audio_buffer.write_pos = (g_audio_buffer.write_pos + 1) % AUDIO_BUFFER_CHUNKS;
        g_audio_buffer.written++;
        
        DEBUG_PRINTF("[AUDIO] Decode ok: decoded=%ld bytes, written=%d, read_pos=%d, fill=%d/%d\n",
                     total, g_audio_buffer.write_pos, g_audio_buffer.read_pos, 
                     g_audio_buffer.written, AUDIO_BUFFER_CHUNKS);
    }
    else if (g_audio_buffer.written >= AUDIO_BUFFER_CHUNKS)
    {
        g_debug_info.audio_wait_write++;
        DEBUG_PRINTF("[AUDIO] Buffer full, write stalled (fill=%d/%d)\n",
                     g_audio_buffer.written, AUDIO_BUFFER_CHUNKS);
    }
}

static SceUID s_audio_thread_id = -1;

static int audio_thread_func(SceSize args, void *argp)
{
    DEBUG_PRINTF("[AUDIO] Audio decode thread started, monitoring buffer fill\n");
    
    int idle_count = 0;
    while (1)
    {
        if (g_audio_buffer.written < AUDIO_BUFFER_CHUNKS)
        {
            audio_update();
            idle_count = 0;
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
}

size_t audio_ogg_callback_read_ogg(void* dst, size_t size1, size_t size2, void* fh)
{
    struct OggFile* of = (struct OggFile*)(fh);
    size_t len = size1 * size2;
    if ( of->cur_ptr + len > of->file_ptr + of->file_size )
    {
        len = of->file_ptr + of->file_size - of->cur_ptr;
    }
    memcpy( dst, of->cur_ptr, len );
    of->cur_ptr += len;
    return len;
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
        DEBUG_PRINTF("[AUDIO] Ogg file loaded: \"%s\" (%ld bytes)\n", filename, fsize);

        char **ptr=ov_comment(&(g_ogg_files[0].vorbis_file),-1)->user_comments;
        vorbis_info *vi=ov_info(&(g_ogg_files[0].vorbis_file),-1);
        while(*ptr){
          DEBUG_PRINTF("\t%s\n",*ptr);
          ++ptr;
        }
        DEBUG_PRINTF("\tBitstream: %d channels, %ld Hz\n",vi->channels,vi->rate);
        DEBUG_PRINTF("\tDecoded length: %ld samples\n", (long)ov_pcm_total(&(g_ogg_files[0].vorbis_file),-1));
        DEBUG_PRINTF("\tEncoded by: %s\n",ov_comment(&(g_ogg_files[0].vorbis_file),-1)->vendor);
    }
#endif

    return 0;
}

int audio_load_ogg_from_archive(char *filename)
{
    size_t file_size = 0;
    g_ogg_files[0].file_ptr = (char *)archive_load_file_entry(filename, &file_size);
    if (g_ogg_files[0].file_ptr == NULL || file_size == 0)
    {
        DEBUG_PRINTF("[AUDIO] audio_load_ogg_from_archive: archive_load_file_entry() failed for \"%s\"\n", filename);
        g_ogg_files[0].cur_ptr = NULL;
        g_ogg_files[0].file_size = 0;
        g_ogg_files[0].in_use = false;
        return -1;
    }

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
        DEBUG_PRINTF("[AUDIO] audio_load_ogg_from_archive: ov_open_callbacks() failed for \"%s\"\n", filename);
        free(g_ogg_files[0].file_ptr);
        g_ogg_files[0].file_ptr = NULL;
        g_ogg_files[0].cur_ptr = NULL;
        g_ogg_files[0].file_size = 0;
        g_ogg_files[0].in_use = false;
        return -1;
    }

#ifdef DEBUG    
    {
        DEBUG_PRINTF("[AUDIO] Ogg file loaded from archive: \"%s\" (%zu bytes)\n", filename, file_size);

        char **ptr=ov_comment(&(g_ogg_files[0].vorbis_file),-1)->user_comments;
        vorbis_info *vi=ov_info(&(g_ogg_files[0].vorbis_file),-1);
        while(*ptr){
          DEBUG_PRINTF("\t%s\n",*ptr);
          ++ptr;
        }
        DEBUG_PRINTF("\tBitstream: %d channels, %ld Hz\n",vi->channels,vi->rate);
        DEBUG_PRINTF("\tDecoded length: %ld samples\n", (long)ov_pcm_total(&(g_ogg_files[0].vorbis_file),-1));
        DEBUG_PRINTF("\tEncoded by: %s\n",ov_comment(&(g_ogg_files[0].vorbis_file),-1)->vendor);
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
    
    DEBUG_PRINTF("[AUDIO] audio_play_ogg: starting playback with ogg_id=%d, speed=%.1f\n", ogg_id, speed);
    
    g_audio_buffer.ogg_id = ogg_id;
    g_audio_buffer.read_pos = 0;
    g_audio_buffer.write_pos = 0;
    g_audio_buffer.written = 0;
    
    /* Clamp speed to valid range [0.5f, 1.0f] */
    g_audio_buffer.speed = (speed > 1.0f ? 1.0f : (speed <= 0.0f ? 1.0f : speed));
    
    /* Always use full buffer size for now - speed scaling can be tuned later */
    g_audio_buffer.src_buffer_size = AUDIO_BUFFER_SIZE;
    
    DEBUG_PRINTF("[AUDIO] Buffer configured: src_size=%d, speed=%.2f\n", 
                 g_audio_buffer.src_buffer_size, g_audio_buffer.speed);
}

void audio_stop()
{
    g_audio_buffer.ogg_id = -1;
    g_audio_buffer.read_pos = 0;
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


