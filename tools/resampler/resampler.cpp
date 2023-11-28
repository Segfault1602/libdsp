#include <cassert>
#include <cmath>
#include <cstdio>
#include <memory>
#include <sndfile.h>
#include <string>
#include <vector>

#include "tool_utils.h"
#include <samplerate.h>
#include <sinc_resampler.h>

void UseSincResample(const float* buffer, size_t input_size, float resampling_ratio, float* out, size_t& out_size);
void UseLibSamplerate(const float* buffer, size_t input_size, double resampling_ratio, float* out, size_t& out_size);

int main(int argc, char** argv)
{
    const std::vector<std::string> args(argv + 1, argv + argc);

    std::string input_file;
    std::string output_file;
    uint32_t target_fs = 44100;
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
    std::unique_ptr<float[]> buffer;
    size_t buffer_size;
    if (!LoadWavFile(input_file, buffer, buffer_size, sf_info))
    {
        return -1;
    }

    auto resampling_ratio = static_cast<float>(target_fs) / static_cast<float>(sf_info.samplerate);

    size_t out_size = std::ceil(static_cast<float>(buffer_size) * resampling_ratio);
    std::vector<float> out(out_size, 0);

    if (!use_libsamplerate)
    {
        UseSincResample(buffer.get(), buffer_size, resampling_ratio, out.data(), out_size);
    }
    else
    {
        // LibSamplerate only support float
        sf_info = {0};
        std::unique_ptr<float[]> float_buffer;
        buffer_size = 0;
        if (!LoadWavFile(input_file, float_buffer, buffer_size, sf_info))
        {
            return -1;
        }
        std::vector<float> float_out(out_size, 0);
        UseLibSamplerate(float_buffer.get(), buffer_size, resampling_ratio, float_out.data(), out_size);

        std::copy(float_out.begin(), float_out.end(), out.begin());
    }

    SF_INFO out_sf_info{0};
    out_sf_info.channels = 1;
    out_sf_info.samplerate = static_cast<int>(target_fs);
    out_sf_info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
    WriteWavFile(output_file, out.data(), out_sf_info, out_size);

    return 0;
}

void UseSincResample(const float* buffer, size_t input_size, float resampling_ratio, float* out, size_t& out_size)
{
    sfdsp::sinc_resample(buffer, input_size, resampling_ratio, out, out_size);
}

void UseLibSamplerate(const float* buffer, size_t input_size, double resampling_ratio, float* out, size_t& out_size)
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