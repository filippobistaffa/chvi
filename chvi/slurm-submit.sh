#!/bin/bash

# default instance parameters
d=5
n=5
seed=$RANDOM

# default job parameters
out=""
err="/dev/null"
time="1:00:00"
cpus="16"
memory="8GB"

args=""

while [[ $# > 0 ]]
do
    key="$1"
    case $key in
        -d|--dimensions)
            shift
            d="$1"
            shift
        ;;
        -n|--size)
            shift
            n="$1"
            shift
        ;;
        -s|--seed)
            shift
            seed="$1"
            shift
        ;;
        --output)
            shift
            out="$1"
            shift
        ;;
        --error)
            shift
            err="$1"
            shift
        ;;
        --time)
            shift
            time="$1"
            shift
        ;;
        --cpus)
            shift
            cpus="$1"
            shift
        ;;
        --memory)
            shift
            memory="$1"
            shift
        ;;
        *)
            args="$args$key "
            shift
        ;;
    esac
done

if [ -z "$out" ]
then
    out=$d-$n-$seed.out
fi

if hash sbatch 2>/dev/null
then

tmpfile=$(mktemp)
sbatch 1> $tmpfile <<EOF
#!/bin/bash
#SBATCH --job-name=chvi-$d-$n-$seed
#SBATCH --time=$time
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=$cpus
#SBATCH --mem=$memory
#SBATCH --output=$out
#SBATCH --error=$err
#SBATCH --mail-type=begin
#SBATCH --mail-type=end
#SBATCH --mail-type=fail
#SBATCH --mail-user=email@domain
srun $HOME/chvi/chvi/chvi -d $d -n $n -s $seed $args
EOF

else
echo "Unknown cluster"
fi
