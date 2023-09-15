#pragma once

#include <cassert>
#include <cmath>
#include <cstdint>

#include "delayline.h"
#include "dsp_base.h"

namespace dsp
{

/// @brief Implements a linear delayline with a maximum delay of MAX_DELAY
/// @tparam MAX_DELAY The maximum delay in samples
class LinearDelayline : public Delayline
{
  public:
    LinearDelayline(size_t max_size, bool reverse = false);
    ~LinearDelayline() override = default;

    float NextOut() override;

    float Tick(float input) override;

    float LastOut() const override;

    float TapOut(float delay) const override;

    void TapIn(float delay, float input) override;
    void SetIn(float delay, float input) override;

  private:
    bool do_next_out_ = true;
    float next_out_ = 0.f;
    float last_out_ = 0.f;
};
} // namespace dsp
