#!/bin/bash

exe=./DetectorTemplate

dir=/global/home/users/suerfu/DetectorTemplate/build
outdir=/global/home/users/suerfu/data/Kamioka/rock

output_name=Rock

for i in {1..10}
do
    #k="NaI"
    #for k in {"Cu","PE","Pb","SS","Ti","PureCu"}
    #do
        for j in {"K40","U238","Th232"}
        do
            echo "Processing ${j} run ${i}"

            sbfile=.${output_name}.txt
            script=macros/rock_${j}.mac

            echo "#!/bin/bash" > $sbfile

            echo "#SBATCH --job-name=${j}${i}" >> $sbfile
            echo "#SBATCH --account=fc_hedm" >> $sbfile
            echo "#SBATCH --partition=savio2_htc" >> $sbfile
            echo "#SBATCH --qos=savio_normal" >> $sbfile

            echo "#SBATCH --nodes=1" >> $sbfile
            echo "#SBATCH --ntasks=1" >> $sbfile
            echo "#SBATCH --cpus-per-task=1" >> $sbfile

            echo "#SBATCH --mem=5000M" >> $sbfile
            echo "#SBATCH --time=47:59:00" >> $sbfile

            if [ $i -eq 10 ]
            then
                echo "#SBATCH --mail-user=suerfu@berkeley.edu" >> $sbfile
                echo "#SBATCH --mail-type=BEGIN" >> $sbfile
                echo "#SBATCH --mail-type=END" >> $sbfile
            fi

            echo "cd $dir" >> $sbfile
            echo "$exe -m $script --output ${outdir}/${output_name}_${j}_run${i}.root" >> $sbfile

            NPrevJobs=$(squeue -u suerfu -o %T | grep RUNNING | wc -l)

            sbatch $sbfile
            rm $sbfile

            NCurJobs=$(squeue -u suerfu -o %T | grep RUNNING | wc -l)
            while [ $NCurJobs -eq $NPrevJobs ]
            do
                sleep 5
                NCurJobs=$(squeue -u suerfu -o %T | grep RUNNING | wc -l)
            done
            sleep 10
            echo "    " $NCurJobs " running..."

        done
#    done
done

