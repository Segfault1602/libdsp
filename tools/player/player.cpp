#include <atomic>
#include <csignal>
#include <cstdio>
#include <thread>

#include <RtAudio.h>

#include "gamepad.h"
#include <dsp_base.h>
#include <bowed_string.h>

constexpr size_t DEFAULT_SAMPLE_RATE = 48000;
constexpr size_t DEFAULT_BUFFER_FRAMES = 256;

std::atomic_bool g_exit = false;
std::unique_ptr<Gamepad> g_gamepad;
std::unique_ptr<dsp::BowedString> g_bowed_string;

int RtOutputCallback(void* outputBuffer, void* /*inputBuffer*/, unsigned int nBufferFrames, double /*streamTime*/,
                     RtAudioStreamStatus /*status*/, void* data);

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

int main(int argc, char** argv)
{
    printf("Instrument Player!\n");

    signal(SIGINT, [](int) {
        printf("Exiting...\n");
        g_exit.store(true);
    });

    g_gamepad = std::make_unique<Gamepad>();
    g_bowed_string = std::make_unique<dsp::BowedString>();
    g_bowed_string->Init(DEFAULT_SAMPLE_RATE);

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

    auto device_info = dac_wrapper.dac.getDeviceInfo(oParams.deviceId);
    printf("Using device: %s\n", device_info.name.c_str());

    uint32_t bufferFrames = DEFAULT_BUFFER_FRAMES;
    auto rtError = dac_wrapper.dac.openStream(&oParams, nullptr, RTAUDIO_FLOAT32, DEFAULT_SAMPLE_RATE, &bufferFrames,
                                              &RtOutputCallback, nullptr);

    if (rtError != RTAUDIO_NO_ERROR)
    {
        printf("rtaudio::openStream failed with error %u!", rtError);
    }

    rtError = dac_wrapper.dac.startStream();
    if (rtError != RTAUDIO_NO_ERROR)
    {
        printf("rtaudio::startStream failed with error %u!", rtError);
    }

    while (!g_exit)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    printf("Stopping stream...\n");
    dac_wrapper.dac.stopStream();
    dac_wrapper.dac.closeStream();

    printf("Goodbye!\n");
    return 0;
}

int RtOutputCallback(void* outputBuffer, void* /*inputBuffer*/, unsigned int nBufferFrames, double /*streamTime*/,
                     RtAudioStreamStatus status, void* data)
{
    if (status)
    {
        std::cout << "Stream underflow detected!" << std::endl;
    }

    GamepadState state;
    (void)g_gamepad->Poll(state);

    DspFloat current_velocity = g_bowed_string->GetVelocity();
    DspFloat current_force = g_bowed_string->GetForce();

    DspFloat new_velocity = state.thumb_left_magnitude;
    DspFloat new_force = 5.f - (4.f * state.thumb_right_magnitude);

    const DspFloat velocity_change_rate = (new_velocity - current_velocity) / static_cast<float>(nBufferFrames);
    const DspFloat force_change_rate = (new_force - current_force) / static_cast<float>(nBufferFrames);

    auto* output = static_cast<DspFloat*>(outputBuffer);

    // Write interleaved audio data.
    for (size_t i = 0; i < nBufferFrames; i++)
    {
        current_velocity += velocity_change_rate;
        current_force += force_change_rate;
        g_bowed_string->SetVelocity(current_velocity);
        // g_bowed_string->SetForce(current_force);

        g_bowed_string->Excite();

        DspFloat sample = g_bowed_string->Tick();
        // assert(std::abs(sample) <= 1.0);
        for (size_t j = 0; j < 2; j++)
        {
            *output++ = sample;
        }
    }

    return 0;
}