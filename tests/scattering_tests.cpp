#include "gmock/gmock-matchers.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <numeric>
#include <tuple>

#include "junction.h"
#include "test_utils.h"
#include "waveguide.h"

TEST(ScatterTest, ConserveEnergy)
{
    constexpr size_t max_size = 10;
    constexpr size_t delay_size = 6;
    dsp::Waveguide wave_(max_size);
    wave_.SetDelay(delay_size);

    // insert dirac
    wave_.TapIn(3, 1.f, 0.f);

    PrintWaveguide(wave_, delay_size);

    constexpr size_t loop_size = 100;

    float start_energy = GetWaveguideTotalEnergy(wave_, delay_size);
    printf("Start energy: %f\n", start_energy);

    for (size_t i = 0; i < loop_size; ++i)
    {
        float right, left;
        wave_.TapOut(3, right, left);

        auto scatter_out = dsp::Scatter2({right, left}, 0.5f);

        wave_.TapIn(3, scatter_out[0] - right, scatter_out[1] - left);

        wave_.NextOut(right, left);
        wave_.Tick(left * -1, right * -1);

        float new_energy = GetWaveguideTotalEnergy(wave_, delay_size);
        // printf("%f\n", new_energy);
        printf("iter #%zu, energy: %f\n", i, new_energy);
        PrintWaveguide(wave_, delay_size);

        // ASSERT_THAT(new_energy, ::testing::FloatNear(start_energy, 0.001f));
    }
}

// class ScatterTest : public ::testing::TestWithParam<std::tuple<std::array<float, 2>, float>>
// {
// };

// // Test that the energy is conserved
// TEST_P(ScatterTest, Scatter2)
// {
//     std::array<float, 2> inputs = std::get<0>(GetParam());
//     float k = std::get<1>(GetParam());
//     auto outputs = dsp::Scatter2(inputs, k);

//     auto input_sum = std::accumulate(inputs.begin(), inputs.end(), 0.f);
//     auto output_sum = std::accumulate(outputs.begin(), outputs.end(), 0.f);

//     ASSERT_THAT(output_sum, ::testing::FloatNear(input_sum, 0.001f));
// }

// INSTANTIATE_TEST_SUITE_P(
//     ScatterTests, ScatterTest,
//     ::testing::Combine(::testing::Values(std::array<float, 2>{1.f, 1.f}, std::array<float, 2>{0.f, 1.f},
//                                          std::array<float, 2>{1.f, 0.f}, std::array<float, 2>{-1.f, -1.f},
//                                          std::array<float, 2>{0.25f, 0.50f}, std::array<float, 2>{0.75f, -1.f},
//                                          std::array<float, 2>{-0.5f, 0.5f}, std::array<float, 2>{0.f, 0.f}),
//                        ::testing::Range(-1.f, 1.f, 0.1f)));