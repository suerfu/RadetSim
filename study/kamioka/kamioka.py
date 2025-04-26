#!/bin/env python3

# Written by Suerfu Burkhant on April 25, 2025
# This script will generate macro and config file based on commandline input
# and invoke RadetSim to simulate gamma background measurement in Kamioka.

import os
import argparse
import subprocess

from datetime import datetime

output_dir = './'
# default output directory

def get_Z_and_A(isotope):
    periodic_table = {
        "H": 1, "He": 2, "Li": 3, "Be": 4, "B": 5, "C": 6, "N": 7, "O": 8, "F": 9, "Ne": 10,
        "Na": 11, "Mg": 12, "Al": 13, "Si": 14, "P": 15, "S": 16, "Cl": 17, "Ar": 18,
        "K": 19, "Ca": 20, "Sc": 21, "Ti": 22, "V": 23, "Cr": 24, "Mn": 25, "Fe": 26,
        "Co": 27, "Ni": 28, "Cu": 29, "Zn": 30, "Ga": 31, "Ge": 32, "As": 33, "Se": 34,
        "Br": 35, "Kr": 36, "Rb": 37, "Sr": 38, "Y": 39, "Zr": 40, "Nb": 41, "Mo": 42,
        "Tc": 43, "Ru": 44, "Rh": 45, "Pd": 46, "Ag": 47, "Cd": 48, "In": 49, "Sn": 50,
        "Sb": 51, "Te": 52, "I": 53, "Xe": 54, "Cs": 55, "Ba": 56, "La": 57, "Ce": 58,
        "Pr": 59, "Nd": 60, "Pm": 61, "Sm": 62, "Eu": 63, "Gd": 64, "Tb": 65, "Dy": 66,
        "Ho": 67, "Er": 68, "Tm": 69, "Yb": 70, "Lu": 71, "Hf": 72, "Ta": 73, "W": 74,
        "Re": 75, "Os": 76, "Ir": 77, "Pt": 78, "Au": 79, "Hg": 80, "Tl": 81, "Pb": 82,
        "Bi": 83, "Po": 84, "At": 85, "Rn": 86, "Fr": 87, "Ra": 88, "Ac": 89, "Th": 90,
        "Pa": 91, "U": 92, "Np": 93, "Pu": 94, "Am": 95, "Cm": 96, "Bk": 97, "Cf": 98,
        "Es": 99, "Fm": 100, "Md": 101, "No": 102, "Lr": 103, "Rf": 104, "Db": 105,
        "Sg": 106, "Bh": 107, "Hs": 108, "Mt": 109, "Ds": 110, "Rg": 111, "Cn": 112,
        "Nh": 113, "Fl": 114, "Mc": 115, "Lv": 116, "Ts": 117, "Og": 118 }

    # Remove digits to get the symbol, remove letters to get the mass number
    symbol = ''.join(filter(str.isalpha, isotope))
    A_str = ''.join(filter(str.isdigit, isotope))

    if not symbol or not A_str:
        raise ValueError(f"Invalid isotope format: {isotope}")

    Z = periodic_table.get(symbol)
    A = int(A_str)

    if Z is None:
        raise ValueError(f"Unknown element symbol: {symbol}")

    return Z, A


def run_simulation( direction, material, isotope, n_particles = 100, output_filename = None, dry = False):
    """
        Generate required macros and config file to run RadetSim in Crystal mode.
    """

    Z, A = get_Z_and_A( isotope )
    
    # get an ID based on current time
    #
    now = datetime.now()
    unique_id = now.strftime("_%H%M%S")

    # Default filenames consist of diameter and energy
    # unless specified otherwise
    #
    pattern = f"d{direction:d}_{material}_{isotope}"
    config_filename = "config_"+pattern+unique_id+".cfg"
    macro_filename  = "macro_"+pattern+unique_id+".mac"

    if output_filename == None:
        output_filename = output_dir + "output_"+pattern+".root"

    # Create crystal config content
    #
    crystal_cfg = f"""
type : KamiokaGamma-2025,
direction : {direction:d},
"""
    # Create Geant4 Macro
    #
    macro = f"""
/geometry/loadconfig ./{config_filename}

/run/initialize
/tracking/verbose 0

/filter/recordWhenHit NaICrystal

/gps/particle ion
/gps/ion {Z} {A}
/gps/energy 0 keV

/generator/setMaterial {material}

/run/printProgress 10000
/run/beamOn {n_particles}
"""

    # Write files
    with open(config_filename, "w") as cfg:
        cfg.write(crystal_cfg)

    with open(macro_filename, "w") as mac:
        mac.write(macro)

    # Run simulation
    if dry == False:
        try:
            subprocess.run(["./RadetSim", "-m", macro_filename, "--output", output_filename], check=True)
            print(f"Output written to: {output_filename}")
    
            print("Processing the simulation files...")
            #subprocess.run(["./ProcessTrack.py", output_filename], check=True)
            #subprocess.run(["touch", './'+output_filename.removesuffix('.root')+'.npz'], check=True )

        except subprocess.CalledProcessError as e:
            print(f"Simulation failed: {e}")

        finally:
            os.remove(config_filename)
            os.remove(macro_filename)
            print(f"Cleaned up: {config_filename}, {macro_filename}")


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Run RadetSim in crystal geometry, with varying crystal dimensions and energy.")

    parser.add_argument("--direction", type=int, required=True, help="Direction (0:int 1:bottom 2:top 3:east 4:west 5:north 6:south")
    parser.add_argument("--material", type=str, required=True, help="Name of material in which to simulate radioactive decays used")
    parser.add_argument("--isotope", type=str, required=True, help="Name of isotope: H-3, U238 or 232Th")
    parser.add_argument("--particles", type=int, default=10000000, help="Number of particles to simulate")
    parser.add_argument("--output", type=str, default=None, help="Name of output file")
    parser.add_argument("--dry-run", action='store_true', help="Write only the macro and config file without executing the command")

    args = parser.parse_args()

    run_simulation(args.direction, args.material, args.isotope, args.particles, args.output, args.dry_run)

