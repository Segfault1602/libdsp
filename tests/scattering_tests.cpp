#include "gmock/gmock-matchers.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <numeric>
#include <tuple>

#include "junction.h"

class ScatterTest : public ::testing::TestWithParam<std::tuple<std::array<float, 2>, float>>
{
};

// Test that the energy is conserved
TEST_P(ScatterTest, Scatter2)
{
    std::array<float, 2> inputs = std::get<0>(GetParam());
    float k = std::get<1>(GetParam());
    auto outputs = dsp::Scatter2(inputs, k);

    auto input_sum = std::accumulate(inputs.begin(), inputs.end(), 0.f);
    auto output_sum = std::accumulate(outputs.begin(), outputs.end(), 0.f);

    ASSERT_THAT(output_sum, ::testing::FloatNear(input_sum, 0.001f));
}

INSTANTIATE_TEST_SUITE_P(
    ScatterTests, ScatterTest,
    ::testing::Combine(::testing::Values(std::array<float, 2>{1.f, 1.f}, std::array<float, 2>{0.f, 1.f},
                                         std::array<float, 2>{1.f, 0.f}, std::array<float, 2>{-1.f, -1.f},
                                         std::array<float, 2>{0.25f, 0.50f}, std::array<float, 2>{0.75f, -1.f},
                                         std::array<float, 2>{-0.5f, 0.5f}, std::array<float, 2>{0.f, 0.f}),
                       ::testing::Range(-1.f, 1.f, 0.1f)));