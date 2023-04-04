""" Bandlimited Interpolation

This scripts implements the windowed sinc resampling algorithm as presented by

Smith, J.O. Digital Audio Resampling Home Page
http://www-ccrma.stanford.edu/~jos/resample/, 2020.
"""

from math import ceil
import numpy as np
import matplotlib.pyplot as plt
from scipy.special import sinc
from scipy import signal
from scipy import io


def plot_spectrogram(title, w, fs):
    """Plot a spectrogram.

    Args:
        title (string): Title to appear on top of the spectrogram
        w (array_like): Time series of measurment values
        fs (float): Sampling rate
    """
    ff, tt, Sxx = signal.spectrogram(w, fs=fs, nfft=1024)
    _, ax = plt.subplots()
    ax.pcolormesh(tt, ff, 10*np.log10(Sxx), cmap='magma', shading='gouraud')
    ax.set_title(title)
    ax.set_xlabel('t (sec)')
    ax.set_ylabel('Frequency (Hz)')
    ax.grid(True)


def sinc_resample_table(x, ratio, h, h_diff, num_zeroes, samples_per_crossing):
    """Resample a time series using the windowed sinc resampling method

    Args:
        x (array_like): Time
        ratio (float): Resampling ratio
        h (array_like): Table containing the "right wing" of a symmetric FIR
        h_diff (array_like): Table of difference between successive FIR sample values
        num_zeroes (int): Number of time the FIR (h) crosses zero
        samples_per_crossing (_type_): Number of sample per zero crossing in 'h'

    Returns:
        array_like: Array of resampled values
    """
    time_step = 1 / ratio
    filter_scale = min(1, ratio)
    filter_step = samples_per_crossing * filter_scale

    output = np.ndarray(shape=ceil(len(x) * ratio))
    out_idx = 0
    t = 0
    while t < len(x):

        acc = 0

        integer_part = int(t)
        fractional_part = t - integer_part

        # Compute left wing
        filter_offset = filter_step * fractional_part

        left_coeff_count = int((len(h) - filter_offset) / filter_step)
        left_coeff_count = min(integer_part, left_coeff_count) # avoid underflow access
        for i in range(-left_coeff_count, 1):
            filter_idx = filter_offset + filter_step * abs(i)
            fraction = filter_idx - int(filter_idx)
            filter_idx = int(filter_idx)

            weight = h[filter_idx] + fraction * h_diff[filter_idx]
            acc += x[integer_part + i] * weight

        # compute right wing
        fractional_part = 1 - fractional_part
        filter_offset = filter_step * fractional_part

        right_coeff_count = int((len(h) - filter_offset) / filter_step)
        right_coeff_count = min(len(x) - integer_part - 1, right_coeff_count) # avoid overflow access
        for i in range(0, right_coeff_count):
            filter_idx = filter_offset + filter_step * i
            fraction = filter_idx - int(filter_idx)
            filter_idx = int(filter_idx)

            weight = h[filter_idx] + fraction * h_diff[filter_idx]
            acc += x[integer_part + 1 + i] * weight

        if out_idx < len(output):
            output[out_idx] = acc * filter_scale
            out_idx += 1
        t += time_step

    return output


def build_sinc_table(num_zeros, samples_per_crossing):
    """Builds the "right wing" of a symmetric FIR filter designed byt the window method base on a Kaiser window.

    Args:
        num_zeros (int): Number of zero crossing
        samples_per_crossing (_type_): Number of samples per zero crossing

    Returns:
        h (array_like): FIR table
        h_diff (array_like): Table of difference between successive FIR value
    """

    SINC_SIZE_ = num_zeros * samples_per_crossing
    KAISER_BETA = 10

    x = np.linspace(-num_zeros, num_zeros, SINC_SIZE_ * 2 + 1)
    y = sinc(x)

    window = signal.windows.kaiser(len(y), KAISER_BETA)

    y = np.multiply(y, window)

    h = y[SINC_SIZE_:]

    h_diff = np.subtract(h[1:], h[:-1])
    h_diff = np.append(h_diff, 0)

    return h, h_diff


NZ = 13
SAMPLES_PER_CROSSING = 1024
h, h_diff = build_sinc_table(NZ, SAMPLES_PER_CROSSING)

# Quick sanity check with a simple sine wave
ORIGINAL_FS = 1000
SIGNAL_FREQUENCY = 200

time = np.linspace(0, 1, ORIGINAL_FS)
in_sine = np.sin(2 * np.pi * time * SIGNAL_FREQUENCY)

TARGET_FS = int(ORIGINAL_FS * 2.5)

out_sine = sinc_resample_table(
    in_sine,
    TARGET_FS /
    ORIGINAL_FS,
    h,
    h_diff,
    NZ,
    SAMPLES_PER_CROSSING)

out_time = np.linspace(0, 1, TARGET_FS)
plt.plot(out_time, out_sine, 'g*--')
plt.plot(time, in_sine, 'b+')
plt.xlim(0.0, 0.02)
plt.grid()

# Create a signal to be resampled
ORIGINAL_FS = 96000
CHIRP_LENGTH_SECONDS = 8
END_CHIRP_FREQUENCY = 44000

time = np.linspace(0, CHIRP_LENGTH_SECONDS, ORIGINAL_FS * CHIRP_LENGTH_SECONDS)
in_chirp = signal.chirp(
    time,
    0,
    CHIRP_LENGTH_SECONDS,
    END_CHIRP_FREQUENCY,
    'quadratic')

in_chirp = in_chirp * 0.5

TARGET_FS = 44100
RESAMPLING_RATIO = TARGET_FS / ORIGINAL_FS

output = sinc_resample_table(
    in_chirp,
    RESAMPLING_RATIO,
    h,
    h_diff,
    NZ,
    SAMPLES_PER_CROSSING)

# plot_spectrogram(f"Original {ORIGINAL_FS} Hz", in_chirp, ORIGINAL_FS)
# plot_spectrogram(f"Resampled to {TARGET_FS} Hz", output, TARGET_FS)

plt.figure(2)
plt.specgram(in_chirp, Fs=ORIGINAL_FS)
plt.figure(3)
plt.specgram(output, Fs=TARGET_FS)
plt.show()

# plt.plot(output)
# plt.show()

io.wavfile.write('chirp_resampled.wav', 44100, output)