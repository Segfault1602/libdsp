#include "waveguide_tests.h"

#include <basic_oscillators.h>

WaveguideTest::WaveguideTest() : waveguide_(21), gate_(false, 0.f, 1.f)
{
}

void WaveguideTest::Init(size_t samplerate)
{
    samplerate_ = samplerate;
    waveguide_.SetDelay(20);

    gate_pos_ = 10.f;
    gate_.SetDelay(gate_pos_);
    gate_dt_ = 0.001f;

    constexpr float freq = 220.f;
    phase_dt_ = freq / static_cast<float>(samplerate);

    name_ = "waveguidetest.wav";
}

float WaveguideTest::Tick()
{
    float right;
    float left;
    waveguide_.NextOut(right, left);

    float x = sfdsp::Sine(phase_);
    phase_ += phase_dt_;

    waveguide_.Tick(x, right * -1);

    if (gate_pos_ < 17.f)
    {
        gate_pos_ += gate_dt_;
        gate_.SetDelay(gate_pos_);
    }

    return left;
}

float WaveguideTest::Tick(float input)
{
    return Tick();
}