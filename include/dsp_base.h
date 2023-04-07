// =============================================================================
// dsp_base.h
// =============================================================================
#pragma once

#define TWO_PI (2.0f * M_PI)

namespace dsp
{
    class DspBase
    {
public:
    DspBase() {}
    virtual ~DspBase() {}

    virtual float Tick(float in) = 0;
    };
}