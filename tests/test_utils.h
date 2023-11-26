#pragma once

#include <string>
#include <vector>

#include <sndfile.h>

#include "waveguide.h"

void PrintWaveguide(sfdsp::Waveguide& wave, size_t delay_size);

void PrintDelayline(const sfdsp::Delayline& line);

float GetWaveguideTotalEnergy(sfdsp::Waveguide& wave, size_t delay_size);

bool LoadWavFile(const std::string& filename, std::unique_ptr<float[]>& buffer, size_t& buffer_size, SF_INFO& sf_info);

bool WriteWavFile(std::string filename, const float* buffer, SF_INFO sf_info, size_t frames);
