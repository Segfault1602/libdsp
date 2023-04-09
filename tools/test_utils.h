#pragma once

#include <memory>
#include <string>

#include <sndfile.h>

bool LoadWavFile(const std::string& filename, std::unique_ptr<float[]>& buffer, size_t& buffer_size, SF_INFO& sf_info);
bool WriteWavFile(std::string filename, const float* buffer, SF_INFO sf_info, size_t frames);