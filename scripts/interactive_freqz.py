# %%
import mpl_interactions.ipyplot as iplt
import numpy as np
import matplotlib.pyplot as plt
from scipy import signal

%matplotlib widget


def get_coeff_from_pole(pole):
    b = [0]
    a = [1, 0]
    b[0] = 1 - abs(pole)
    a[0] = 1
    a[1] = -pole
    return b, a


plt.ion()
# Define filter coefficients
pole = 0.9
b, a = get_coeff_from_pole(pole)

# Calculate frequency response
w, h = signal.freqz(b, a)


def freq(x, pole):
    b, a = get_coeff_from_pole(pole)
    w, h = signal.freqz(b, a)
    return 20 * np.log10(abs(h))


def phase(x, pole):
    b, a = get_coeff_from_pole(pole)
    w, h = signal.freqz(b, a, fs=48000)
    return np.angle(h)


pole = np.linspace(-0.999, 0.999, 1000)
x = 20 * np.log10(abs(h))

fig, (ax1, ax2) = plt.subplots(nrows=2)
ax1.set_ylabel('Amplitude [dB]', color='b')
ax1.set_ylim(-40, 0)
ax1.grid()
controls = iplt.plot(
    w,
    freq,
    pole=pole,
    label="freq",
    ax=ax1)
plt.xticks(np.arange(0, np.pi + np.pi / 4, step=(np.pi / 4)),
           ['0', 'π/4', 'π/2', '3π/4', 'π'])

ax2.set_ylabel('Angle (radians)', color='g')
ax2.set_ylim(-np.pi, np.pi)
iplt.plot(w, phase, controls=controls, label="phase",
          ax=ax2, ylim=(-np.pi, np.pi))
_ = plt.legend()
plt.xticks(np.arange(0, np.pi + np.pi / 4, step=(np.pi / 4)),
           ['0', 'π/4', 'π/2', '3π/4', 'π'])
plt.grid()
plt.show()

# %%
