#include <stdio.h>
#include "common_audio/vad/include/webrtc_vad.h"
#include <vector>
#include <iostream>

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

typedef struct VadSegment {
    float start;
    float end;
    bool is_speech;
} vad_segment;

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
        res->numSamples =  res->header.Subchunk2Size / res->bytesPerSample; //How many samples are in the wav file?
        // std::cout << "Subchunk2Size: " << res->header.Subchunk2Size << std::endl;
        static const uint16_t BUFFER_SIZE = 4096;

        std::cout << "Allocating audio buffer of size: " << res->header.Subchunk2Size << " bytes" << std::endl;
        int8_t* buffer = new int8_t[res->header.Subchunk2Size];
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
    // frame_length - is in samples
    return WebRtcVad_Process(
        vadptr,
        sr,
        buf,
        frame_length
    );
}


int main(int argc, char** argv) {
    std::cout << "Initializing vad" << std::endl;

    VadInst* vad = vad_create();
    vad = vad_init(vad);
    if (vad) {
        WebRtcVad_set_mode(vad, 3);
    } else {
        printf("Failed to init vad!\n");
    }

    std::cout << "Args provided (" << argc << "):" << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::cout << "\t" << argv[i] << std::endl;
    }

    const char* audio_filepath = argc == 2 ? argv[1] : "./test_wav.wav";
    std::cout << "Making vad predictions for:" << std::endl << "\t" << audio_filepath << std::endl;
    
    Wav* wav = read_wav(audio_filepath);
    if (!wav) {
        std::cout << "Failed to load test_wav" << std::endl;
        vad_free(vad);
        return 1;
    }

    // TODO: move to separate function to simplify audio predictions 
    bool is_speech = false;
    std::vector<VadSegment> res;
    const size_t frame_len = 160; // in samples
    int sr = wav->header.SamplesPerSec;
    int bytes_per_sample = wav->bytesPerSample;
    std::cout << "Predicting..." << std::endl;

    std::cout << "Sample rate: " << sr << std::endl;
    std::cout << "Bytes per sample: " << bytes_per_sample << std::endl;
    std::cout << "Total prediction steps: " << wav->numSamples / frame_len << std::endl;

    for (int i = 0; i < wav->numSamples / frame_len; ++i) { 
        is_speech = vad_predict(vad, &wav->audio[i * frame_len], sr, frame_len);
        //std::cout << (i + 1) * frame_len << " " << float((i + 1) * frame_len) / float(sr) <<  " " << is_speech << std::endl;
        if (!res.size() || res.back().is_speech != is_speech) {
            res.push_back({
                float(i * frame_len) / float(sr),
                float((i + 1) * frame_len) / float(sr),
                is_speech
            });
        }
        else {
            res.back().end = float((i + 1) * frame_len) / float(sr);
        }
    }

    // output json results 
    if (res.size()) {
        std::cout << "Predictions:" << std::endl << "[";
        int i = 0;
        for (auto & segm: res) {
            std::cout << "{"
                      << "\"start\": " << segm.start << ","
                      << "\"end\": " << segm.end << ","
                      << "\"label\": " << (segm.is_speech ? "\"speech\"" : "\"background\"");
            ++i;
            if (i < res.size()) {
                std::cout << "}," << std::endl;
            }
        }
        std::cout << "}]" << std::endl;
    }
    else {
        std::cout << "[]" << std::endl;
    }
    
    delete wav->audio;
    delete wav;
    vad_free(vad);
    return 0;
}
