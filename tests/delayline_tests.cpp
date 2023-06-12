#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

#include "delayline_linear.h"
#include "resources.h"

template <size_t max_size>
class DelayLine
{
  public:
    DelayLine()
    {
        inputs_.fill(0);

        inPoint_ = 0;
        doNextOut_ = true;
    }
    ~DelayLine()
    {
    }

    /** sets the delay time in samples
        If a float is passed in, a fractional component will be calculated for interpolating the delay line.
    */
    inline void SetDelay(float delay)
    {
        if (delay + 1 > inputs_.size())
        { // The value is too big.
            return;
        }

        if (delay < 0)
        {
            return;
        }

        float outPointer = inPoint_ - delay; // read chases write
        delay_ = delay;

        while (outPointer < 0)
            outPointer += inputs_.size(); // modulo maximum length

        outPoint_ = (long)outPointer;     // integer part

        alpha_ = outPointer - outPoint_;  // fractional part
        omAlpha_ = (float)1.0 - alpha_;

        if (outPoint_ == inputs_.size())
            outPoint_ = 0;
        doNextOut_ = true;
    }

    float nextOut(void)
    {
        if (doNextOut_)
        {
            // First 1/2 of interpolation
            nextOutput_ = inputs_[outPoint_] * omAlpha_;
            // Second 1/2 of interpolation
            if (outPoint_ + 1 < inputs_.size())
                nextOutput_ += inputs_[outPoint_ + 1] * alpha_;
            else
                nextOutput_ += inputs_[0] * alpha_;
            doNextOut_ = false;
        }

        return nextOutput_;
    }

    float Tick(float input)
    {
        inputs_[inPoint_++] = input;

        // Increment input pointer modulo length.
        if (inPoint_ == inputs_.size())
            inPoint_ = 0;

        lastFrame_ = nextOut();
        doNextOut_ = true;

        // Increment output pointer modulo length.
        if (++outPoint_ == inputs_.size())
            outPoint_ = 0;

        return lastFrame_;
    }

    float TapOut(unsigned long tapDelay)
    {
        long tap = inPoint_ - tapDelay - 1;
        while (tap < 0) // Check for wraparound.
            tap += inputs_.size();

        return inputs_[tap];
    }

    void TapIn(unsigned long tapDelay, float value)
    {
        long tap = inPoint_ - tapDelay - 1;
        while (tap < 0) // Check for wraparound.
            tap += inputs_.size();

        inputs_[tap] = value;
    }

  private:
    unsigned long inPoint_;
    unsigned long outPoint_;
    float delay_;
    float alpha_;
    float omAlpha_;
    float nextOutput_;
    float lastFrame_;
    bool doNextOut_;
    std::array<float, max_size> inputs_;
};

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

TEST(LinearDelaylineTests, TapIn)
{
    constexpr size_t max_delay_size = 100;
    dsp::LinearDelayline<max_delay_size> line_;

    constexpr float delay = 10;
    line_.SetDelay(delay);

    constexpr DspFloat sample = 0.1234f;
    line_.TapIn(5, sample);
    ASSERT_EQ(line_.TapOut(5), sample);

    line_.TapIn(delay, sample);
    ASSERT_EQ(line_.TapOut(delay), sample);

    line_.TapIn(delay + 1, sample);
    ASSERT_EQ(line_.TapOut(delay), sample);
}