#pragma once

#include <cstdint>

namespace dsp
{
/// @brief Simple ramp generator.
class Line
{
  public:
    Line(float start, float end, uint32_t time_samples);
    ~Line() = default;

    float Tick();

  private:
    float start_;
    float end_;
    float output_;
    float increment_;
};
} // namespace dsp