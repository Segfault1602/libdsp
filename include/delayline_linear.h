#ifndef STK_DELAYL_H
#define STK_DELAYL_H

#include <cassert>
#include <cmath>

#include "delayline.h"
#include "dsp_base.h"

namespace dsp
{

template <size_t MAX_DELAY>
class LinearDelayline : public Delayline
{
  public:
    LinearDelayline() : Delayline(MAX_DELAY)
    {
        line_.fill(0.f);
    }
    ~LinearDelayline() override = default;

    DspFloat NextOut() override
    {
        if (do_next_out_)
        {
            DspFloat a = line_[read_ptr_];
            DspFloat b = line_[(read_ptr_ + 1) % MAX_DELAY];
            next_out_ = a + (b - a) * frac_;
            do_next_out_ = false;
        }

        return next_out_;
    }

    DspFloat Tick(DspFloat input) override
    {
        last_out_ = NextOut();
        do_next_out_ = true;

        line_[write_ptr_] = input;
        write_ptr_ = (write_ptr_ + 1) % MAX_DELAY;

        read_ptr_ = (read_ptr_ + 1) % MAX_DELAY;

        return last_out_;
    }

    DspFloat LastOut() const override
    {
        return last_out_;
    }

    DspFloat TapOut(DspFloat delay) const override
    {
        DspFloat read_ptr = write_ptr_ - delay - 1;
        while (read_ptr < 0)
        {
            read_ptr += MAX_DELAY;
        }

        size_t read_idx = static_cast<size_t>(read_ptr);
        DspFloat frac = read_ptr - read_idx;

        DspFloat a = line_[read_idx];
        DspFloat b = line_[(read_idx + 1) % MAX_DELAY];

        return a + (b - a) * frac;
    }

    void TapIn(DspFloat delay, DspFloat input) override
    {
        DspFloat write_ptr = write_ptr_ - delay - 1;
        while (write_ptr < 0)
        {
            write_ptr += MAX_DELAY;
        }

        size_t write_idx = static_cast<size_t>(write_ptr);
        DspFloat frac = write_ptr - write_idx;

        line_[write_idx] += input * (1.f - frac);
        line_[(write_idx + 1) % MAX_DELAY] += input * frac;
    }

  private:
    bool do_next_out_ = true;
    DspFloat next_out_ = 0.f;
    DspFloat last_out_ = 0.f;

    std::array<DspFloat, MAX_DELAY> line_ = {0, 1, 2, 3, 4, 5};
};

} // namespace dsp

#endif