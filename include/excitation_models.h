#pragma once

namespace dsp
{

class ExcitationModel
{
  public:
    ExcitationModel() = default;
    virtual ~ExcitationModel() = default;

    virtual float Tick(float input) const = 0;
};

/// @brief Simple bowed string non-linear function taken from the STK.
/// https://github.com/thestk/stk/blob/master/include/BowTable.h
class BowTable : public ExcitationModel
{
  public:
    BowTable() = default;
    ~BowTable() = default;

    void SetForce(float f)
    {
        force_ = f;
    }

    void SetOffset(float o)
    {
        offset_ = o;
    }

    float Tick(float input) const override;

  private:
    float force_ = 3.f;
    float offset_ = 0.001;
};
} // namespace dsp