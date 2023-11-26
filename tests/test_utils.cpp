#include "test_utils.h"

#include <cstdio>

void PrintWaveguide(sfdsp::Waveguide& wave, size_t delay_size)
{
    std::vector<float> right_samples, left_samples;
    for (size_t i = 1; i <= delay_size; ++i)
    {
        float right, left;
        wave.TapOut(i, right, left);
        right_samples.push_back(right);
        left_samples.push_back(left);
    }

    for (auto sample : right_samples)
    {
        printf("%5.1f ", sample);
    }
    printf("\n");

    for (auto sample : left_samples)
    {
        printf("%5.1f ", sample);
    }
    printf("\n");
}

void PrintDelayline(const sfdsp::Delayline& line)
{
    for (auto i = 0; i < line.GetDelay(); ++i)
    {
        printf("%5.2f ", line[i + 1]);
    }
    printf("\n");
}

float GetWaveguideTotalEnergy(sfdsp::Waveguide& wave, size_t delay_size)
{
    float total_energy = 0.f;
    for (size_t i = 1; i <= delay_size; ++i)
    {
        float right, left;
        wave.TapOut(i, right, left);
        total_energy += right * right + left * left;
    }

    return total_energy;
}

bool LoadWavFile(const std::string& filename, std::unique_ptr<float[]>& buffer, size_t& buffer_size, SF_INFO& sf_info)
{
    SNDFILE* file = sf_open(filename.c_str(), SFM_READ, &sf_info);

    if (file == nullptr)
    {
        printf("Error: %s\n", sf_strerror(file));
        return false;
    }

    buffer = std::make_unique<float[]>(sf_info.frames);

    sf_count_t count = sf_readf_float(file, buffer.get(), sf_info.frames);
    assert(count == sf_info.frames);

    buffer_size = count;

    sf_close(file);
    return true;
}

bool WriteWavFile(std::string filename, const float* buffer, SF_INFO sf_info, size_t frames)
{
    SNDFILE* out_file = sf_open(filename.c_str(), SFM_WRITE, &sf_info);
    if (out_file == nullptr)
    {
        printf("Error: %s\n", sf_strerror(out_file));
        return false;
    }

    sf_writef_float(out_file, buffer, static_cast<sf_count_t>(frames));
    sf_write_sync(out_file); // Is this needed?
    sf_close(out_file);
    return true;
}