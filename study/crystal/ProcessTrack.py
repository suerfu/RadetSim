#!/bin/env python3

import uproot
import numpy as np
from collections import defaultdict

def extract_edep_streaming( root_file_path, volumes_of_interest, tree_name="events"):
    """
    Extracts energy deposits from track-level ROOT file produced by RadetSim Monte Carlo simulation.

    Parameters:
        root_file_path (str): Path to the ROOT file.
        volumes_of_interest (list[str]): Volumes for which Edep should be collected.
        tree_name (str): Name of the tree inside the ROOT file.

    Returns:
        dict[str, list[float]]: Dictionary mapping volume name to a list of total Edep per event.
    """

    volumes_of_interest = set(volumes_of_interest)
    energy_by_volume = {vol: [] for vol in volumes_of_interest}

    with uproot.open(root_file_path) as file:

        tree = file[tree_name]

        n_entries = tree.num_entries
        counter = 0
        print( "Total number of entries:", n_entries )

        # Stream branches
        branches = tree.iterate(
            ["eventID", "trackID", "process", "volume", "Edep"],
            library = "np",
            step_size = 100000
        )

        current_event_edep = {vol: 0.0 for vol in volumes_of_interest}

        for batch in branches:

            eventIDs = batch["eventID"]
            trackIDs = batch["trackID"]
            processes = batch["process"]
            volumes = batch["volume"]
            edeps = batch["Edep"]

            #print( volumes, edeps )

            for eid, tid, proc, vol, edep in zip(eventIDs, trackIDs, processes, volumes, edeps):

                counter += 1

                # Detect start of a new event
                if eid == -1 and tid == -1 and proc == "newEvent":
                    # Store accumulated Edep for the last event
                    for vol_key in volumes_of_interest:
                        energy_by_volume[vol_key].append(current_event_edep[vol_key])

                        #print( vol_key, current_event_edep[vol_key])

                        current_event_edep[vol_key] = 0.0
                    continue

                # Accumulate Edep if volume matches
                vol_str = vol.decode("utf-8") if isinstance(vol, bytes) else vol
                if vol_str in volumes_of_interest:
                    current_event_edep[vol_str] += edep

            print('{0:.2f}% processed'.format( 100.0 * float(counter)/n_entries), end='\r')

        # Final flush (in case file doesn't end with newEvent)
        #for vol_key in volumes_of_interest:
        #    energy_by_volume[vol_key].append(current_event_edep[vol_key])

    return energy_by_volume



import sys

if len(sys.argv ) < 2:
    print("Please specify file to be procesed")
    exit(-1)

import matplotlib.pyplot as plt

data = extract_edep_streaming( sys.argv[1], ['Crystal'] )
np.savez( sys.argv[1].removesuffix(".root")+'.npz', data["Crystal"])
#print(data.keys())  # Lists all volumes

#plt.figure()
#plt.hist(data["Crystal"], bins=300 )  # Energy deposits in the "Crystal" volume
#plt.yscale('log')
#plt.show()

