#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <thread>

#include <RtAudio.h>

#include "bowed_string.h"
#include "dsp_base.h"
#include "excitation_models.h"
#include "gamepad.h"
#include "line.h"
#include "midi_controller.h"

constexpr size_t DEFAULT_SAMPLE_RATE = 44100;
constexpr size_t DEFAULT_BUFFER_FRAMES = 512;

size_t g_period_microseconds = DEFAULT_BUFFER_FRAMES * 1000000 / DEFAULT_SAMPLE_RATE;

std::atomic_bool g_exit = false;
std::unique_ptr<Gamepad> g_gamepad;
std::unique_ptr<dsp::BowedString> g_bowed_string;
std::unique_ptr<dsp::BowTable> g_bow_table;
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

struct AudioContext
{
    dsp::ExcitationModel* excitation = nullptr;
    bool do_pitch_bend = false;
};

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

    uint8_t midi_port = 0;

    const std::vector<std::string> args(argv + 1, argv + argc);

    for (size_t i = 0; i < args.size(); ++i)
    {
        if (args[i] == "-m" || args[i] == "--midi")
        {
            if (i + 1 >= args.size())
            {
                printf("Missing midi port number!\n");
                return -1;
            }
            midi_port = std::stoi(args[i + 1]);
        }
    }

    signal(SIGINT, [](int) {
        printf("Exiting...\n");
        g_exit.store(true);
    });

    if (!g_midi_controller.Init(midi_port))
    {
        printf("Failed to initialize MIDI controller!\n");
        return -1;
    }

    g_gamepad = std::make_unique<Gamepad>();
    g_bowed_string = std::make_unique<dsp::BowedString>();
    g_bowed_string->Init(DEFAULT_SAMPLE_RATE);

    g_bow_table = std::make_unique<dsp::BowTable>();

    AudioContext audio_context;

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
                                              &RtOutputCallback, &audio_context);

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
    auto start = std::chrono::high_resolution_clock::now();
    if (status)
    {
        std::cout << "Stream underflow detected!" << std::endl;
    }

    auto* audio_context = static_cast<AudioContext*>(data);

    // For bigger buffer sizes we might want to move this code inside the for loop and poll every x frames where
    // x < nBufferFrames.
    constexpr size_t MAX_MIDI_POLL = 10;
    MidiMessage message;
    bool more_messages = true;
    size_t midi_poll = 0;
    dsp::Line pitch_bend(0.f, 1.f, 1.f);

    while (more_messages && midi_poll < MAX_MIDI_POLL)
    {
        more_messages = g_midi_controller.GetMidiMessage(message);
        if (more_messages)
        {
            switch (message.type)
            {
            case MidiType::NoteOff:
            {
                audio_context->excitation = nullptr;
                break;
            }
            case MidiType::NoteOn:
            {
                g_bowed_string->SetLastMidiNote(message.byte1);
                g_bowed_string->SetFrequency(dsp::midi_to_freq[message.byte1]);
                audio_context->excitation = g_bow_table.get();
                break;
            }
            case MidiType::ControllerChange:
            {
                float normalized_value = static_cast<float>(message.byte2) / 127.f;
                if (message.byte1 == 0x05)
                {
                    g_bowed_string->SetVelocity(0.3f * normalized_value);
                }
                else if (message.byte1 == 0x23)
                {
                    g_bow_table->SetForce(5.f - (4.f * normalized_value));
                }
            }
            case MidiType::PitchBend:
            {
                auto pitchbend_value = static_cast<float>(GetPitchBendValue(message));

                // This will give us a normalize value between -2 and 2.
                float normalized_value = (pitchbend_value - 8192.f) / 4096.f;

                float new_midi_pitch = g_bowed_string->GetLastMidiNote() + normalized_value;
                float new_freq = dsp::MidiToFreq(new_midi_pitch);
                float old_freq = g_bowed_string->GetFrequency();
                pitch_bend = dsp::Line(old_freq, new_freq, nBufferFrames);
                audio_context->do_pitch_bend = true;
                break;
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
        if (audio_context->do_pitch_bend)
        {
            g_bowed_string->SetFrequency(pitch_bend.Tick());
        }
        float sample = g_bowed_string->Tick(audio_context->excitation) * 0.7f;
        assert(sample < 1.f && sample > -1.f);

        for (size_t j = 0; j < 2; j++)
        {
            *output++ = sample;
        }
    }

    audio_context->do_pitch_bend = false;

    auto end = std::chrono::high_resolution_clock::now();
    auto time_span = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    (void)time_span;
    // assert(time_span < g_period_microseconds * 0.5f);
    // printf("Callback took %lld microsec out of %lld\n", time_span, g_period_microseconds);

    return 0;
}