#pragma once

namespace dsp
{
/// @brief Simple bowed string non-linear function taken from the STK.
/// https://github.com/thestk/stk/blob/master/include/BowTable.h
class BowTable
{
  public:
    BowTable() = default;
    ~BowTable() = default;

    /// @brief Sets the force of the bow
    /// @param f Force of the bow
    void SetForce(float f)
    {
        force_ = 5.f - (4.5f * f);
    }

    /// @brief Returns the force of the bow
    /// @return the force of the bow
    float GetForce() const
    {
        return force_;
    }

    /// @brief Table offset value
    /// @param o Offset value
    /// @note From STK source: "The table offset is a bias which controls the
    /// symmetry of the friction. If you want the friction to vary with direction,
    /// use a non-zero value for the offset."
    void SetOffset(float o)
    {
        offset_ = o;
    }

    /// @brief Tick the bow table
    /// @param input The velocity delta of the bow and string
    /// @return The output of the bow table
    float Tick(float input) const;

  private:
    float force_ = 3.f;
    float offset_ = 0.001f;
};
} // namespace dsp