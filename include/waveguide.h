#pragma once

#include <cstddef>

#include "dsp_base.h"
#include "delayline_linear.h"
#include "filter.h"

template <size_t MAX_SIZE>
class Waveguide
{
public:
    Waveguide() = default;

    void SetDelay(DspFloat delay)
    {
        left_to_right_.SetDelay(delay);
        right_to_left_.SetDelay(delay);
    }

    DspFloat Tick()
    {
        DspFloat right = left_to_right_.Read();

        right = right_filter_.Tick(right);

        DspFloat left = right_to_left_.Tick(right);
        left = left_filter_.Tick(left)
        (void)left_to_right_.Tick(left);
    }

private:
    LinearDelayLine<MAX_SIZE> left_to_right_;
    LinearDelayLine<MAX_SIZE> right_to_left_;

    Filter* left_filter_;
    Filter* right_filter_;
};