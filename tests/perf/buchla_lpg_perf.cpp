#include "doctest.h"
#include "nanobench.h"
#include <chrono>
#include <memory>

#include "basic_oscillators.h"
#include "buchla_lpg.h"

using namespace ankerl;
using namespace std::chrono_literals;

constexpr size_t kSamplerate = 96000;
constexpr size_t kOutputSize = 96000;

TEST_CASE("BuchlaLPG")
{
    nanobench::Bench bench;
    bench.title("Buchla LPG");
    bench.timeUnit(1ms, "ms");
    bench.performanceCounters(true);
    bench.minEpochIterations(50);
    bench.warmup(10);

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

    // lpg.ProcessBlock(cv_out.get(), audio_buffer.get(), audio_buffer.get(), kOutputSize);

    std::unique_ptr<float[]> audio_out = std::make_unique<float[]>(kOutputSize);
    constexpr size_t kBlockSize = 128;

    bench.run("Buchla LPG", [&]() {
        // lpg.ProcessBlock(cv_out.get(), audio_buffer.get(), audio_out.get(), kOutputSize);
        sfdsp::GetCurrent(cv_out.get(), cv_out.get(), kOutputSize, false);
        lpg.ProcessCurrent(cv_out.get(), cv_out.get(), kOutputSize);
        lpg.ProcessAudio(cv_out.get(), audio_buffer.get(), audio_buffer.get(), kOutputSize);
    });
}

TEST_CASE("BuchlaLPG_Detailed")
{
    nanobench::Bench bench;
    bench.title("Buchla LPG - Detailed");
    bench.relative(true);
    bench.timeUnit(1ms, "ms");
    bench.performanceCounters(true);
    bench.minEpochIterations(50);
    bench.warmup(10);

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

    bench.run("GetCurrent", [&]() { sfdsp::GetCurrent(cv_out.get(), cv_out.get(), kOutputSize, false); });

    bench.run("ProcessCurrent", [&]() { lpg.ProcessCurrent(cv_out.get(), cv_out.get(), kOutputSize); });

    bench.run("ProcessAudio",
              [&]() { lpg.ProcessAudio(cv_out.get(), audio_buffer.get(), audio_out.get(), kOutputSize); });
}
