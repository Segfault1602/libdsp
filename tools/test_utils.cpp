#include "test_utils.h"

#include <cassert>

bool LoadWavFile(const std::string& filename, std::unique_ptr<float[]> &buffer, size_t &buffer_size, SF_INFO &sf_info)
{
    sf_info = {0};

    SNDFILE *file = sf_open(filename.c_str(), SFM_READ, &sf_info);

    if (file == nullptr)
    {
        printf("Error: %s\n", sf_strerror(file));
        return false;
    }

    buffer = std::make_unique<float[]>(sf_info.frames);

    sf_count_t count = sf_readf_float(file, buffer.get(), sf_info.frames);
    assert(count == sf_info.frames);

    buffer_size = count;

    sf_close(file);
    return true;
}

bool WriteWavFile(std::string filename, const float *buffer, SF_INFO sf_info, size_t frames)
{
    SNDFILE *out_file = sf_open(filename.c_str(), SFM_WRITE, &sf_info);
    if (out_file == nullptr)
    {
        printf("Error: %s\n", sf_strerror(out_file));
        return false;
    }

    sf_writef_float(out_file, buffer, static_cast<sf_count_t>(frames));
    sf_write_sync(out_file); // Is this needed?
    sf_close(out_file);
    return true;
}