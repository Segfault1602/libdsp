#pragma once

#include <memory>
#include <string>
#include <type_traits>

#include <sndfile.h>

#include "dsp_base.h"

template <typename T>
bool LoadWavFile(const std::string& filename, std::unique_ptr<T[]>& buffer, size_t& buffer_size, SF_INFO& sf_info)
{
    SNDFILE* file = sf_open(filename.c_str(), SFM_READ, &sf_info);

    if (file == nullptr)
    {
        printf("Error: %s\n", sf_strerror(file));
        return false;
    }

    buffer = std::make_unique<T[]>(sf_info.frames);

    sf_count_t count = 0;

    if constexpr (std::is_same<float, typename std::remove_cv<T>::type>::value)
    {
        count = sf_readf_float(file, buffer.get(), sf_info.frames);
    }
    else
    {
        static_assert(std::is_same<double, typename std::remove_cv<T>::type>::value);
        count = sf_readf_double(file, buffer.get(), sf_info.frames);
    }

    assert(count == sf_info.frames);
    buffer_size = count;

    sf_close(file);
    return true;
}

template <typename T>
bool WriteWavFile(std::string filename, const T* buffer, SF_INFO sf_info, size_t frames)
{
    SNDFILE* out_file = sf_open(filename.c_str(), SFM_WRITE, &sf_info);
    if (out_file == nullptr)
    {
        printf("Error: %s\n", sf_strerror(out_file));
        return false;
    }

    if constexpr (std::is_same<float, typename std::remove_cv<T>::type>::value)
    {
        sf_writef_float(out_file, buffer, static_cast<sf_count_t>(frames));
    }
    else
    {
        static_assert(std::is_same<double, typename std::remove_cv<T>::type>::value);
        sf_writef_double(out_file, buffer, static_cast<sf_count_t>(frames));
    }

    sf_write_sync(out_file); // Is this needed?
    sf_close(out_file);
    return true;
}