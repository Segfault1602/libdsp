#ifndef STK_DELAYL_H
#define STK_DELAYL_H

#include <cassert>
#include <cmath>

#include "delayline.h"
#include "dsp_base.h"

namespace dsp
{

template <size_t MAX_DELAY>
class LinearDelayline : public Delayline<MAX_DELAY>
{
  public:
    LinearDelayline() = default;
    ~LinearDelayline() override = default;

    DspFloat Read() override
    {
        DspFloat next = line_[out_ptr_] * inv_frac_;
        size_t next_ptr = std::fmodf(out_ptr_ + 1, MAX_DELAY);
        next += line_[next_ptr] * frac_;

        return next;
    }

    DspFloat Tick(DspFloat input) override
    {
        line_[in_ptr_++] = input;
        in_ptr_ = (in_ptr_ + 1) % MAX_DELAY;

        DspFloat next = line_[out_ptr_] * inv_frac_;
        out_ptr_ = fmodf(out_ptr_ + 1, MAX_DELAY);
        next += line_[out_ptr_] * frac_;

        return next;
    }

    DspFloat TapOut(DspFloat delay) const override
    {
        DspFloat out_ptr = std::fmodf(in_ptr_ - delay, MAX_DELAY);

        DspFloat out_idx = static_cast<size_t>(out_ptr);
        DspFloat frac = out_ptr - out_idx;

        DspFloat out = line_[out_idx] * (1.f - frac);
        out_idx = std::fmodf(out_idx + 1, MAX_DELAY);
        out += line_[out_idx] * frac;

        return out;
    }

  private:
    using Delayline<MAX_DELAY>::line_;
    using Delayline<MAX_DELAY>::in_ptr_;
    using Delayline<MAX_DELAY>::out_ptr_;
    using Delayline<MAX_DELAY>::frac_;
    using Delayline<MAX_DELAY>::inv_frac_;
};

} // namespace dsp

#endif