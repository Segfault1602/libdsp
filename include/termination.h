#pragma once

#include "dsp_utils.h"

#include "delayline.h"
#include "filter.h"

namespace sfdsp
{
/// @brief Base class for termination points between two delaylines
/// @note For now, a termination is simply a gain and a filter and the class mostly exists for readability.
class Termination
{
  public:
    /// @brief Construct a new Termination object.
    /// @param gain Gain of the termination. Defaults the -1.
    Termination(float gain = -1.f);
    virtual ~Termination() = default;

    /// @brief Set the gain of the termination.
    /// @param gain
    virtual void SetGain(float gain);

    /// @brief Set the filter of the termination.
    /// @param filter Filter to set.
    virtual void SetFilter(Filter* filter);

    /// @brief Tick the termination.
    /// @param in Input sample.
    /// @return Output sample.
    /// @note If a filter is set, the input sample will be filtered before being multiplied by the gain.
    virtual float Tick(float in);

  protected:
    /// @brief The gain of the termination.
    float gain_ = -1.f;

    /// @brief Optional filter used to filter signals coming through the termination.
    Filter* filter_ = nullptr;
};
} // namespace sfdsp