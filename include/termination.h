#pragma once

#include "dsp_base.h"

#include "delayline.h"
#include "filter.h"

namespace dsp
{
/// @brief Base class for termination points between two delaylines
class Termination
{
  public:
    Termination() = default;
    virtual ~Termination() = default;

    virtual void SetGain(DspFloat gain)
    {
        gain_ = gain;
    }

    virtual void SetFilter(Filter* filter)
    {
        filter_ = filter;
    }

    virtual void Tick(Delayline& left_traveling_line, Delayline& right_traveling_line) = 0;

  protected:
    DspFloat gain_ = -1.f;
    Filter* filter_ = nullptr;
};

/// @brief Simple Termination that takes one sample from the left traveling lane and feeds it
/// to the right traveling lane with a gain adjustment (gain defaults to -1).
class LeftTermination : public Termination
{
  public:
    LeftTermination()
    {
        SetGain(-1.f);
    }

    virtual ~LeftTermination() override = default;

    virtual void Tick(Delayline& left_traveling_line, Delayline& right_traveling_line) override
    {
        DspFloat next_out = left_traveling_line.NextOut();
        if (filter_)
        {
            next_out = filter_->Tick(next_out);
        }
        (void)right_traveling_line.Tick(next_out * -1.f);
    }
};

/// @brief Simple Termination that takes one sample from the right traveling lane and feeds it
/// to the left traveling lane with a gain adjustment (gain defaults to -0.99).
class RightTermination : public Termination
{
  public:
    RightTermination()
    {
        SetGain(-0.99);
    }

    virtual ~RightTermination() override = default;

    virtual void Tick(Delayline& left_traveling_line, Delayline& right_traveling_line) override
    {
        DspFloat next_out = right_traveling_line.LastOut();
        if (filter_)
        {
            next_out = filter_->Tick(next_out);
        }
        (void)left_traveling_line.Tick(next_out * gain_);
    }
};
} // namespace dsp