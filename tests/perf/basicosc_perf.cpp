#include "doctest.h"
#include "nanobench.h"
#include <chrono>
#include <memory>

#include "basic_oscillators.h"

using namespace ankerl;
using namespace std::chrono_literals;

constexpr size_t kSamplerate = 48000;
constexpr size_t kOutputSize = kSamplerate * 10;
constexpr float kFreq = 750;
constexpr size_t kBlockSize = 512;

constexpr const char* OscillatorTypeToString(sfdsp::OscillatorType type)
{
    switch (type)
    {
    case sfdsp::OscillatorType::Sine:
        return "Sine";
    case sfdsp::OscillatorType::Tri:
        return "Triangle";
    case sfdsp::OscillatorType::Saw:
        return "Saw";
    case sfdsp::OscillatorType::Square:
        return "Square";
    default:
        return "Unknown";
    }
}

void RenderBlock(sfdsp::OscillatorType type, nanobench::Bench& bench)
{
    sfdsp::BasicOscillator osc;
    osc.Init(kSamplerate, kFreq, type);
    auto out = std::make_unique<float[]>(kOutputSize);

    constexpr const char* format_string = "BasicOscillator::ProcessBlock (%s)";
    char buffer[128];
    snprintf(buffer, sizeof(buffer), format_string, OscillatorTypeToString(type));

    bench.run(buffer, [&]() {
        const size_t block_count = kOutputSize / kBlockSize;
        float* write_ptr = out.get();
        for (size_t i = 0; i < block_count; ++i)
        {
            osc.ProcessBlock(write_ptr, kBlockSize);
            write_ptr += kBlockSize;
        }

        // Process reminder
        const size_t reminder = kOutputSize % kBlockSize;
        osc.ProcessBlock(write_ptr, reminder);
    });
}

void RenderTick(sfdsp::OscillatorType type, nanobench::Bench& bench)
{
    sfdsp::BasicOscillator osc;
    osc.Init(kSamplerate, kFreq, type);
    auto out = std::make_unique<float[]>(kOutputSize);

    constexpr const char* format_string = "BasicOscillator::Tick (%s)";
    char buffer[128];
    snprintf(buffer, sizeof(buffer), format_string, OscillatorTypeToString(type));

    bench.run(buffer, [&]() {
        for (auto i = 0; i < kOutputSize; ++i)
            out[i] = osc.Tick();
    });
}

// Render 5 second of audio
TEST_CASE("BasicOscillator")
{
    nanobench::Bench bench;
    bench.title("BasicOscillator");
    bench.relative(true);
    bench.minEpochIterations(5);
    bench.timeUnit(1ms, "ms");

    RenderBlock(sfdsp::OscillatorType::Sine, bench);
    RenderBlock(sfdsp::OscillatorType::Tri, bench);
    RenderBlock(sfdsp::OscillatorType::Saw, bench);
    RenderBlock(sfdsp::OscillatorType::Square, bench);

    RenderTick(sfdsp::OscillatorType::Sine, bench);
    RenderTick(sfdsp::OscillatorType::Tri, bench);
    RenderTick(sfdsp::OscillatorType::Saw, bench);
    RenderTick(sfdsp::OscillatorType::Square, bench);
}
