#!/bin/env python3

# Make 2D plot of incident energy v.s. probability of full absorption and average energy deposit.
# Edep is normalized.

# usage is ./script_name.py /path/to/directory

# where directory contains .npz files produced by processing the track-level ROOT file with ProcessTrack.py

import os
import re
import sys

import numpy as np
import matplotlib.pyplot as plt


# Directory containing the .npz files
DATA_DIR = './'

if len(sys.argv) < 2:
    print('usage:', sys.argv[0], '/path/to/npz-directory')
    exit(1)

DATA_DIR = sys.argv[1]


# Regex pattern to extract diameter and energy from filename
#
pattern = re.compile(r"output_d(\d+(?:\.\d+)?)_E(\d+(?:\.\d+)?)MeV\.npz")

# Storage for data
#
diameters = set()
energies = set()

results = {}


def find_nearest(array, value):
    """
    returns element in array that is closest to the given value
    """
    array = np.asarray(array)
    idx = (np.abs(array - value)).argmin()
    return array[idx]


# Scan directory
#
for filename in os.listdir(DATA_DIR):

    match = pattern.match(filename)

    if match:

        print(filename, end='\r')

        d = float(match.group(1))        # diameter in cm
        E = 1000 * float(match.group(2))        # gamma energy in MeV

        #d = find_nearest( diameters, d)
        #E = find_nearest( energies,  E)

        data = np.load(os.path.join(DATA_DIR, filename))
        edep = data["arr_0"]

        total_events = len(edep)

        full_abs = edep[ np.isclose(edep, E, atol=0.01 * E) ]  # ~1% tolerance
        full_abs_fraction = len(full_abs) / total_events

        avg_edep = np.mean(edep/E)

        diameters.add(d)
        energies.add(E)
        results[(d, E)] = (full_abs_fraction, avg_edep)

print('')

# Sort and index
#
diameters = sorted(diameters)
energies = sorted(energies)

D, E = np.meshgrid(diameters, energies)

Z_abs = np.zeros_like(D)
Z_avg = np.zeros_like(D)

for i, e in enumerate(energies):

    for j, d in enumerate(diameters):

        frac, avg = results.get((d, e), (-1, -1))
        Z_abs[i, j] = frac
        Z_avg[i, j] = avg

# Plotting
#
fig, axs = plt.subplots(1, 2, figsize=(14, 6), constrained_layout=True)

im1 = axs[0].imshow(Z_abs, origin='lower', aspect='auto',
                    extent=[min(diameters), max(diameters), min(energies), max(energies)],
                    cmap='viridis')
contours = axs[0].contour( D, E, Z_abs, levels=np.arange(0.1,1,0.1), colors='white', linewidths=1.5)
axs[0].clabel(contours, inline=True, fontsize=8, fmt="%.1f")
axs[0].set_title("Probability of Full Absorption")
axs[0].set_xlabel("Crystal Diameter [cm]")
axs[0].set_ylabel("Gamma Energy [keV]")
axs[0].minorticks_on()
axs[0].grid(True)
plt.colorbar(im1, ax=axs[0])

im2 = axs[1].imshow(Z_avg, origin='lower', aspect='auto',
                    extent=[min(diameters), max(diameters), min(energies), max(energies)],
                    cmap='plasma')
contours = axs[1].contour( D, E, Z_avg, levels=np.arange(0.1,1,0.1), colors='white', linewidths=1.5)
axs[1].clabel(contours, inline=True, fontsize=8, fmt="%.1f")
axs[1].set_title("Standardized Average Energy Deposit")
axs[1].set_xlabel("Crystal Diameter [cm]")
axs[1].set_ylabel("Gamma Energy [keV]")
axs[1].minorticks_on()
axs[1].grid(True)
plt.colorbar(im2, ax=axs[1])

plt.suptitle("Tantalum", fontsize=16)
plt.show()
