#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <mutex>
#include <thread>

#include <RtAudio.h>

#include "gamepad.h"
#include <bowed_string.h>
#include <dsp_base.h>

constexpr size_t DEFAULT_SAMPLE_RATE = 44100;
constexpr size_t DEFAULT_BUFFER_FRAMES = 255;

size_t g_period_microseconds = DEFAULT_BUFFER_FRAMES * 1000000 / DEFAULT_SAMPLE_RATE;

std::atomic_bool g_exit = false;
std::unique_ptr<Gamepad> g_gamepad;
std::unique_ptr<dsp::BowedString> g_bowed_string;

struct InstrumentControl
{
    float velocity = 0.25f;
    float velocity_change_rate = 0.f;
    float force = 3.f;
    float force_change_rate = 0.f;
    bool bow_down = false;
    bool strike = false;
} g_instrument_control;

std::mutex g_instrument_control_mutex;

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

    // Control loop will run at ~60Hz
    constexpr auto control_loop_period = std::chrono::milliseconds(15);

    // We will interpolate control values between control loop iterations
    const float dt = (control_loop_period.count() / 1000.f) * DEFAULT_SAMPLE_RATE;

    while (!g_exit)
    {
        GamepadState state;
        bool update = g_gamepad->Poll(state);
        if (update)
        {

            g_instrument_control.bow_down = state.left_trigger > 0.f;

            std::lock_guard<std::mutex> lock(g_instrument_control_mutex);
            g_instrument_control.strike = state.a;
            g_instrument_control.velocity = 0.03f + (0.2f * state.left_trigger);
            g_instrument_control.force = 5.f - (4.f * state.right_trigger);

            g_instrument_control.velocity_change_rate =
                (g_instrument_control.velocity - g_bowed_string->GetVelocity()) / dt;
            g_instrument_control.force_change_rate = (g_instrument_control.force - g_bowed_string->GetForce()) / dt;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(control_loop_period));
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
    auto start = std::chrono::high_resolution_clock::now();
    if (status)
    {
        std::cout << "Stream underflow detected!" << std::endl;
    }

    InstrumentControl control;
    {
        std::lock_guard<std::mutex> lock(g_instrument_control_mutex);
        control = g_instrument_control;

        // Find a better way to do this:
        g_instrument_control.strike = false;
    }

    auto* output = static_cast<float*>(outputBuffer);

    if (control.strike)
    {
        g_bowed_string->Strike();
    }

    // Write interleaved audio data.
    for (size_t i = 0; i < nBufferFrames; i++)
    {
        float current_velocity = g_bowed_string->GetVelocity();

        if ((control.velocity_change_rate < 0.f && current_velocity > control.velocity) ||
            (control.velocity_change_rate > 0.f && current_velocity < control.velocity))
        {
            g_bowed_string->SetVelocity(current_velocity + control.velocity_change_rate);
        }

        float current_force = g_bowed_string->GetForce();
        if ((control.force_change_rate < 0.f && current_force > control.force) ||
            (control.force_change_rate > 0.f && current_force < control.force))
        {
            g_bowed_string->SetForce(control.force + control.force_change_rate);
        }

        if (control.bow_down)
        {
            g_bowed_string->Excite();
        }

        float sample = g_bowed_string->Tick() * 0.1f;
        assert(sample < 1.f && sample > -1.f);

        for (size_t j = 0; j < 2; j++)
        {
            *output++ = sample;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto time_span = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    (void)time_span;
    // assert(time_span < g_period_microseconds * 0.5f);
    // printf("Callback took %lld microsec out of %lld\n", time_span, g_period_microseconds);

    return 0;
}