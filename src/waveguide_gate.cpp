#include "waveguide_gate.h"

namespace sfdsp
{

WaveguideGate::WaveguideGate(bool flip, float delay, float coeff)
    : coeff_(coeff), flip_(flip ? -1.f : 1.f), delay_left_(4), delay_right_(4)
{
    SetDelay(delay);
}

void WaveguideGate::SetDelay(float delay)
{
    delay_ = delay;
    size_t new_delay_int = static_cast<int>(delay);

    // When the delay crosses an integer boundary, it can cause a discontinuity in the waveguide that
    // is particularly audible during glissando and vibrato.
    // If the delay increase or decrease is less than 1 sample, we can smooth out the discontinuity
    // by using values that are already in a the delay lines.
    // For greater changes in delay, it is probably best to use a second gate and crossfade between them.
    if (new_delay_int == (delay_integer_ - 1))
    {
        delay_decreased_ = true;
    }
    else if (new_delay_int == (delay_integer_ + 1))
    {
        delay_increased_ = true;
    }

    delay_integer_ = new_delay_int;
    delay_fractional_ = delay - delay_integer_;
    inv_delay_fractional_ = 1.f - delay_fractional_;

    delay_left_.SetDelay(delay_fractional_ * 2.f);
    delay_right_.SetDelay(inv_delay_fractional_ * 2.f);
}

float WaveguideGate::GetDelay() const
{
    return delay_;
}

void WaveguideGate::SetCoeff(float c)
{
    coeff_ = c;
}

void WaveguideGate::Process(Delayline& left_traveling_line, Delayline& right_traveling_line)
{
    if (delay_ == 0 || delay_ >= left_traveling_line.GetDelay() - 2)
    {
        return;
    }

    // When the delay crosses an integer boundary, it can cause a discontinuity in the waveguide that
    // is particularly audible during glissando and vibrato.
    // If the delay increase or decrease is less than 1 sample, we can smooth out the discontinuity
    // by using values that are already in a the delay lines.
    if (delay_decreased_)
    {
        right_traveling_line[delay_integer_ + 2] = delay_right_[1] * flip_;
        delay_decreased_ = false;
    }
    else if (delay_increased_)
    {
        left_traveling_line[delay_integer_] = delay_left_[1] * flip_;
        delay_increased_ = false;
    }

    // A block diagram and explanation of the algorithm can be found in section 4 of Välimäki's paper.
    // (http://users.spa.aalto.fi/vpv/publications/vesan_vaitos/ch4_pt2_allpass.pdf)

    const size_t kLeftGate = delay_integer_;
    const size_t kGate = delay_integer_ + 1;
    const size_t kRightGate = delay_integer_ + 2;

    float left_input = right_traveling_line[kLeftGate];
    float left_interpolated = delay_left_.Tick(left_input);
    float left_gate_sample = left_traveling_line[kGate];

    left_traveling_line[kGate] = left_gate_sample * (1 - coeff_) + left_interpolated * (coeff_)*flip_;

    float right_input = left_traveling_line[kRightGate];
    float right_interpolated = delay_right_.Tick(right_input);
    float right_gate_sample = right_traveling_line[kGate];
    right_traveling_line[kGate] = right_gate_sample * (1 - coeff_) + right_interpolated * (coeff_)*flip_;
}

void WaveguideGate::Process(Waveguide& wave)
{
    return Process(wave.left_traveling_line_, wave.right_traveling_line_);
}

} // namespace sfdsp