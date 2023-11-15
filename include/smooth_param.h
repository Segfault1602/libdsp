#pragma once

#include "filter.h"

namespace sfdsp
{

class SmoothParam
{
  public:
    enum class SmoothingType
    {
        None,
        Exponential,
    };

    SmoothParam() = default;

    void Init(size_t samplerate, SmoothingType type, float value = 0);

    void SetTarget(float target);

    float GetTarget() const;

    float Tick();

  private:
    SmoothingType type_;
    float value_;

    OnePoleFilter smoothing_filter_;
};
} // namespace sfdsp