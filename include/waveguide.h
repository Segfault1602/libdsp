#pragma once

#include <cstddef>

#include "delayline.h"
#include "dsp_base.h"
#include "interpolation_strategy.h"
#include "junction.h"

namespace sfdsp
{

/// @brief Simple waveguide model composed of a right traveling wave and a left traveling wave.
class Waveguide
{
  public:
    /// @brief Construct a new Waveguide object.
    /// @param max_size Maximum size of the delaylines.
    /// @param interpolation_type Interpolation type to use for the delaylines.
    Waveguide(size_t max_size, InterpolationType interpolation_type = InterpolationType::Linear);
    ~Waveguide() = default;

    /// @brief Set the delay of the waveguide.
    /// @param delay Delay in samples.
    void SetDelay(float delay);

    /// @brief Returns the delay of the waveguide.
    /// @return
    float GetDelay() const;

    /// @brief Returns the output sample of the right traveling wave and left traveling wave.
    /// @param right Output sample of the right traveling wave.
    /// @param left Output sample of the left traveling wave.
    void NextOut(float& right, float& left);

    /// @brief Tick the waveguide.
    /// @param right The sample going into the right traveling wave.
    /// @param left The sample going into the left traveling wave.
    void Tick(float right, float left);

    /// @brief Add energy to the waveguide at a given delay.
    /// @param delay The delay in samples.
    /// @param input The input sample.
    void TapIn(float delay, float input);

    /// @brief Add energy to the waveguide at a given delay.
    /// @param delay The delay in samples.
    /// @param right The input sample for the right traveling wave.
    /// @param left The input sample for the left traveling wave.
    void TapIn(float delay, float right, float left);

    /// @brief Return the sum of the right and left traveling wave at a given delay.
    /// @param delay Delay in samples.
    /// @return The sum of the right and left traveling wave
    float TapOut(float delay);

    /// @brief Return the sum of the right and left traveling wave at a given delay using a given interpolation
    /// strategy.
    /// @param delay Delay in samples.
    /// @param interpolation_strategy Interpolation strategy to use.
    /// @return The sum of the right and left traveling wave
    float TapOut(float delay, InterpolationStrategy* interpolation_strategy);

    /// @brief Return the sample of the right and left traveling wave at a given delay.
    /// @param delay The delay in samples.
    /// @param right_out The output sample of the right traveling wave.
    /// @param left_out The output sample of the left traveling wave.
    void TapOut(float delay, float& right_out, float& left_out);

    /// @brief Return the sample of the right and left traveling wave at a given delay using a given interpolation
    /// @param delay Delay in samples.
    /// @param right_out The output sample of the right traveling wave.
    /// @param left_out The output sample of the left traveling wave.
    /// @param interpolation_strategy
    void TapOut(float delay, float& right_out, float& left_out, InterpolationStrategy* interpolation_strategy);

    /// @brief Subscript operator to access the delaylines.
    /// @param index 0 is the right traveling wave, 1 is the left traveling wave.
    /// @return
    const Delayline& operator[](size_t index) const;
    Delayline& operator[](size_t index);

  private:
    const size_t max_size_ = 0;
    float current_delay_ = 0.f;
    Delayline right_traveling_line_;
    Delayline left_traveling_line_;

    friend class WaveguideGate;
};

} // namespace sfdsp