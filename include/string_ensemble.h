#pragma once

#include <array>

#include "bowed_string.h"
#include "dsp_base.h"

namespace sfdsp
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

    /// @brief Lift the finger off the string. Pitch of the string goes back to the open string pitch.
    /// @param string_number
    void FingerOff(uint8_t string_number);

    /// @brief Set the amount of energy transmitted from one string to the others
    /// @param t
    void SetBridgeTransmission(float t);

    float GetBridgeTransmission() const;

    float Tick();

    void ProcessBlock(float* out, size_t size);

    const BowedString& operator[](uint8_t string_number) const;

    BowedString& operator[](uint8_t string_number);

  private:
    std::array<BowedString, kStringCount> strings_;
    std::array<float, kStringCount> openTuning_;
    float bridgeTransmission_ = 0.1f;

    OnePoleFilter transmission_filter_;
    Biquad body_filters_[6];
};
} // namespace sfdsp