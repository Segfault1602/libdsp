#include "buchla_lpg.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>

namespace
{
// constexpr float kNormalizedFb = 0.f;
constexpr float kTauOn = 0.012f; // Vactrol attack time
constexpr float kTauOff = 0.25f; // Vactrol decay time

constexpr float kRhoOn = 0.9f;
constexpr float kRhoOff = 0.9f;

constexpr float kWcOn = 5.f / kTauOn;
constexpr float kWcOff = 5.f / kTauOff;

constexpr float kBVac = 1136.2129956; // ohms, Light Resistance. From Parker & D'Angelo
constexpr float kAVac = 3.4645912f;   // omhs*amp^1.4. From Parker & D'Angelo

constexpr float kC1 = 1.e-9f;
constexpr float kC2 = 220.e-12f;
constexpr float kC3 = 0.f;
constexpr float kRa = 5.0e6f;
constexpr float kB4 = kC3 / kC2;

constexpr float kOffset = 0.f;

constexpr float kMaxVoltage = 10.7f;
constexpr float kIs = 10.1e-5f; // minimum forware current
constexpr float kVT = 26e-3;    // Thermal voltage
constexpr float kBeta = 87;

int8_t sign(float x)
{
    if (x > 0)
    {
        return 1;
    }

    if (x < 0)
    {
        return -1;
    }

    return 0;
}

} // namespace

namespace sfdsp
{

float GetCurrent(float vc, float offset, bool smoothed)
{
    if ((offset + vc) > kMaxVoltage)
    {
        vc = kMaxVoltage - offset;
    }

    constexpr float kN = 3.9696f; // diode constant
    float current = kIs * (std::exp((vc + offset) * kBeta / kN * kVT) - 1.f);
    if (current > 0.04f)
    {
        current = 0.04f;
    }

    return current;
}

void BuchlaLPG::Init(float samplerate)
{
    samplerate_ = samplerate;
    dt_ = 1.f / samplerate_;

    f_ = 1.f / (2.f * samplerate_);
}

void BuchlaLPG::ProcessBlock(const float* cv_in, const float* in, float* out, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        // Process the CV
        const float current = GetCurrent(cv_in[i], kOffset, /*smoothed=*/true);
        int8_t dir = sign(current - prev_current_);
        if (dir != prev_dir_)
        {
            switch (dir)
            {
            case 1:
                wc_ = kWcOn * (1.f - kRhoOn + kRhoOn * current / 0.040f);
                break;
            case -1:
                wc_ = kWcOff * (1.f + kRhoOff - (kRhoOff) * (yc_) / 0.040f);
                break;
            case 0:
                break;
            default:
                assert(false);
                break;
            }
        }

        const float g = wc_ * dt_ * 0.5f;
        const float vc = (current - sc_) * g / (1.f + g);
        yc_ = vc + sc_;
        sc_ = yc_ + vc;

        prev_current_ = current;
        prev_dir_ = dir;

        const float processed_current = std::max(yc_, 1.01e-5f);
        const float rf = 1.f / (std::pow(processed_current, 1.4f)) * kAVac + kBVac;

        // Process  the audio
        // const float amax = (2.f * kC1 * kRa + (kC2 + kC3) * (kRa + rf)) / (kC3 * kRa);
        const float a = 0.f; // kNormalizedFb * amax;
        const float a1 = 1.f / (rf * kC1);
        const float a2 = -1.f / kC1 * (1 / rf + 1.f / kRa);
        const float b1 = 1.f / (rf * kC2);
        const float b2 = -2.f / (rf * kC2);
        const float b3 = b1;
        const float d1 = a;

        const float yi = in[i];
        const float d_o = 1.f / (1.f - f_ * a2);
        const float d_x = 1.f / (1.f - f_ * b2);

        constexpr float kD2 = -1.f;

        if (non_lin_)
        {
        }
        else
        {
            float yx = (sx_ + f_ * b1 * yi + f_ * b3 * d_o * so_ + f_ * kB4 * sd_ + kB4 * d1 * d_o * so_) * d_x;
            yx = yx / (1 - d_x * (f_ * f_ * b3 * d_o * a1 + kB4 * f_ * d1 * d_o * a1 + kB4 * kD2));
            yo_ = (so_ + f_ * a1 * yx) * d_o;
            const float yd = sd_ + (1.f / f_) * (d1 * yo_ + kD2 * yx);

            sx_ = sx_ + 2.f * f_ * (b1 * yi + b2 * yx + b3 * yo_ + kB4 * yd);

            so_ = so_ + 2.f * f_ * (a1 * yx + a2 * yo_);

            sd_ = -sd_ - (2.f / f_) * (d1 * yo_ + kD2 * yx);
        }

        out[i] = yo_;
    }
}

} // namespace sfdsp