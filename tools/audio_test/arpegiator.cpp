#include "arpegiator.h"

void Arpegiator::SetNotes(const std::vector<float>& notes)
{
    buffer_ = notes;
}

float Arpegiator::Next()
{
    if (buffer_.empty())
    {
        return 0.0f;
    }

    float note = buffer_[index_];
    index_ = (index_ + 1) % buffer_.size();
    return note;
}