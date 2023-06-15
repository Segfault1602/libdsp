#pragma once

/// @file filter.h
/// @brief Simple filters implementation. Differential equations where taken from here:
/// https://ccrma.stanford.edu/~jos/filters/Elementary_Filter_Sections.html

#include "dsp_base.h"

#include <array>

template <size_t N>
class Filter
{
  public:
    Filter() = default;
    virtual ~Filter() = default;

    virtual DspFloat Tick(DspFloat in) = 0;

  protected:
    std::array<DspFloat, N> b_ = {0.f};
    std::array<DspFloat, N> a_ = {0.f};
    std::array<DspFloat, N> outputs_ = {0.f};
    std::array<DspFloat, N> inputs_ = {0.f};
};

/// @brief Implements a simple one pole filter with differential equation y(n) = b0*x(n) - a1*y(n-1)
class OnePoleFilter : public Filter<2>
{
  public:
    OnePoleFilter() = default;
    ~OnePoleFilter() override = default;

    DspFloat Tick(DspFloat in) override
    {
        outputs_[0] = in * b_[0] - outputs_[0] * a_[1];
        return outputs_[0];
    }
};

/// @brief Implements a simple one zero filter with differential equation y(n) = b0*x(n) + b1*x(n-1)
class OneZeroFilter : public Filter<2>
{
  public:
    OneZeroFilter() = default;
    ~OneZeroFilter() override = default;

    DspFloat Tick(DspFloat in) override
    {
        DspFloat out = in * b_[0] + inputs_[0] * b_[1];
        inputs_[0] = in;
        return out;
    }
};

/// @brief Implements a simple two pole filter with differential equation y(n) = b0*x(n) - a1*y(n-1) - a2*y(n-2)
class TwoPoleFilter : public Filter<3>
{
  public:
    TwoPoleFilter() = default;
    ~TwoPoleFilter() override = default;

    DspFloat Tick(DspFloat in) override
    {
        outputs_[0] = in * b_[0] - outputs_[1] * a_[1] - outputs_[2] * a_[2];
        outputs_[2] = outputs_[1];
        outputs_[1] = outputs_[0];
        return outputs_[0];
    }
};

class TwoZeroFilter : public Filter<2>
{
  public:
    TwoZeroFilter() = default;
    ~TwoZeroFilter() override = default;

    DspFloat Tick(DspFloat in) override
    {
    }
};