#include "filter.h"

namespace dsp
{
void Filter::SetGain(float gain)
{
    gain_ = gain;
}

void Filter::SetA(const float (&a)[COEFFICIENT_COUNT])
{
    for (size_t i = 0; i < COEFFICIENT_COUNT; ++i)
    {
        a_[i] = a[i];
    }
}

void Filter::SetB(const float (&b)[COEFFICIENT_COUNT])
{
    for (size_t i = 0; i < COEFFICIENT_COUNT; ++i)
    {
        b_[i] = b[i];
    }
}

void OnePoleFilter::SetPole(float pole)
{
    // https://ccrma.stanford.edu/~jos/fp/One_Pole.html
    // If the filter has a pole at z = -a, then a_[1] will be -pole;
    assert(pole < 1.f && pole > -1.f);

    // Set the b value to 1 - |a| to get a peak gain of 1.
    b_[0] = 1.f - std::abs(pole);
    a_[1] = -pole;
}

float OnePoleFilter::Tick(float in)
{
    outputs_[0] = gain_ * in * b_[0] - outputs_[1] * a_[1];
    outputs_[1] = outputs_[0];
    return outputs_[0];
}

float OneZeroFilter::Tick(float in)
{
    float out = gain_ * in * b_[0] + inputs_[0] * b_[1];
    inputs_[0] = in;
    return out;
}

float TwoPoleFilter::Tick(float in)
{
    outputs_[0] = gain_ * in * b_[0] - outputs_[1] * a_[1] - outputs_[2] * a_[2];
    outputs_[2] = outputs_[1];
    outputs_[1] = outputs_[0];
    return outputs_[0];
}

float TwoZeroFilter::Tick(float in)
{
    float out = gain_ * in * b_[0] + inputs_[0] * b_[1] + inputs_[1] * b_[2];
    inputs_[1] = inputs_[0];
    inputs_[0] = in;
    return out;
}

void Biquad::SetCoefficients(float b0, float b1, float b2, float a1, float a2)
{
    b_[0] = b0;
    b_[1] = b1;
    b_[2] = b2;
    a_[1] = a1;
    a_[2] = a2;
}

float Biquad::Tick(float in)
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
} // namespace dsp