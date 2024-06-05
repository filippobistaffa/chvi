#!/bin/bash

function wait_empty_queue {
    while true
    do
        if hash condor_submit 2>/dev/null
        then
            remaining=$( condor_q | tail -n 3 | head -n 1 | cut -f 4 -d\  )
        elif hash sbatch 2>/dev/null
        then
            remaining=$( squeue --me -h | wc -l )
        else
            echo "Unknown cluster"
            exit 1
        fi
        echo "$remaining jobs remaining"
        if [[ $remaining == 0 ]]
        then
            return
        else
            echo "Waiting queue to be empty..."
            sleep 60
        fi
    done
}

memory="32GB"
cpus="64"
max=50
j=0

for d in $( seq 7 9 )
do
    for n in $( seq 2 9 )
    do
        if [ "$d" -ge 9 ] && [ "$n" -ge 4 ]; then
            continue
        elif [ "$d" -ge 8 ] && [ "$n" -ge 6 ]; then
            continue
        elif [ "$d" -ge 7 ] && [ "$n" -ge 9 ]; then
            continue
        fi
        if [ "$d" -ge 7 ]; then
            budget="4:00:00"
        else
            budget="1:00:00"
        fi
        for s in $( seq 1 20 )
        do
            if [[ $j == $max ]]
            then
                wait_empty_queue
                j=0
            fi
            echo "Submitted CHVI (d=$d n=$n s=$s) with a time budget of $budget (cpus=$cpus, memory=$memory)"
            $HOME/chvi/chvi/slurm-submit-pipeline.sh -d $d -n $n -s $s --time $budget --cpus $cpus --memory $memory
            j=$((j+1))
        done
    done
done
