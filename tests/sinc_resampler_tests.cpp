#include "gtest/gtest.h"

#include "sinc_resampler.h"
#include "chorus.h"
#include <numeric>

TEST(SincInterpolateTest, NoOp)
{
    dsp::SincResampler sinc;

    constexpr size_t array_size = 512;
    std::array<float, array_size> input{0};
    std::iota(input.begin(), input.end(), 0.f);

    std::array<float, array_size> output{0};

    for (size_t i = 0; i < sinc.BaseDelayInSamples(); ++i)
    {
        float out = 0;
        size_t out_size = 1;
        sinc.Process(input[i], &out, out_size);
        ASSERT_FLOAT_EQ(out, 0.f);
        ASSERT_EQ(out_size, 1);
    }

    size_t out_idx = 0;
    for (size_t i = sinc.BaseDelayInSamples(); i < input.size(); ++i)
    {
        float out = 0;
        size_t out_size = 1;
        sinc.Process(input[i], &out, out_size);

        ASSERT_EQ(out_size, 1);
        output[out_idx++] = out;
    }

    for (size_t i = 0; i < output.size() - sinc.BaseDelayInSamples(); ++i)
    {
        ASSERT_FLOAT_EQ(output[i], input[i]);
    }
}

TEST(SincInterpolateTest, Upsample2X)
{
    constexpr float sample_ratio = 2.f;
    dsp::SincResampler sinc(sample_ratio);

    constexpr size_t array_size = 256;
    std::array<float, array_size> input{0};
    std::iota(input.begin(), input.end(), 0.f);

    constexpr size_t final_out_size = array_size * sample_ratio;
    std::array<float, final_out_size> output{0};

    for (size_t i = 0; i < sinc.BaseDelayInSamples(); ++i)
    {
        std::array<float, 2> out{0};
        size_t out_size = out.max_size();
        sinc.Process(input[i], &out[0], out_size);
        ASSERT_EQ(out_size, 2);
    }

    size_t out_idx = 0;
    for (size_t i = sinc.BaseDelayInSamples(); i < input.size(); ++i)
    {
        std::array<float, 2> out{0};
        size_t out_size = out.max_size();
        sinc.Process(input[i], &out[0], out_size);

        ASSERT_EQ(out_size, 2);
        for (size_t j = 0; j < out_size; ++j)
        {
            output[out_idx++] = out[j];
        }
    }

    for (size_t i = 0; i < input.size() - sinc.BaseDelayInSamples(); ++i)
    {
        ASSERT_FLOAT_EQ(output[i*2], input[i]);
    }
}

TEST(SincInterpolateTest, Downsample2X)
{
    constexpr float sample_ratio = 0.5f;
    dsp::SincResampler sinc(sample_ratio);

    constexpr size_t array_size = 256;
    std::array<float, array_size> input{0};
    std::iota(input.begin(), input.end(), 0.f);

    constexpr size_t final_out_size = array_size * sample_ratio;
    std::array<float, final_out_size> output{0};

    for (size_t i = 0; i < sinc.BaseDelayInSamples(); ++i)
    {
        std::array<float, 2> out{0};
        size_t out_size = out.max_size();
        sinc.Process(input[i], &out[0], out_size);
    }

    size_t out_idx = 0;
    for (size_t i = sinc.BaseDelayInSamples(); i < input.size(); ++i)
    {
        std::array<float, 2> out{0};
        size_t out_size = out.max_size();
        sinc.Process(input[i], &out[0], out_size);

        ASSERT_LT(out_size, 2);
        for (size_t j = 0; j < out_size; ++j)
        {
            output[out_idx++] = out[j];
        }
    }

    for (size_t i = 0; i < input.size() - sinc.BaseDelayInSamples(); ++i)
    {
        ASSERT_FLOAT_EQ(output[i], input[i*2]);
    }
}