#include "interpolation_strategy.h"

#include <cstdint>

namespace dsp
{
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
} // namespace dsp