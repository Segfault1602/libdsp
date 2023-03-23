// =============================================================================
// chorus.h
// Simple chorus effect.
// =============================================================================

#include "delayline_sinc.h"

namespace dsp
{
template <size_t MAX_DELAY_SIZE> class Chorus
{
  public:
    Chorus(float delay, float width = 50.f, float speed = 5.f) : delay_(delay)
    {
    }

    ~Chorus() = default;
    Chorus(const Chorus &c) = delete;

    void Reset()
    {
        delay_.Reset();
    }

    void SetDelay(float delay)
    {
        delay_.SetDelay(delay);
    }

    float Tick(float in)
    {
        return delay_.Tick(in);
    }

  private:
    DelaySinc<MAX_DELAY_SIZE> delay_;
    float width_ = 50.f;
    float speed_ = 5.f;
};
} // namespace dsp