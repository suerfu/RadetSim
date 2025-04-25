#!/bin/env python3

# Job submission script for crystal.py

import numpy as np
import os
import subprocess
import tempfile
import time
from datetime import datetime

# default output directory
#    
output_dir = "/home/qup/suerfu/data/suerfu/crystal/Se/"


# generate diameters and energies to be simulated
#
diameters = np.linspace( 1.0, 12.0, 50 )
energies = np.linspace( 0.1, 3.00, 50 )



def submit_lsf_job(job_name, command, queue='s', num_cores=1, walltime="5:00", memory="4G"):

    walltime = "5:00" if queue == 's' else "40:00"

    # Generate temporary LSF job submission script
    #
    with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix='.sh') as script_file:
        script_path = script_file.name
        print(script_path)
        script_file.write(f"""
#!/bin/bash
#BSUB -J {job_name}
#BSUB -q {queue}
#BSUB -n {num_cores}
#BSUB -W {walltime}
#BSUB -R "rusage[mem={memory}]"
#BSUB -o tmp/{job_name}.out
#BSUB -e tmp/{job_name}.err

cd /home/qup/suerfu/RadetSim/study/crystal/
source /home/qup/suerfu/snippets/settings/kekcc_load_hepsoftware.sh
source ~/venvs/py38/bin/activate
{command}
""")

    # Flag to control job submission; useful in dry runs.
    #
    submit = True

    if submit:

        try:
            subprocess.run(["bsub", "-L", "/bin/bash"], shell=True, stdin=open(f"{script_path}", 'r'))
            #subprocess.run( command.split(' ') )
                # this line is used to execute job directly in the terminal without submitting to cluster

        except subprocess.CalledProcessError as e:
            print(f"Error submitting job: {e}")

        os.remove(script_path)


if __name__ == "__main__":


    # if there's a file called missing.txt, then only runs specified in the missing.txt will be submitted.
    #
    missing_list = []
    missing_file = "missing.txt"

    # check if the file exists or not
    #
    if os.path.exists( missing_file ):
        with open(missing_file, "r") as f:
            missing_list = set(line.strip().removesuffix('.root').removesuffix('.npz') for line in f if line.strip())

    
    # iteratively submit the jobs
    #
    for d in diameters:

        for e in energies:

            job_name = f"d{d:.2f}_E{e:.2f}"
            
            output_pattern = f"output_d{d:.3f}_E{e:.3f}MeV"
            output_filename = output_dir + output_pattern +'.root'

            command = f"./crystal.py --diameter {d:.3f} --energy {e:.3f} --particles 1000000 --output {output_filename}"
            
            submit = True

            # if there is a file called missing.txt, then only generate the files in this missing list
            # submit should default to False, and set True only when it is in the list.

            if missing_list:    # list is not empty

                submit = False

                if output_pattern in missing_list:
                    submit = True

            if submit == True:
                print( output_pattern+".root" )
                print( command )
                submit_lsf_job( job_name, command, queue='s', memory="4G" )
                time.sleep( 5 )
