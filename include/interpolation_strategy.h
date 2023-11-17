#pragma once

#include <cstddef>

namespace sfdsp
{

/// @brief Enum defining the interpolation types.
enum class InterpolationType
{
    /// @brief No interpolation
    None,
    /// @brief Linear interpolation
    Linear,
    /// @brief Allpass interpolation
    Allpass
};

/// @brief Base class for interpolation strategies.
class InterpolationStrategy
{
  public:
    InterpolationStrategy() = default;
    virtual ~InterpolationStrategy() = default;

    /// @brief Returns the interpolated sample at the specified delay.
    /// @param buffer The memory buffer to read from.
    /// @param max_size The maximum size of the buffer.
    /// @param write_ptr The write pointer of the buffer. This is where the delay is relative to.
    /// @param delay The delay in samples.
    /// @return
    virtual float TapOut(float* buffer, size_t max_size, size_t write_ptr, float delay) = 0;

    /// @brief Add a sample to the buffer at the specified delay.
    /// @param buffer The memory buffer to write to.
    /// @param max_size The maximum size of the buffer.
    /// @param write_ptr The write pointer of the buffer. This is where the delay is relative to.
    /// @param delay The delay in samples.
    /// @param input The sample to add to the buffer.
    virtual void TapIn(float* buffer, size_t max_size, size_t write_ptr, float delay, float input) = 0;
};

/// @brief No interpolation strategy.
/// @details If a fractional delay is specified, the sample at the integer part of the delay is returned.
class NoInterpolation : public InterpolationStrategy
{
  public:
    NoInterpolation() = default;
    ~NoInterpolation() override = default;

    float TapOut(float* buffer, size_t max_size, size_t write_ptr, float delay) override;
    void TapIn(float* buffer, size_t max_size, size_t write_ptr, float delay, float input) override;
};

/// @brief Linear interpolation strategy.
/// @details Performs a linear interpolation with equation `y = x[n] + (x[n+1] - x[n]) * frac` where `n` is the integer
/// part and `frac` is the fractional part.
class LinearInterpolation : public InterpolationStrategy
{
  public:
    LinearInterpolation() = default;
    ~LinearInterpolation() override = default;

    float TapOut(float* buffer, size_t max_size, size_t write_ptr, float delay) override;
    void TapIn(float* buffer, size_t max_size, size_t write_ptr, float delay, float input) override;
};

/// @brief Allpass interpolation strategy.
/// @details Implements a simple first order allpass interpolation. Unlike the linear interpolation, this strategy is
/// not stateless. It is recommended to use this strategy if the delay is _not_ changing over time.
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
} // namespace sfdsp