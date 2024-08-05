#include "doctest.h"
#include "nanobench.h"
#include <chrono>
#include <memory>

#include <iostream>
#include <stdlib.h>

using namespace ankerl;
using namespace std::chrono_literals;

TEST_CASE("Aligned")
{

    constexpr size_t size = 1024 * 1024;

    nanobench::Bench bench;
    bench.title("BasicOscillator");
    bench.relative(true);
    bench.minEpochIterations(5);
    bench.timeUnit(1ms, "ms");

    float* data_in = new float[size];
    float* data_out = new float[size];

    bench.run("Not-Aligned", [&]() {
        for (size_t i = 0; i < size; i++)
        {
            data_in[i] = i;
        }

        constexpr size_t block_size = 256;
        const size_t block_count = size / block_size;
        float* write_ptr = data_out;
        for (size_t i = 0; i < block_count; ++i)
        {
            std::copy(data_in + i * block_size, data_in + (i + 1) * block_size, write_ptr);
            write_ptr += block_size;
        }
    });

    for (size_t i = 0; i < size; i++)
    {
        CHECK(data_in[i] == data_out[i]);
    }

    delete[] data_in;
    delete[] data_out;

    float* data_in_aligned = static_cast<float*>(_aligned_malloc(size * sizeof(float), sizeof(float) * 4));
    float* data_out_aligned = static_cast<float*>(_aligned_malloc(size * sizeof(float), sizeof(float) * 4));

    bench.run("Aligned", [&]() {
        for (size_t i = 0; i < size; i++)
        {
            data_in_aligned[i] = i;
        }

        constexpr size_t block_size = 256;
        const size_t block_count = size / block_size;
        float* write_ptr = data_out_aligned;
        for (size_t i = 0; i < block_count; ++i)
        {
            std::copy(data_in_aligned + i * block_size, data_in_aligned + (i + 1) * block_size, write_ptr);
            write_ptr += block_size;
        }
    });

    for (size_t i = 0; i < size; i++)
    {
        CHECK(data_in_aligned[i] == data_out_aligned[i]);
    }

    _aligned_free(data_in_aligned);
    _aligned_free(data_out_aligned);
}