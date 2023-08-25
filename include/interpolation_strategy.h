#pragma once

#include <cstddef>

namespace dsp
{
class InterpolationStrategy
{
  public:
    InterpolationStrategy() = default;
    virtual ~InterpolationStrategy() = default;

    virtual float TapOut(float* buffer, size_t max_size, size_t write_ptr, float delay) = 0;
    virtual void TapIn(float* buffer, size_t max_size, size_t write_ptr, float delay, float input) = 0;
};

class LinearInterpolation : public InterpolationStrategy
{
  public:
    LinearInterpolation() = default;
    ~LinearInterpolation() override = default;

    float TapOut(float* buffer, size_t max_size, size_t write_ptr, float delay) override;
    void TapIn(float* buffer, size_t max_size, size_t write_ptr, float delay, float input) override;
};

class AllpassInterpolation : public InterpolationStrategy
{
  public:
    AllpassInterpolation() = default;
    ~AllpassInterpolation() override = default;

    float TapOut(float* buffer, size_t max_size, size_t write_ptr, float delay) override;
    void TapIn(float* buffer, size_t max_size, size_t write_ptr, float delay, float input) override;
};
} // namespace dsp