#include "chorus.h"

#include "basic_oscillators.h"

namespace dsp
{
Chorus::Chorus(size_t max_delay_size) : delay_(max_delay_size)
{
}

void Chorus::Init(uint32_t samplerate, float delay_ms, float width, float speed)
{
    samplerate_ = samplerate;
    SetDelay(delay_ms);
    delay_.SetDelay(base_delay_);
    width_ = width;
    speed_ = speed;

    float mod_period = static_cast<float>(samplerate) / speed;
    mod_phase_dt_ = 1.f / mod_period;
    delta_t_ = 1.f / static_cast<float>(samplerate);
}

void Chorus::Reset()
{
    delay_.Reset();
}

void Chorus::SetDelay(float delay_ms)
{
    // TODO: add max delay check here
    if (base_delay_ms_ != delay_ms)
    {
        base_delay_ = delay_ms * samples_per_ms_;
        base_delay_ms_ = delay_ms;
    }
}

void Chorus::SetMix(float mix)
{
    chorus_mix_ = mix;
    input_mix_ = 1.f - mix;
}

void Chorus::SetWidth(float width)
{
    width_ = width;
}

void Chorus::SetSpeed(float speed)
{
    if (speed_ != speed)
    {
        speed_ = speed;
        float mod_period = static_cast<float>(samplerate_) / speed;
        mod_phase_dt_ = 1.f / mod_period;
    }
}

float Chorus::Tick(float in)
{
    // Simple chorus structure based on J. Dattoro, Effect Design Part 2: Delay-line modulation and chorus,
    // as presented in DAFX Second Edition, p. 77-78, figure 2.34
    float mod = Sine(mod_phase_) * width_;
    mod_phase_ += mod_phase_dt_;
    if (mod_phase_ >= 1.f)
    {
        mod_phase_ -= 1.f;
    }

    // Value taken from DAFX Second editions, Table 2.9, p. 77
    // Ideally, these values should be settable by the user.
    constexpr float FB = -0.7f;
    constexpr float FF = 1.f;
    constexpr float BL = 0.7f;

    float feedback = delay_.TapOut(base_delay_) * FB;
    float xn = in + feedback;

    delay_.SetDelay(base_delay_ + mod);

    return delay_.Tick(xn) * FF + BL * xn;
}
} // namespace dsp