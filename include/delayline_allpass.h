#pragma once

#include "delayline.h"

namespace dsp
{
template <size_t MAX_DELAY>
class AllpassDelayline : public Delayline<MAX_DELAY>
{
  public:
    AllpassDelayline(size_t max_size, bool reverse = false);
    ~AllpassDelayline() override = default;

    float NextOut() override;
    float Tick(float input) override;
    float TapOut(float delay) const override;
    void TapIn(float delay, float input) override;
    void SetIn(float delay, float input) override;
    float LastOut() const override;
};
} // namespace dsp
