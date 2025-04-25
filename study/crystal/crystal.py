#!/bin/env python3

# Written by Suerfu Burkhant on April 23, 2025
# This script will generate macro and config file based on commandline input
# and invoke RadetSim in crystal mode.

import os
import argparse
import subprocess
from datetime import datetime

output_dir = "/home/qup/suerfu/data/suerfu/crystal/Se/"
# default output directory

def run_simulation(diameter_cm, energy_mev, n_particles, output_filename = None):
    """
        Generate required macros and config file to run RadetSim in Crystal mode.
    """

    # Set world dimensions larger than the crystal
    #
    margin = 10  # cm
    world_dim = diameter_cm + margin

    # get an ID based on current time
    #
    now = datetime.now()
    unique_id = now.strftime("_%H%M%S%f")

    # Default filenames consist of diameter and energy
    # unless specified otherwise
    #
    pattern = f"d{diameter_cm:.1f}_E{energy_mev:.2f}MeV"
    config_filename = "crystal_"+pattern+unique_id+".cfg"
    macro_filename = "macro_"+pattern+unique_id+".mac"

    if output_filename == None:
        output_filename = output_dir + "output_"+pattern+".root"

    # Create crystal config content
    #
    crystal_cfg = f"""
type : Crystal,

# dimension of world xyz in centi-meter

world {{
    x : {world_dim},
    y : {world_dim},
    z : {world_dim},
}}

crystal {{
    diameter : {diameter_cm},
    height : {diameter_cm},
    material : G4_Se,
}}
"""
    # Set particle gun position just outside the crystal along z-axis
    #
    gun_z = diameter_cm / 2 + 1  # 1 cm outside

    # Create Geant4 Macro
    #
    macro = f"""
/geometry/loadconfig ./{config_filename}

/run/initialize
/tracking/verbose 0

/filter/recordWhenHit Crystal

/gps/particle gamma
/gps/position 0 0 {gun_z:.2f}
/gps/direction 0 0 -1
/gps/energy {energy_mev} MeV

/run/printProgress 10000
/run/beamOn {n_particles}
"""

    # Write files
    with open(config_filename, "w") as cfg:
        cfg.write(crystal_cfg)

    with open(macro_filename, "w") as mac:
        mac.write(macro)

    # Run simulation
    try:
        subprocess.run(["./RadetSim", "-m", macro_filename, "--output", output_filename], check=True)
        print(f"Output written to: {output_filename}")
    
        print("Processing the simulation files...")
        subprocess.run(["./ProcessTrack.py", output_filename], check=True)
        #subprocess.run(["touch", './'+output_filename.removesuffix('.root')+'.npz'], check=True )

    except subprocess.CalledProcessError as e:
        print(f"Simulation failed: {e}")

    finally:
        os.remove(config_filename)
        os.remove(macro_filename)
        print(f"Cleaned up: {config_filename}, {macro_filename}")


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Run RadetSim in crystal geometry, with varying crystal dimensions and energy.")

    parser.add_argument("--diameter", type=float, required=True, help="Diameter (and height) of the crystal in cm")
    parser.add_argument("--energy", type=float, required=True, help="Gamma energy in MeV")
    parser.add_argument("--particles", type=int, default=500, help="Number of particles to simulate")
    parser.add_argument("--output", type=str, default=None, help="Name of output file")

    args = parser.parse_args()

    run_simulation(args.diameter, args.energy, args.particles, args.output)

