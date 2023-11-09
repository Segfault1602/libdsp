#include "gmock/gmock-matchers.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "delayline.h"
#include "interpolation_strategy.h"
#include "test_resources.h"
#include "test_utils.h"

namespace sfdsp
{
std::ostream& operator<<(std::ostream& os, const sfdsp::InterpolationType& t)
{
    switch (t)
    {
    case sfdsp::InterpolationType::None:
        os << "None";
        break;
    case sfdsp::InterpolationType::Linear:
        os << "Linear";
        break;
    case sfdsp::InterpolationType::Allpass:
        os << "Allpass";
        break;
    default:
        os << "Unknown";
    }

    return os;
}
} // namespace sfdsp

class DelayInterpolationTest : public ::testing::TestWithParam<sfdsp::InterpolationType>
{
  public:
    DelayInterpolationTest() = default;
};

class DelayNotFractionalTest : public ::testing::TestWithParam<sfdsp::InterpolationType>
{
  public:
    DelayNotFractionalTest() = default;
};

TEST_P(DelayInterpolationTest, NoInterpolation)
{
    constexpr size_t max_delay_size = 100;
    constexpr size_t delay = 10;

    sfdsp::InterpolationType interpolation_type = GetParam();
    sfdsp::Delayline line(max_delay_size, false, interpolation_type);

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

TEST_P(DelayInterpolationTest, WithInterpolation)
{
    constexpr size_t max_delay_size = 100;
    constexpr float delay = 10.5f;

    sfdsp::InterpolationType interpolation_type = GetParam();
    sfdsp::Delayline line(max_delay_size, false, interpolation_type);

    line.SetDelay(delay);

    constexpr size_t loop_count = 100;
    std::vector<float> output;

    for (size_t i = 0; i < loop_count; ++i)
    {
        output.push_back(line.Tick(i));
    }

    const float* expected_result = nullptr;
    size_t expected_result_size = 0;
    switch (interpolation_type)
    {
    case sfdsp::InterpolationType::Linear:
        expected_result = stkLinearInterpolationResult1.data();
        expected_result_size = stkLinearInterpolationResult1.size();
        break;
    case sfdsp::InterpolationType::Allpass:
        expected_result = stkAllpassInterpolationResult.data();
        expected_result_size = stkAllpassInterpolationResult.size();
        break;
    default:
        FAIL() << "Unknown interpolation type";
    }

    ASSERT_EQ(output.size(), expected_result_size);

    for (size_t i = 0; i < output.size(); ++i)
    {
        ASSERT_THAT(output[i], ::testing::FloatNear(expected_result[i], 0.00001f));
    }
}

TEST(LinearDelaylineTests, WithInterpolation2)
{
    constexpr size_t max_delay_size = 100;
    sfdsp::Delayline line(max_delay_size);

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

TEST_P(DelayNotFractionalTest, TapOut)
{
    constexpr size_t max_delay_size = 100;
    sfdsp::Delayline line(max_delay_size, false);

    constexpr float delay = 10;
    line.SetDelay(delay);

    constexpr size_t loop_count = delay;

    // This will give us a delay line that looks like
    // write->[9 8 7 6 5 4 3 2 1 0]<-read
    for (size_t i = 0; i < loop_count; ++i)
    {
        line.Tick(i);
        ASSERT_THAT(line.TapOut(1), ::testing::FloatEq(i));
    }

    for (size_t i = 0; i < loop_count; ++i)
    {
        float tap = line.TapOut(i + 1);
        ASSERT_EQ(tap, loop_count - i - 1);
    }

    // When asking to tap out a sample past the current delay the
    // function should simply return the last sample available
    float tap_out_of_bound = line.TapOut(delay + 1);
    ASSERT_EQ(tap_out_of_bound, line.TapOut(delay));
}

TEST_P(DelayNotFractionalTest, TapOutReverse)
{
    constexpr size_t max_delay_size = 100;
    sfdsp::Delayline line(max_delay_size, true);

    constexpr float delay = 10;
    line.SetDelay(delay);

    // This will give us a delay line that looks like
    // read->[1 2 3 4 5 6 7 8 9 10]<-write
    for (size_t i = 1; i <= delay; ++i)
    {
        line.Tick(i);
    }

    for (size_t i = 1; i <= delay; ++i)
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
    sfdsp::Delayline line(max_delay_size);

    constexpr float delay = 10;
    line.SetDelay(delay);

    constexpr size_t loop_count = delay;

    // This will give us a delay line that looks like
    // read->[0 1 2 3 4 5 6 7 8 9]<-write
    for (size_t i = 0; i < loop_count; ++i)
    {
        line.Tick(i);
    }

    for (float i = 1; i < loop_count; i += 0.25f)
    {
        float tap = line.TapOut(i);
        ASSERT_EQ(tap, loop_count - i);
    }

    // When asking to tap out a sample past the current delay the
    // function should simply return the last sample available
    float tap_out_of_bound = line.TapOut(delay + 0.5f);
    ASSERT_EQ(tap_out_of_bound, line.TapOut(delay));
}

TEST_P(DelayNotFractionalTest, TapIn)
{
    constexpr size_t max_delay_size = 100;
    sfdsp::Delayline line(max_delay_size);

    constexpr float delay = 10;
    line.SetDelay(delay);

    constexpr size_t loop_count = delay;

    // This will give us a delay line that looks like
    // read->[0 1 2 3 4 5 6 7 8 9]<-write
    for (size_t i = 1; i < loop_count; ++i)
    {
        line.TapIn(i, i);
        float tap = line.TapOut(i);
        ASSERT_EQ(tap, i);
    }
}

TEST_P(DelayNotFractionalTest, TapIn2)
{
    constexpr size_t max_delay_size = 7;
    constexpr float delay = max_delay_size - 1;
    sfdsp::Delayline line(max_delay_size);
    line.SetDelay(delay);
    constexpr float input[max_delay_size] = {1, 2, 3, 4, 5, 6};

    for (size_t i = 1; i <= delay; ++i)
    {
        line.TapIn(i, input[i - 1]);
        float out = line.TapOut(i);
        ASSERT_THAT(input[i - 1], ::testing::Eq(out));
    }
}

TEST_P(DelayNotFractionalTest, TapInTick)
{
    constexpr size_t max_delay_size = 100;
    sfdsp::Delayline line(max_delay_size);

    constexpr float delay = max_delay_size - 1;
    line.SetDelay(delay);

    for (size_t i = 0; i < delay; ++i)
    {
        // If we tap in a sample at `delay`, we expect the next Tick()
        // to return that same sample.
        const float SAMPLE = 0.1234f * i;
        line.TapIn(delay, SAMPLE);

        constexpr float TICK_SAMPLE = 1.f;
        float out = line.Tick(TICK_SAMPLE);
        ASSERT_THAT(out, ::testing::Eq(SAMPLE));
    }
}

TEST_P(DelayNotFractionalTest, SubscriptOperator)
{
    constexpr size_t max_delay_size = 11;
    sfdsp::Delayline line(max_delay_size);

    constexpr float delay = max_delay_size - 1;
    line.SetDelay(delay);

    for (size_t i = 1; i <= delay; ++i)
    {
        line[i] = i;
        float out = line[i];
        ASSERT_EQ(i, out);

        out = line.TapOut(i);
        ASSERT_EQ(i, out);
    }

    sfdsp::Delayline line2(max_delay_size);
    line2.SetDelay(delay);

    // This will give us a delay line that looks like
    // read->[0 1 2 3 4 5 6 7 8 9]<-write
    for (size_t i = delay; i > 0; --i)
    {
        line2.Tick(i);
    }

    PrintDelayline(line2);
    for (size_t i = 1; i <= delay; ++i)
    {
        ASSERT_EQ(line2[i], i);
    }
}

TEST(LinearDelaylineTests, TapInFrac)
{
    constexpr size_t max_delay_size = 100;
    sfdsp::Delayline line(max_delay_size);

    constexpr float delay = max_delay_size - 1;
    line.SetDelay(delay);

    line.TapIn(2.5, 1.f);
    ASSERT_THAT(line.TapOut(2), ::testing::FloatEq(0.5f));
    ASSERT_THAT(line.TapOut(3), ::testing::FloatEq(0.5f));
}

INSTANTIATE_TEST_SUITE_P(InterpolationTest, DelayInterpolationTest,
                         ::testing::Values(sfdsp::InterpolationType::Linear, sfdsp::InterpolationType::Allpass));

INSTANTIATE_TEST_SUITE_P(DelaylineParamTest, DelayNotFractionalTest,
                         ::testing::Values(sfdsp::InterpolationType::None, sfdsp::InterpolationType::Linear,
                                           sfdsp::InterpolationType::Allpass),
                         ::testing::PrintToStringParamName());