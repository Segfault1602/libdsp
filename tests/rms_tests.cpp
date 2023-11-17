#include "gmock/gmock-matchers.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <cmath>

#include "dsp_utils.h"
#include "rms.h"

class RMSTest : public ::testing::TestWithParam<float>
{
};

TEST_P(RMSTest, PureSine)
{

    sfdsp::RMS rms(4096);

    const float kAmplitude = GetParam();
    constexpr float kFreq = 440.f;
    constexpr float kSampleRate = 48000.f;
    constexpr float kPhaseInc = kFreq / kSampleRate;

    constexpr size_t kLoopLength = kSampleRate;

    float phase = 0.f;

    for (size_t i = 0; i < kLoopLength; i++)
    {
        auto s = kAmplitude * std::sin(phase * TWO_PI);
        phase = std::fmod(phase + kPhaseInc, 1.f);

        (void)rms.Tick(s);
    }

    auto rms_value = rms.GetRMS();

    float expected_rms = kAmplitude / std::sqrt(2.f);

    ASSERT_THAT(rms_value, ::testing::FloatNear(expected_rms, 0.001f));
}

INSTANTIATE_TEST_SUITE_P(RMSTesting, RMSTest, ::testing::Range(0.1f, 1.f, 0.1f));