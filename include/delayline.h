#pragma once

#include <cassert>
#include <cstddef>
#include <memory>

#include "dsp_base.h"
#include "interpolation_strategy.h"

namespace dsp
{

/// @brief Simple delayline.
class Delayline
{
  public:
    /// @brief Construct a delayline
    /// @param max_size The maximum size of the delayline in samples.
    /// @param reverse If true, TapIn() and TapOut() will access the delayline in reverse. See note.
    /// @param interpolation_type The interpolation type to use.
    /// @note In a canonical waveguide structure, you have a delayline going in one direction and a delayline going in
    /// the other. For example, to access the rightmost samples of the delayline, you would use TapOut(0) on the right
    /// traveling line and TapOut(end) on the left traveling one. When `reverse` is True, the delayline will be accessed
    /// in reverse which allows you to access the two delaylines with the same index.
    Delayline(size_t max_size, bool reverse = false, InterpolationType interpolation_type = InterpolationType::Linear);
    ~Delayline() = default;

    /// @brief  Set the delay in samples.
    /// @param delay Delay in samples
    void SetDelay(float delay);

    /// @brief Returns the delay in samples.
    /// @return The delay in samples.
    float GetDelay() const;

    /// @brief Reset the delayline.
    /// @note This will clear the delayline with zeros.
    void Reset();

    /// @brief Returns the next sample from the delayline without advancing the write pointer.
    /// @return The next sample from the delayline.
    float NextOut();

    /// @brief Returns the sample that was last returned by Tick().
    /// @return The sample that was last returned by Tick().
    float LastOut() const;

    /// @brief  Tick the delayline.
    /// @param input Input sample
    /// @return Output sample
    float Tick(float input);

    /// @brief  Read a sample from the delayline at a specific delay using a specific interpolation strategy.
    /// @param delay Delay in samples
    /// @param interpolation_strategy The interpolation strategy to use. If nullptr, the default interpolation strategy
    /// is Linear.
    /// @return The sample at the specified delay.
    float TapOut(float delay, InterpolationStrategy* interpolation_strategy = nullptr);

    /// @brief Add a sample to the delayline at a specific delay. If the delay is not an integer, linear interpolation
    /// is used.
    /// @param delay Delay in samples
    /// @param input Input sample
    void TapIn(float delay, float input);

    /// @brief Add a sample to the delayline at a specific delay. This method overwrites the sample at the specified
    /// delay.
    /// @param delay Delay in samples
    /// @param input Input sample
    void SetIn(float delay, float input);

    /// @brief Move the write pointer of the delayline back one sample.
    void Rewind();

    float& operator[](size_t index) const;
    float& operator[](size_t index);

  protected:
    const size_t max_size_ = 0;
    const bool reverse_ = false;

    bool do_next_out_ = true;
    float next_out_ = 0.f;
    float last_out_ = 0.f;

    size_t write_ptr_ = 0;
    float delay_ = 0;

    std::unique_ptr<InterpolationStrategy> interpolation_strategy_;
    std::unique_ptr<float[]> line_;
};
} // namespace dsp