#pragma once

#include <cassert>
#include <cstddef>
#include <memory>

#include "dsp_base.h"
#include "interpolation_strategy.h"

namespace dsp
{

class Delayline
{
  public:
    Delayline(size_t max_size, bool reverse = false, InterpolationType interpolation_type = InterpolationType::Linear);
    ~Delayline() = default;

    void SetDelay(float delay);
    float GetDelay() const;
    void Reset();
    float NextOut();
    float LastOut() const;
    float Tick(float input);
    float TapOut(float delay) const;
    void TapIn(float delay, float input);
    void SetIn(float delay, float input);

  protected:
    const size_t max_size_ = 0;
    const bool reverse_ = false;

    bool do_next_out_ = true;
    float next_out_ = 0.f;
    float last_out_ = 0.f;

    size_t write_ptr_ = 0;
    float delay_ = 0;

    std::unique_ptr<InterpolationStrategy> interpolation_strategy_;
    std::unique_ptr<float[]> line_;
};
} // namespace dsp