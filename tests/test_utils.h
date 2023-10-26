#pragma once

#include <vector>

#include "waveguide.h"

void PrintWaveguide(dsp::Waveguide& wave, size_t delay_size);

float GetWaveguideTotalEnergy(dsp::Waveguide& wave, size_t delay_size);