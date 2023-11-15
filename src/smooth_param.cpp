#include "smooth_param.h"

namespace sfdsp
{
void SmoothParam::Init(size_t samplerate, SmoothingType type, float value)
{
    type_ = type;
    value_ = value;

    constexpr float kDecayDb = -24.f;
    constexpr float kDecayTimeMs = 10.f;
    smoothing_filter_.SetDecayFilter(kDecayDb, kDecayTimeMs, samplerate);
}

void SmoothParam::SetTarget(float target)
{
    value_ = target;
}

float SmoothParam::GetTarget() const
{
    return value_;
}

float SmoothParam::Tick()
{
    switch (type_)
    {
    case SmoothingType::None:
        return value_;
    case SmoothingType::Exponential:
        return smoothing_filter_.Tick(value_);
    }

    assert(false);
    return value_;
}

} // namespace sfdsp