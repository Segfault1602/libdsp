#pragma once

#include <cstddef>

#include "delayline.h"
#include "dsp_base.h"
#include "junction.h"

namespace dsp
{

class Waveguide
{
  public:
    Waveguide(size_t max_size, InterpolationType interpolation_type = InterpolationType::Linear);
    ~Waveguide() = default;

    void SetDelay(float delay);

    float GetDelay() const;

    void SetJunction(float pos);

    void NextOut(float& right, float& left);

    void Tick(float right, float left);

    void TapIn(float delay, float input);
    void TapIn(float delay, float right, float left);

    float TapOut(float delay);

    void TapOut(float delay, float& right_out, float& left_out);

  private:
    const size_t max_size_ = 0;
    float current_delay_ = 0.f;
    Delayline right_traveling_line_;
    Delayline left_traveling_line_;

    Junction junction_;
};

} // namespace dsp