import matplotlib.pyplot as plt
import scipy.io
import sys
import os
from dataclasses import dataclass
import re
import numpy as np


@dataclass
class Waveform:
    samples: np.ndarray
    freq: int
    force: float
    vel: float

    def __init__(self, samples, freq, force, vel):
        self.samples = samples
        self.freq = freq
        self.force = force
        self.vel = vel


def PlotWaveform(waves, outputs_folder):
    forces = set([wave.force for wave in waves])
    forces = sorted(forces, reverse=True)
    vels = set([wave.vel for wave in waves])
    vels = sorted(vels)

    fig, axes = plt.subplots(len(forces), len(vels), sharey='all')
    fig.suptitle(f"Frequency: {waves[0].freq}Hz")

    for i, force in enumerate(forces):
        for j, vel in enumerate(vels):
            wave = [wave for wave in waves if wave.force ==
                    force and wave.vel == vel][0]
            axes[i][j].plot(wave.samples)
            axes[i][j].set_title(f"Force: {force}, Vel: {vel}")
    fig.tight_layout(pad=1.0)
    plt.show()


def main():
    args = sys.argv[1:]
    if len(args) != 1:
        print("Usage: plot_waveform.py <outputs_folder>")
        sys.exit(1)
    outputs_folder = args[0]

    dir_list = os.listdir(outputs_folder)

    # Filename ex: 196Hz_f0.00_v0.25.wav
    pattern = re.compile(
        r"(?P<freq>\d+)Hz_f(?P<force>\d+\.\d+)_v(?P<vel>\d+\.\d+)\.wav")

    waves = []

    for file in dir_list:
        match = pattern.match(file)
        if match:
            freq = int(match.group("freq"))
            force = float(match.group("force"))
            vel = float(match.group("vel"))
            fs, samples = scipy.io.wavfile.read(
                os.path.join(outputs_folder, file))

            # Lets just grab 2 periods worth of samples at the middle of the
            # file
            samples = samples[int(len(samples) / 2 - fs / freq)
                                  :int(len(samples) / 2 + fs / freq)]
            waves.append(Waveform(samples, freq, force, vel))

    # List all the frequencies we have
    freqs = set([wave.freq for wave in waves])

    for freq in freqs:
        waves_freq = [wave for wave in waves if wave.freq == freq]
        PlotWaveform(waves_freq, outputs_folder)


if __name__ == "__main__":
    main()
