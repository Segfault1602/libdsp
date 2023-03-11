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
    Chorus() = default;
    ~Chorus() = default;
    Chorus(const Chorus &c) = delete;

    void Reset()
    {
        delay_.Reset();
    }

  private:
    DelayLineSinc<MAX_DELAY_SIZE> delay_;
};
} // namespace dsp