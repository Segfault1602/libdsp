#pragma once

#include <array>
#include <cassert>
#include <cstddef>

#include "dsp_base.h"

namespace dsp
{

class Delayline
{
  public:
    Delayline(size_t max_delay) : max_delay_(max_delay)
    {
        SetDelay(max_delay_);
    }

    virtual ~Delayline() = default;

    void SetDelay(DspFloat delay)
    {
        if ((delay + 1) > max_delay_)
        {
            delay = max_delay_ - 1;
        }

        DspFloat read_ptr = write_ptr_ - delay;
        delay_ = delay;

        while (read_ptr < 0)
            read_ptr += max_delay_;

        read_ptr_ = static_cast<size_t>(read_ptr);
        frac_ = read_ptr - read_ptr_;
        inv_frac_ = 1.f - frac_;

        if (read_ptr_ == max_delay_)
            read_ptr_ = 0;
    }

    DspFloat GetDelay() const
    {
        return delay_;
    }

    virtual DspFloat NextOut() = 0;
    virtual DspFloat Tick(DspFloat input) = 0;
    virtual DspFloat TapOut(DspFloat delay) const = 0;
    virtual void TapIn(DspFloat delay, DspFloat input) = 0;
    virtual DspFloat LastOut() const = 0;

  protected:
    size_t write_ptr_ = 0;
    size_t read_ptr_ = 0;
    DspFloat delay_ = 0.f;
    DspFloat frac_ = 0.f;
    DspFloat inv_frac_ = 0.f;

    const size_t max_delay_ = 0.f;

};
}