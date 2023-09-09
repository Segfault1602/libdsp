#include "gtest/gtest.h"

#include "chorus.h"
#include "dsp_base.h"
#include "sinc_resampler.h"
#include <numeric>

TEST(SincInterpolateTest, NoOp)
{
    constexpr size_t array_size = 512;
    std::array<float, array_size> input{0};
    std::iota(input.begin(), input.end(), 0.f);

    std::array<float, array_size> output{0};

    size_t out_size = output.max_size();
    dsp::sinc_resample(input.data(), array_size, 1, output.data(), out_size);

    ASSERT_EQ(out_size, array_size);

    for (size_t i = 0; i < output.size(); ++i)
    {
        // Lets treat anything smaller than this as zero
        constexpr float float_zero = 1e-15;
        if (output[i] < float_zero)
        {
            ASSERT_LT(input[i], float_zero);
        }
        else
        {
            ASSERT_FLOAT_EQ(output[i], input[i]);
        }
    }
}

TEST(SincInterpolateTest, Upsample2X)
{
    constexpr float sample_ratio = 2.f;

    constexpr size_t array_size = 256;
    std::array<float, array_size> input{0};
    std::iota(input.begin(), input.end(), 0.f);

    constexpr size_t final_out_size = array_size * sample_ratio;
    std::array<float, final_out_size> output{0};

    size_t out_size = output.max_size();
    dsp::sinc_resample(input.data(), array_size, sample_ratio, output.data(), out_size);

    for (auto& val : output)
    {
        std::cout << val << ", ";
    }
}

TEST(SincInterpolateTest, Downsample2X)
{
    constexpr float sample_ratio = 0.5f;

    constexpr size_t array_size = 256;
    std::array<float, array_size> input{0};
    std::iota(input.begin(), input.end(), 0.f);

    constexpr size_t final_out_size = array_size * sample_ratio;
    std::array<float, final_out_size> output{0};

    size_t out_size = output.max_size();
    dsp::sinc_resample(input.data(), array_size, sample_ratio, output.data(), out_size);
}