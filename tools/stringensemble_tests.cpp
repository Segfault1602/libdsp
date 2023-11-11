#include "stringensemble_tests.h"

void StringEnsembleTest::Init(size_t samplerate)
{
    ensemble_.Init(static_cast<float>(samplerate), sfdsp::kDefaultFrequencies);
    ensemble_[2].SetForce(0.05f);
    ensemble_[2].SetVelocity(0.5f);

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