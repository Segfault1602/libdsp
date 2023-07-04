#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <mutex>
#include <thread>

#include <RtAudio.h>

#include "gamepad.h"
#include "midi_controller.h"
#include <bowed_string.h>
#include <dsp_base.h>

constexpr size_t DEFAULT_SAMPLE_RATE = 44100;
constexpr size_t DEFAULT_BUFFER_FRAMES = 255;

size_t g_period_microseconds = DEFAULT_BUFFER_FRAMES * 1000000 / DEFAULT_SAMPLE_RATE;

std::atomic_bool g_exit = false;
std::unique_ptr<Gamepad> g_gamepad;
std::unique_ptr<dsp::BowedString> g_bowed_string;
MidiController g_midi_controller;

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

    if (!g_midi_controller.Init())
    {
        printf("Failed to initialize MIDI controller!\n");
        return -1;
    }

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

    // For bigger buffer sizes we might want to move this code inside the for loop and poll every x frames where
    // x < nBufferFrames.
    constexpr size_t MAX_MIDI_POLL = 10;
    MidiMessage message;
    bool more_messages = true;
    size_t midi_poll = 0;
    while (more_messages && midi_poll < MAX_MIDI_POLL)
    {
        more_messages = g_midi_controller.GetMessage(message);
        if (more_messages)
        {
            switch (message.type)
            {
            case MidiType::NoteOff:
            {
                g_bowed_string->BowOn(false);
                break;
            }
            case MidiType::NoteOn:
            {
                g_bowed_string->SetFrequency(dsp::midi_to_freq[message.byte1]);
                g_bowed_string->BowOn(true);
            }
            break;
            case MidiType::ControllerChange:
            {
                float normalized_value = static_cast<float>(message.byte2) / 127.f;
                if (message.byte1 == 0x05)
                {
                    g_bowed_string->SetVelocity(0.3f * normalized_value);
                }
                else if (message.byte1 == 0x23)
                {
                    g_bowed_string->SetForce(5.f - (4.f * normalized_value));
                }
            }
            default:
                break;
            }
        }

        ++midi_poll;
    }

    auto* output = static_cast<float*>(outputBuffer);

    // Write interleaved audio data.
    for (size_t i = 0; i < nBufferFrames; i++)
    {
        float sample = g_bowed_string->Tick() * 0.7f;
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