#include "doctest.h"
#include "nanobench.h"
#include <chrono>

#include "phaseshapers.h"

using namespace ankerl;
using namespace std::chrono_literals;

constexpr size_t kSamplerate = 48000;
constexpr size_t kOutputSize = kSamplerate * 10;
constexpr float kFreq = 750;
constexpr size_t kBlockSize = 512;

constexpr const char* PhaseshaperTypeToString(sfdsp::Phaseshaper::Waveform type)
{
    switch (type)
    {
    case sfdsp::Phaseshaper::Waveform::VARIABLE_SLOPE:
        return "Variable Slope";
    case sfdsp::Phaseshaper::Waveform::WAVESLICE:
        return "Waveslice";
    case sfdsp::Phaseshaper::Waveform::SUPERSAW:
        return "Supersaw";
    case sfdsp::Phaseshaper::Waveform::RIPPLE:
        return "Ripple";
    case sfdsp::Phaseshaper::Waveform::SOFTSYNC:
        return "Softsync";
    case sfdsp::Phaseshaper::Waveform::TRIANGLE_MOD:
        return "Triangle Mod";
    default:
        return "Unknown";
    }
}

void RenderTick(sfdsp::Phaseshaper::Waveform type, nanobench::Bench& bench)
{
    sfdsp::Phaseshaper ps;
    ps.Init(kSamplerate);
    ps.SetFreq(kFreq);
    ps.SetWaveform(type);
    auto out = std::make_unique<float[]>(kOutputSize);

    constexpr const char* format_string = "Phaseshaper::Tick (%s)";
    char buffer[128];
    snprintf(buffer, sizeof(buffer), format_string, PhaseshaperTypeToString(type));

    bench.run(buffer, [&]() {
        for (auto i = 0; i < kOutputSize; ++i)
            out[i] = ps.Process();
    });
}

TEST_CASE("Phaseshaper")
{
    nanobench::Bench bench;
    bench.title("Phaseshaper");
    bench.relative(true);
    bench.minEpochIterations(5);
    bench.timeUnit(1ms, "ms");

    for (size_t i = 0; i < static_cast<size_t>(sfdsp::Phaseshaper::Waveform::NUM_WAVES); ++i)
    {
        RenderTick(static_cast<sfdsp::Phaseshaper::Waveform>(i), bench);
    }

    sfdsp::Phaseshaper ps;
    ps.Init(kSamplerate);
    ps.SetFreq(kFreq);
    ps.SetMod(0.9f);
    ps.SetWaveform(sfdsp::Phaseshaper::Waveform::VARIABLE_SLOPE);
    auto out = std::make_unique<float[]>(kOutputSize);
    bench.run("VariableSlope_Block", [&]() { ps.ProcessBlock(out.get(), kOutputSize); });
}