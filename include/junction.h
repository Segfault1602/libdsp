#pragma once

#include "dsp_base.h"

#include "delayline.h"
#include <cstdio>

namespace dsp
{
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
} // namespace dsp