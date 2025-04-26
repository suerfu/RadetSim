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
output_dir = "/home/qup/suerfu/data/kamioka/gamma/simulation/"


# generate diameters and energies to be simulated
#
isotope = { 'K40':10000000, 'U238':1000000, 'Th232':1000000 }
#isotope = { 'K40':1000, 'U238':1000, 'Th232':1000 }
material = [ 'Rock' ]
direction = [ 1 ]

NRun = 50

def submit_lsf_job(job_name, command, queue='l', num_cores=1, walltime="5:00", memory="4G"):

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

cd /home/qup/suerfu/RadetSim/study/kamioka/
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


def has_pending_jobs():
    """
    Returns True if there are any pending jobs (STAT == PEND) in the LSF system.
    """
    try:
        # Run 'bjobs' and capture output
        result = subprocess.run(['bjobs'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        output = result.stdout

        # Look for any line that contains 'PEND'
        for line in output.splitlines():
            if 'PEND' in line:
                return True

        return False

    except Exception as e:
        print(f"Error checking pending jobs: {e}")
        return False


if __name__ == "__main__":

    # iteratively submit the jobs
    #

    counter = 0

    while True:

        for d in direction:

            for m in material:

                for iso,num in isotope.items():

                    job_name = f"Dir{d}_{m}_{iso}"
            
                    output_filename = output_dir + "Output_" + job_name + f"_Run{counter}.root"

                    command = f"./kamioka.py --direction {d} --material {m} --isotope {iso} --particles {num} --output {output_filename}"
            
                    print( command )
                    submit_lsf_job( job_name, command, queue='l', memory="4G" )

        counter += 1

        while has_pending_jobs():
            time.sleep(1)

        if counter == NRun:
            break;
