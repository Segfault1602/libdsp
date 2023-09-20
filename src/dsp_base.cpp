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

    return (8.17579891564f * expf(.0577622650f * midi_note));
}

float FastMidiToFreq(float midi_note)
{
    if (midi_note < 0.f)
    {
        return 0;
    }
    else if (midi_note >= 127.f)
    {
        return midi_to_freq[127];
    }

    int index = (int)midi_note;
    float frac = midi_note - index;

    return (midi_to_freq[index] + frac * (midi_to_freq[index + 1] - midi_to_freq[index]));
}

float FreqToMidi(float freq)
{
    if (freq <= 0.f)
    {
        return 0;
    }

    return (17.3123405046f * logf(.12231220585f * freq));
}
} // namespace dsp