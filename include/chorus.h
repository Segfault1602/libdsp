// =============================================================================
// chorus.h
// Simple chorus effect.
// =============================================================================
#pragma once

#define CHORUS_LINEAR_DELAY

#ifdef CHORUS_LINEAR_DELAY
#include "delayline_linear.h"
#else
#include "delayline_sinc.h"
#endif
#include "dsp_base.h"
#include <cmath>

namespace dsp
{
template <size_t MAX_DELAY_SIZE> class Chorus : public DspBase
{
  public:
    Chorus() = default;

    ~Chorus() override = default;
    Chorus(const Chorus& c) = delete;

    void Init(uint32_t samplerate, DspFloat delay_ms, DspFloat width = 100, DspFloat speed = .5f)
    {
        samplerate_ = samplerate;
        SetDelay(delay_ms);
        delay_.SetDelay(base_delay_);
        width_ = width;
        speed_ = speed;

        DspFloat mod_period = samplerate / speed;
        mod_phase_dt_ = 1.f / mod_period;
        delta_t_ = 1.f / samplerate_;
    }

    void Reset()
    {
        delay_.Reset();
    }

    void SetDelay(DspFloat delay_ms)
    {
        // TODO: add max delay check here
        if (base_delay_ms_ != delay_ms)
        {
            base_delay_ = delay_ms * samples_per_ms_;
            base_delay_ms_ = delay_ms;
        }
    }

    void SetMix(DspFloat mix)
    {
        chorus_mix_ = mix;
        input_mix_ = 1.f - mix;
    }

    void SetWidth(DspFloat width)
    {
        width_ = width;
    }

    void SetSpeed(DspFloat speed)
    {
        if (speed_ != speed)
        {
            speed_ = speed;
            DspFloat mod_period = samplerate_ / speed;
            mod_phase_dt_ = 1.f / mod_period;
        }
    }

    DspFloat Tick(DspFloat in) override
    {

        DspFloat mod = Sine(mod_phase_) * width_;
        mod_phase_ += mod_phase_dt_;
        if (mod_phase_ >= 1.f)
        {
            mod_phase_ -= 1.f;
        }

        constexpr DspFloat FB = -0.7f;
        constexpr DspFloat FF = 1.f;
        constexpr DspFloat BL = 0.7f;

        DspFloat feedback = delay_.TapOut(base_delay_) * FB;
        DspFloat xn = in + feedback;

        delay_.SetDelay(base_delay_ + mod);

        return delay_.Tick(xn) * FF + BL * xn;
    }

  private:
    uint32_t samplerate_ = 48000;
    DspFloat samples_per_ms_ = samplerate_ / 1000.f;

    DspFloat base_delay_ms_ = 0.f;
    DspFloat base_delay_ = 0.f;
#ifdef CHORUS_LINEAR_DELAY
    LinearDelayline<MAX_DELAY_SIZE> delay_;
#else
    DelaySinc<MAX_DELAY_SIZE> delay_;
#endif
    DspFloat width_ = 50.f;
    DspFloat speed_ = 5.f;

    DspFloat mod_phase_dt_ = 0.f;
    DspFloat mod_phase_ = 0.f;
    DspFloat t_ = 0;
    DspFloat delta_t_ = 0.f;

    DspFloat chorus_mix_ = 0.5f;
    DspFloat input_mix_ = 0.5f;

    DspFloat last_frame_ = 0.f;
    DspFloat feedback_ = 0.0f;

  private:
    DspFloat Sine(DspFloat phase)
    {
        return sinf(TWO_PI * phase);
    }
};
} // namespace dsp