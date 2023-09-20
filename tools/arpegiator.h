#pragma once

#include <vector>

#include <dsp_base.h>

class Arpegiator
{
  public:
    Arpegiator() = default;
    ~Arpegiator() = default;

    void SetNotes(const std::vector<float>& notes);

    float Next();

  private:
    std::vector<float> buffer_;
    size_t index_ = 0;
};