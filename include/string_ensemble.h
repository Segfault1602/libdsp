#pragma once

#include <array>

#include "bowed_string.h"
#include "dsp_utils.h"

namespace sfdsp
{

/// @brief The number of strings in the ensemble.
constexpr size_t kStringCount = 4;

/// @brief The classic violin tuning.
constexpr std::array<float, kStringCount> kDefaultFrequencies{196.f, 293.7f, 440.f, 659.3f};

/// @brief Physical model of an instrument with multiple bowed strings.
class StringEnsemble
{
  public:
    StringEnsemble() = default;
    ~StringEnsemble() = default;

    /// @brief Initialize the string ensemble.
    /// @param samplerate The samplerate of the system.
    /// @param frequencies The initial open tuning for the strings.
    void Init(float samplerate, const std::array<float, kStringCount>& frequencies = kDefaultFrequencies);

    /// @brief Set the amount of energy transmitted from one string to the others
    /// @param t The amount of energy transmitted from one string to the others, between 0 and 1.
    void SetBridgeTransmission(float t);

    /// @brief Get the bridge transmission coefficient.
    /// @return The bridge transmission coefficient.
    float GetBridgeTransmission() const;

    /// @brief Process and return one sample.
    /// @return The processed sample.
    float Tick();

    /// @brief Process and return block of samples.
    /// @param out The output buffer where the processed samples will be written.
    /// @param size The size of the output buffer.
    void ProcessBlock(float* out, size_t size);

    /// @brief Subscript operator to access each individual string
    /// @param string_number The string number to access. Between 0 and `kStringCount-1`
    /// @return A reference to the string object.
    const BowedString& operator[](uint8_t string_number) const;

    /// @brief Subscript operator to access each individual string
    /// @param string_number The string number to access. Between 0 and `kStringCount-1`
    /// @return A reference to the string object.
    BowedString& operator[](uint8_t string_number);

    /// @brief Parameters for the string ensemble.
    enum class ParamId
    {
        /// @brief The amount of energy transmitted from one string to the others.
        BridgeTransmission,
        /// @brief The cutoff frequency of the filter applied to the bridge transmission.
        BridgeTransmissionFilterCutoff,
    };

    /// @brief Set a parameter of the string ensemble.
    /// @param param_id The parameter to set.
    /// @param value The value to set the parameter to. Between 0 and 1.
    void SetParameter(ParamId param_id, float value);

  private:
    std::array<BowedString, kStringCount> strings_;
    std::array<float, kStringCount> openTuning_;
    float bridgeTransmission_ = 0.0f;

    OnePoleFilter transmission_filter_;
    std::array<Biquad, 6> body_filters_;
};
} // namespace sfdsp