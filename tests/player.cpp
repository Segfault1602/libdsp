#include <algorithm>
#include <cassert>
#include <chrono>
#define _USE_MATH_DEFINES
#include <cmath>
#include <memory>
#include <stdio.h>
#include <vector>

#include <RtAudio.h>
#include <sndfile.h>

#include "chorus.h"
#include "test_utils.h"

int RtOutputCallback(void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames, double /*streamTime*/,
                     RtAudioStreamStatus /*status*/, void *data);

int ProcessWithRTAudio();
int ProcessToFile();

struct CbContext
{
    size_t numFrames = 0;
    size_t readPtr = 0;
};

constexpr float g_base_delay = 500;
dsp::Chorus<44100> chorus(g_base_delay);

// Globals
bool g_realtime = false;
std::unique_ptr<float[]> g_input_buffer;
SF_INFO g_sf_info = {0};

int main()
{
    size_t buffer_size;
    if (!LoadWavFile("c:\\source\\libdsp\\waves\\piano.wav", g_input_buffer, buffer_size, g_sf_info))
    {
        return -1;
    }

    // For now I'm only working in mono
    assert(g_sf_info.channels == 1);

    if (g_realtime)
    {
        return ProcessWithRTAudio();
    }
    else
    {
        return ProcessToFile();
    }

    return 0;
}

int RtOutputCallback(void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames, double /*streamTime*/,
                     RtAudioStreamStatus /*status*/, void *data)
{
    CbContext *context = static_cast<CbContext *>(data);

    size_t frameToRead = std::min(static_cast<size_t>(nBufferFrames), context->numFrames - context->readPtr);

    for (size_t i = 0; i < frameToRead; ++i)
    {
        static_cast<float *>(outputBuffer)[i] =
            chorus.Tick(g_input_buffer[context->readPtr]) * 0.5f + g_input_buffer[context->readPtr] * 0.5f;
        ++context->readPtr;
    }

    return frameToRead == 0;
}

int ProcessWithRTAudio()
{
    RtAudio dac;
    std::vector<unsigned int> deviceIds = dac.getDeviceIds();
    if (deviceIds.size() < 1)
    {
        printf("No audio devices found!\n");
        return -1;
    }

    // Set our stream parameters for output only.
    RtAudio::StreamParameters oParams;
    oParams.deviceId = dac.getDefaultOutputDevice();
    oParams.nChannels = 1;

    CbContext context;
    context.numFrames = g_sf_info.frames;
    context.readPtr = 0;

    uint32_t bufferFrames = 512;
    auto rtError = dac.openStream(&oParams, NULL, RTAUDIO_FLOAT32, g_sf_info.samplerate, &bufferFrames,
                                  &RtOutputCallback, static_cast<void *>(&context));

    if (rtError != RTAUDIO_NO_ERROR)
    {
        printf("rtaudio::openStream failed with erro %u!", rtError);
    }

    if (dac.isStreamOpen() == false)
        goto cleanup;

    if (dac.startStream())
        goto cleanup;

    printf("Playing raw file...\n");
    while (1)
    {
        Sleep(100); // wake every 100 ms to check if we're done
        if (dac.isStreamRunning() == false)
        {
            printf("Done streaming!\n");
            break;
        }
    }
cleanup:
    if (dac.isStreamOpen())
        dac.closeStream();

    return 0;
}

int ProcessToFile()
{
    size_t extra_time = g_sf_info.samplerate; // Adding a second at the end in case we are testing long delay
    size_t out_size = g_sf_info.frames + extra_time;
    SF_INFO out_sf_info{0};
    out_sf_info.channels = 1;
    out_sf_info.format = g_sf_info.format;
    out_sf_info.samplerate = g_sf_info.samplerate;
    out_sf_info.frames = out_size;

    auto out = std::make_unique<float[]>(out_size);

    const float delta_t = 1.f / g_sf_info.samplerate;
    const float mod_freq = 5.f;
    float t = 0.f;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < g_sf_info.frames; ++i)
    {
        float mod = 10 * std::sin(2 * M_PI * mod_freq * t);
        chorus.SetDelay(g_base_delay + mod);
        float out_sample = chorus.Tick(g_input_buffer[i]);
        float mix = out_sample * 0.5f + g_input_buffer[i] * 0.5;
        out[i] = mix;
        t += delta_t;
    }
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    printf("Time elapsed: %f\n", time_span.count());

    float file_duration = static_cast<double>(g_sf_info.frames) / g_sf_info.samplerate;

    printf("Which is %f %% of the audio time (%f seconds).\n", time_span.count() / file_duration, file_duration);

    // Send silence for the rest
    for (size_t i = 0; i < extra_time; ++i)
    {
        float out_sample = chorus.Tick(0.f);
        float mix = out_sample * 0.5f;
    }

    return WriteWavFile("out.wav", out.get(), out_sf_info) != 0;
}