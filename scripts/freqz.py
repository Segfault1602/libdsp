# %%
import numpy as np
import matplotlib.pyplot as plt
from scipy import signal

cutoff = 2000
Fs = 48000
w = cutoff / Fs  # Normalize the frequency
wc = 2 * np.pi * w

y = 1 - np.cos(wc)
p = -y + np.sqrt(y**2 + 2 * y)
p = 1 - p

# Define filter coefficients
b = [1 - p]
a = [1, -p]

# Calculate frequency response
w, h = signal.freqz(b, a, fs=Fs)

# Plot frequency response
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(w, 20 * np.log10(abs(h)))
ax1.set(
    title=f"Magnitude response pole={p}",
    xlabel="Frequency (rad/sample)",
    ylabel="Magnitude (dB)",
    xlim=(0, cutoff * 2),
)
ax1.plot([0, cutoff], [-3, -3], "r--", linewidth=0.5)
ax1.grid()

ax2.plot(w, np.angle(h))
ax2.set(
    title="Phase response", xlabel="Frequency (rad/sample)", ylabel="Phase (radians)"
)
ax2.grid()

fig.tight_layout()

plt.show()

# %%
