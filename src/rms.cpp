#include "rms.h"

#include <cmath>

namespace sfdsp
{
RMS::RMS(size_t size) : buffer_(size, false, InterpolationType::None), factor_(1.f / size)
{
}

float RMS::Tick(float input)
{
    float input_squared = input * input * factor_;

    running_sum_ += input_squared - buffer_.Tick(input_squared);

    last_out_ = std::sqrt(running_sum_);
    return last_out_;
}

float RMS::GetRMS() const
{
    return last_out_;
}

} // namespace sfdsp