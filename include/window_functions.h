#pragma once

#include <cmath>

#include "dsp_base.h"

namespace dsp
{
/// @brief Returns the value of a Hann window of size L at index x.
/// @param i Index of the Hann window.
/// @param size Size of the Hann window.
/// @return float
float Hann(float x, float L)
{
    return 0.5f * (1.f - std::cos((TWO_PI * x) / L));
}
} // namespace dsp