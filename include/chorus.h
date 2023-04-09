// =============================================================================
// chorus.h
// Simple chorus effect.
// =============================================================================
#pragma once

#include "delayline_sinc.h"
#include "dsp_base.h"
#include <cmath>

namespace dsp
{
template <size_t MAX_DELAY_SIZE> class Chorus : public DspBase
{
  public:
    Chorus(uint32_t samplerate, float delay, float width = 10.f, float speed = 5.f)
        : samplerate_(samplerate), base_delay_(delay), delay_(delay), width_(width), speed_(speed)
    {
        float mod_period = samplerate / speed;
        mod_phase_dt_ = 1.f / mod_period;
        delta_t_ = 1.f / samplerate_;
    }

    ~Chorus() = default;
    Chorus(const Chorus &c) = delete;

    void Reset()
    {
        delay_.Reset();
    }

    void SetDelay(float delay)
    {
        delay_.SetDelay(delay);
    }

    float Tick(float in) override
    {

        // float mod = Sine(mod_phase_) * width_;
        // mod_phase_ += mod_phase_dt_;
        // if (mod_phase_ > 1.f)
        // {
        //     mod_phase_ = 0.f;
        // }

        float mod = width_ * std::sin(2 * M_PI * speed_ * t_);
        t_ += delta_t_;

        SetDelay(base_delay_ + mod);

        float last_frame_ = delay_.Tick(in * (1.f - feedback_) + last_frame_ * feedback_);

        return  last_frame_ * chorus_mix_ + in * input_mix_;
    }

  private:
    uint32_t samplerate_ = 48000;
    float base_delay_ = 0.f;
    DelaySinc<MAX_DELAY_SIZE> delay_ = {0};
    float width_ = 50.f;
    float speed_ = 5.f;

    float mod_phase_dt_ = 0.f;
    float mod_phase_ = 0.f;
    float t_ = 0;
    float delta_t_ = 0.f;

    float chorus_mix_ = 0.5f;
    float input_mix_ = 0.5f;

    float last_frame_ = 0.f;
    float feedback_ = 0.5f;

  private:
    float Sine(float phase)
    {
        return sinf(TWO_PI * phase);
    }
};
} // namespace dsp