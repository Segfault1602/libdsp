#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "termination.h"
#include "test_utils.h"
#include "waveguide.h"
#include "window_functions.h"

TEST(WaveguideTests, EmptyWaveguide)
{
    // Check that energy is not magically introduce into the waveguide
    constexpr size_t WAVEGUIDE_SIZE = 128;
    sfdsp::Waveguide wave(WAVEGUIDE_SIZE);

    constexpr size_t LOOP_SIZE = WAVEGUIDE_SIZE * 10;

    sfdsp::Termination left_termination(-1.f);
    sfdsp::Termination right_termination(-1.f);

    for (size_t i = 0; i < LOOP_SIZE; ++i)
    {
        float right, left;
        wave.NextOut(right, left);
        wave.Tick(left_termination.Tick(left), right_termination.Tick(right));
        float sample = wave.TapOut(WAVEGUIDE_SIZE / 2);
        ASSERT_THAT(0.f, ::testing::FloatEq(sample));
    }
}

TEST(WaveguideTests, Dirac)
{
    // Check that energy is not magically introduce into the waveguide
    constexpr size_t WAVEGUIDE_SIZE = 10;
    sfdsp::Waveguide wave(WAVEGUIDE_SIZE);

    constexpr size_t LOOP_SIZE = 16;
    constexpr float DELAY_SIZE = 6.25;
    wave.SetDelay(DELAY_SIZE);
    wave.TapIn(3, 1);

    sfdsp::Termination left_termination(-1.f);
    sfdsp::Termination right_termination(-1.f);

    PrintWaveguide(wave, DELAY_SIZE);

    for (size_t i = 0; i < LOOP_SIZE; ++i)
    {
        float right, left;
        wave.NextOut(right, left);
        wave.Tick(left_termination.Tick(left), right_termination.Tick(right));
        printf("iter #%zu\n", i);
        PrintWaveguide(wave, DELAY_SIZE);
    }
}

TEST(WaveguideTests, StabilityTestInteger)
{
    constexpr size_t WAVEGUIDE_SIZE = 128;
    sfdsp::Waveguide wave(WAVEGUIDE_SIZE);

    sfdsp::Termination left_termination(-1.f);
    sfdsp::Termination right_termination(-1.f);

    constexpr size_t LOOP_SIZE = WAVEGUIDE_SIZE * 100;
    constexpr size_t tap_in_pos = WAVEGUIDE_SIZE / 3;
    constexpr size_t tap_out_pos = WAVEGUIDE_SIZE - WAVEGUIDE_SIZE / 3;
    for (size_t i = 1; i <= LOOP_SIZE; ++i)
    {
        wave.TapIn(tap_in_pos, 1.f);
        float right, left;
        wave.NextOut(right, left);
        wave.Tick(left_termination.Tick(left), right_termination.Tick(right));

        float sample = wave.TapOut(tap_out_pos);
        ASSERT_THAT(std::fabs(sample), ::testing::Le(1.f));
    }
}

TEST(WaveguideTests, StabilityTestFrac)
{
    constexpr size_t WAVEGUIDE_SIZE = 7;
    sfdsp::Waveguide wave(WAVEGUIDE_SIZE);

    constexpr size_t DELAY_SIZE = WAVEGUIDE_SIZE - 1;
    wave.SetDelay(DELAY_SIZE);

    sfdsp::Termination left_termination(-1.f);
    sfdsp::Termination right_termination(-1.f);

    constexpr size_t LOOP_SIZE = DELAY_SIZE * 2;

    constexpr float tap_in_pos = 2.5f;
    constexpr float tap_out_pos = 4.5f;
    for (size_t i = 1; i < LOOP_SIZE; ++i)
    {
        wave.TapIn(tap_in_pos, 1.f);
        printf("iter #%zu\n", i);
        PrintWaveguide(wave, DELAY_SIZE);
        float right, left;
        wave.NextOut(right, left);
        wave.Tick(left_termination.Tick(left), right_termination.Tick(right));

        printf("After tick:\n");
        PrintWaveguide(wave, DELAY_SIZE);

        float sample = wave.TapOut(tap_out_pos);
        ASSERT_THAT(std::fabs(sample), ::testing::Le(1.f));
    }
}

TEST(WaveguideTests, TapInTapOut)
{
    // Check that energy is not magically introduce into the waveguide
    constexpr size_t WAVEGUIDE_SIZE = 128;
    sfdsp::Waveguide wave(WAVEGUIDE_SIZE);

    constexpr size_t DELAY_SIZE = WAVEGUIDE_SIZE - 1;
    wave.SetDelay(DELAY_SIZE);

    for (size_t i = 1; i <= DELAY_SIZE; ++i)
    {
        wave.TapIn(i, i);
        float out = wave.TapOut(i);

        // TapOut will return the sum of both delayline so we should expect two time
        // the energy we tapped in.
        ASSERT_THAT(i * 2, ::testing::Eq(out));
    }
}

TEST(WaveguideTests, TapInTapOut2)
{
    constexpr size_t WAVEGUIDE_SIZE = 7;
    sfdsp::Waveguide wave(WAVEGUIDE_SIZE);

    constexpr size_t DELAY_SIZE = WAVEGUIDE_SIZE - 1;
    wave.SetDelay(DELAY_SIZE);

    constexpr float input[DELAY_SIZE] = {1, 2, 3, 4, 5, 6};
    for (size_t i = 1; i <= DELAY_SIZE; ++i)
    {
        wave.TapIn(i, input[i - 1]);
        float out = wave.TapOut(i);
        ASSERT_THAT(input[i - 1] * 2, ::testing::Eq(out));
    }

    PrintWaveguide(wave, DELAY_SIZE);
    // Waveguide should now look like this:
    //    ▶  1  2  3  4  5  6   ▼
    // Left                    Right
    //    ▲  1  2  3  4  5  6   ◀

    for (float i = 1; i <= DELAY_SIZE - 1; i += .25f)
    {
        float right, left;
        wave.TapOut(i, right, left);
        ASSERT_THAT(right, ::testing::FloatEq(i));
        ASSERT_THAT(left, ::testing::FloatEq(i));
    }
}

TEST(WaveguideTests, GainTest)
{
    constexpr size_t WAVEGUIDE_SIZE = 70;
    sfdsp::Waveguide wave(WAVEGUIDE_SIZE);

    constexpr size_t DELAY_SIZE = 6;
    wave.SetDelay(DELAY_SIZE);

    // Set the gain to -1 so we can check that no energy is lost.
    sfdsp::Termination left_termination(-1.f);
    sfdsp::Termination right_termination(-1.f);

    constexpr float input[DELAY_SIZE] = {1, 2, 3, 4, 5, 6};
    for (size_t i = 1; i <= DELAY_SIZE; ++i)
    {
        wave.TapIn(i, input[i - 1]);
        float out = wave.TapOut(i);
        ASSERT_THAT(input[i - 1] * 2, ::testing::Eq(out));
    }

    PrintWaveguide(wave, DELAY_SIZE);

    // Waveguide should now look like this:
    //    ▶  1  2  3  4  5  6   ▼
    // Left                    Right
    //    ▲  1  2  3  4  5  6   ◀

    for (size_t i = 1; i <= DELAY_SIZE; ++i)
    {
        float right, left;
        wave.NextOut(right, left);
        wave.Tick(left_termination.Tick(left), right_termination.Tick(right));
        printf("iter #%zu\n", i);
        PrintWaveguide(wave, DELAY_SIZE);
        printf("Next out: %f %f\n", left, right);
    }

    PrintWaveguide(wave, DELAY_SIZE);

    // Waveguide should now look like this:
    //    ▶  -6 -5 -4 -3 -2 -1  ▼
    // Left                    Right
    //    ▲  -6 -5 -4 -3 -2 -1  ◀

    for (int i = 1; i <= DELAY_SIZE; ++i)
    {
        float out = wave.TapOut(i);
        ASSERT_THAT(out, ::testing::Eq(-1 * (input[DELAY_SIZE - i] * 2)));
    }

    for (size_t i = 1; i <= DELAY_SIZE; ++i)
    {
        float right, left;
        wave.NextOut(right, left);
        wave.Tick(left_termination.Tick(left), right_termination.Tick(right));
    }

    // Waveguide should now be back to initial state:
    //    ▶  1  2  3  4  5  6   ▼
    // Left                    Right
    //    ▲  1  2  3  4  5  6   ◀

    for (int i = 1; i <= DELAY_SIZE; ++i)
    {
        float out = wave.TapOut(i);
        ASSERT_THAT(out, ::testing::Eq(input[i - 1] * 2));
    }
}

TEST(WaveguideTests, DISABLED_Pluck)
{
    constexpr size_t WAVEGUIDE_SIZE = 501;
    sfdsp::Waveguide wave(WAVEGUIDE_SIZE, sfdsp::InterpolationType::Allpass);

    constexpr size_t DELAY_SIZE = 500;
    wave.SetDelay(DELAY_SIZE);

    for (size_t i = 1; i <= DELAY_SIZE; ++i)
    {
        wave.TapIn(i, sfdsp::Hann(i - 1, DELAY_SIZE));
    }

    PrintWaveguide(wave, DELAY_SIZE);
}