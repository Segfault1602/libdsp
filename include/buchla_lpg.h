#pragma once

#include <cstdint>

namespace sfdsp
{

float GetCurrent(float vc, float offset, bool smoothed);

float ProcessCurrent(float c);

/// @brief Buchla Low Pass Gate based on Josh Rohs' MATLAB implementation
/// Original paper: http://www.music.mcgill.ca/~gary/courses/projects/618_2018/rohs/MUMT618_Buchla_LPG_Report.pdf
class BuchlaLPG
{
  public:
    BuchlaLPG() = default;
    ~BuchlaLPG() = default;

    /// @brief Initialize the LPG
    /// @param samplerate The samplerate of the system
    void Init(float samplerate);

    /// @brief Process a block of samples
    /// @param in The input buffer
    /// @param out The output buffer. Can be the same as `in`
    /// @param size The size of the input and output buffer
    void ProcessBlock(const float* cv_in, const float* in, float* out, size_t size);

  private:
    float samplerate_;
    float dt_ = 0.f;
    float f_ = 0.f;

    float prev_current_ = 0.f;

    enum class SignDir
    {
        Positive,
        Negative,
        Unset
    };

    int8_t prev_dir_ = 2;
    float wc_ = 0.f;
    float sc_ = 0.f;
    float yc_ = 0.f;

    float sx_ = 0.f;
    float sd_ = 0.f;
    float so_ = 0.f;
    float yo_ = 0.f;
    float xo_ = 0.f;

    bool non_lin_ = false;
};

} // namespace sfdsp