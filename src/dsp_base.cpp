#include "dsp_utils.h"

#include <algorithm>
#include <cmath>

namespace sfdsp
{

float MidiToFreq(float midi_note)
{
    midi_note = std::clamp(midi_note, 0.f, 127.f);
    return (8.17579891564f * expf(.0577622650f * midi_note));
}

float FastMidiToFreq(float midi_note)
{
    midi_note = std::clamp(midi_note, 0.f, 127.f);

    if (midi_note == 127.f)
    {
        return midi_to_freq[static_cast<size_t>(midi_note)];
    }

    int index = (int)midi_note;
    float frac = midi_note - static_cast<float>(index);

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
} // namespace sfdsp