#include "gmock/gmock-matchers.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "basic_oscillators.h"
#include "dsp_utils.h"
#include "sin_table.h"

#include <chrono>
#include <cmath>

TEST(BasicOscillatorsTest, Sine)
{
    float inc = 1.f / 1024;
    for (auto i = -TWO_PI; i < TWO_PI * 4; i += inc)
    {
        auto s = sfdsp::Sine(i);
        auto t = std::sin(i * TWO_PI);
        ASSERT_NEAR(s, t, 0.0001f);
    }
}

TEST(BasicOscillatorsTest, SineBlock)
{
    constexpr size_t kSamplerate = 48000;
    constexpr float kFreq = 440;
    sfdsp::BasicOscillator osc;
    osc.Init(kSamplerate, kFreq, sfdsp::OscillatorType::Sine);

    float phase_increment = kFreq / kSamplerate;

    float out_std[1024];
    float p = 0.f;
    for (auto i = 0; i < 1024; i++)
    {
        out_std[i] = std::sin(p * TWO_PI);
        p += phase_increment;
    }

    float out[1024];
    osc.ProcessBlock(out, 512);
    osc.ProcessBlock(out + 512, 512);

    for (auto i = 0; i < 1024; ++i)
    {
        ASSERT_NEAR(out[i], out_std[i], 0.00001f);
    }
}

TEST(BasicOscillatorsTest, PerfTest)
{
    constexpr size_t kSamplerate = 48000;
    constexpr float kFreq = 480;

    sfdsp::BasicOscillator osc1;
    osc1.Init(kSamplerate, kFreq, sfdsp::OscillatorType::Sine);

    sfdsp::BasicOscillator osc2;
    osc2.Init(kSamplerate, kFreq, sfdsp::OscillatorType::Sine);

    constexpr size_t out_size = kSamplerate;
    float out1[out_size] = {0.f};
    float out2[out_size] = {0.f};

    constexpr size_t block_size = 8;
    auto start = std::chrono::high_resolution_clock::now();
    const size_t block_count = out_size / block_size;
    float* write_ptr = out1;
    for (auto i = 0; i < block_count; ++i)
    {
        osc1.ProcessBlock(write_ptr, block_size);
        write_ptr += block_size;
    }

    // Process reminder
    const size_t reminder = out_size % block_size;
    osc1.ProcessBlock(write_ptr, reminder);

    auto end = std::chrono::high_resolution_clock::now();

    auto duration_block = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    for (auto i = 0; i < out_size; ++i)
    {
        out2[i] = osc2.Tick();
    }
    end = std::chrono::high_resolution_clock::now();

    auto duration_tick = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    std::cout << "Block: " << duration_block << "us" << std::endl;
    std::cout << "Tick: " << duration_tick << "us" << std::endl;

    for (auto i = 0; i < out_size; ++i)
    {
        ASSERT_NEAR(out1[i], out2[i], 0.00012f);
    }
}

TEST(BasicOscillatorsTests, Noise)
{
    for (auto i = 0; i < 1000; ++i)
    {
        auto n = sfdsp::Noise();
        ASSERT_THAT(n, ::testing::Le(1.f));
        ASSERT_THAT(n, ::testing::Ge(-1.f));
    }
}