/// @file
/// Implementation of basic oscillators
/// @defgroup Oscillators
#pragma once

namespace sfdsp
{
/// @brief Simple sine wave
/// @param phase Phase of the sine wave
/// @return The value of the sine wave at the given phase
/// @ingroup Oscillators
float Sine(float phase);

/// @brief Simple triangle wave
/// @param phase Phase of the triangle wave
/// @return The value of the triangle wave at the given phase
/// @ingroup Oscillators
float Tri(float phase);

/// @brief Simple saw wave
/// @param phase Phase of the saw wave
/// @return The value of the saw wave at the given phase
/// @ingroup Oscillators
float Saw(float phase);

/// @brief Simple square wave
/// @param phase Phase of the square wave
/// @return The value of the square wave at the given phase
/// @ingroup Oscillators
float Square(float phase);

/// @brief Simple noise generator
/// @return A random value between -1 and 1
/// @ingroup Oscillators
float Noise();
} // namespace sfdsp