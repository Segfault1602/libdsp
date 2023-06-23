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

    virtual void SetGain(float gain)
    {
        gain_ = gain;
    }

    virtual void SetFilter(Filter* filter)
    {
        filter_ = filter;
    }

    virtual float Tick(float in)
    {
        if (filter_)
        {
            in = filter_->Tick(in);
        }

        return in * gain_;
    }

  protected:
    float gain_ = -1.f;
    Filter* filter_ = nullptr;
};
} // namespace dsp