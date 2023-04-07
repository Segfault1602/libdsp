from scipy.signal import chirp
import scipy.io
import numpy as np

ATTENUATION = 0.1 # Attenuation to avoid clipping due to floating point math

## 8 seconds quadratic chirp signal at 96kHz

Fs = 96000
chirp_length_seconds = 8
time = np.linspace(0, chirp_length_seconds, Fs*chirp_length_seconds)

x = chirp(time, 0, chirp_length_seconds, 48000, 'quadratic')
x = ATTENUATION * x

scipy.io.wavfile.write('chirp.wav', Fs, x)

## 1 second 1kHz tone

Fs = 96000
F0 = 1000
tone_length_seconds = 1
time = np.linspace(0, tone_length_seconds, Fs*tone_length_seconds)

x = np.sin(2*np.pi*time * F0)
x = ATTENUATION * x

scipy.io.wavfile.write('1ktone.wav', Fs, x)

## Dirac
Fs = 96000
IMPULSE_LENGTH = 100
x = np.zeros(IMPULSE_LENGTH)
x[round(IMPULSE_LENGTH/2)] = 1

scipy.io.wavfile.write('dirac.wav', Fs, x);