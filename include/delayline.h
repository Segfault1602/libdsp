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
    Delayline(size_t max_delay);

    virtual ~Delayline() = default;

    void SetDelay(float delay);

    float GetDelay() const;

    virtual float NextOut() = 0;
    virtual float Tick(float input) = 0;
    virtual float TapOut(float delay) const = 0;
    virtual void TapIn(float delay, float input) = 0;
    virtual float LastOut() const = 0;

  protected:
    size_t write_ptr_ = 0;
    size_t read_ptr_ = 0;
    size_t delay_ = 0;
    float frac_ = 0.f;
    float inv_frac_ = 0.f;

    const size_t max_delay_ = 0;
};
} // namespace dsp