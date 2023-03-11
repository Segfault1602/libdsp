from scipy.signal import chirp
from scipy.signal import spectrogram
import numpy as np
import matplotlib.pyplot as plt
import scipy.io


def plot_spectrogram(title, w, fs):
    ff, tt, Sxx = spectrogram(w, fs=fs, nperseg=256, nfft=256)
    fig, ax = plt.subplots()
    ax.pcolormesh(tt, ff[:145], Sxx[:145], cmap='gray_r',
                  shading='gouraud')
    ax.set_title(title)
    ax.set_xlabel('t (sec)')
    ax.set_ylabel('Frequency (Hz)')
    ax.grid(True)


Fs = 96000
chirp_length_seconds = 8
time = np.linspace(0, chirp_length_seconds, Fs*chirp_length_seconds)

x = chirp(time, 0, chirp_length_seconds, 48000, 'quadratic')
plot_spectrogram(f'Quadratic Chirp, f(0)=0, f({8})=48000', x, Fs)
plt.show()

scipy.io.wavfile.write('chirp.wav', Fs, x)