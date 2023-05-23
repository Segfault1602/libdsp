#pragma once

#include "dsp_base.h"

class Filter
{
public:
    Filter() = default;
    ~Filter() = default;

    virtual DspFloat Tick(DspFloat in) = 0;
};