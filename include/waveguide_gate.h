#pragma once

#include "dsp_base.h"

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
    WaveguideGate(bool flip, float delay, float coeff);
    ~WaveguideGate() = default;

    void SetDelay(float delay);
    float GetDelay() const;

    void SetCoeff(float c);

    void Process(Delayline& left_traveling_line, Delayline& right_traveling_line);

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