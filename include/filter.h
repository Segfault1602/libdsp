#pragma once

/// @file filter.h
/// @brief Simple filters implementation. Differential equations where taken from here:
/// https://ccrma.stanford.edu/~jos/filters/Elementary_Filter_Sections.html
/// Implementation for a lot of these functions were also taken from the STK (Synthesis ToolKit) library:
/// https://github.com/thestk/stk

#include "dsp_base.h"

#include <array>
#include <cassert>

/// @brief Base class for filters
class Filter
{
    static constexpr size_t COFFICIENT_COUNT = 3;

  public:
    Filter() = default;
    virtual ~Filter() = default;

    virtual DspFloat Tick(DspFloat in) = 0;

    void SetGain(DspFloat gain)
    {
        gain_ = gain;
    }
    void SetA(const DspFloat (&a)[COFFICIENT_COUNT])
    {
        for (size_t i = 0; i < COFFICIENT_COUNT; ++i)
        {
            a_[i] = a[i];
        }
    }

    void SetB(const DspFloat (&b)[COFFICIENT_COUNT])
    {
        for (size_t i = 0; i < COFFICIENT_COUNT; ++i)
        {
            b_[i] = b[i];
        }
    }

  protected:
    DspFloat gain_ = 1.f;
    std::array<DspFloat, 3> b_ = {0.f};
    std::array<DspFloat, 3> a_ = {0.f};
    std::array<DspFloat, 3> outputs_ = {0.f};
    std::array<DspFloat, 3> inputs_ = {0.f};
};

/// @brief Implements a simple one pole filter with differential equation y(n) = b0*x(n) - a1*y(n-1)
class OnePoleFilter : public Filter
{
  public:
    OnePoleFilter() = default;
    ~OnePoleFilter() override = default;

    void SetPole(DspFloat pole)
    {
        assert(pole < 1.f && pole > -1.f);

        if (pole > 0.f)
        {
            b_[0] = 1.f - pole;
        }
        else
        {
            b_[0] = 1.f + pole;
        }

        a_[1] = -pole;
    }

    DspFloat Tick(DspFloat in) override
    {
        outputs_[0] = gain_ * in * b_[0] - outputs_[1] * a_[1];
        outputs_[1] = outputs_[0];
        return outputs_[0];
    }
};

/// @brief Implements a simple one zero filter with differential equation y(n) = b0*x(n) + b1*x(n-1)
class OneZeroFilter : public Filter
{
  public:
    OneZeroFilter() = default;
    ~OneZeroFilter() override = default;

    DspFloat Tick(DspFloat in) override
    {
        DspFloat out = gain_ * in * b_[0] + inputs_[0] * b_[1];
        inputs_[0] = in;
        return out;
    }
};

/// @brief Implements a simple two pole filter with differential equation y(n) = b0*x(n) - a1*y(n-1) - a2*y(n-2)
class TwoPoleFilter : public Filter
{
  public:
    TwoPoleFilter() = default;
    ~TwoPoleFilter() override = default;

    DspFloat Tick(DspFloat in) override
    {
        outputs_[0] = gain_ * in * b_[0] - outputs_[1] * a_[1] - outputs_[2] * a_[2];
        outputs_[2] = outputs_[1];
        outputs_[1] = outputs_[0];
        return outputs_[0];
    }
};

/// @brief Implements a simple two zero filter with differential equation y(n) = b0*x(n) + b1*x(n-1) + b2*x(n-2)
class TwoZeroFilter : public Filter
{
  public:
    TwoZeroFilter() = default;
    ~TwoZeroFilter() override = default;

    DspFloat Tick(DspFloat in) override
    {
        DspFloat out = gain_ * in * b_[0] + inputs_[0] * b_[1] + inputs_[1] * b_[2];
        inputs_[1] = inputs_[0];
        inputs_[0] = in;
        return out;
    }
};

class Biquad : public Filter
{
  public:
    Biquad() = default;
    ~Biquad() override = default;

    void SetCoefficients(DspFloat b0, DspFloat b1, DspFloat b2, DspFloat a1, DspFloat a2)
    {
        b_[0] = b0;
        b_[1] = b1;
        b_[2] = b2;
        a_[1] = a1;
        a_[2] = a2;
    }

    DspFloat Tick(DspFloat in) override
    {
        inputs_[0] = gain_ * in;
        outputs_[0] = inputs_[0] * b_[0] + inputs_[1] * b_[1] + inputs_[2] * b_[2];
        outputs_[0] -= outputs_[1] * a_[1] + outputs_[2] * a_[2];
        inputs_[2] = inputs_[1];
        inputs_[1] = inputs_[0];
        outputs_[2] = outputs_[1];
        outputs_[1] = outputs_[0];
        return outputs_[0];
    }
};