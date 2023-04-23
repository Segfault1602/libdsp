// =============================================================================
// dsp_base.h
// =============================================================================
#pragma once

#define PI_F 3.14159265358979323846
#define TWO_PI (2.0f * PI_F)

namespace dsp
{
class DspBase
{
  public:
    DspBase()
    {
    }
    virtual ~DspBase()
    {
    }

    virtual float Tick(float in) = 0;
};
} // namespace dsp