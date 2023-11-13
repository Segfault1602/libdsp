# %%
import numpy as np
import matplotlib.pyplot as plt
from scipy import signal

cutoff = 5000
Fs = 48000

w = cutoff / Fs  # Normalize the frequency
x = np.exp(-2 * np.pi * w)  # Calculate the pole

pole = np.cos(2 * np.pi * w)
pole = x
# pole = 0.75 - (0.2 * 22050 / Fs)

# Define filter coefficients
b = [1 - np.abs(pole)]
a = [1, -pole]

# Calculate frequency response
w, h = signal.freqz(b, a, fs=Fs)

# Plot frequency response
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(w, 20 * np.log10(abs(h)))
ax1.set(
    title=f"Magnitude response pole={pole}",
    xlabel="Frequency (rad/sample)",
    ylabel="Magnitude (dB)",
    xlim=(0, 10000),
)
ax1.grid()

ax2.plot(w, np.angle(h))
ax2.set(
    title="Phase response", xlabel="Frequency (rad/sample)", ylabel="Phase (radians)"
)
ax2.grid()

fig.tight_layout()

plt.show()

# %%
