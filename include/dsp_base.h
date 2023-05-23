// =============================================================================
// dsp_base.h
// =============================================================================
#pragma once

#define PI_F 3.14159265358979323846
#define TWO_PI (2.0f * PI_F)

// #define DSP_USE_DOUBLE
#ifdef DSP_USE_DOUBLE
#define DspFloat  double
#else
#define DspFloat  float
#endif

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

    virtual DspFloat Tick(DspFloat in) = 0;
};
} // namespace dsp