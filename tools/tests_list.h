#pragma once

#include <memory>

#include "bowedstring_tests.h"
#include "dsp_tester.h"
#include "phaseshapers_tests.h"
#include "stringensemble_tests.h"

#define FOREACH_TESTS(TEST)                                                                                            \
    TEST(ChorusTester)                                                                                                 \
    TEST(SimpleBowedString)                                                                                            \
    TEST(CrescendoBowedStringTester)                                                                                   \
    TEST(PitchSlideBowedStringTester)                                                                                  \
    TEST(OscVelocityBowedStringTester)                                                                                 \
    TEST(VibratoBowedStringTester)                                                                                     \
    TEST(ScaleBowedStringTester)                                                                                       \
    TEST(PhaseShaperTest)                                                                                              \
    TEST(StringEnsembleTest)

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#define MAKE_TEST(TEST)                                                                                                \
    case TesterType::TEST:                                                                                             \
        return std::make_unique<TEST>();

enum class TesterType
{
    FOREACH_TESTS(GENERATE_ENUM)
};

static const char* TEST_STRING[] = {FOREACH_TESTS(GENERATE_STRING)};

std::unique_ptr<DspTester> CreateTester(TesterType type)
{
    switch (type)
    {
        FOREACH_TESTS(MAKE_TEST)
    default:
        assert(false);
        return nullptr;
    }
}