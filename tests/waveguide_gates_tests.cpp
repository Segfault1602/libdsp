#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "delayline.h"
#include "test_utils.h"
#include "waveguide_gate.h"

TEST(WaveguideGatesTest, OpenGates)
{
    constexpr size_t kDelaySize = 11;
    constexpr float kDelay = 10;
    constexpr float kGatePos = 5;
    constexpr float kCoeff = 0.0f;

    dsp::Delayline right_traveling_line(kDelaySize, false, dsp::InterpolationType::Linear);
    dsp::Delayline left_traveling_line(kDelaySize, true, dsp::InterpolationType::Linear);

    left_traveling_line.SetDelay(kDelay);
    right_traveling_line.SetDelay(kDelay);

    // Feed a dirac pulse into both delay line
    left_traveling_line.TapIn(2, 1.f);
    right_traveling_line.TapIn(2, 1.f);

    dsp::WaveguideGate gate(true, kGatePos, kCoeff);

    // Looping for 2 * kDelay should bring us back to initial state
    for (auto i = 0; i < kDelay * 2; ++i)
    {
        gate.Process(left_traveling_line, right_traveling_line);

        float left_sample = left_traveling_line.NextOut();
        float right_sample = right_traveling_line.NextOut();

        left_traveling_line.Tick(right_sample * -1.f);
        right_traveling_line.Tick(left_sample * -1.f);
    }

    for (auto i = 1; i <= kDelaySize; ++i)
    {
        if (i == 2)
        {
            ASSERT_EQ(left_traveling_line[i], 1);
            ASSERT_EQ(right_traveling_line[i], 1);
        }
        else
        {
            ASSERT_EQ(left_traveling_line[i], 0);
            ASSERT_EQ(right_traveling_line[i], 0);
        }
    }
}

TEST(WaveguideGatesTest, HalfOpenGates)
{
    constexpr size_t kDelaySize = 11;
    constexpr float kDelay = 10;
    constexpr float kGatePos = 5;
    constexpr float kCoeff = 0.5f;

    dsp::Delayline right_traveling_line(kDelaySize, false);
    dsp::Delayline left_traveling_line(kDelaySize, true);

    left_traveling_line.SetDelay(kDelay);
    right_traveling_line.SetDelay(kDelay);

    // Feed a dirac pulse into both delay line
    left_traveling_line.TapIn(2, 1.f);
    right_traveling_line.TapIn(2, 1.f);

    PrintDelayline(right_traveling_line);
    PrintDelayline(left_traveling_line);

    dsp::WaveguideGate gate(true, kGatePos, kCoeff);

    // Looping for 2 * kDelay should bring us back to initial state
    for (auto i = 0; i < kDelay * 2; ++i)
    {
        gate.Process(left_traveling_line, right_traveling_line);

        float left_sample = left_traveling_line.NextOut();
        float right_sample = right_traveling_line.NextOut();

        left_traveling_line.Tick(right_sample * -1.f);
        right_traveling_line.Tick(left_sample * -1.f);

        printf("Iteration %d\n", i);
        PrintDelayline(right_traveling_line);
        PrintDelayline(left_traveling_line);
    }

    for (auto i = 1; i <= kDelaySize; ++i)
    {
        if (i == 2 || i == 9)
        {
            ASSERT_EQ(std::abs(left_traveling_line[i]), 0.5f);
            ASSERT_EQ(std::abs(right_traveling_line[i]), 0.5f);
        }
        else
        {
            ASSERT_EQ(left_traveling_line[i], 0);
            ASSERT_EQ(right_traveling_line[i], 0);
        }
    }
}

TEST(WaveguideGatesTest, ClosedGates)
{
    constexpr size_t kDelaySize = 11;
    constexpr float kDelay = 10;
    constexpr float kGatePos = 5;
    constexpr float kCoeff = 1.0f;

    dsp::Delayline right_traveling_line(kDelaySize, false, dsp::InterpolationType::Linear);
    dsp::Delayline left_traveling_line(kDelaySize, true, dsp::InterpolationType::Linear);

    right_traveling_line.SetDelay(kDelay);
    left_traveling_line.SetDelay(kDelay);

    // Feed a dirac pulse into both delay line
    right_traveling_line.TapIn(2, 1.f);
    left_traveling_line.TapIn(2, 1.f);

    right_traveling_line.TapIn(8, 1.f);
    left_traveling_line.TapIn(8, 1.f);

    PrintDelayline(right_traveling_line);
    PrintDelayline(left_traveling_line);

    dsp::WaveguideGate gate(true, kGatePos, kCoeff);

    // Looping for kDelay should bring us back to initial state
    for (auto i = 0; i < kDelay; ++i)
    {
        gate.Process(left_traveling_line, right_traveling_line);

        float left_sample = left_traveling_line.NextOut();
        float right_sample = right_traveling_line.NextOut();

        left_traveling_line.Tick(right_sample * -1.f);
        right_traveling_line.Tick(left_sample * -1.f);

        printf("Iteration %d\n", i);
        PrintDelayline(right_traveling_line);
        PrintDelayline(left_traveling_line);
    }

    for (auto i = 1; i <= kDelaySize; ++i)
    {
        if (i == 2 || i == 8)
        {
            ASSERT_EQ(left_traveling_line[i], 1);
            ASSERT_EQ(right_traveling_line[i], 1);
        }
        else
        {
            ASSERT_EQ(left_traveling_line[i], 0);
            ASSERT_EQ(right_traveling_line[i], 0);
        }
    }
}

TEST(WaveguideGatesTest, FractionalGates)
{
    constexpr size_t kDelaySize = 11;
    constexpr float kDelay = 10;
    constexpr float kGatePos = 4.5;
    constexpr float kCoeff = 1.0f;

    dsp::Delayline right_traveling_line(kDelaySize, false, dsp::InterpolationType::Linear);
    dsp::Delayline left_traveling_line(kDelaySize, true, dsp::InterpolationType::Linear);

    right_traveling_line.SetDelay(kDelay);
    left_traveling_line.SetDelay(kDelay);

    // Feed a dirac pulse into both delay line
    right_traveling_line.TapIn(2, 1.f);
    left_traveling_line.TapIn(2, 1.f);

    right_traveling_line.TapIn(8, 1.f);
    left_traveling_line.TapIn(8, 1.f);

    PrintDelayline(right_traveling_line);
    PrintDelayline(left_traveling_line);

    dsp::WaveguideGate gate(true, kGatePos, kCoeff);

    // Looping for kGatePos*2 should bring us back to initial state
    for (auto i = 0; i < kGatePos * 2; ++i)
    {
        gate.Process(left_traveling_line, right_traveling_line);

        float left_sample = left_traveling_line.NextOut();
        float right_sample = right_traveling_line.NextOut();

        left_traveling_line.Tick(right_sample * -1.f);
        right_traveling_line.Tick(left_sample * -1.f);

        printf("Iteration %d\n", i);
        PrintDelayline(right_traveling_line);
        PrintDelayline(left_traveling_line);
    }

    ASSERT_EQ(left_traveling_line[2], 1);
    ASSERT_EQ(right_traveling_line[2], 1);
}