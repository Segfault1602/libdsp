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

    return (8.17579891564 * exp(.0577622650 * midi_note));
}

float FreqToMidi(float freq)
{
    if (freq <= 0.f)
    {
        return 0;
    }

    return (17.3123405046 * log(.12231220585 * freq));
}
} // namespace dsp