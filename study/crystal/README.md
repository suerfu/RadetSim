# Crystal

These set of scripts are used to automate study of gamma ray (or any particle) interaction with crystals with cylindrical geometry.

Relevant scripts are:
- `crystal.py`: generate temporary geometry config file and Geant4 macros and invoke `RadetSim` in the `Crystal` mode. After the simulation, it automatically calls `ProcessTrack.py` to sum the energy deposits in the track-level output ROOT file to generate event-level energy deposits.
  - usage: `crystal.py [-h] --diameter DIAMETER --energy ENERGY [--particles PARTICLES] [--output OUTPUT]`
  - To simulate crystals of different materials, the `material` part of Geant4 macro needs to be manually changed.
- `submit_crystal.py`: generate and submit LSF batch scripts for KEK's computing cluster for different combinations of crystal diameters and gamma energies. Number of particles and output directory can be changed in the script.
- `plot_r_vs_E.py`: make a 2D plot showing 1) probability of full absorption, and 2) average energy deposits (normalized) for different crystal dimensions and incident gamma energy.
  - usage: `plot_r_vs_E.py /path/to/directory`
  - the directory needs to contain all relevant `.npz` files produced as the event-level energy deposits.

# Example Images

![NaI](https://github.com/user-attachments/assets/3b7acd6c-0633-453b-9037-115cb2fcf16d)
![Si](https://github.com/user-attachments/assets/10bc7be3-53d9-469e-b431-718d37d142e4)
![Ta](https://github.com/user-attachments/assets/15cf4b8e-3465-4d28-ab70-c8bf65c14b73)

