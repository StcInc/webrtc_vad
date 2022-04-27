#include <stdio.h>
#include "common_audio/vad/include/webrtc_vad.h"

typedef struct  WAV_HEADER
{
    /* RIFF Chunk Descriptor */
    uint8_t         RIFF[4];        // RIFF Header Magic header
    uint32_t        ChunkSize;      // RIFF Chunk Size
    uint8_t         WAVE[4];        // WAVE Header
    /* "fmt" sub-chunk */
    uint8_t         fmt[4];         // FMT header
    uint32_t        Subchunk1Size;  // Size of the fmt chunk
    uint16_t        AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
    uint16_t        NumOfChan;      // Number of channels 1=Mono 2=Sterio
    uint32_t        SamplesPerSec;  // Sampling Frequency in Hz
    uint32_t        bytesPerSec;    // bytes per second
    uint16_t        blockAlign;     // 2=16-bit mono, 4=16-bit stereo
    uint16_t        bitsPerSample;  // Number of bits per sample
    /* "data" sub-chunk */
    uint8_t         Subchunk2ID[4]; // "data"  string
    uint32_t        Subchunk2Size;  // Sampled data length
} wav_hdr;

typedef struct WAV {
    wav_hdr header;
    uint16_t bytesPerSample;
    uint64_t numSamples;
    int16_t* audio;
} Wav;

Wav* read_wav(const char* filePath) {
    Wav* res = new Wav;
    int headerSize = sizeof(wav_hdr);

    FILE* wavFile = fopen(filePath, "r");
    if (wavFile == nullptr)
    {
        fprintf(stderr, "Unable to open wave file: %s\n", filePath);
        delete res;
        return nullptr;
    }

    //Read the header
    size_t bytesRead = fread(&res->header, 1, headerSize, wavFile);
    if (bytesRead > 0)
    {
        res->bytesPerSample = res->header.bitsPerSample / 8;      //Number     of bytes per sample
        res->numSamples = res->header.ChunkSize / res->bytesPerSample; //How many samples are in the wav file?

        static const uint16_t BUFFER_SIZE = 4096;

        int8_t* buffer = new int8_t[res->bytesPerSample * res->numSamples];
        int8_t* i = buffer;
        while ((bytesRead = fread(i, sizeof(int8_t), BUFFER_SIZE / (sizeof(int8_t)), wavFile)) > 0)
        {
            i += BUFFER_SIZE;
        }
        res->audio = reinterpret_cast<int16_t*>(buffer);
        fclose(wavFile);
        return res;
    }
    fclose(wavFile);
    delete res;
    return nullptr;

}


void vad_free(VadInst *vadptr)
{
    WebRtcVad_Free(vadptr);
}

VadInst *vad_create()
{
    VadInst* vadptr = WebRtcVad_Create();
    return vadptr;
}

VadInst *vad_init(VadInst *vadptr)
{
    if (WebRtcVad_Init(vadptr)) {
        return NULL;
    }
    return vadptr;
}


int vad_predict(VadInst *vadptr, const int16_t *buf, int sr, size_t frame_length) {
    return WebRtcVad_Process(
        vadptr,
        sr,
        buf,
        frame_length
    );
}


unsigned int count_set_bits(unsigned long long n)
{
    unsigned int count = 0;
    while (n) {
        n &= (n - 1);
        count++;
    }
    return count;
}


unsigned long long windowed_vad_prediction(VadInst *vadptr, const int16_t *buf, size_t sr, size_t frame_length, unsigned long long window, size_t max_window_width) {

    int cur_pred = vad_predict(vadptr, buf, sr, frame_length);

    window = (window << 1) + cur_pred;

    /* 1 -> 0001000 -> 0000111 (for max_window_width == 3) */
    unsigned long long mask = (1 << (max_window_width + 1)) - 1;

    window = window & mask;

    return window;
}





int main() {
    printf("Hello, Mike!\n");

    VadInst* vad = vad_create();
    vad = vad_init(vad);
    if (vad) {
        WebRtcVad_set_mode(vad, 3);
    } else {
        printf("Failed to init vad!\n");
    }

    Wav* wav = read_wav("../test_wav.wav");
    if (!wav) {
        vad_free(vad);
        return 1;
    }

    int i = 0;
    int step = 160;
    FILE* out = fopen("../out.txt", "w");
    while (i < wav->numSamples * wav->bytesPerSample) {
        int res = vad_predict(vad, wav->audio + i, wav->header.SamplesPerSec, 320);
        fprintf(out, "%d\n", res);
        i += step;
    }
    fclose(out);
    delete wav->audio;
    delete wav;
    vad_free(vad);
    return 0;
}
