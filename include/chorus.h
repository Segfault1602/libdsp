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

    void Init(uint32_t samplerate, float delay_ms, float width = 100, float speed = .5f)
    {
        samplerate_ = samplerate;
        SetDelay(delay_ms);
        delay_.SetDelay(base_delay_, /*resetFilterScale=*/true);
        width_ = width;
        speed_ = speed;

        float mod_period = samplerate / speed;
        mod_phase_dt_ = 1.f / mod_period;
        delta_t_ = 1.f / samplerate_;
    }

    void Reset()
    {
        delay_.Reset();
    }

    void SetDelay(float delay_ms)
    {
        // TODO: add max delay check here
        if (base_delay_ms_ != delay_ms)
        {
            base_delay_ = delay_ms * samples_per_ms_;
            base_delay_ms_ = delay_ms;
        }
    }

    void SetMix(float mix)
    {
        chorus_mix_ = mix;
        input_mix_ = 1.f - mix;
    }

    void SetWidth(float width)
    {
        width_ = width;
    }

    void SetSpeed(float speed)
    {
        if (speed_ != speed)
        {
            speed_ = speed;
            float mod_period = samplerate_ / speed;
            mod_phase_dt_ = 1.f / mod_period;
        }
    }

    float Tick(float in) override
    {

        float mod = Sine(mod_phase_) * width_;
        mod_phase_ += mod_phase_dt_;
        if (mod_phase_ >= 1.f)
        {
            mod_phase_ -= 1.f;
        }

        constexpr float FB = -0.7f;
        constexpr float FF = 1.f;
        constexpr float BL = 0.7f;

        float feedback = delay_.TapOut(base_delay_) * FB;
        float xn = in + feedback;

        delay_.SetDelay(base_delay_ + mod, false);

        return delay_.Tick(xn) * FF + BL * xn;
    }

  private:
    uint32_t samplerate_ = 48000;
    float samples_per_ms_ = samplerate_ / 1000.f;

    float base_delay_ms_ = 0.f;
    float base_delay_ = 0.f;
#ifdef CHORUS_LINEAR_DELAY
    DelayL<MAX_DELAY_SIZE> delay_ = {0};
#else
    DelaySinc<MAX_DELAY_SIZE> delay_ = {0};
#endif
    float width_ = 50.f;
    float speed_ = 5.f;

    float mod_phase_dt_ = 0.f;
    float mod_phase_ = 0.f;
    float t_ = 0;
    float delta_t_ = 0.f;

    float chorus_mix_ = 0.5f;
    float input_mix_ = 0.5f;

    float last_frame_ = 0.f;
    float feedback_ = 0.0f;

  private:
    float Sine(float phase)
    {
        return sinf(TWO_PI * phase);
    }
};
} // namespace dsp