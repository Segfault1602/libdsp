#pragma once

#include <cstdint>

namespace sfdsp
{
/// @brief Simple ramp generator.
class Line
{
  public:
    Line() = default;
    Line(float start, float end, uint32_t time_samples);
    ~Line() = default;

    float Tick();

  private:
    float start_ = 0.f;
    float end_ = 0.f;
    float output_ = 0.f;
    float increment_ = 0.f;
};
} // namespace sfdsp