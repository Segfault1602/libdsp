#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <sndfile.h>

#include "basic_oscillators.h"
#include "delayline.h"
#include "test_utils.h"
#include "waveguide_gate.h"

TEST(WaveguideGatesTest, OpenGates)
{
    constexpr size_t kDelaySize = 11;
    constexpr float kDelay = 10;
    constexpr float kGatePos = 5;
    constexpr float kCoeff = 0.0f;

    sfdsp::Delayline right_traveling_line(kDelaySize, false, sfdsp::InterpolationType::Linear);
    sfdsp::Delayline left_traveling_line(kDelaySize, true, sfdsp::InterpolationType::Linear);

    left_traveling_line.SetDelay(kDelay);
    right_traveling_line.SetDelay(kDelay);

    // Feed a dirac pulse into both delay line
    left_traveling_line.TapIn(2, 1.f);
    right_traveling_line.TapIn(2, 1.f);

    sfdsp::WaveguideGate gate(true, kGatePos, kCoeff);

    // Looping for 2 * kDelay should bring us back to initial state
    for (auto i = 0; i < kDelay * 2; ++i)
    {
        gate.Process(left_traveling_line, right_traveling_line);

        float left_sample = left_traveling_line.NextOut();
        float right_sample = right_traveling_line.NextOut();

        left_traveling_line.Tick(right_sample * -1.f);
        right_traveling_line.Tick(left_sample * -1.f);
    }

    for (auto i = 1; i <= kDelaySize; ++i)
    {
        if (i == 2)
        {
            ASSERT_EQ(left_traveling_line[i], 1);
            ASSERT_EQ(right_traveling_line[i], 1);
        }
        else
        {
            ASSERT_EQ(left_traveling_line[i], 0);
            ASSERT_EQ(right_traveling_line[i], 0);
        }
    }
}

TEST(WaveguideGatesTest, HalfOpenGates)
{
    constexpr size_t kDelaySize = 11;
    constexpr float kDelay = 10;
    constexpr float kGatePos = 5;
    constexpr float kCoeff = 0.5f;

    sfdsp::Delayline right_traveling_line(kDelaySize, false);
    sfdsp::Delayline left_traveling_line(kDelaySize, true);

    left_traveling_line.SetDelay(kDelay);
    right_traveling_line.SetDelay(kDelay);

    // Feed a dirac pulse into both delay line
    left_traveling_line.TapIn(2, 1.f);
    right_traveling_line.TapIn(2, 1.f);

    PrintDelayline(right_traveling_line);
    PrintDelayline(left_traveling_line);

    sfdsp::WaveguideGate gate(true, kGatePos, kCoeff);

    // Looping for 2 * kDelay should bring us back to initial state
    for (auto i = 0; i < kDelay * 2; ++i)
    {
        gate.Process(left_traveling_line, right_traveling_line);

        float left_sample = left_traveling_line.NextOut();
        float right_sample = right_traveling_line.NextOut();

        left_traveling_line.Tick(right_sample * -1.f);
        right_traveling_line.Tick(left_sample * -1.f);

        printf("Iteration %d\n", i);
        PrintDelayline(right_traveling_line);
        PrintDelayline(left_traveling_line);
    }

    for (auto i = 1; i <= kDelaySize; ++i)
    {
        if (i == 2 || i == 9)
        {
            ASSERT_EQ(std::abs(left_traveling_line[i]), 0.5f);
            ASSERT_EQ(std::abs(right_traveling_line[i]), 0.5f);
        }
        else
        {
            ASSERT_EQ(left_traveling_line[i], 0);
            ASSERT_EQ(right_traveling_line[i], 0);
        }
    }
}

TEST(WaveguideGatesTest, ClosedGates)
{
    constexpr size_t kDelaySize = 11;
    constexpr float kDelay = 10;
    constexpr float kGatePos = 5;
    constexpr float kCoeff = 1.0f;

    sfdsp::Delayline right_traveling_line(kDelaySize, false, sfdsp::InterpolationType::Linear);
    sfdsp::Delayline left_traveling_line(kDelaySize, true, sfdsp::InterpolationType::Linear);

    right_traveling_line.SetDelay(kDelay);
    left_traveling_line.SetDelay(kDelay);

    // Feed a dirac pulse into both delay line
    right_traveling_line.TapIn(2, 1.f);
    left_traveling_line.TapIn(2, 1.f);

    right_traveling_line.TapIn(8, 1.f);
    left_traveling_line.TapIn(8, 1.f);

    PrintDelayline(right_traveling_line);
    PrintDelayline(left_traveling_line);

    sfdsp::WaveguideGate gate(true, kGatePos, kCoeff);

    // Looping for kDelay should bring us back to initial state
    for (auto i = 0; i < kDelay; ++i)
    {
        gate.Process(left_traveling_line, right_traveling_line);

        float left_sample = left_traveling_line.NextOut();
        float right_sample = right_traveling_line.NextOut();

        left_traveling_line.Tick(right_sample * -1.f);
        right_traveling_line.Tick(left_sample * -1.f);

        printf("Iteration %d\n", i);
        PrintDelayline(right_traveling_line);
        PrintDelayline(left_traveling_line);
    }

    for (auto i = 1; i <= kDelaySize; ++i)
    {
        if (i == 2 || i == 8)
        {
            ASSERT_EQ(left_traveling_line[i], 1);
            ASSERT_EQ(right_traveling_line[i], 1);
        }
        else
        {
            ASSERT_EQ(left_traveling_line[i], 0);
            ASSERT_EQ(right_traveling_line[i], 0);
        }
    }
}

TEST(WaveguideGatesTest, FractionalGates)
{
    constexpr size_t kDelaySize = 11;
    constexpr float kDelay = 10;
    constexpr float kGatePos = 4.5;
    constexpr float kCoeff = 1.0f;

    sfdsp::Delayline right_traveling_line(kDelaySize, false, sfdsp::InterpolationType::Linear);
    sfdsp::Delayline left_traveling_line(kDelaySize, true, sfdsp::InterpolationType::Linear);

    right_traveling_line.SetDelay(kDelay);
    left_traveling_line.SetDelay(kDelay);

    // Feed a dirac pulse into both delay line
    right_traveling_line.TapIn(2, 1.f);
    left_traveling_line.TapIn(2, 1.f);

    right_traveling_line.TapIn(8, 1.f);
    left_traveling_line.TapIn(8, 1.f);

    PrintDelayline(right_traveling_line);
    PrintDelayline(left_traveling_line);

    sfdsp::WaveguideGate gate(true, kGatePos, kCoeff);

    // Looping for kGatePos*2 should bring us back to initial state
    for (auto i = 0; i < kGatePos * 2; ++i)
    {
        gate.Process(left_traveling_line, right_traveling_line);

        float left_sample = left_traveling_line.NextOut();
        float right_sample = right_traveling_line.NextOut();

        left_traveling_line.Tick(right_sample * -1.f);
        right_traveling_line.Tick(left_sample * -1.f);

        printf("Iteration %d\n", i);
        PrintDelayline(right_traveling_line);
        PrintDelayline(left_traveling_line);
    }

    ASSERT_EQ(left_traveling_line[2], 1);
    ASSERT_EQ(right_traveling_line[2], 1);
}

TEST(WaveguideGatesTest, MovingGate)
{
    constexpr size_t kDelaySize = 11;
    constexpr float kDelay = 10;
    constexpr float kGatePos = 5;
    constexpr float kCoeff = 1.0f;

    sfdsp::Delayline right_traveling_line(kDelaySize, false, sfdsp::InterpolationType::Linear);
    sfdsp::Delayline left_traveling_line(kDelaySize, true, sfdsp::InterpolationType::Linear);

    right_traveling_line.SetDelay(kDelay);
    left_traveling_line.SetDelay(kDelay);

    right_traveling_line.TapIn(1, 1);
    left_traveling_line.TapIn(1, 1);
    right_traveling_line.TapIn(2, 2);
    left_traveling_line.TapIn(2, 2);
    right_traveling_line.TapIn(3, 3);
    left_traveling_line.TapIn(3, 3);
    right_traveling_line.TapIn(4, 4);
    left_traveling_line.TapIn(4, 4);
    right_traveling_line.TapIn(5, 5);
    left_traveling_line.TapIn(5, 5);

    PrintDelayline(right_traveling_line);
    PrintDelayline(left_traveling_line);

    sfdsp::WaveguideGate gate(false, kGatePos, kCoeff);
    for (auto i = 0; i < 2; ++i)
    {
        gate.Process(left_traveling_line, right_traveling_line);

        float left_sample = left_traveling_line.NextOut();
        float right_sample = right_traveling_line.NextOut();

        left_traveling_line.Tick(right_sample);
        right_traveling_line.Tick(left_sample);

        printf("Iteration %d\n", i);
        PrintDelayline(right_traveling_line);
        PrintDelayline(left_traveling_line);
    }

    // Move the gate to the right
    gate.SetDelay(kGatePos + 0.25f);
    printf("\nMoving the delay\n");

    for (auto i = 0; i < 1; ++i)
    {
        gate.Process(left_traveling_line, right_traveling_line);

        float left_sample = left_traveling_line.NextOut();
        float right_sample = right_traveling_line.NextOut();

        left_traveling_line.Tick(right_sample);
        right_traveling_line.Tick(left_sample);

        printf("Iteration %d\n", i);
        PrintDelayline(right_traveling_line);
        PrintDelayline(left_traveling_line);
    }

    gate.SetDelay(kGatePos + 0.5f);
    printf("\nMoving the delay\n");
    for (auto i = 0; i < 1; ++i)
    {
        gate.Process(left_traveling_line, right_traveling_line);

        float left_sample = left_traveling_line.NextOut();
        float right_sample = right_traveling_line.NextOut();

        left_traveling_line.Tick(right_sample);
        right_traveling_line.Tick(left_sample);

        printf("Iteration %d\n", i);
        PrintDelayline(right_traveling_line);
        PrintDelayline(left_traveling_line);
    }

    gate.SetDelay(kGatePos + 0.75f);
    printf("\nMoving the delay\n");
    for (auto i = 0; i < 1; ++i)
    {
        gate.Process(left_traveling_line, right_traveling_line);

        float left_sample = left_traveling_line.NextOut();
        float right_sample = right_traveling_line.NextOut();

        left_traveling_line.Tick(right_sample);
        right_traveling_line.Tick(left_sample);

        printf("Iteration %d\n", i);
        PrintDelayline(right_traveling_line);
        PrintDelayline(left_traveling_line);
    }

    gate.SetDelay(kGatePos + 1.f);
    printf("\nMoving the delay\n");
    for (auto i = 0; i < 10; ++i)
    {
        gate.Process(left_traveling_line, right_traveling_line);

        float left_sample = left_traveling_line.NextOut();
        float right_sample = right_traveling_line.NextOut();

        left_traveling_line.Tick(right_sample);
        right_traveling_line.Tick(left_sample);

        printf("Iteration %d\n", i);
        PrintDelayline(right_traveling_line);
        PrintDelayline(left_traveling_line);
    }
}

bool WriteWavFile(std::string filename, const float* buffer, SF_INFO sf_info, size_t frames)
{
    SNDFILE* out_file = sf_open(filename.c_str(), SFM_WRITE, &sf_info);
    if (out_file == nullptr)
    {
        printf("Error: %s\n", sf_strerror(out_file));
        return false;
    }

    sf_writef_float(out_file, buffer, static_cast<sf_count_t>(frames));
    sf_write_sync(out_file); // Is this needed?
    sf_close(out_file);
    return true;
}

TEST(DISABLED_WaveguideGatesTest, SineWave)
{
    constexpr size_t kDelaySize = 21;
    constexpr float kDelay = 20;
    constexpr float kGatePos = 10;
    constexpr float kCoeff = 1.0f;
    constexpr size_t outSize = 600;
    float gatePosDt = 0.1f;
    constexpr size_t samplerate = 50000.f;
    constexpr float kFreq = 1000;

    sfdsp::Delayline right_traveling_line(kDelaySize, false, sfdsp::InterpolationType::Linear);
    sfdsp::Delayline left_traveling_line(kDelaySize, true, sfdsp::InterpolationType::Linear);

    right_traveling_line.SetDelay(kDelay);
    left_traveling_line.SetDelay(kDelay);

    float phase = 0.f;
    float phase_dt = kFreq / samplerate;

    sfdsp::WaveguideGate gate(true, kGatePos, kCoeff);

    std::array<float, outSize> output;

    float currentGatePos = kGatePos;
    for (auto i = 0; i < outSize; ++i)
    {
        if (currentGatePos >= 12.f)
        {
            gatePosDt *= -1;
        }
        else if (currentGatePos <= 8.f)
        {
            gatePosDt *= -1;
        }

        currentGatePos += gatePosDt;
        gate.SetDelay(currentGatePos);

        gate.Process(left_traveling_line, right_traveling_line);

        float x = sfdsp::Sine(phase);
        phase += phase_dt;

        // This block tests the "left" side of the gate
        // float right_sample = right_traveling_line.NextOut();
        // output[i] = left_traveling_line.NextOut();
        // right_traveling_line.Tick(x);
        // left_traveling_line.Tick(right_sample);

        // Comment out the above block and uncomment the below block to test the "right" side of the gate
        float left_sample = left_traveling_line.NextOut();
        right_traveling_line.Tick(left_sample);
        left_traveling_line.Tick(x);
        output[i] = right_traveling_line.NextOut();

        // printf("Iteration %d, gatePos: %f\n", i, currentGatePos);
        // PrintDelayline(right_traveling_line);
        // PrintDelayline(left_traveling_line);
    }

    SF_INFO info;
    info.samplerate = static_cast<int>(samplerate);
    info.channels = 1;
    info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;

    WriteWavFile("SineWaveTest.wav", output.data(), info, output.size());
}
