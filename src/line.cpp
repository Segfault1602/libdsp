#include "line.h"

namespace sfdsp
{
Line::Line(float start, float end, uint32_t time_samples) : start_(start), end_(end), output_(start)
{
    if (time_samples <= 0)
    {
        time_samples = 1;
    }
    increment_ = (end_ - start_) / static_cast<float>(time_samples);
}

float Line::Tick()
{
    if (increment_ < 0 && output_ <= end_)
    {
        return end_;
    }
    else if (increment_ > 0 && output_ >= end_)
    {
        return end_;
    }

    output_ += increment_;
    return output_;
}

} // namespace sfdsp