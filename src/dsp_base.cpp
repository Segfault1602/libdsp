#include "dsp_base.h"

#include <cmath>

namespace dsp
{

float MidiToFreq(float midi_note)
{
    if (midi_note < 0.f)
    {
        return 0;
    }
    else if (midi_note > 127.f)
    {
        return 127.f;
    }

    return (8.17579891564f * exp(.0577622650f * midi_note));
}

float FreqToMidi(float freq)
{
    if (freq <= 0.f)
    {
        return 0;
    }

    return (17.3123405046f * log(.12231220585f * freq));
}
} // namespace dsp