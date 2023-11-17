#pragma once

#include "dsp_utils.h"

#include <memory>
#include <string>

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

    const std::string& GetOutputFileName() const
    {
        return name_;
    }

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

    std::string name_ = "output.wav";
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
    sfdsp::Chorus chorus_;
};
