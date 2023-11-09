#include "stringensemble_tests.h"

void StringEnsembleTest::Init(size_t samplerate)
{
    ensemble_.Init(static_cast<float>(samplerate), sfdsp::kDefaultFrequencies);
    ensemble_.SetForce(2, 0.05f);
    ensemble_.SetVelocity(2, 0.5f);

    name_ = "stringensemble.wav";
}

float StringEnsembleTest::Tick()
{
    return ensemble_.Tick();
}

float StringEnsembleTest::Tick(float input)
{
    return ensemble_.Tick();
}