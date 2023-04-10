#include <algorithm>
#include <cassert>
#include <chrono>
#define _USE_MATH_DEFINES
#include <chrono>
#include <cmath>
#include <cstdio>
#include <memory>
#include <thread>
#include <vector>

#include <RtAudio.h>
#include <sndfile.h>

#include "chorus.h"
#include "dsp_base.h"
#include "test_utils.h"

int RtOutputCallback(void* outputBuffer, void* /*inputBuffer*/, unsigned int nBufferFrames, double /*streamTime*/,
                     RtAudioStreamStatus /*status*/, void* data);

int ProcessWithRTAudio();
int ProcessToFile();

struct RtAudioWrapper
{
    RtAudio dac;

    ~RtAudioWrapper()
    {
        if (dac.isStreamOpen())
        {
            dac.closeStream();
        }
    }
};

struct CbContext
{
    size_t numFrames = 0;
    size_t readPtr = 0;
    dsp::DspBase* effect = nullptr;
};

// Globals
std::unique_ptr<float[]> g_input_buffer;
SF_INFO g_sf_info = {0};

int main(int argc, char** argv)
{
    const std::vector<std::string> args(argv + 1, argv + argc);

    bool realtime = false;
    std::string input_file;

    for (size_t i = 0; i < args.size(); ++i)
    {
        if (args[i] == "-f")
        {
            input_file = args[i + 1];
            ++i;
        }
        else if (args[i] == "-r")
        {
            realtime = true;
        }
    }

    size_t buffer_size;
    if (!LoadWavFile(input_file, g_input_buffer, buffer_size, g_sf_info))
    {
        return -1;
    }

    // For now I'm only working in mono
    assert(g_sf_info.channels == 1);

    int ret = 0;
    if (realtime)
    {
        ret = ProcessWithRTAudio();
    }
    else
    {
        ret = ProcessToFile();
    }

    return ret;
}

double lastValues[2] = {0, 0};

int RtOutputCallback(void* outputBuffer, void* /*inputBuffer*/, unsigned int nBufferFrames, double /*streamTime*/,
                     RtAudioStreamStatus status, void* data)
{
    if (status)
    {
        std::cout << "Stream underflow detected!" << std::endl;
    }
    auto* context = static_cast<CbContext*>(data);

    size_t frameToRead = std::min(static_cast<size_t>(nBufferFrames), context->numFrames - context->readPtr);

    auto* output = static_cast<float*>(outputBuffer);

    // Write interleaved audio data.
    for (size_t i = 0; i < frameToRead; i++)
    {
        float sample = context->effect->Tick(g_input_buffer[context->readPtr]);
        ++context->readPtr;
        for (size_t j = 0; j < 2; j++)
        {
            *output++ = sample;
        }
    }

    return frameToRead == 0;
}

int ProcessWithRTAudio()
{
    RtAudioWrapper dac_wrapper;
    std::vector<unsigned int> deviceIds = dac_wrapper.dac.getDeviceIds();
    if (deviceIds.empty())
    {
        printf("No audio devices found!\n");
        return -1;
    }

    // Set our stream parameters for output only.
    RtAudio::StreamParameters oParams;
    oParams.deviceId = dac_wrapper.dac.getDefaultOutputDevice();
    oParams.nChannels = 2;
    oParams.firstChannel = 0;

    constexpr float g_base_delay = 256.33;
    dsp::Chorus<512> chorus(g_sf_info.samplerate, g_base_delay, 50, 1);

    CbContext context;
    context.numFrames = g_sf_info.frames;
    context.readPtr = 0;
    context.effect = &chorus;

    uint32_t bufferFrames = 256;
    auto rtError = dac_wrapper.dac.openStream(&oParams, nullptr, RTAUDIO_FLOAT32, g_sf_info.samplerate, &bufferFrames,
                                              &RtOutputCallback, static_cast<void*>(&context));

    if (rtError != RTAUDIO_NO_ERROR)
    {
        printf("rtaudio::openStream failed with erro %u!", rtError);
    }

    rtError = dac_wrapper.dac.startStream();
    if (rtError != RTAUDIO_NO_ERROR)
    {
        printf("rtaudio::startStream failed with erro %u!", rtError);
    }

    printf("Playing raw file...\n");
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (!dac_wrapper.dac.isStreamRunning())
        {
            printf("Done streaming!\n");
            break;
        }
    }

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
    out_sf_info.frames = static_cast<sf_count_t>(out_size);

    auto out = std::make_unique<float[]>(out_size);
    memset(out.get(), 0, out_size);

    constexpr float g_base_delay = 64;
    dsp::Chorus<128> chorus(g_sf_info.samplerate, g_base_delay, 50.0f, 1.f);

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < g_sf_info.frames; ++i)
    {
        float out_sample = chorus.Tick(g_input_buffer[i]);
        float mix = out_sample;
        out[i] = out_sample;
    }
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    printf("Time elapsed: %f\n", time_span.count());

    float file_duration = static_cast<float>(g_sf_info.frames) / static_cast<float>(g_sf_info.samplerate);

    printf("Which is %f %% of the audio time (%f seconds).\n", time_span.count() / file_duration, file_duration);

    // Send silence for the rest
    for (size_t i = 0; i < extra_time; ++i)
    {
        float out_sample = chorus.Tick(0.f);
        float mix = out_sample * 0.5f;
    }

    return WriteWavFile("out.wav", out.get(), out_sf_info, out_size) != 0;
}