#!/usr/bin/env bash

bin="./chvi"
qcachegrind="./qcachegrind"
pprof="google-pprof"
trace="trace"
heapcheck="normal" #strict draconian

if [[ $1 != "cpu" && $1 != "leak" ]]
then
	echo "Usage: $0 [cpu|leak] arguments"
	exit
fi

if [[ $1 == "cpu" ]]
then
        ${bin} ${@:2}
        ${pprof} --callgrind ${bin} ${trace}.prof > ${trace}.callgrind
        ${qcachegrind} ${trace}.callgrind &
else
        env PPROF_PATH=`which ${pprof}` HEAPCHECK=${heapcheck} ${bin} ${@:2}
fi
