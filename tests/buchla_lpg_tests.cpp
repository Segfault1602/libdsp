#include "gtest/gtest.h"
#include <sndfile.h>

#include "basic_oscillators.h"
#include "buchla_lpg.h"
#include "test_utils.h"

TEST(BuchlaLPGTest, Current)
{
    constexpr float kSamplerate = 96000.f;

    sfdsp::BasicOscillator cv;
    cv.Init(kSamplerate, 4, sfdsp::OscillatorType::Square);
    cv.SetDuty(0.05f);

    constexpr size_t kSize = 4 * kSamplerate;
    auto cv_out = std::make_unique<float[]>(kSize);
    cv.ProcessBlock(cv_out.get(), kSize);

    constexpr float kModDepth = 8.f;

    for (size_t i = 0; i < kSize; ++i)
    {
        cv_out[i] = kModDepth / 2.f * (1.f + cv_out[i]);
    }

    auto audio_buffer = std::make_unique<float[]>(kSize);
    sfdsp::BasicOscillator audio;
    audio.Init(kSamplerate, 440, sfdsp::OscillatorType::Square);
    audio.ProcessBlock(audio_buffer.get(), kSize);

    for (size_t i = 0; i < kSize; ++i)
    {
        audio_buffer[i] = 0.3f * audio_buffer[i];
    }

    sfdsp::BuchlaLPG lpg;
    lpg.Init(kSamplerate);

    lpg.ProcessBlock(cv_out.get(), audio_buffer.get(), audio_buffer.get(), kSize);

    SF_INFO info;
    info.channels = 1;
    info.samplerate = kSamplerate;
    info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
    WriteWavFile("out_lpg.wav", audio_buffer.get(), info, kSize);
}

TEST(BuchlaLPGTestPerf, Current)
{
    constexpr float kSamplerate = 96000.f;

    sfdsp::BasicOscillator cv;
    cv.Init(kSamplerate, 4, sfdsp::OscillatorType::Square);
    cv.SetDuty(0.05f);

    constexpr size_t kSize = 4 * kSamplerate;
    auto cv_out = std::make_unique<float[]>(kSize);
    cv.ProcessBlock(cv_out.get(), kSize);

    constexpr float kModDepth = 8.f;

    for (size_t i = 0; i < kSize; ++i)
    {
        cv_out[i] = kModDepth / 2.f * (1.f + cv_out[i]);
    }

    auto audio_buffer = std::make_unique<float[]>(kSize);
    sfdsp::BasicOscillator audio;
    audio.Init(kSamplerate, 440, sfdsp::OscillatorType::Square);
    audio.ProcessBlock(audio_buffer.get(), kSize);

    for (size_t i = 0; i < kSize; ++i)
    {
        audio_buffer[i] = 0.3f * audio_buffer[i];
    }

    sfdsp::BuchlaLPG lpg;
    lpg.Init(kSamplerate);

    sfdsp::GetCurrent(cv_out.get(), cv_out.get(), kSize, false);
    lpg.ProcessCurrent(cv_out.get(), cv_out.get(), kSize);
    lpg.ProcessAudio(cv_out.get(), audio_buffer.get(), audio_buffer.get(), kSize);

    SF_INFO info;
    info.channels = 1;
    info.samplerate = kSamplerate;
    info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
    WriteWavFile("perf_lpg.wav", audio_buffer.get(), info, kSize);
}