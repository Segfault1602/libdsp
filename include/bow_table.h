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

    void SetForce(float f)
    {
        force_ = 5.f - (4.f * f);
    }

    void SetOffset(float o)
    {
        offset_ = o;
    }

    float Tick(float input) const;

  private:
    float force_ = 3.f;
    float offset_ = 0.001f;
};
} // namespace dsp