#pragma once

#include "dsp_base.h"

#include "delayline.h"
#include <array>

namespace dsp
{

template <size_t N>
std::array<float, N> Scatter(std::array<float, N> input, float k);

std::array<float, 2> Scatter2(std::array<float, 2> input, float k);

/// @brief Implements a junction point between two delaylines
class Junction
{
  public:
    Junction() = default;
    ~Junction() = default;

    void SetDelay(float delay);
    float GetDelay() const;

    void Tick(Delayline& left_traveling_line, Delayline& right_traveling_line);

  private:
    float delay_ = 0;
    // float gain_ = 1.f;
};

template <size_t N>
std::array<float, N> Scatter(std::array<float, N> input, float k)
{
    float sum = 0.f;
    for (auto& sample : input)
    {
        sum += sample;
    }

    std::array<float, N> output;
    for (size_t i = 0; i < N; ++i)
    {
        output[i] = input[i] - (1.f - k) * sum;
    }

    return output;
}

} // namespace dsp