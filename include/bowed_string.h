#pragma once

#include "dsp_base.h"
#include "waveguide.h"

#include <algorithm>

namespace dsp
{

class BowedString
{
  public:
    BowedString()
    {
    }
    ~BowedString() = default;

    void Init(DspFloat samplerate)
    {
        samplerate_ = samplerate;
        SetFrequency(220);
    }

    void SetFrequency(DspFloat f)
    {
        freq_ = f;
        DspFloat new_delay = (samplerate_ / freq_) * 0.5;
        waveguide_.SetDelay(new_delay);
        // waveguide_.SetJunction(waveguide_.GetDelay() - new_delay);
        output_pickup_ = new_delay - 10;
    }

    void Excite()
    {
        constexpr DspFloat relative_position = 0.66f;
        DspFloat pos = waveguide_.GetDelay() * relative_position;
        DspFloat out = waveguide_.TapOut(pos);
        waveguide_.TapIn(pos, ComputeBowOutput(-0.25*out, 1.f));
    }

    DspFloat Tick()
    {
        waveguide_.Tick();
        return waveguide_.TapOut(output_pickup_);
    }

  private:
    // Simple bowed string non-linear function taken from the STK.
    // https://github.com/thestk/stk/blob/master/include/BowTable.h
    DspFloat ComputeBowOutput(DspFloat in, DspFloat force)
    {
        DspFloat sample = (in + offset_) * force;

        DspFloat out = std::fabs(sample + 0.75f);
        out = std::pow(out, -4.0);

        constexpr DspFloat min_output = 0.01;
        constexpr DspFloat max_output = 0.98;
        out = std::clamp(out, min_output, max_output);
        return out;
    }

    Waveguide<4096> waveguide_;
    DspFloat samplerate_;
    DspFloat output_pickup_;
    DspFloat freq_;
    DspFloat offset_ = 0.001f;
    // DspFloat velocity_ = 0.25f;
    // DspFloat force_ = 3.f;
};
} // namespace dsp