from scipy.signal import spectrogram
import numpy as np
import matplotlib.pyplot as plt
import scipy.io
import os
import subprocess

def plot_spectrogram(title, w, fs, ax):
    ff, tt, Sxx = spectrogram(w, fs=fs, window='hann', nperseg=512, nfft=1024)
    ax.pcolormesh(tt, ff, 20*np.log10(Sxx), shading='gouraud')
    ax.set_title(title)
    ax.set_xlabel('t (sec)')
    ax.set_ylabel('Frequency (Hz)')
    ax.grid(True)


def main():
    """Main function
    """
    SCRIPT_DIR = os.path.realpath(os.path.dirname(__file__))
    BUILD_DIR = os.path.join(SCRIPT_DIR, "..", "build")

    RESAMPLER_EXE_PATH = os.path.join(BUILD_DIR, "tools", "resampler")

    WAVES_DIR = os.path.join(SCRIPT_DIR, "..", "waves")
    CHIRP_FILE = os.path.join(WAVES_DIR, "chirp.wav")
    TONE_FILE = os.path.join(WAVES_DIR, "1ktone.wav")
    DIRAC_FILE = os.path.join(WAVES_DIR, "dirac.wav")

    OUTPUT_DIR = os.path.join(SCRIPT_DIR, "..", "tmp_wav")
    OUT_CHIRP = os.path.join(OUTPUT_DIR, "out_chirp.wav")
    OUT_TONE = os.path.join(OUTPUT_DIR, "out_1ktone.wav")
    OUT_DIRAC = os.path.join(OUTPUT_DIR, "out_dirac.wav")

    OUT_CHIRP_LIBSAMPLERATE = os.path.join(OUTPUT_DIR, "out_chirp_libsamplerate.wav")
    OUT_TONE_LIBSAMPLERATE = os.path.join(OUTPUT_DIR, "out_1ktone_libsamplerate.wav")
    OUT_DIRAC_LIBSAMPLERATE = os.path.join(OUTPUT_DIR, "out_dirac_libsamplerate.wav")

    if not os.path.exists(OUTPUT_DIR):
        os.mkdir(OUTPUT_DIR)

    subprocess.run([RESAMPLER_EXE_PATH, "-f", CHIRP_FILE, "-t", "44100", "-o", OUT_CHIRP], check=True)
    subprocess.run([RESAMPLER_EXE_PATH, "-f", TONE_FILE, "-t", "44100", "-o", OUT_TONE], check=True)
    subprocess.run([RESAMPLER_EXE_PATH, "-f", DIRAC_FILE, "-t", "44100", "-o", OUT_DIRAC], check=True)

    subprocess.run([RESAMPLER_EXE_PATH, "-f", CHIRP_FILE, "-t", "44100", "-o", OUT_CHIRP_LIBSAMPLERATE, "-s"], check=True)
    subprocess.run([RESAMPLER_EXE_PATH, "-f", TONE_FILE, "-t", "44100", "-o", OUT_TONE_LIBSAMPLERATE, "-s"], check=True)
    subprocess.run([RESAMPLER_EXE_PATH, "-f", DIRAC_FILE, "-t", "44100", "-o", OUT_DIRAC_LIBSAMPLERATE, "-s"], check=True)

    fig, axes = plt.subplots(3, 2)
    fs, chirp = scipy.io.wavfile.read(OUT_CHIRP)
    plot_spectrogram("Chirp resampled", chirp, fs, axes[0][0])

    fs, chirp = scipy.io.wavfile.read(OUT_CHIRP_LIBSAMPLERATE)
    plot_spectrogram("Chirp resampled (libsamplerate)", chirp, fs, axes[0][1])

    fs, tone = scipy.io.wavfile.read(OUT_TONE)
    plot_spectrogram("1k tone resampled", tone, fs, axes[1][0])

    fs, tone = scipy.io.wavfile.read(OUT_TONE_LIBSAMPLERATE)
    plot_spectrogram("1k tone resampled (libsamplerate)", tone, fs, axes[1][1])

    fs, impulse = scipy.io.wavfile.read(OUT_DIRAC)
    axes[2][0].plot(impulse)

    fs, impulse = scipy.io.wavfile.read(OUT_DIRAC_LIBSAMPLERATE)
    axes[2][1].plot(impulse)

    fig.tight_layout(pad=1.0)
    plt.show()

if __name__ == "__main__":
    main()
