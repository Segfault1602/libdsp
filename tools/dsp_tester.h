#pragma once

#include "dsp_base.h"

#include "bowed_string.h"
#include "chorus.h"
#include "line.h"

#define DEFAULT_SAMPLERATE (48000u)
#define DEFAULT_FRAME_COUNT_RATIO (2u)

class DspTester
{
  public:
    DspTester() = default;
    virtual ~DspTester() = default;

    virtual void Init(size_t samplerate) = 0;
    virtual float Tick() = 0;
    virtual float Tick(float input) = 0;

    size_t GetSamplerate() const
    {
        return samplerate_;
    }

    /// @brief Set the number of frames to process
    /// @param frame_count
    void SetFrameCount(uint32_t frame_count);

    /// @brief Get the number of frames to process
    /// @return The number of frames to process
    /// @note Defaults to 2 seconds of audio
    uint32_t GetFrameCount() const;

  protected:
    size_t samplerate_ = DEFAULT_SAMPLERATE;
    uint32_t frame_count_ = DEFAULT_FRAME_COUNT_RATIO * DEFAULT_SAMPLERATE;
};

class ChorusTester : public DspTester
{
  public:
    ChorusTester();
    ~ChorusTester() override = default;

    void Init(size_t samplerate) override;
    float Tick() override;
    float Tick(float input) override;

  private:
    dsp::Chorus chorus_;
};

class SimpleBowedStringTester : public DspTester
{
  public:
    SimpleBowedStringTester() = default;
    ~SimpleBowedStringTester() override = default;

    void Init(size_t samplerate) override;
    float Tick() override;
    float Tick(float input) override;

  private:
    dsp::BowedString string_;

    size_t midway_frame_ = 0;
    size_t current_frame_ = 0;

    float current_velocity_ = 0.f;
    float current_force_ = 0.f;
    float param_delta_ = 0.f;
    dsp::Line param_value_;
};

class OscVelocityBowedStringTester : public DspTester
{
  public:
    OscVelocityBowedStringTester() = default;
    ~OscVelocityBowedStringTester() override = default;

    void Init(size_t samplerate) override;
    float Tick() override;
    float Tick(float input) override;

  private:
    dsp::BowedString string_;
    const float kFrequency = 5.f;
    float phase_dt_ = 0.f;
    float phase_ = 0.f;
};