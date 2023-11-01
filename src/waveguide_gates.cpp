#include "waveguide_gates.h"

namespace dsp
{

WaveguideGates::WaveguideGates() : delay_left_(8), delay_right_(8)
{
}

void WaveguideGates::SetDelay(float delay)
{
    delay_ = delay;
    delay_integer_ = static_cast<int>(delay);
    delay_fractional_ = delay - delay_integer_;
    inv_delay_fractional_ = 1.f - delay_fractional_;

    delay_left_.SetDelay(delay_fractional_ * 2.f);
    delay_right_.SetDelay(inv_delay_fractional_ * 2.f);
}

float WaveguideGates::GetDelay() const
{
    return delay_;
}

void WaveguideGates::SetCoeff(float c)
{
    coeff_ = c;
}

void WaveguideGates::Process(Delayline& left_traveling_line, Delayline& right_traveling_line)
{
    const float kLeftGate = delay_integer_ - 1;
    const float kRightGate = delay_integer_ + 1;

    float left_input = right_traveling_line.TapOut(kLeftGate);
    float left_interpolated = delay_left_.Tick(left_input);
    float left_gate_sample = left_traveling_line.TapOut(delay_integer_);
    float new_sample = left_gate_sample * (1 - coeff_) + left_interpolated * (coeff_);
    left_traveling_line.SetIn(delay_integer_, left_interpolated);

    float right_input = left_traveling_line.TapOut(kRightGate);
    float right_interpolated = delay_right_.Tick(right_input);
    float right_gate_sample = right_traveling_line.TapOut(delay_integer_);
    new_sample += right_gate_sample * (1 - coeff_) + right_interpolated * (coeff_);
    right_traveling_line.SetIn(delay_integer_, right_interpolated);
}

} // namespace dsp