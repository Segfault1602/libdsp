"""_summary_
"""

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation


class LinearInterpolator:
    memory = np.zeros(5)
    delay_int = 0
    delay_frac = 0

    def __init__(self, delay):
        self.delay_int = np.floor(delay).astype(int)
        self.delay_frac = delay - self.delay_int

    def tick(self, input):
        self.memory = np.roll(self.memory, 1)
        self.memory[0] = input

        a = self.memory[self.delay_int]
        b = self.memory[self.delay_int + 1]

        return a + (b - a) * self.delay_frac



Fs = 48000

# Waveguide Length
N = 100

# Gate position
GATE_RATIO = 0.5
GATE = np.floor(N * GATE_RATIO).astype(int)

# Gate coefficient
K = 0.5

# Bridge damping
DAMPING = 1

# Fractional part for the gate position
DELTA = 0.0

# Simulation time
T = Fs * 4

right = np.zeros(N)
# right[1]=1
right[0:np.floor(GATE/2).astype(int)] = np.linspace(0,1,np.floor(GATE/2).astype(int), endpoint=False)
right[np.floor(GATE/2).astype(int):GATE] = np.linspace(1,0,np.ceil(GATE/2).astype(int))

left = right

total = right + left

INV_DELTA = 1 - DELTA
left_gate = GATE-1
right_gate = GATE + 1

LEFT_DELAY = DELTA*2
RIGHT_DELAY = INV_DELTA*2

A_D = LinearInterpolator(LEFT_DELAY)
A_delta = LinearInterpolator(RIGHT_DELAY)

X = np.arange(0, N)

fig, axs = plt.subplots(2, 1, layout='constrained')
right_plot, = axs[0].plot(X, right, label='right')
left_plot, = axs[0].plot(X, left, label='left')
axs[0].set_title('Right/Left traveling waves')
axs[0].legend()
axs[0].set_ylim([-1, 1])
axs[0].grid()

total_plot, = axs[1].plot(X, total, label='total')
axs[1].legend()
axs[1].set_ylim([-2, 2])
axs[1].grid()

def update(frame):
    global right, left, right_plot, left_plot, total_plot, A_D, A_delta

    left_input = right[left_gate]
    left_interpolated = A_D.tick(left_input)
    left[GATE] = left[GATE]*(1-K) + left_interpolated*-K

    right_input = left[right_gate]
    right_interpolated = A_delta.tick(right_input)
    right[GATE] = right[GATE]*(1-K) + right_interpolated*-K

    right_end = right[-1]
    left_end = left[0] * DAMPING

    right = np.roll(right, 1)
    left = np.roll(left, -1)

    right[0] = -left_end
    left[-1] = -right_end

    total = right + left

    # axs[0].cla()
    # axs[1].cla()
    # right_plot = axs[0].stem(X, right, label='right')
    # left_plot = axs[0].stem(X, left, label='left', linefmt='green')
    # axs[0].set_title('Right/Left traveling waves')
    # axs[0].legend()
    # axs[0].set_ylim([-1, 1])
    # axs[0].grid()

    # total_plot, = axs[1].plot(X, total, label='total')
    # axs[1].legend()
    # axs[1].set_ylim([-2, 2])
    # axs[1].grid()
    # plt.show()


    right_plot.set_data(X, right)
    left_plot.set_data(X, left)
    total_plot.set_data(X, total)

    return right_plot, left_plot, total_plot

ani = FuncAnimation(fig, update, frames=T, interval=20, blit=True)

plt.show()

# for i in range(T):
#     update(i)
