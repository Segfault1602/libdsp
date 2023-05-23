#pragma once

#include <array>
#include <cassert>
#include <cstddef>

#include "dsp_base.h"

namespace dsp
{

template <size_t MAX_DELAY>
class Delayline
{
  public:
    Delayline()
    {
        line_.fill(0.f);
    }

    virtual ~Delayline() = default;

    void SetDelay(DspFloat delay)
    {
        if (delay > MAX_DELAY)
        {
            delay = MAX_DELAY;
        }

        DspFloat out_ptr = in_ptr_ - delay;
        delay_ = delay;

        while (out_ptr < 0)
            out_ptr += MAX_DELAY;

        out_ptr_ = static_cast<size_t>(out_ptr);
        frac_ = out_ptr - out_ptr_;
        inv_frac_ = 1.f - frac_;

        if (out_ptr_ == MAX_DELAY)
            out_ptr_ = 0;
    }

    virtual DspFloat Read() = 0;
    virtual DspFloat Tick(DspFloat input) = 0;
    virtual DspFloat TapOut(DspFloat delay) const = 0;

    DspFloat TapOut(size_t delay) const
    {
        size_t tap_ptr = in_ptr_ - delay;
        while (tap_ptr < 0)
            tap_ptr += MAX_DELAY;

        return line_[tap_ptr];
    }

  protected:
    size_t in_ptr_ = 0;
    size_t out_ptr_ = 0;
    DspFloat delay_ = 0.f;
    DspFloat frac_ = 0.f;
    DspFloat inv_frac_ = 0.f;

    std::array<DspFloat, MAX_DELAY> line_;
};
}