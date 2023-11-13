#include "stringensemble_tests.h"

void StringEnsembleTest::Init(size_t samplerate)
{
    samplerate_ = samplerate;
    ensemble_.Init(static_cast<float>(samplerate), sfdsp::kDefaultFrequencies);
    ensemble_[1].SetForce(0.5f);
    ensemble_[1].SetVelocity(0.9f);
    // ensemble_[0].SetFrequency(420.f);

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