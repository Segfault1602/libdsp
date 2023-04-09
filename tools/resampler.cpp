#include <cassert>
#include <cmath>
#include <cstdio>
#include <memory>
#include <sndfile.h>
#include <string>
#include <vector>

#include <samplerate.h>
#include <sinc_resampler.h>

void UseSincResample(const float* buffer, size_t input_size, float resampling_ratio, float* out, size_t& out_size);
void UseLibSamplerate(const float* buffer, size_t input_size, float resampling_ratio, float* out, size_t& out_size);

int main(int argc, char** argv)
{
    const std::vector<std::string> args(argv + 1, argv + argc);

    std::string input_file;
    std::string output_file;
    uint32_t target_fs;
    bool use_libsamplerate = false;

    if (args.size() < 6)
    {
        printf("Invalid command line options! \n");
        printf("Usage: resampler.exe -f <wav_file> -t <target_fs>\n");
        return -1;
    }

    for (size_t i = 0; i < args.size(); ++i)
    {
        if (args[i] == "-f")
        {
            input_file = args[i + 1];
            ++i;
        }
        else if (args[i] == "-t")
        {
            size_t pos_unused;
            target_fs = std::stol(args[i + 1], &pos_unused);
            ++i;
        }
        else if (args[i] == "-o")
        {
            output_file = args[i + 1];
            ++i;
        }
        else if (args[i] == "-s")
        {
            use_libsamplerate = true;
        }
    }

    SF_INFO sf_info{0};
    SNDFILE* file = sf_open(input_file.c_str(), SFM_READ, &sf_info);

    if (file == nullptr)
    {
        printf("Failed to open input file! (sf_strerror: %s)\n", sf_strerror(file));
        return -1;
    }

    // For benchmarking purposes, load the whole file into memory first.
    std::vector<float> buffer(sf_info.frames, 0);

    sf_count_t count = sf_readf_float(file, buffer.data(), sf_info.frames);
    assert(count == sf_info.frames);
    sf_close(file);

    if (sf_info.channels != 1)
    {
        printf("Only mono is currently supported!\n");
        return -1;
    }

    auto resampling_ratio = static_cast<float>(target_fs / sf_info.samplerate);

    size_t out_size = std::ceil(static_cast<float>(count) * resampling_ratio);
    std::vector<float> out(out_size, 0);

    if (!use_libsamplerate)
    {
        UseSincResample(buffer.data(), count, resampling_ratio, out.data(), out_size);
    }
    else
    {
        UseLibSamplerate(buffer.data(), count, resampling_ratio, out.data(), out_size);
    }

    SF_INFO out_sf_info{0};
    out_sf_info.channels = 1;
    out_sf_info.samplerate = static_cast<int>(target_fs);
    out_sf_info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
    SNDFILE* out_file = sf_open(output_file.c_str(), SFM_WRITE, &out_sf_info);
    if (out_file == nullptr)
    {
        printf("Failed to open output file! (sf_strerror: %s)\n", sf_strerror(out_file));
        return -1;
    }

    sf_write_float(out_file, out.data(), static_cast<sf_count_t>(out_size));
    sf_write_sync(out_file);
    sf_close(out_file);

    return 0;
}

void UseSincResample(const float* buffer, size_t input_size, float resampling_ratio, float* out, size_t& out_size)
{
    dsp::sinc_resample(buffer, input_size, resampling_ratio, out, out_size);
}

void UseLibSamplerate(const float* buffer, size_t input_size, float resampling_ratio, float* out, size_t& out_size)
{
    SRC_DATA src_data;
    src_data.data_in = buffer;
    src_data.input_frames = static_cast<long>(input_size);
    src_data.data_out = out;
    src_data.output_frames = static_cast<long>(out_size);
    src_data.src_ratio = resampling_ratio;

    int ret = src_simple(&src_data, SRC_SINC_FASTEST, 1);

    if (ret != 0)
    {
        printf("Failed to resample file using libsamplerate! (src_strerror: %s)", src_strerror(ret));
    }
}