#include "bow_table.h"

#include <algorithm>
#include <cmath>

/// @brief Simple bowed string non-linear function taken from the STK.
/// https://github.com/thestk/stk/blob/master/include/BowTable.cpp

using namespace dsp;

float BowTable::Tick(float in) const
{
    float sample = (in + offset_) * force_;

    float out = std::fabs(sample) + 0.75f;
    out = std::pow(out, -4.0f);

    constexpr float min_output = 0.01f;
    constexpr float max_output = 0.98f;
    out = std::clamp(out, min_output, max_output);
    return out;
}