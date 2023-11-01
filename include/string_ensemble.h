#pragma once

#include <array>

#include "bowed_string.h"
#include "dsp_base.h"

namespace dsp
{

constexpr size_t kStringCount = 4;

// Usual violin tuning
constexpr std::array<float, kStringCount> kDefaultFrequencies{196.f, 293.7f, 440.f, 659.3f};

class StringEnsemble
{
  public:
    StringEnsemble() = default;
    ~StringEnsemble() = default;

    void Init(float samplerate, const std::array<float, kStringCount>& frequencies = kDefaultFrequencies);

    void TuneStrings(uint8_t string_num, float frequencies);

    /// @brief Set the frequency of the string
    /// @param string_number The string number to set the frequency of
    /// @param f The frequency of the string in Hz
    void SetFrequency(uint8_t string_number, float f);

    /// @brief Returns the frequency of the string in Hz
    /// @param string_number The string number to get the frequency of
    /// @return The frequency of the string in Hz
    float GetFrequency(uint8_t string_number) const;

    /// @brief Set the velocity of the bow for a given string
    /// @param string_number The string number to set the velocity of
    /// @param v The velocity of the bow for the given string
    void SetVelocity(uint8_t string_number, float v);

    /// @brief Get the velocity of the bow for a given string
    /// @param string_number The string number to get the velocity of
    /// @return The velocity of the bow for the given string
    float GetVelocity(uint8_t string_number) const;

    /// @brief Set the force of the bow for a given string
    /// @param string_number The string number to set the force of
    /// @param f The force of the bow for the given string
    void SetForce(uint8_t string_number, float f);

    /// @brief Get the force of the bow for a given string
    /// @param string_number The string number to get the force of
    /// @return The force of the bow for the given string
    float GetForce(uint8_t string_number) const;

    float Tick();

  private:
    std::array<dsp::BowedString, kStringCount> strings_;

    OnePoleFilter transmission_filter_;
};
} // namespace dsp