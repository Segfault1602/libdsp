#pragma once

#include "dsp_base.h"

#include "delayline.h"
#include <array>

namespace sfdsp
{

/// @brief Implements a junction point between two delaylines
class Junction
{
  public:
    Junction() = default;
    ~Junction() = default;

    void SetDelay(float delay);
    float GetDelay() const;

    void Tick(Delayline& left_traveling_line, Delayline& right_traveling_line) const;

  private:
    float delay_ = 0;
    // float gain_ = 1.f;
};
} // namespace sfdsp