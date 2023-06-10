#pragma once

#include "dsp_base.h"

#include "delayline.h"

namespace dsp
{
class Termination
{
  public:
    Termination() = default;
    virtual ~Termination() = default;

    virtual void Tick(Delayline& left_traveling_line, Delayline& right_traveling_line) = 0;
};

class LeftTermination : public Termination
{
public:
    LeftTermination() = default;
    virtual ~LeftTermination() override = default;

    virtual void Tick(Delayline& left_traveling_line, Delayline& right_traveling_line) override
    {
        DspFloat next_out = left_traveling_line.NextOut();
        (void)right_traveling_line.Tick(next_out * -1.f);
    }

};

class RightTermination : public Termination
{
public:
    RightTermination(DspFloat gain = -0.99f): gain_(gain) {};
    virtual ~RightTermination() override = default;

    virtual void Tick(Delayline& left_traveling_line, Delayline& right_traveling_line) override
    {
        DspFloat next_out = right_traveling_line.LastOut();
        (void)left_traveling_line.Tick(next_out * gain_);
    }

private:
    DspFloat gain_;

};
} // namespace dsp