#pragma once

#include <vector>

#include "waveguide.h"

void PrintWaveguide(sfdsp::Waveguide& wave, size_t delay_size);

void PrintDelayline(const sfdsp::Delayline& line);

float GetWaveguideTotalEnergy(sfdsp::Waveguide& wave, size_t delay_size);