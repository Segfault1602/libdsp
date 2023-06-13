#include "gtest/gtest.h"

#include "delayline_linear.h"
#include "test_resources.h"

TEST(LinearDelaylineTests, NoInterpolation)
{
    constexpr size_t max_delay_size = 100;
    dsp::LinearDelayline<max_delay_size> line_;

    constexpr size_t delay = 10;
    line_.SetDelay(delay);

    constexpr size_t loop_count = 100;
    std::vector<DspFloat> output;

    for (size_t i = 0; i < loop_count; ++i)
    {
        output.push_back(line_.Tick(i));
    }

    // The delayline starts empty so we should first see `delay` zeros.
    for (size_t i = 0; i < delay; ++i)
    {
        ASSERT_EQ(output[i], 0);
    }

    for (size_t i = 0; i < loop_count - delay; ++i)
    {
        ASSERT_EQ(output[delay + i], i);
    }
}

TEST(LinearDelaylineTests, WithInterpolation)
{
    constexpr size_t max_delay_size = 100;
    dsp::LinearDelayline<max_delay_size> line_;

    constexpr float delay = 10.5f;
    line_.SetDelay(delay);

    constexpr size_t loop_count = 100;
    std::vector<DspFloat> output;

    for (size_t i = 0; i < loop_count; ++i)
    {
        output.push_back(line_.Tick(i));
    }

    ASSERT_EQ(output.size(), stkLinearInterpolationResult1.size());

    for (size_t i = 0; i < output.size(); ++i)
    {
        ASSERT_EQ(output[i], stkLinearInterpolationResult1[i]);
    }
}

TEST(LinearDelaylineTests, WithInterpolation2)
{
    constexpr size_t max_delay_size = 100;
    dsp::LinearDelayline<max_delay_size> line_;

    constexpr float delay = 10.75f;
    line_.SetDelay(delay);

    constexpr size_t loop_count = 100;
    std::vector<DspFloat> output;

    for (size_t i = 0; i < loop_count; ++i)
    {
        output.push_back(line_.Tick(i));
    }

    ASSERT_EQ(output.size(), stkLinearInterpolationResult1.size());

    for (size_t i = 0; i < output.size(); ++i)
    {
        ASSERT_EQ(output[i], stkLinearInterpolationResult2[i]);
    }
}

TEST(LinearDelaylineTests, TapOut)
{
    constexpr size_t max_delay_size = 100;
    dsp::LinearDelayline<max_delay_size> line_;

    constexpr float delay = 10;
    line_.SetDelay(delay);

    constexpr size_t loop_count = delay;

    // This will give us a delay line that looks like
    // read->[0 1 2 3 4 5 6 7 8 9]<-write
    for (size_t i = 0; i < loop_count; ++i)
    {
        line_.Tick(i);
    }

    for (size_t i = 0; i < loop_count; ++i)
    {
        DspFloat tap = line_.TapOut(i);
        ASSERT_EQ(tap, loop_count - i - 1);
    }

    // When asking to tap out a sample past the current delay the
    // function should simply return the last sample available
    DspFloat tap_out_of_bound = line_.TapOut(delay + 1);
    ASSERT_EQ(tap_out_of_bound, line_.TapOut(delay));
}

TEST(LinearDelaylineTests, TapOutInterpolation)
{
    constexpr size_t max_delay_size = 100;
    dsp::LinearDelayline<max_delay_size> line_;

    constexpr float delay = 10;
    line_.SetDelay(delay);

    constexpr size_t loop_count = delay;

    // This will give us a delay line that looks like
    // read->[0 1 2 3 4 5 6 7 8 9]<-write
    for (size_t i = 0; i < loop_count; ++i)
    {
        line_.Tick(i);
    }

    for (size_t i = 0; i < loop_count - 1; ++i)
    {
        DspFloat tap = line_.TapOut(i + 0.25f);
        ASSERT_EQ(tap, loop_count - i - 1 - 0.25f);
    }

    // When asking to tap out a sample past the current delay the
    // function should simply return the last sample available
    DspFloat tap_out_of_bound = line_.TapOut(delay + 0.5f);
    ASSERT_EQ(tap_out_of_bound, line_.TapOut(delay));
}