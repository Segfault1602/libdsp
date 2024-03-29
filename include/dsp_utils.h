#pragma once

#include <cmath>

/// @mainpage LibDSP
/// C++ audio dsp library with no external dependencies.
/// @section build Building
/// The library use CMake and can probably be built in multiple of ways, but here's how I build it:
/// ```
/// > cmake -GNinja -Bbuild
/// > cmake --build ./build
/// ```
/// A toolchain file for cross compiling libDSP for the Daisy platform. This assumes that you
/// already have the [Daisy Toolchain
/// installed](https://github.com/electro-smith/DaisyWiki/wiki/1.-Setting-Up-Your-Development-Environment).
/// ```
/// > cmake -GNinja -Bdaisy -DCMAKE_TOOLCHAIN_FILE=cmake/daisy.cmake
/// > cmake --build ./daisy
/// ```
/// @section note_waveguide A note on waveguides
/// The waveguide nomenclature used in this library is based on the common depiction of a waveguide
/// in the litterature with a delay line on top containing the right-going wave and a delay line on
/// the bottom containing the left-going wave. This nomenclature does not affect in any way the
/// implementation of the waveguide, it's just a way to make documentation easier to understand.

#ifndef PI_F
#define PI_F 3.14159265358979323846f
#endif

#define TWO_PI (2.0f * PI_F)

namespace sfdsp
{

inline float fast_mod(float x, float m)
{
    return x - m * std::floor(x / m);
}

inline float fast_mod1(float x)
{
    return x - std::floor(x);
}

/// @brief Convert midi note to frequency.
/// @param midi_note The midi note number. Valid range is 0 to 127.
/// @return The frequency in Hz.
float MidiToFreq(float midi_note);

/// @brief Convert midi note to frequency using a lookup table and linear interpolation.
/// @param midi_note The midi note number.
/// @return The frequency in Hz.
float FastMidiToFreq(float midi_note);

/// @brief Convert frequency to midi note.
/// @param freq The frequency in Hz.
/// @return The midi note number.
/// @note This function can return a value outside the valid midi note range.
float FreqToMidi(float freq);

// Midi note to frequency table precomputed with f(n) = 440 * 2 ^ ((n - 69) / 12)
// Can be used to convert exact midi notes to frequencies efficiently.
constexpr float midi_to_freq[128] = {
    8.17579891564f, 8.66195721803f, 9.17702399742f, 9.72271824132f, 10.3008611535f, 10.9133822323f, 11.5623257097f,
    12.2498573744f, 12.9782717994f, 13.75f,         14.5676175474f, 15.4338531643f, 16.3515978313f, 17.3239144361f,
    18.3540479948f, 19.4454364826f, 20.6017223071f, 21.8267644646f, 23.1246514195f, 24.4997147489f, 25.9565435987f,
    27.5f,          29.1352350949f, 30.8677063285f, 32.7031956626f, 34.6478288721f, 36.7080959897f, 38.8908729653f,
    41.2034446141f, 43.6535289291f, 46.249302839f,  48.9994294977f, 51.9130871975f, 55.0f,          58.2704701898f,
    61.735412657f,  65.4063913251f, 69.2956577442f, 73.4161919794f, 77.7817459305f, 82.4068892282f, 87.3070578583f,
    92.4986056779f, 97.9988589954f, 103.826174395f, 110.0f,         116.54094038f,  123.470825314f, 130.81278265f,
    138.591315488f, 146.832383959f, 155.563491861f, 164.813778456f, 174.614115717f, 184.997211356f, 195.997717991f,
    207.65234879f,  220.0f,         233.081880759f, 246.941650628f, 261.625565301f, 277.182630977f, 293.664767917f,
    311.126983722f, 329.627556913f, 349.228231433f, 369.994422712f, 391.995435982f, 415.30469758f,  440.0f,
    466.163761518f, 493.883301256f, 523.251130601f, 554.365261954f, 587.329535835f, 622.253967444f, 659.255113826f,
    698.456462866f, 739.988845423f, 783.990871963f, 830.60939516f,  880.0f,         932.327523036f, 987.766602512f,
    1046.5022612f,  1108.73052391f, 1174.65907167f, 1244.50793489f, 1318.51022765f, 1396.91292573f, 1479.97769085f,
    1567.98174393f, 1661.21879032f, 1760.0f,        1864.65504607f, 1975.53320502f, 2093.0045224f,  2217.46104781f,
    2349.31814334f, 2489.01586978f, 2637.0204553f,  2793.82585146f, 2959.95538169f, 3135.96348785f, 3322.43758064f,
    3520.0f,        3729.31009214f, 3951.06641005f, 4186.00904481f, 4434.92209563f, 4698.63628668f, 4978.03173955f,
    5274.04091061f, 5587.65170293f, 5919.91076339f, 6271.92697571f, 6644.87516128f, 7040.0f,        7458.62018429f,
    7902.1328201f,  8372.01808962f, 8869.84419126f, 9397.27257336f, 9956.06347911f, 10548.0818212f, 11175.3034059f,
    11839.8215268f,
};
} // namespace sfdsp