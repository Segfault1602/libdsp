#pragma once

#include "dsp_utils.h"

#include "delayline.h"
#include "waveguide.h"

namespace sfdsp
{

/// @brief Implements a "reflection" point in a waveguide.
/// @details This class is used to implement a "reflection" point in a waveguide, for example, a finger on a string.
/// While it can look like a scattering junction, the math is slightly different. The implementation is based on the
/// procedure described in "Discrete-Time Modeling of Acoustic Tubes Using Fractional Delay Filters" by Vesa Välimäki
class WaveguideGate
{
  public:
    /// @brief Construct a WaveguideGate object
    /// @param flip Whether or not the gate performs a 180° phase flip.
    /// @param delay The delay of the gate, in samples, in relation to the waveguide.
    /// @param coeff The reflection coefficient of the gate. 0 = no reflection, 1 = full reflection.
    WaveguideGate(bool flip, float delay, float coeff);
    ~WaveguideGate() = default;

    /// @brief Set the delay of the gate, in samples, in relation to the waveguide.
    /// @param delay The delay of the gate, in samples, in relation to the waveguide.
    void SetDelay(float delay);

    /// @brief Returns the delay of the gate, in samples, in relation to the waveguide.
    /// @return The delay of the gate, in samples, in relation to the waveguide.
    float GetDelay() const;

    /// @brief Sets the reflection coefficient of the gate.
    /// @param c The reflection coefficient of the gate. 0 = no reflection, 1 = full reflection.
    void SetCoeff(float c);

    /// @brief Process the gate. The gate introduces a reflection point between the two delaylines at the specified
    /// delay.
    /// @param left_traveling_line  The left traveling waveguide.
    /// @param right_traveling_line The right traveling waveguide.
    void Process(Delayline& left_traveling_line, Delayline& right_traveling_line);

    /// @brief Process the gate. The gate introduces a reflection point in the waveguide at the specified delay.
    /// @param wave The waveguide to process.
    void Process(Waveguide& wave);

  private:
    float coeff_ = 0.f;
    float delay_ = 0.f;
    size_t delay_integer_ = 0;
    float delay_fractional_ = 0.f;
    float inv_delay_fractional_ = 0.f;
    float flip_ = 1.f;
    Delayline delay_left_;
    Delayline delay_right_;

    bool delay_decreased_ = false;
    bool delay_increased_ = false;
};
} // namespace sfdsp