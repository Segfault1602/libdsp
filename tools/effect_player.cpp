#include <algorithm>
#include <cassert>
#include <chrono>
#define _USE_MATH_DEFINES
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <memory>
#include <thread>
#include <vector>

#include <RtAudio.h>
#include <sndfile.h>

#include "bowed_string.h"
#include "chorus.h"
#include "dsp_base.h"
#include "dsp_tester.h"
#include "test_utils.h"

int RtOutputCallback(void* outputBuffer, void* /*inputBuffer*/, unsigned int nBufferFrames, double /*streamTime*/,
                     RtAudioStreamStatus /*status*/, void* data);

int ProcessWithRTAudio(DspTester* dsp);
int ProcessToFile(DspTester* dsp);

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
    DspTester* dsp = nullptr;
};

// Globals
std::unique_ptr<float[]> g_input_buffer;
SF_INFO g_sf_info = {0};
bool g_use_input_file = false;

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

    uint32_t samplerate = 48000;
    uint32_t frame_count = samplerate * 2;

    if (!input_file.empty())
    {
        size_t buffer_size;
        if (!LoadWavFile(input_file, g_input_buffer, buffer_size, g_sf_info))
        {
            return -1;
        }
        // For now I'm only working in mono
        assert(g_sf_info.channels == 1);

        samplerate = static_cast<uint32_t>(g_sf_info.samplerate);
        frame_count = static_cast<uint32_t>(g_sf_info.frames);
        g_use_input_file = true;
    }

    auto dsp_test = std::make_unique<WaveguideTester>();
    dsp_test->Init(samplerate, frame_count);
    int ret = 0;
    if (realtime)
    {
        ret = ProcessWithRTAudio(dsp_test.get());
    }
    else
    {
        ret = ProcessToFile(dsp_test.get());
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
        float sample;
        if (g_use_input_file)
        {
            sample = context->dsp->Tick(g_input_buffer[context->readPtr]);
        }
        else
        {
            sample = context->dsp->Tick();
        }
        ++context->readPtr;

        for (size_t j = 0; j < 2; j++)
        {
            *output++ = sample;
        }
    }

    return frameToRead == 0;
}

int ProcessWithRTAudio(DspTester* dsp)
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

    CbContext context;
    context.numFrames = dsp->GetFrameCount();
    context.readPtr = 0;
    context.dsp = dsp;

    uint32_t bufferFrames = 256;
    auto rtError =
        dac_wrapper.dac.openStream(&oParams, nullptr, RTAUDIO_FLOAT32, static_cast<uint32_t>(dsp->GetSamplerate()),
                                   &bufferFrames, &RtOutputCallback, static_cast<void*>(&context));

    if (rtError != RTAUDIO_NO_ERROR)
    {
        printf("rtaudio::openStream failed with error %u!", rtError);
    }

    rtError = dac_wrapper.dac.startStream();
    if (rtError != RTAUDIO_NO_ERROR)
    {
        printf("rtaudio::startStream failed with error %u!", rtError);
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

int ProcessToFile(DspTester* dsp)
{
    size_t out_size = dsp->GetFrameCount();
    SF_INFO out_sf_info{0};
    out_sf_info.channels = 1;
    out_sf_info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
    out_sf_info.samplerate = static_cast<int>(dsp->GetSamplerate());
    out_sf_info.frames = static_cast<sf_count_t>(out_size);

    auto out = std::make_unique<float[]>(out_size);
    std::memset(out.get(), 0, out_size);

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < static_cast<size_t>(dsp->GetFrameCount()); ++i)
    {
        float out_sample;
        if (g_use_input_file)
        {
            out_sample = dsp->Tick(g_input_buffer[i]);
        }
        else
        {
            out_sample = dsp->Tick();
        }
        out[i] = out_sample;
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    printf("Time elapsed: %f\n", time_span.count());

    float file_duration = static_cast<float>(dsp->GetFrameCount()) / static_cast<float>(dsp->GetSamplerate());

    printf("Which is %f %% of the audio time (%f seconds).\n", time_span.count() / file_duration, file_duration);

    return WriteWavFile("out.wav", out.get(), out_sf_info, out_size) != 0;
}