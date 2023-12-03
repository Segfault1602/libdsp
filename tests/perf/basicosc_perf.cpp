#include "doctest.h"
#include "nanobench.h"
#include <chrono>

#include "basic_oscillators.h"

using namespace ankerl;
using namespace std::chrono_literals;

constexpr size_t kSamplerate = 48000;
constexpr size_t kOutputSize = kSamplerate * 5;
constexpr float kFreq = 750;
constexpr size_t kBlockSize = 512;

// Render 5 second of audio
TEST_CASE("Sine")
{
    sfdsp::BasicOscillator osc;
    osc.Init(kSamplerate, kFreq, sfdsp::OscillatorType::Sine);
    auto out = std::make_unique<float[]>(kOutputSize);

    nanobench::Bench bench;
    bench.title("BasicOscillator (Sine)");
    bench.relative(true);
    bench.minEpochIterations(5);
    bench.timeUnit(1ms, "ms");

    bench.run("BasicOscillator::ProcessBlock (Sine)", [&]() {
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

    sfdsp::BasicOscillator osc2;
    osc2.Init(kSamplerate, kFreq, sfdsp::OscillatorType::Sine);

    bench.run("BasicOscillator::Tick (Sine)", [&]() {
        for (auto i = 0; i < kOutputSize; ++i)
            out[i] = osc2.Tick();
    });
}
