#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "delayline_linear.h"
#include "test_resources.h"

TEST(LinearDelaylineTests, NoInterpolation)
{
    constexpr size_t max_delay_size = 100;
    dsp::LinearDelayline<max_delay_size> line(true);

    constexpr size_t delay = 10;
    line.SetDelay(delay);

    constexpr size_t loop_count = 100;
    std::vector<float> output;

    for (size_t i = 0; i < loop_count; ++i)
    {
        output.push_back(line.Tick(i));
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
    dsp::LinearDelayline<max_delay_size> line(true);

    constexpr float delay = 10.5f;
    line.SetDelay(delay);

    constexpr size_t loop_count = 100;
    std::vector<float> output;

    for (size_t i = 0; i < loop_count; ++i)
    {
        output.push_back(line.Tick(i));
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
    dsp::LinearDelayline<max_delay_size> line;

    constexpr float delay = 10.75f;
    line.SetDelay(delay);

    constexpr size_t loop_count = 100;
    std::vector<float> output;

    for (size_t i = 0; i < loop_count; ++i)
    {
        output.push_back(line.Tick(i));
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
    dsp::LinearDelayline<max_delay_size> line(false);

    constexpr float delay = 10;
    line.SetDelay(delay);

    constexpr size_t loop_count = delay;

    // This will give us a delay line that looks like
    // read->[0 1 2 3 4 5 6 7 8 9]<-write
    for (size_t i = 0; i < loop_count; ++i)
    {
        line.Tick(i);
        ASSERT_THAT(line.TapOut(0), ::testing::FloatEq(i));
    }

    for (size_t i = 0; i < loop_count; ++i)
    {
        float tap = line.TapOut(i);
        ASSERT_EQ(tap, loop_count - i - 1);
    }

    // When asking to tap out a sample past the current delay the
    // function should simply return the last sample available
    float tap_out_of_bound = line.TapOut(delay + 1);
    ASSERT_EQ(tap_out_of_bound, line.TapOut(delay));
}

TEST(LinearDelaylineTests, TapOutReverse)
{
    constexpr size_t max_delay_size = 100;
    dsp::LinearDelayline<max_delay_size> line(true);

    constexpr float delay = 10;
    line.SetDelay(delay);

    constexpr size_t loop_count = delay;

    // This will give us a delay line that looks like
    // read->[0 1 2 3 4 5 6 7 8 9]<-write
    for (size_t i = 0; i < loop_count; ++i)
    {
        line.Tick(i);
        ASSERT_THAT(line.TapOut(0), ::testing::FloatEq(0));
    }

    for (size_t i = 0; i < loop_count; ++i)
    {
        float tap = line.TapOut(i);
        ASSERT_EQ(tap, i);
    }

    // When asking to tap out a sample past the current delay the
    // function should simply return the last sample available
    float tap_out_of_bound = line.TapOut(delay + 1);
    ASSERT_EQ(tap_out_of_bound, line.TapOut(delay));
}

TEST(LinearDelaylineTests, TapOutInterpolation)
{
    constexpr size_t max_delay_size = 100;
    dsp::LinearDelayline<max_delay_size> line;

    constexpr float delay = 10;
    line.SetDelay(delay);

    constexpr size_t loop_count = delay;

    // This will give us a delay line that looks like
    // read->[0 1 2 3 4 5 6 7 8 9]<-write
    for (size_t i = 0; i < loop_count; ++i)
    {
        line.Tick(i);
    }

    for (float i = 0; i < loop_count - 1; i += 0.25f)
    {
        float tap = line.TapOut(i);
        ASSERT_EQ(tap, loop_count - i - 1);
    }

    // When asking to tap out a sample past the current delay the
    // function should simply return the last sample available
    float tap_out_of_bound = line.TapOut(delay + 0.5f);
    ASSERT_EQ(tap_out_of_bound, line.TapOut(delay));
}

TEST(LinearDelaylineTests, TapIn)
{
    constexpr size_t max_delay_size = 100;
    dsp::LinearDelayline<max_delay_size> line;

    constexpr float delay = 10;
    line.SetDelay(delay);

    constexpr size_t loop_count = delay;

    // This will give us a delay line that looks like
    // read->[0 1 2 3 4 5 6 7 8 9]<-write
    for (size_t i = 0; i < loop_count; ++i)
    {
        line.TapIn(i, i);
        float tap = line.TapOut(i);
        ASSERT_EQ(tap, i);
    }
}

TEST(LinearDelaylineTests, TapInTick)
{
    constexpr size_t max_delay_size = 100;
    dsp::LinearDelayline<max_delay_size> line;

    constexpr float delay = max_delay_size - 1;
    line.SetDelay(delay);

    for (size_t i = 0; i < delay; ++i)
    {
        // If we tap in a sample at `delay-1`, we expect the next Tick()
        // to return that same sample.
        const float SAMPLE = 0.1234f * i;
        line.TapIn(delay - 1, SAMPLE);

        constexpr float TICK_SAMPLE = 1.f;
        float out = line.Tick(TICK_SAMPLE);
        ASSERT_THAT(out, ::testing::Eq(SAMPLE));
    }
}

TEST(LinearDelaylineTests, TapInFrac)
{
    constexpr size_t max_delay_size = 100;
    dsp::LinearDelayline<max_delay_size> line;

    constexpr float delay = max_delay_size - 1;
    line.SetDelay(delay);

    line.TapIn(2.5, 1.f);
    ASSERT_THAT(line.TapOut(2), ::testing::FloatEq(0.5f));
    ASSERT_THAT(line.TapOut(3), ::testing::FloatEq(0.5f));
}