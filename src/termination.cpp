#include "termination.h"

namespace sfdsp
{
Termination::Termination(float gain) : gain_(gain)
{
}

void Termination::SetGain(float gain)
{
    gain_ = gain;
}

void Termination::SetFilter(Filter* filter)
{
    filter_ = filter;
}

float Termination::Tick(float in)
{
    if (filter_)
    {
        in = filter_->Tick(in);
    }

    return in * gain_;
}
} // namespace sfdsp