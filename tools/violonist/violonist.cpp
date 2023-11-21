#include <array>
#include <cstring>
#include <string>
#include <vector>

#include <sndfile.h>

#include "bowed_string.h"
#include "dsp_utils.h"

#include "test_utils.h"

constexpr size_t kSampleRate = 96000;
constexpr size_t kFrameCount = kSampleRate;
constexpr char kOutputFilenameFormat[] = "%dHz_f%.2f_v%.2f_b%.2f.wav";

struct TestConfig
{
    float force;
    float pos;
    float velocity;
    uint32_t frequency;
};

constexpr float kDefaultVelocity = 0.9f;

constexpr std::array Forces{0.75f, 0.80f, 0.85f, 0.90f, 1.0f};
constexpr std::array BowPositions{0.06f, 0.07f, 0.08f, 0.1f, 0.2f, 0.3f};
constexpr std::array Frequencies{196.f, 293.7f, 440.f, 659.3f, 1567.f};

bool Run(TestConfig config);

int main(int, char**)
{
    for (auto force : Forces)
    {
        for (auto pos : BowPositions)
        {
            for (auto freq : Frequencies)
            {
                TestConfig c{force, pos, kDefaultVelocity, static_cast<uint32_t>(freq)};
                Run(c);
            }
        }
    }
    return 0;
}

bool Run(TestConfig config)
{
    sfdsp::BowedStringConfig stringConfig = sfdsp::kDefaultStringConfig;
    stringConfig.samplerate = static_cast<float>(kSampleRate);
    stringConfig.open_string_tuning = 190.f;

    sfdsp::BowedString string;
    string.Init(stringConfig);

    string.SetFrequency(config.frequency);
    string.SetParameter(sfdsp::BowedString::ParamId::Force, config.force);
    string.SetParameter(sfdsp::BowedString::ParamId::BowPosition, config.pos);
    string.SetParameter(sfdsp::BowedString::ParamId::Velocity, config.velocity);

    size_t out_size = kFrameCount;
    SF_INFO out_sf_info{0};
    out_sf_info.channels = 1;
    out_sf_info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
    out_sf_info.samplerate = static_cast<int>(kSampleRate);
    out_sf_info.frames = static_cast<sf_count_t>(out_size);

    auto out = std::make_unique<float[]>(out_size);
    std::memset(out.get(), 0, out_size);

    for (size_t i = 0; i < kSampleRate; ++i)
    {
        float bridge = string.NextOut();
        out[i] = string.Tick(bridge);
    }

    char out_filename[128];
    snprintf(out_filename, sizeof(out_filename), kOutputFilenameFormat, config.frequency, config.force, config.velocity,
             config.pos);
    std::string filename = out_filename;

    return WriteWavFile(filename, out.get(), out_sf_info, out_size);
}