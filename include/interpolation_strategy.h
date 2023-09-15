#pragma once

#include <cstddef>

namespace dsp
{

enum class InterpolationType
{
    None,
    Linear,
    Allpass
};

class InterpolationStrategy
{
  public:
    InterpolationStrategy() = default;
    virtual ~InterpolationStrategy() = default;

    virtual float TapOut(float* buffer, size_t max_size, size_t write_ptr, float delay) = 0;
    virtual void TapIn(float* buffer, size_t max_size, size_t write_ptr, float delay, float input) = 0;
};

class NoInterpolation : public InterpolationStrategy
{
  public:
    NoInterpolation() = default;
    ~NoInterpolation() override = default;

    float TapOut(float* buffer, size_t max_size, size_t write_ptr, float delay) override;
    void TapIn(float* buffer, size_t max_size, size_t write_ptr, float delay, float input) override;
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

  private:
    void CalculateCoeff(float delay);

    float delay_;
    size_t allpass_delay_;
    float coeff_;
    float last_out_;
    float allpass_input_;
};
} // namespace dsp