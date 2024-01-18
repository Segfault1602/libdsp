#include "doctest.h"
#include "nanobench.h"
#include <chrono>
#include <memory>

#include "basic_oscillators.h"
#include "buchla_lpg.h"

using namespace ankerl;
using namespace std::chrono_literals;

constexpr size_t kSamplerate = 96000;
constexpr size_t kOutputSize = 960;

TEST_CASE("BuchlaLPG")
{
    nanobench::Bench bench;
    bench.title("Buchla LPG");
    // bench.relative(true);
    bench.timeUnit(1ms, "ms");
    bench.performanceCounters(true);

    sfdsp::BasicOscillator cv;
    cv.Init(kSamplerate, 4, sfdsp::OscillatorType::Square);
    cv.SetDuty(0.05f);
    auto cv_out = std::make_unique<float[]>(kOutputSize);
    cv.ProcessBlock(cv_out.get(), kOutputSize);

    constexpr float kModDepth = 8.f;
    for (size_t i = 0; i < kOutputSize; ++i)
    {
        cv_out[i] = kModDepth / 2.f * (1.f + cv_out[i]);
    }

    auto audio_buffer = std::make_unique<float[]>(kOutputSize);
    sfdsp::BasicOscillator audio;
    audio.Init(kSamplerate, 440, sfdsp::OscillatorType::Square);
    audio.ProcessBlock(audio_buffer.get(), kOutputSize);

    for (size_t i = 0; i < kOutputSize; ++i)
    {
        audio_buffer[i] = 0.3f * audio_buffer[i];
    }

    sfdsp::BuchlaLPG lpg;
    lpg.Init(kSamplerate);

    lpg.ProcessBlock(cv_out.get(), audio_buffer.get(), audio_buffer.get(), kOutputSize);

    std::unique_ptr<float[]> audio_out = std::make_unique<float[]>(kOutputSize);
    constexpr size_t kBlockSize = 128;

    bench.run("Buchla LPG", [&]() {
        for (size_t i = 0; i < kOutputSize; i += kBlockSize)
        {
            lpg.ProcessBlock(cv_out.get() + i, audio_buffer.get() + i, audio_out.get() + i, kBlockSize);
        }
    });
}
