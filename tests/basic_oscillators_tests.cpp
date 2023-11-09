#include "gmock/gmock-matchers.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "basic_oscillators.h"
#include "dsp_base.h"
#include "sin_table.h"

#include <cmath>

TEST(BasicOscillatorsTest, Sine)
{
    float inc = 1.f / 1024;
    for (auto i = 0.f; i < TWO_PI; i += inc)
    {
        auto s = sfdsp::Sine(i);
        auto t = std::sin(i * TWO_PI);
        ASSERT_NEAR(s, t, 0.0001f);
    }
}

TEST(BasicOscillatorsTests, Noise)
{
    for (auto i = 0; i < 1000; ++i)
    {
        auto n = sfdsp::Noise();
        ASSERT_THAT(n, ::testing::Le(1.f));
        ASSERT_THAT(n, ::testing::Ge(-1.f));
    }
}