#include "test_utils.h"

void PrintWaveguide(dsp::Waveguide& wave, size_t delay_size)
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

float GetWaveguideTotalEnergy(dsp::Waveguide& wave, size_t delay_size)
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