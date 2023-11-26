#include "gmock/gmock-matchers.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <sndfile.h>

#include "basic_oscillators.h"
#include "dsp_utils.h"
#include "sin_table.h"
#include "test_utils.h"

#include <chrono>
#include <cmath>
#include <memory>

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
    constexpr float kFreq = 750;
    sfdsp::BasicOscillator osc;
    osc.Init(kSamplerate, kFreq, sfdsp::OscillatorType::Sine);

    float phase_increment = kFreq / kSamplerate;

    constexpr size_t kSize = 1024;
    float out_std[kSize];
    float p = 0.f;
    for (auto i = 0; i < kSize; i++)
    {
        out_std[i] = sfdsp::Sine(p);
        p += phase_increment;
        p = std::fmod(p, 1.f);
    }

    float out[kSize];
    const size_t block_size = 512;
    osc.ProcessBlock(out, block_size);
    osc.ProcessBlock(out + block_size, block_size);

    for (auto i = 0; i < kSize; ++i)
    {
        ASSERT_NEAR(out[i], out_std[i], 0.0001f);
    }
}

TEST(BasicOscillatorsTest, PerfTest)
{
    constexpr size_t kSamplerate = 48000;
    constexpr float kFreq = 750;

    sfdsp::BasicOscillator osc1;
    osc1.Init(kSamplerate, kFreq, sfdsp::OscillatorType::Sine);

    sfdsp::BasicOscillator osc2;
    osc2.Init(kSamplerate, kFreq, sfdsp::OscillatorType::Sine);

    constexpr size_t out_size = kSamplerate * 50;
    auto out1 = std::make_unique<float[]>(out_size);
    auto out2 = std::make_unique<float[]>(out_size);

    constexpr size_t block_size = 512;
    auto start = std::chrono::high_resolution_clock::now();
    const size_t block_count = out_size / block_size;
    float* write_ptr = out1.get();
    for (size_t i = 0; i < block_count; ++i)
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
    for (size_t i = 0; i < out_size; ++i)
    {
        out2[i] = osc2.Tick();
    }
    end = std::chrono::high_resolution_clock::now();

    auto duration_tick = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    std::cout << "Block: " << duration_block << "us" << std::endl;
    std::cout << "Tick:  " << duration_tick << "us" << std::endl;

    ASSERT_THAT(duration_block, ::testing::Le(duration_tick));

    for (auto i = 0; i < out_size; ++i)
    {
        ASSERT_NEAR(out1[i], out2[i], 0.001f);
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

TEST(BasicOscillatorsTests, Triangle)
{
    constexpr size_t kSamplerate = 48000;
    constexpr float kFreq = 440;
    constexpr size_t kSize = kSamplerate;
    SF_INFO info;

    auto test_buffer = std::make_unique<float[]>(kSize);
    size_t test_buffer_size = kSize;

    LoadWavFile("waves/triangle_48k_440hz.wav", test_buffer, test_buffer_size, info);

    auto out = std::make_unique<float[]>(test_buffer_size);

    sfdsp::BasicOscillator osc;
    osc.Init(kSamplerate, kFreq, sfdsp::OscillatorType::Tri);

    for (auto i = 0; i < test_buffer_size; ++i)
    {
        out[i] = osc.Tick();
    }

    for (auto i = 0; i < test_buffer_size; ++i)
    {
        ASSERT_NEAR(out[i], test_buffer[i], 0.0001f);
    }
}

TEST(BasicOscillatorsTests, TriangleBlock)
{
    constexpr size_t kSamplerate = 48000;
    constexpr float kFreq = 440;
    constexpr size_t kSize = kSamplerate;
    SF_INFO info;

    auto test_buffer = std::make_unique<float[]>(kSize);
    size_t test_buffer_size = kSize;

    LoadWavFile("waves/triangle_48k_440hz.wav", test_buffer, test_buffer_size, info);

    auto out = std::make_unique<float[]>(test_buffer_size);

    sfdsp::BasicOscillator osc;
    osc.Init(kSamplerate, kFreq, sfdsp::OscillatorType::Tri);

    const size_t block_size = 512;
    const size_t block_count = test_buffer_size / block_size;
    for (auto i = 0; i < block_count; ++i)
    {
        osc.ProcessBlock(out.get() + i * block_size, block_size);
    }

    // Process reminder
    const size_t reminder = test_buffer_size % block_size;
    osc.ProcessBlock(out.get() + block_count * block_size, reminder);

    for (auto i = 0; i < test_buffer_size; ++i)
    {
        ASSERT_NEAR(out[i], test_buffer[i], 0.0001f);
    }
}

TEST(BasicOscillatorsTests, Saw)
{
    constexpr size_t kSamplerate = 48000;
    constexpr float kFreq = 440;
    constexpr size_t kSize = kSamplerate;
    SF_INFO info;

    auto test_buffer = std::make_unique<float[]>(kSize);
    size_t test_buffer_size = kSize;

    LoadWavFile("waves/saw_48k_440hz.wav", test_buffer, test_buffer_size, info);

    auto out = std::make_unique<float[]>(test_buffer_size);

    sfdsp::BasicOscillator osc;
    osc.Init(kSamplerate, kFreq, sfdsp::OscillatorType::Saw);

    for (auto i = 0; i < test_buffer_size; ++i)
    {
        out[i] = osc.Tick();
    }

    for (auto i = 0; i < test_buffer_size; ++i)
    {
        ASSERT_NEAR(out[i], test_buffer[i], 0.0001f);
    }
}