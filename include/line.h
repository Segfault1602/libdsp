#pragma once

#include <cstdint>

namespace sfdsp
{
/// @brief Simple ramp generator.
/// @details Given a start point, an end point and a number of samples, this class will generate a ramp from the
/// starting point to the end point in the specified number of samples using linear interpolation. Once the ramp reaches
/// the end point, it will keep returning the same value forever.
class Line
{
  public:
    /// @brief Default constructor provided for convenience. Will always return 0.
    Line() = default;

    /// @brief Constructor.
    /// @param start The starting point of the ramp.
    /// @param end The end point of the ramp.
    /// @param time_samples The number of samples to generate the ramp in.
    Line(float start, float end, uint32_t time_samples);
    ~Line() = default;

    /// @brief Returns the next sample from the ramp.
    /// @return The next sample from the ramp.
    float Tick();

  private:
    float start_ = 0.f;
    float end_ = 0.f;
    float output_ = 0.f;
    float increment_ = 0.f;
};
} // namespace sfdsp