#pragma once

#include "dsp_utils.h"

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

    /// @brief Sets the delay of the junction, in samples.
    /// @param delay
    void SetDelay(float delay);

    /// @brief Returns the delay of the junction, in samples.
    float GetDelay() const;

    /// @brief Tick the junction. The junction introduces a reflection point between the two delaylines at the specified
    /// delay. The reflection is from the right traveling line to the left traveling line.
    /// @param left_traveling_line
    /// @param right_traveling_line
    void Tick(Delayline& left_traveling_line, Delayline& right_traveling_line) const;

  private:
    float delay_ = 0;
};
} // namespace sfdsp