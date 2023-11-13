#pragma once

/// @file filter.h
/// @brief Simple filters implementation. Differential equations where taken from here:
/// https://ccrma.stanford.edu/~jos/filters/Elementary_Filter_Sections.html
/// Implementation for a lot of these functions were also taken from the STK (Synthesis ToolKit) library:
/// https://github.com/thestk/stk

#include "dsp_base.h"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>

namespace sfdsp
{
/// @brief Base class for filters
class Filter
{
    static constexpr size_t COEFFICIENT_COUNT = 3;

  public:
    Filter() = default;
    virtual ~Filter() = default;

    /// @brief  Tick the filter.
    /// @param in Input sample
    /// @return Output sample
    virtual float Tick(float in) = 0;

    /// @brief Filter a block of samples. 'in' and 'out' can be the same buffer.
    /// @param in The input buffer.
    /// @param out The output buffer.
    /// @param size The size of the buffer.
    void ProcessBlock(float* in, float* out, size_t size);

    /// @brief Set the gain of the filter.
    /// @param gain
    void SetGain(float gain);

    /// @brief Set the 'a' coefficients of the filter.
    /// @param a Array of size COEFFICIENT_COUNT containing the 'a' coefficients.
    void SetA(const float (&a)[COEFFICIENT_COUNT]);

    /// @brief Set the 'b' coefficients of the filter.
    /// @param b Array of size COEFFICIENT_COUNT containing the 'b' coefficients.
    void SetB(const float (&b)[COEFFICIENT_COUNT]);

  protected:
    float gain_ = 1.f;
    std::array<float, 3> b_ = {0.f, 0.f, 0.f};
    std::array<float, 3> a_ = {1.f, 0.f, 0.f};
    std::array<float, 3> outputs_ = {0.f};
    std::array<float, 3> inputs_ = {0.f};
};

/// @brief Implements a simple one pole filter with differential equation y(n) = b0*x(n) - a1*y(n-1)
class OnePoleFilter : public Filter
{
  public:
    OnePoleFilter() = default;
    ~OnePoleFilter() override = default;

    /// @brief Set the pole of the filter.
    /// @param pole The pole of the filter.
    ///
    void SetPole(float pole);

    /// @brief Set the pole of the filter to obtain an exponential decay filter.
    /// @param decayDb The decay in decibels.
    /// @param timeMs The time in milliseconds.
    /// @param samplerate The samplerate.
    void SetDecayFilter(float decayDb, float timeMs, float samplerate);

    void SetCutOff(float cutoff, float samplerate);

    float Tick(float in) override;
};

/// @brief Implements a simple one zero filter with differential equation y(n) = b0*x(n) + b1*x(n-1)
class OneZeroFilter : public Filter
{
  public:
    OneZeroFilter() = default;
    ~OneZeroFilter() override = default;

    float Tick(float in) override;
};

/// @brief Implements a simple two pole filter with differential equation y(n) = b0*x(n) - a1*y(n-1) - a2*y(n-2)
class TwoPoleFilter : public Filter
{
  public:
    TwoPoleFilter() = default;
    ~TwoPoleFilter() override = default;

    float Tick(float in) override;
};

/// @brief Implements a simple two zero filter with differential equation y(n) = b0*x(n) + b1*x(n-1) + b2*x(n-2)
class TwoZeroFilter : public Filter
{
  public:
    TwoZeroFilter() = default;
    ~TwoZeroFilter() override = default;

    float Tick(float in) override;
};

/// @brief Implements a simple biquad filter with differential equation
/// y(n) = b0*x(n) + b1*x(n-1) + b2*x(n-2) - a1*y(n-1) - a2*y(n-2)
class Biquad : public Filter
{
  public:
    Biquad() = default;
    ~Biquad() override = default;

    void SetCoefficients(float b0, float b1, float b2, float a1, float a2);

    float Tick(float in) override;
};
} // namespace sfdsp