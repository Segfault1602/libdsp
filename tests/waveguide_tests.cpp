#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "waveguide.h"

template <size_t size>
void PrintWaveguide(dsp::Waveguide<size>& wave, size_t delay_size)
{
    std::vector<DspFloat> right_samples, left_samples;
    for (size_t i = 0; i < delay_size; ++i)
    {
        DspFloat right, left;
        wave.TapOut(i, right, left);
        right_samples.push_back(right);
        left_samples.push_back(left);
    }

    for (auto sample : right_samples)
    {
        printf("%f ", sample);
    }
    printf("\n");

    for (auto sample : left_samples)
    {
        printf("%f ", sample);
    }
    printf("\n");
}

TEST(WaveguideTests, EmptyWaveguide)
{
    // Check that energy is not magically introduce into the waveguide
    constexpr size_t WAVEGUIDE_SIZE = 128;
    dsp::Waveguide<WAVEGUIDE_SIZE> wave;

    constexpr size_t LOOP_SIZE = WAVEGUIDE_SIZE * 10;

    for (size_t i = 0; i < LOOP_SIZE; ++i)
    {
        wave.Tick();
        DspFloat sample = wave.TapOut(WAVEGUIDE_SIZE / 2);
        ASSERT_THAT(0.f, ::testing::FloatEq(sample));
    }
}

TEST(WaveguideTests, TapInTapOut)
{
    // Check that energy is not magically introduce into the waveguide
    constexpr size_t WAVEGUIDE_SIZE = 6;
    dsp::Waveguide<WAVEGUIDE_SIZE> wave;

    constexpr size_t DELAY_SIZE = WAVEGUIDE_SIZE;
    wave.SetDelay(DELAY_SIZE);

    for (size_t i = 0; i < DELAY_SIZE; ++i)
    {
        wave.TapIn(i, i);
        DspFloat out = wave.TapOut(i);

        // TapOut will return the sum of both delayline so we should expect two time
        // the energy we tapped in.
        ASSERT_THAT(i * 2, ::testing::Eq(out));
    }
}

TEST(WaveguideTests, GainTest)
{
    // Check that energy is not magically introduce into the waveguide
    constexpr size_t WAVEGUIDE_SIZE = 7;
    dsp::Waveguide<WAVEGUIDE_SIZE> wave;

    constexpr size_t DELAY_SIZE = WAVEGUIDE_SIZE-1;
    wave.SetDelay(DELAY_SIZE);

    wave.SetGain(-1);

    constexpr DspFloat input[DELAY_SIZE] = {1, 2, 3, 4, 5, 6};
    for (size_t i = 0; i < DELAY_SIZE; ++i)
    {
        wave.TapIn(i, input[i]);
        DspFloat out = wave.TapOut(i);
        ASSERT_THAT(input[i] * 2, ::testing::Eq(out));
    }

    PrintWaveguide(wave, DELAY_SIZE);

    for (size_t i = 0; i < DELAY_SIZE; ++i)
    {
        printf("Iter %zu:\n", i);
        wave.Tick();
        PrintWaveguide(wave, DELAY_SIZE);
    }

    for (int i = 0; i < DELAY_SIZE; ++i)
    {
        DspFloat out = wave.TapOut(i);
        ASSERT_THAT(-1 * (i * 2), ::testing::Eq(out));
    }
}