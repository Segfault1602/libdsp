#include "interpolation_strategy.h"

#include <cstdint>

namespace dsp
{

float NoInterpolation::TapOut(float* buffer, size_t max_size, size_t write_ptr, float delay)
{
    return buffer[(write_ptr + static_cast<size_t>(delay)) % max_size];
}

void NoInterpolation::TapIn(float* buffer, size_t max_size, size_t write_ptr, float delay, float input)
{
    buffer[(write_ptr + static_cast<size_t>(delay)) % max_size] += input;
}

float LinearInterpolation::TapOut(float* buffer, size_t max_size, size_t write_ptr, float delay)
{
    uint32_t delay_integer = static_cast<uint32_t>(delay);
    float frac = delay - static_cast<float>(delay_integer);

    float a = buffer[(write_ptr + delay_integer) % max_size];
    float b = buffer[(write_ptr + delay_integer + 1) % max_size];

    return a + (b - a) * frac;
}

void LinearInterpolation::TapIn(float* buffer, size_t max_size, size_t write_ptr, float delay, float input)
{
    uint32_t delay_integer = static_cast<uint32_t>(delay);
    float frac = delay - static_cast<float>(delay_integer);

    buffer[(write_ptr + delay_integer) % max_size] += input * (1.f - frac);
    buffer[(write_ptr + delay_integer + 1) % max_size] += input * frac;
}

float AllpassInterpolation::TapOut(float* buffer, size_t max_size, size_t write_ptr, float delay)
{
    CalculateCoeff(delay);
    size_t read_ptr = (write_ptr + allpass_delay_) % max_size;

    float out = -coeff_ * last_out_;
    out += allpass_input_ + (coeff_ * buffer[read_ptr]);
    allpass_input_ = buffer[read_ptr];
    last_out_ = out;

    return out;
}

void AllpassInterpolation::TapIn(float* buffer, size_t max_size, size_t write_ptr, float delay, float input)
{
    uint32_t delay_integer = static_cast<uint32_t>(delay);
    float frac = delay - static_cast<float>(delay_integer);

    buffer[(write_ptr + delay_integer) % max_size] += input * (1.f - frac);
    buffer[(write_ptr + delay_integer + 1) % max_size] += input * frac;
}

void AllpassInterpolation::CalculateCoeff(float delay)
{
    if (delay != delay_)
    {
        delay_ = delay;
        allpass_delay_ = static_cast<size_t>(delay);
        float alpha = delay - static_cast<uint32_t>(delay);

        if (alpha < 0.5f)
        {
            alpha += 1.f;
            allpass_delay_ -= 1;
        }

        coeff_ = (1.f - alpha) / (1.f + alpha);
    }
}

} // namespace dsp