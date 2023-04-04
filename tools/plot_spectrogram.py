from scipy.signal import spectrogram
import matplotlib.pyplot as plt
import scipy.io

def plot_spectrogram(title, w, fs):
    ff, tt, Sxx = spectrogram(w, fs=fs)
    fig, ax = plt.subplots()
    ax.pcolormesh(tt, ff, Sxx, cmap='magma',
                  shading='gouraud')
    ax.set_title(title)
    ax.set_xlabel('t (sec)')
    ax.set_ylabel('Frequency (Hz)')
    ax.grid(True)


fs, audio = scipy.io.wavfile.read("c:/source/libdsp/out.wav")

plot_spectrogram("Spectrogram", audio, fs)
plt.show()